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
#include <kglobal.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kstdaccel.h>
#include <kmessagebox.h>
#include <konq_operations.h>

#include "ksquirrel.h"
#include "sq_archivehandler.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_thumbnailsize.h"
#include "sq_pixmapcache.h"
#include "sq_libraryhandler.h"
#include "sq_externaltool.h"
#include "sq_popupmenu.h"
#include "sq_imagebasket.h"
#include "sq_categoriesview.h"
#include "sq_categorybrowsermenu.h"
#include "sq_navigatordropmenu.h"
#include "sq_previewwidget.h"
#include "sq_converter.h"
#include "sq_treeview.h"

static const int  SQ_MAX_WORD_LENGTH = 50;

SQ_DirOperator::SQ_DirOperator(const KURL &url, ViewT type_, QWidget *parent, const char *name)
    : KDirOperator(url, parent, name), type(type_)
{
    kdDebug() << "+SQ_DirOperator" << endl;

    usenew = false;

    // create and insert new actions in context menu
    setupActions();

    connect(this, SIGNAL(finishedLoading()), this, SLOT(slotFinishedLoading()));
    connect(this, SIGNAL(updateInformation(int,int)), this, SLOT(slotUpdateInformation(int,int)));
    connect(this, SIGNAL(urlEntered(const KURL&)), this, SLOT(slotUrlEntered(const KURL&)));
    connect(this, SIGNAL(dropped(const KFileItem *, QDropEvent*, const KURL::List&)),
            this, SLOT(slotDropped(const KFileItem *, QDropEvent*, const KURL::List&)));

    connect(SQ_ExternalTool::instance()->constPopupMenu(), SIGNAL(activated(int)), this, SLOT(slotActivateExternalTool(int)));
    connect(dirLister(), SIGNAL(deleteItem(KFileItem *)), this, SLOT(slotItemDeleted(KFileItem *)));
    connect(dirLister(), SIGNAL(newItems(const KFileItemList &)), this, SLOT(slotNewItems(const KFileItemList &)));
    connect(dirLister(), SIGNAL(refreshItems(const KFileItemList &)), this, SLOT(slotRefreshItems(const KFileItemList &)));

    timer_preview = new QTimer(this);
    connect(timer_preview, SIGNAL(timeout()), this, SLOT(slotPreview()));

    setAcceptDrops(true);

    fileview = 0;
}

SQ_DirOperator::~SQ_DirOperator()
{
    kdDebug() << "-SQ_DirOperator" << endl;
}

void SQ_DirOperator::slotUrlEntered(const KURL &url)
{
    usenew = false;

    clearListers();

    SQ_WidgetStack::instance()->setURLForCurrent(url);
}

void SQ_DirOperator::slotExecuted(KFileItem *fi)
{
    execute(fi);
}

void SQ_DirOperator::slotSelected(KFileItem *fi)
{
    highlight(fi);

    timer_preview->start(SQ_PreviewWidget::instance()->delay(), true);
}

void SQ_DirOperator::prepareView(ViewT t)
{
    type = t;

    switch(type)
    {
        case  SQ_DirOperator::TypeDetailed:
        {
            SQ_FileDetailView *dv;
            fileview = dv = new SQ_FileDetailView(this, "detail view");

            connect(dv, SIGNAL(launch(KFileItem *)), this, SLOT(slotExecuted(KFileItem *)));
            connect(dv, SIGNAL(highlighted(KFileItem *)), this, SLOT(slotSelected(KFileItem *)));
        }
        break;

        case  SQ_DirOperator::TypeList:
        case  SQ_DirOperator::TypeIcons:
        {
            SQ_FileIconView *iv;
            fileview = iv = new SQ_FileIconView(this, (type == SQ_DirOperator::TypeIcons) ? "icon view":"list view");

            connect(iv, SIGNAL(launch(KFileItem *)), this, SLOT(slotExecuted(KFileItem *)));
            connect(iv, SIGNAL(highlighted(KFileItem *)), this, SLOT(slotSelected(KFileItem *)));

            disableSpecificActions(iv);
        }
        break;

        case  SQ_DirOperator::TypeThumbs:
        {
            SQ_FileThumbView *tv;
            fileview = tv = new SQ_FileThumbView(this, "thumb view");

            connect(tv, SIGNAL(launch(KFileItem *)), this, SLOT(slotExecuted(KFileItem *)));
            connect(tv, SIGNAL(highlighted(KFileItem *)), this, SLOT(slotSelected(KFileItem *)));
            connect(dirLister(), SIGNAL(canceled()), tv, SLOT(stopThumbnailUpdate()));

            disableSpecificActions(tv);
        }
        break;
    }
}

