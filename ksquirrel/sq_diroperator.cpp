/***************************************************************************
                          sq_diroperator.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qtimer.h>
#include <qlabel.h>
#include <qapplication.h>

#include <kstringhandler.h>
#include <klocale.h>
#include <kprogress.h>
#include <kpopupmenu.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kmessagebox.h>

#include "ksquirrel.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_thumbnailsize.h"
#include "sq_pixmapcache.h"
#include "sq_externaltool.h"

#define SQ_MAX_WORD_LENGTH 50

SQ_DirOperator::SQ_DirOperator(const KURL &url, ViewT type_, QWidget *parent, const char *name) :
    SQ_DirOperatorBase(url, type_, parent, name)
{
    kdDebug() << "+SQ_DirOperator" << endl;

    // do some importatnt actions, when diroperator has loaded a directory
    connect(this, SIGNAL(finishedLoading()), this, SLOT(slotFinishedLoading()));

    connect(SQ_ExternalTool::instance()->constPopupMenu(), SIGNAL(activated(int)), this, SLOT(slotActivateExternalTool(int)));

    // update information
    connect(this, SIGNAL(updateInformation(int,int)), this, SLOT(slotUpdateInformation(int,int)));

    connect(dirLister(), SIGNAL(deleteItem(KFileItem *)), this, SLOT(slotItemDeleted(KFileItem *)));
}

SQ_DirOperator::~SQ_DirOperator()
{
    kdDebug() << "-SQ_DirOperator" << endl;
}

void SQ_DirOperator::slotFinishedLoading()
{
    int dirs = numDirs(), files = numFiles(), total = dirs+files;

    slotUpdateInformation(files, dirs);

    // clear QLabels in statusbar, if current directory has
    // no files/directories
    if(!total)
    {
        KSquirrel::app()->sbarWidget("fileIcon")->clear();
        KSquirrel::app()->sbarWidget("fileName")->clear();
        return;
    }

    QTimer::singleShot(50, this, SLOT(slotDelayedFinishedLoading()));
}

void SQ_DirOperator::slotUpdateInformation(int files, int dirs)
{
    int total = dirs + files;

    QString str = QString(i18n(" Total %1 (%2, %3) "))
                    .arg(total)
                    .arg(i18n("1 directory", "%n dirs", dirs))
                    .arg(i18n("1 file", "%n files", files));

    KSquirrel::app()->sbarWidget("dirInfo")->setText(str);
}

void SQ_DirOperator::slotDelayedFinishedLoading()
{
    KURL up_url = url().upURL();

    KFileItem *first = fileview->firstFileItem();

    if(!first) return;

    // ignore ".." item
    if(first->url() == up_url)
        first = fileview->nextItem(first);

    // hell!
    if(!first) return;

    SQ_Config::instance()->setGroup("Fileview");

    // SQ_WidgetStack will select first supported image for us
    if(SQ_Config::instance()->readBoolEntry("tofirst", true))
    {
        if(SQ_WidgetStack::instance()->moveTo(SQ_WidgetStack::Next, first) == SQ_WidgetStack::moveFailed)
            setCurrentItem(first);
    }
    else
        setCurrentItem(first);

    startOrNotThumbnailUpdate();
}

/*
 *  Change thumbnail size. KSquirrel will emit thumbSizeChanged() signal, and
 *  diroperator will catch it. Only diroperator, which manages SQ_FileThumbView
 *  won't ignore this signal. See also SQ_WidgetStack::raiseWidget().
 */
void SQ_DirOperator::slotSetThumbSize(const QString &size)
{
    // for safety...
    if(type != SQ_DirOperator::TypeThumbs)
        return;

    SQ_ThumbnailSize::instance()->setPixelSize(size);

    // reinsert files
    removeCdUpItem();
    smartUpdate();

    // finally, update thumbnails
    startOrNotThumbnailUpdate();
}

/*
 *  Smart update. Store all file items, reset view,
 *  and transfer all items back.
 *
 *  Taken from kdelibs-3.2.3/kio/kfile/kdiroperator.cpp
 */
void SQ_DirOperator::smartUpdate()
{
    // store old file items
    KFileItemList items = *fileview->items();
    KFileItemList oldSelected = *fileview->selectedItems();
    KFileItem *oldCurrentItem = fileview->currentFileItem();

    // heh, 'items' now contains "..", we should remove it!
    removeCdUpItem();

    // clear view and add items (see SQ_FileThumbView::insertItem() for more)
    fileview->clear();

    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_Config::instance()->setGroup("Thumbnails");

        // determine new grid size
        int newgrid = SQ_ThumbnailSize::instance()->currentPixelSize() 
                    + SQ_Config::instance()->readNumEntry("margin", 2) + 2;

        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        tv->setGridX(newgrid);
        tv->arrangeItemsInGrid();
    }

    fileview->addItemList(items);

    // set current item...
    if(oldCurrentItem)
    {
        fileview->setCurrentItem(oldCurrentItem);
        fileview->setSelected(oldCurrentItem, false);
        fileview->ensureItemVisible(oldCurrentItem);
    }

    // restore selection
    if(!oldSelected.isEmpty())
    {
        KFileItemListIterator it(oldSelected);

        for(; it.current();++it)
            fileview->setSelected(it.current(), true);
    }
}