void SQ_DirOperator::setPreparedView()
{
    setView(fileview);
}

void SQ_DirOperator::slotDropped(const KFileItem *i, QDropEvent*, const KURL::List &urls)
{
    KURL url = i ? i->url() : SQ_WidgetStack::instance()->url();

    // setup and show popup menu with actions
    SQ_NavigatorDropMenu::instance()->setupFiles(urls, url);
    SQ_NavigatorDropMenu::instance()->exec(QCursor::pos());
}

// Insert new actions in context menu.
void SQ_DirOperator::setupActions()
{
    KActionSeparator *sep = new KActionSeparator;

    KAction *pAEditMime = new KAction(i18n("Edit file type"), 0, 0, this, SLOT(slotEditMime()), actionCollection(), "SQ Edit Mime");
    KAction *basketAction = new KAction(i18n("Add To Basket"), "folder_image", CTRL+Key_B,
            this, SLOT(slotAddToBasket()), actionCollection(), "dirop_tobasket");
    KActionMenu *file = new KActionMenu(i18n("File actions"), actionCollection());

    actionCollection()->action("mkdir")->setShortcut(KShortcut(CTRL+Qt::Key_N));

    // remove "View" submenu, since we will insert our's one.
    setupMenu(KDirOperator::SortActions | KDirOperator::NavActions | KDirOperator::FileActions);

    KActionMenu *pADirOperatorMenu = dynamic_cast<KActionMenu *>(actionCollection()->action("popupMenu"));

    pADirOperatorMenu->insert(pAEditMime);
    pADirOperatorMenu->insert(sep);
    pADirOperatorMenu->popupMenu()->insertItem(i18n("Add To &Category"), SQ_CategoriesBox::instance()->popupMenu());
    pADirOperatorMenu->insert(basketAction);

    connect(actionCollection()->action("properties"), SIGNAL(enabled(bool)), basketAction, SLOT(setEnabled(bool)));

    connect(actionCollection()->action("properties"), SIGNAL(enabled(bool)),
        SQ_ExternalTool::instance()->constPopupMenu(), SLOT(setEnabled(bool)));

    connect(actionCollection()->action("properties"), SIGNAL(enabled(bool)),
        SQ_CategoriesBox::instance()->popupMenu(), SLOT(setEnabled(bool)));

    connect(actionCollection()->action("properties"), SIGNAL(enabled(bool)),
        file, SLOT(setEnabled(bool)));

    file->insert(new KAction(i18n("Copy"), "editcopy", KStdAccel::copy(), SQ_WidgetStack::instance(), SLOT(slotFileCopy()), actionCollection(), "dirop_copy"));
    file->insert(new KAction(i18n("Cut"), "editcut", KStdAccel::cut(), SQ_WidgetStack::instance(), SLOT(slotFileCut()), actionCollection(), "dirop_cut"));
    file->insert(new KAction(i18n("Paste"), "editpaste", KStdAccel::paste(), SQ_WidgetStack::instance(), SLOT(slotFilePaste()), actionCollection(), "dirop_paste"));
    file->insert(sep);
    file->insert(new KAction(i18n("Copy to..."), 0, 0, SQ_WidgetStack::instance(), SLOT(slotFileCopyTo()), actionCollection(), "dirop_copyto"));
    file->insert(new KAction(i18n("Move to..."), 0, 0, SQ_WidgetStack::instance(), SLOT(slotFileMoveTo()),actionCollection(), "dirop_cutto"));
    file->insert(new KAction(i18n("Link to..."), 0, 0, SQ_WidgetStack::instance(), SLOT(slotFileLinkTo()), actionCollection(), "dirop_linkto"));
    file->insert(sep);
    file->insert(new KAction(i18n("Run"), "launch", CTRL+Key_J, SQ_WidgetStack::instance(), SLOT(slotRunSeparately()), actionCollection(), "dirop_runsepar"));
    file->insert(sep);
    file->insert(new KAction(i18n("Recreate selected thumbnails"), "reload", CTRL+Key_T, SQ_WidgetStack::instance(),
        SLOT(slotRecreateThumbnail()), actionCollection(), "dirop_recreate_thumbnails"));
    file->insert(sep);
    file->insert(new KAction(i18n("Convert..."), 0, CTRL+Key_K, SQ_Converter::instance(), SLOT(slotStartEdit()),
        actionCollection(), "dirop_convert"));

    pADirOperatorMenu->popupMenu()->insertItem(i18n("File actions"), file->popupMenu(), -1, 0);
    pADirOperatorMenu->popupMenu()->insertItem(i18n("&External Tools"), SQ_ExternalTool::instance()->constPopupMenu(), -1, 1);
    pADirOperatorMenu->popupMenu()->insertSeparator(2);
}

void SQ_DirOperator::slotEditMime()
{
    KFileItem *f = view()->currentFileItem();

    if(f)
        KonqOperations::editMimeType(f->mimetype());
}

void SQ_DirOperator::slotAddToBasket()
{
    SQ_ImageBasket::instance()->add(*selectedItems());
}

void SQ_DirOperator::slotFinishedLoading()
{
//    printf("*** FINISHED\n");

    int dirs = numDirs(), files = numFiles();
    int total = dirs+files;

    slotUpdateInformation(files, dirs);

    if(lasturl.equals(url(), true))
        return;
    else
    {
        lasturl = url();
        usenew = true;
    }

    // clear QLabels in statusbar, if current directory has
    // no files/directories
    if(!total)
    {
        lasturl = KURL();
        usenew = false;
        KSquirrel::app()->sbarWidget("fileIcon")->clear();
        KSquirrel::app()->sbarWidget("fileName")->clear();
        return;
    }

    // setting current file directly doesn't work. let
    // events to be processed and only then set current file
    QTimer::singleShot(1, this, SLOT(slotDelayedFinishedLoading()));
}

void SQ_DirOperator::slotDelayedFinishedLoading()
{
    KURL up_url = url().upURL();

    KFileItem *first = fileview->firstFileItem();

    if(!first) return;

    // ignore ".." item
    if(up_url.equals(first->url(), true))
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

//    printf("START_OR_NOT\n");
    startOrNotThumbnailUpdate();
}

void SQ_DirOperator::slotUpdateInformation(int files, int dirs)
{
    int total = dirs + files;

    QString str = QString(i18n(" Total %1 (%2, %3) "))
                    .arg(total)
                    .arg(i18n("1 folder", "%n folders", dirs))
                    .arg(i18n("1 file", "%n files", files));

    KSquirrel::app()->sbarWidget("dirInfo")->setText(str);
}

/*
 *  Deselect all items, set current item, select this item,
 *  and ensure it visible.
 */
void SQ_DirOperator::setCurrentItem(KFileItem *item)
{
    // for safety...
    if(!item)
        return;

    // clear selection
    fileview->clearSelection();

    // set current item and select it
    fileview->setCurrentItem(item);
    fileview->setSelected(item, true);

    // make this item visible
    fileview->ensureItemVisible(item);
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

    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

    SQ_ThumbnailSize::instance()->setPixelSize(size);

    tv->rebuildCachedPixmaps();

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

        int newgrid = SQ_ThumbnailSize::instance()->extended() ?
                        SQ_ThumbnailSize::instance()->extendedSize().width()
                        : SQ_ThumbnailSize::instance()->pixelSize();

        newgrid += (SQ_Config::instance()->readNumEntry("margin", 2) + 2);

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

void SQ_DirOperator::startOrNotThumbnailUpdate()
{
    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv)
            tv->startThumbnailUpdate();
    }
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

void SQ_DirOperator::clearListers()
{
    SQ_Listers::iterator itEnd = listers.end();

    // delete all listers
    for(SQ_Listers::iterator it = listers.begin();it != itEnd;++it)
        delete it.data();

    listers.clear();
}

void SQ_DirOperator::urlAdded(const KURL &_url)
{
    // avoid duplications
    if(listers.contains(_url) || _url.equals(url(), true))
        return;

    KDirLister *lister = new KDirLister(true);

    lister->setMainWindow(dirLister()->mainWindow());

    // redirect all signals to dirLister() - nice hack
    connect(lister, SIGNAL(refreshItems(const KFileItemList &)), dirLister(), SIGNAL(refreshItems(const KFileItemList &)));
    connect(lister, SIGNAL(deleteItem(KFileItem *)), dirLister(), SIGNAL(deleteItem(KFileItem *)));
    connect(lister, SIGNAL(newItems(const KFileItemList &)), dirLister(), SIGNAL(newItems(const KFileItemList &)));

    connect(lister, SIGNAL(started(const KURL &)), dirLister(), SIGNAL(started(const KURL &)));
    connect(lister, SIGNAL(completed()), dirLister(), SIGNAL(completed()));
    connect(lister, SIGNAL(canceled()), dirLister(), SIGNAL(canceled()));

    // save this lister for us
    listers[_url] = lister;

    lister->openURL(_url);
}