/*
 *  Remove ".." from currently listed items
 */
void SQ_DirOperator::removeCdUpItem()
{
    // for safety...
    if(!fileview)
        return;

    KFileItemList m_items = *fileview->items();
    KFileItemListIterator it_s(m_items);
    KURL up_url = url().upURL();

    for(; it_s.current();++it_s)
    {
        if(it_s.current()->url() == up_url)
        {
            fileview->removeItem(it_s.current());
            break;
        }
    }
}

/*
 *  Some item has been selected.
 */
void SQ_DirOperator::slotSelected(QIconViewItem *item)
{
    if(!item) return;

    KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item);

    if(f)
    {
        KFileItem *fi = f->fileInfo();

        // update statusbar
        statusFile(fi);
    }
}

void SQ_DirOperator::slotSelected(QListViewItem *item)
{
    if(!item) return;

    KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item);

    if(f)
    {
        KFileItem *fi = f->fileInfo();

        // update statusbar
        statusFile(fi);
    }
}

/*
 *  Update current file's icon and name
 *  in statusbar. Will be called after some item
 *  has been selected.
 */
void SQ_DirOperator::statusFile(KFileItem *fi)
{
    QString str;
    QPixmap px;

    // determine pixmap
    px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, KIcon::SizeSmall);
    KSquirrel::app()->sbarWidget("fileIcon")->setPixmap(px);

    // costruct name and size
    str = QString("  %1 %2").arg(fi->text()).arg((fi->isDir())?"":QString(" (" + KIO::convertSize(fi->size()) + ")"));

    // update statusbar
    KSquirrel::app()->sbarWidget("fileName")->setText(KStringHandler::csqueeze(str, SQ_MAX_WORD_LENGTH));

    SQ_WidgetStack::instance()->selectFile(fi, this);
}

/*
 *  If current view type is "thumbnail view", then
 *     - if thumbnail view is visible (QWidget::isVisible()), start
 *       thumbnail update;
 *     - if thumbnail view is not visible, keep in mind
 *       (SQ_FileThumbView::waitForShowEvent()) that we should start
 *       thumbnail update later.
 */
void SQ_DirOperator::startOrNotThumbnailUpdate()
{
    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs)
    {
        qApp->processEvents();

        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        // start thumbnail update only when this
        // diroperator is visible
        if(tv->isVisible())
            tv->startThumbnailUpdate();
        else
            tv->waitForShowEvent();
    }
}

/*
 *  Invoked, when some item has been deleted. We should
 *  remove appropriate thumbnail from pixmap cache.
 */
void SQ_DirOperator::slotItemDeleted(KFileItem *item)
{
    // hell!
    if(!item)
        return;

    kdDebug() << "Deleting " << item->url().path() << endl;

    // remove thumbnail from cache,
    SQ_PixmapCache::instance()->remove(item->url().path());
}

/*
 *  Invoked, when user selected some external tool in menu.
 */
void SQ_DirOperator::slotActivateExternalTool(int id)
{
    // get currently selected items
    KFileItemList *items = const_cast<KFileItemList *>(selectedItems());
    QStringList list;

    if(!items || items->isEmpty()) return;

    int index = SQ_ExternalTool::instance()->constPopupMenu()->itemParameter(id);

    KFileItem *f = items->first();

    while(f)
    {
        list.append(f->url().path());
        f = items->next();
    }

    if(list.empty()) return;

    KShellProcess proc;

    // get appropriate desktop file
    Tool *tool = &SQ_ExternalTool::instance()->at(index);
    QString comm = tool->command;

    int per_f = comm.contains("%f");
    int per_F = comm.contains("%F");

    // %f = single file
    // %F = multiple files
    if(per_f && per_F)
    {
        KMessageBox::error(this, i18n("Command cann't contain both \"%f\" and \"%F\""), i18n("Error processing command"));
        return;
    }
    else if(!per_f && !per_F)
    {
        KMessageBox::error(this, i18n("Command should contain \"%f\" or \"%F\""), i18n("Error processing command"));
        return;
    }
    else if(per_f)
    {
        comm.replace("%f", "");
        proc << comm << KShellProcess::quote(list.first());
    }
    else
    {
        comm.replace("%F", "");
        proc << comm;

        for(QStringList::iterator it = list.begin();it != list.end();++it)
            proc << KShellProcess::quote(*it);
    }

    // start process
    proc.start(KProcess::DontCare);
}