void SQ_DirOperator::urlRemoved(const KURL &url)
{
    // for safety
    SQ_Listers::iterator it = listers.find(url);

    if(it == listers.end())
        return;

    KDirLister *lister = it.data();

    KFileItemList list = lister->items();

    for(KFileItem *it = list.first();it;it = list.next())
        itemDeleted(it);

    // remove this lister from map
    listers.remove(url);

    delete lister;

    // update view
    KFileIconView *vi = dynamic_cast<KFileIconView *>(view());

    if(vi)
        vi->arrangeItemsInGrid();
}

void SQ_DirOperator::execute(KFileItem *fi)
{
    QString fullpath = fi->url().path();
    QFileInfo fm(fullpath);

    QString tobeDecoded = fi->url().path();

    // Ok, this file is image file. Let's try to load it
    if(SQ_LibraryHandler::instance()->libraryForFile(tobeDecoded))
        SQ_GLWidget::window()->startDecoding(tobeDecoded);
    else
    {
        SQ_GLView::window()->sbarWidget("SBDecoded")->setText(i18n("Unsupported format \"%1\"").arg(fm.extension(false)));

        SQ_Config::instance()->setGroup("Fileview");

        // archive ?
        if(SQ_ArchiveHandler::instance()->findProtocolByFile(fi) != -1)
            if(SQ_Config::instance()->readBoolEntry("archives", true))
                emit tryUnpack(fi);
            else;
        // not image, not archive. Read settings and run this file separately
        // with default application (if needed).
        else if(SQ_Config::instance()->readBoolEntry("run unknown", false))
            emit runSeparately(fi);
    }
}

void SQ_DirOperator::highlight(KFileItem *fi)
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
}

void SQ_DirOperator::disableSpecificActions(KFileIconView *v)
{
    KAction *a;

    a = v->actionCollection()->action("zoomIn");
    if(a) a->setEnabled(false);

    a = v->actionCollection()->action("zoomOut");
    if(a) a->setEnabled(false);

    a = v->actionCollection()->action("show previews");
    if(a) a->setEnabled(false);
}

void SQ_DirOperator::slotPreview()
{
    KFileItem *fi = view() ? view()->currentFileItem() : 0;

    if(fi) SQ_PreviewWidget::instance()->load(fi->url());
}

void SQ_DirOperator::slotNewItems(const KFileItemList &list)
{
    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs && usenew)
    {
//        printf("*** APPEND\n");
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv)
            tv->appendItems(list);
    }
}

void SQ_DirOperator::slotRefreshItems(const KFileItemList &list)
{
//    printf("*** REFRESH 1\n");

    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs)
    {
//        printf("*** REFRESH 2\n");
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv)
        {
            KFileItemListIterator it(list);
            bool b = tv->updateRunning();

            for(; it.current();++it)
            {
                if(b)
                    tv->itemRemoved(it.current());

                 SQ_PixmapCache::instance()->removeEntryFull(it.current()->url().path());
            }

//            printf("*** REFRESH_APPEND\n");
            tv->appendItems(list);
        }
    }
}

/*
 *  Invoked, when some item has been deleted. We should
 *  remove appropriate thumbnail from pixmap cache.
 */
void SQ_DirOperator::slotItemDeleted(KFileItem *item)
{
    if(!item) return;

    kdDebug() << "Deleting " << item->url().path() << endl;

    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv && tv->updateRunning())
            tv->itemRemoved(item);
    }

    // remove thumbnail from cache,
    SQ_PixmapCache::instance()->remove(item->url().path());
}

void SQ_DirOperator::stopPreview()
{
    timer_preview->stop();
}

void SQ_DirOperator::stopThumbnailUpdate()
{
    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv && tv->updateRunning())
            tv->stopThumbnailUpdate();
    }
}

void SQ_DirOperator::startThumbnailUpdate()
{
    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv && !tv->updateRunning())
            tv->startThumbnailUpdate();
    }
}

#include "sq_diroperator.moc"
