/***************************************************************************
                          sq_widgetstack.cpp  -  description
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

#include <qlabel.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qtimer.h>
#include <qfileinfo.h>
#include <qapplication.h>

#include <kaction.h>
#include <kcombobox.h>
#include <kstandarddirs.h>
#include <kstdaccel.h>
#include <klocale.h>
#include <kio/job.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_externaltool.h"
#include "sq_treeview.h"
#include "sq_hloptions.h"
#include "sq_archivehandler.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_quickbrowser.h"
#include "sq_libraryhandler.h"
#include "sq_diroperator.h"
#include "sq_bookmarkowner.h"
#include "sq_thumbnailsize.h"
#include "sq_progress.h"
#include "sq_thumbnailjob.h"
#include "sq_pixmapcache.h"
#include "sq_selectdeselectgroup.h"

SQ_WidgetStack * SQ_WidgetStack::m_instance = NULL;

SQ_WidgetStack::SQ_WidgetStack(QWidget *parent, const int id) : QVBox(parent)
{
    m_instance = this;

    kdDebug() << "+SQ_WidgetStack" << endl;

    SQ_DirOperatorBase::ViewT m_type = static_cast<SQ_DirOperatorBase::ViewT>(id);

    QString path;

    // Check Options for path
    SQ_Config::instance()->setGroup("Fileview");

    if(SQ_HLOptions::instance()->path.isEmpty())
    {
        switch(SQ_Config::instance()->readNumEntry("set path", 0))
        {
            case 2: path = SQ_Config::instance()->readEntry("custom directory", "/"); break;
            case 1: path = ""; break;
            case 0: path = SQ_Config::instance()->readEntry("last visited", "/"); break;

            default: path = "/";
        }
    }
    else // path from command line
    {
        QFileInfo fm(SQ_HLOptions::instance()->path);
        path = fm.isDir() ? SQ_HLOptions::instance()->path : fm.dirPath(true);
    }

    KURL _url = path;

    dirop = new SQ_DirOperator(_url, static_cast<SQ_DirOperatorBase::ViewT>(id), this);

    Q_ASSERT(dirop);

    raiseWidget(m_type);

    connect(KSquirrel::app(), SIGNAL(thumbSizeChanged(const QString&)), dirop, SLOT(slotSetThumbSize(const QString&)));
    connect(dirop, SIGNAL(tryUnpack(KFileItem *)), this, SLOT(tryUnpack(KFileItem *)));

    KSquirrel::app()->historyCombo()->setEditURL(dirop->url());

    timerShowProgress = new QTimer(this);
    connect(timerShowProgress, SIGNAL(timeout()), this, SLOT(slotDelayedShowProgress()));
}

SQ_WidgetStack::~SQ_WidgetStack()
{
    kdDebug() << "-SQ_WidgetStack" << endl;
}

/*
 *  Get current url of visible diroperator.
 */
KURL SQ_WidgetStack::url() const
{
    return dirop->url();
}

/*
 *  Set current url.
 */
void SQ_WidgetStack::setURL(const QString &newpath, bool cl, bool parseTree)
{
    KURL url = newpath;

    setURL(url, cl, parseTree);
}

void SQ_WidgetStack::setURL(const KURL &newurl, bool cl, bool parseTree)
{
    KURL url = newurl;
    url.adjustPath(1);

    // update bookmarks' url
    if(SQ_BookmarkOwner::instance()) SQ_BookmarkOwner::instance()->setURL(url);

    // update history combobox
    if(KSquirrel::app()->historyCombo())
    {
        KSquirrel::app()->historyCombo()->addToHistory(url.prettyURL());
        KSquirrel::app()->historyCombo()->setEditURL(url);
    }

    // synchronize with SQ_QuickBrowser
    if(SQ_GLWidget::window())
    {
        SQ_QuickBrowser::quickOperator()->blockSignals(true);
        SQ_QuickBrowser::quickOperator()->setURL(url, cl);
        SQ_QuickBrowser::quickOperator()->blockSignals(false);
    }

    // set url for file tree
    if(SQ_TreeView::instance() && parseTree)
    {
        SQ_Config::instance()->setGroup("Fileview");
        int sync_type = SQ_Config::instance()->readNumEntry("sync type", 0);

        if(sync_type == 2 || sync_type == 0)
            SQ_TreeView::instance()->emitNewURL(url);
    }
}

/*
 *  User selected some url in file tree. Let's set it for
 *  current diroperator and other widgets (with setURL()).
 */
void SQ_WidgetStack::setURLfromtree(const KURL &newurl)
{
    bool cl = true;

    setURL(newurl, cl, false);

    dirop->setURL(newurl, cl);
}

/*
 *  Select next/prevous supported image in filemanager.
 *  Do nothing, if no more supported images found in given
 *  direction.
 */
int SQ_WidgetStack::moveTo(Direction direction, KFileItem *it)
{
    KFileView *local;
    KFileItem *item;

    // current fileview
    local = dirop->view();

    if(it)
        item = it;
    else
        item = (direction == SQ_WidgetStack::Next)?
                    local->nextItem(local->currentFileItem()):
                    local->prevItem(local->currentFileItem());

    if(!item)
        return moveFailed;

    while(true)
    {
        if(item->isFile())
            // supported image type ?
            if(SQ_LibraryHandler::instance()->supports(item->url().path()))
                break;

        item = (direction == SQ_WidgetStack::Next)?
                (local->nextItem(item)):
                (local->prevItem(item));

        if(!item)
            return moveFailed;
    }

    // set current file, select it
    selectFile(item);

    return moveSuccess;
}

/*
 *  Set filter.
 */
void SQ_WidgetStack::setNameFilter(const QString &f)
{
    dirop->setNameFilter(f);
    if(SQ_GLWidget::window()) SQ_QuickBrowser::quickOperator()->setNameFilter(f);

    dirop->actionCollection()->action("reload")->activate();
    if(SQ_GLWidget::window()) SQ_QuickBrowser::quickOperator()->actionCollection()->action("reload")->activate();
}

/*
 *  Get current filter.
 */
QString SQ_WidgetStack::nameFilter() const
{
    return dirop->nameFilter();
}

/*
 *  Change view type. See SQ_DirOperatorBase::ViewT for more.
 */
void SQ_WidgetStack::raiseWidget(SQ_DirOperatorBase::ViewT id)
{
    dirop->removeCdUpItem();
    dirop->prepareView(id);

    switch(id)
    {
        case SQ_DirOperator::TypeList:
        case SQ_DirOperator::TypeIcons:
        {
            SQ_FileIconView *iv = dynamic_cast<SQ_FileIconView *>(dirop->preparedView());

            dirop->setPreparedView();
            iv->setSelectionMode(KFile::Extended);
            action("short view")->activate();

            if(id == SQ_DirOperator::TypeList)iv->actionCollection()->action("small columns")->activate();
            else iv->actionCollection()->action("large rows")->activate();
        }
        break;

        case SQ_DirOperator::TypeDetailed:
        {
            SQ_FileDetailView *dv = dynamic_cast<SQ_FileDetailView *>(dirop->preparedView());

            action("detailed view")->activate();
            dv->setSelectionMode(KFile::Extended);
        }
        break;

        case SQ_DirOperator::TypeThumbs:
        {
            SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->preparedView());
            dirop->setPreparedView();
            action("short view")->activate();
            tv->actionCollection()->action("large rows")->activate();
            tv->setSelectionMode(KFile::Extended);
            updateGrid(true);
            dirop->actionCollection()->action("reload")->activate();//startOrNotThumbnailUpdate();
        }
        break;
    }

    // enable/disable menu with thumbnail sizes
    KSquirrel::app()->enableThumbsMenu(id == SQ_DirOperator::TypeThumbs);
}

/*
 *  Selected items in currently visible diroperator.
 */
const KFileItemList* SQ_WidgetStack::selectedItems() const
{
    return dirop->view()->selectedItems();
}

/*
 *  All items in currently visible diroperator.
 */
const KFileItemList* SQ_WidgetStack::items() const
{
    return dirop->view()->items();
}

void SQ_WidgetStack::emitNextSelected()
{
    if(moveTo(SQ_WidgetStack::Next) == SQ_WidgetStack::moveSuccess)
        SQ_GLWidget::window()->slotStartDecoding(dirop->view()->currentFileItem()->url());
}

void SQ_WidgetStack::emitPreviousSelected()
{
    if(moveTo(SQ_WidgetStack::Previous) == SQ_WidgetStack::moveSuccess)
        SQ_GLWidget::window()->slotStartDecoding(dirop->view()->currentFileItem()->url());
}

void SQ_WidgetStack::tryUnpack(KFileItem *item)
{
    // is archive type supported ?
    if(SQ_ArchiveHandler::instance()->findProtocolByFile(item) != -1)
    {
        SQ_ArchiveHandler::instance()->setFile(item);

        // unpack!
        if(SQ_ArchiveHandler::instance()->unpack())
        {
            QTimer::singleShot(0, this, SLOT(slotDelayedSetExtractURL()));
        }
    }
}

/*
 *  Go to unpacked archive.
 */
void SQ_WidgetStack::slotDelayedSetExtractURL()
{
    KURL _url;
    _url.setPath(SQ_ArchiveHandler::instance()->itemExtractedPath());

    dirop->setURL(_url, true);
}

/*
 *  Reconfigure clicking policy.
 */
void SQ_WidgetStack::configureClickPolicy()
{
    dirop->reconnectClick();

    SQ_QuickBrowser::quickOperator()->reconnectClick();
}

/*
 *  Set current item to 'item', select it, and synchronize with
 *  SQ_QuickBrowser.
 */
void SQ_WidgetStack::selectFile(KFileItem *item, SQ_DirOperatorBase *workAround)
{
    if(!item) return;

    SQ_DirOperatorBase *local = dynamic_cast<SQ_DirOperatorBase*>(dirop);

    // let diroperator set current item
    if(local != workAround) local->setCurrentItem(item);

    // also set current item for Quick Browser
    if(SQ_QuickBrowser::quickOperator() != workAround)
    {
        SQ_FileIconView *iv = dynamic_cast<SQ_FileIconView *>(SQ_QuickBrowser::quickOperator()->view());
        iv->blockSignals(true);
        SQ_QuickBrowser::quickOperator()->setCurrentItem(item);
        iv->blockSignals(false);
    }
}

// Go to first file
void SQ_WidgetStack::slotFirstFile()
{
    KFileView *local_view = dirop->view();
    KFileItemList *list = const_cast<KFileItemList*>(local_view->items());
    KFileItem *item = list->first();

    if(!item)
        return;

    if(moveTo(SQ_WidgetStack::Next, item) == moveFailed)
        return;

    item = local_view->currentFileItem();
    dirop->setCurrentItem(item);
    SQ_GLWidget::window()->slotStartDecoding(item->url());
}

// Go to last file
void SQ_WidgetStack::slotLastFile()
{
    KFileView *local_view = dirop->view();
    KFileItemList *list = const_cast<KFileItemList*>(local_view->items());
    KFileItem *item = list->last();

    if(!item)
        return;

    if(moveTo(SQ_WidgetStack::Previous, item) == moveFailed)
        return;

    item = local_view->currentFileItem();
    dirop->setCurrentItem(item);
    SQ_GLWidget::window()->slotStartDecoding(item->url());
}

/*
 *  Update grid for thumbnail view. New grid is calcalated from item
 *  margins (from Options).
 */
void SQ_WidgetStack::updateGrid(bool arrange)
{
    if(dirop->viewType() != SQ_DirOperator::TypeThumbs)
        return;

    SQ_Config::instance()->setGroup("Thumbnails");
    int newgrid = SQ_ThumbnailSize::instance()->currentPixelSize() 
                    + SQ_Config::instance()->readNumEntry("margin", 2) + 2;

    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    // different grid ?
    if(tv->gridX() != newgrid)
    {
        tv->setGridX(newgrid);

        if(arrange)
            tv->arrangeItemsInGrid();
    }
}

void SQ_WidgetStack::thumbnailsUpdateEnded()
{
    static QPixmap play_pixmap = QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumb_resume.png"));

    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(!tv) return;

    timerShowProgress->stop();
    tv->progressBox->hide();
    tv->progress->flush();
    tv->buttonStop->setPixmap(play_pixmap);
}

void SQ_WidgetStack::thumbnailUpdateStart(int count)
{
    static QPixmap stop_pixmap = QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumb_stop.png"));

    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(!tv) return;

    tv->progress->setTotalSteps(count);
    tv->buttonStop->setPixmap(stop_pixmap);

    timerShowProgress->start(1000, true);
}

void SQ_WidgetStack::slotDelayedShowProgress()
{
    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(!tv) return;

    tv->progressBox->show();
}

void SQ_WidgetStack::thumbnailProcess()
{
    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(!tv) return;

    tv->progress->advance(1);
}

void SQ_WidgetStack::setURLForCurrent(const QString &path)
{
    KURL url = path;

    setURLForCurrent(url);
}

void SQ_WidgetStack::setURLForCurrent(const KURL &url)
{
    setURL(url, true);

    dirop->setURL(url, true);
}

void SQ_WidgetStack::updateView()
{
    dirop->actionCollection()->action("reload")->activate();
}

void SQ_WidgetStack::slotRunSeparately()
{
    const KFileItemList *selected = selectedItems();
    KFileItemListIterator it(*selected);

    KFileItem *item;

    while((item = it.current()) != 0)
    {
        if(item)
            item->run();

        ++it;
    }
}

// recreate selected thumbnails
void SQ_WidgetStack::slotRecreateThumbnail()
{
    // oops, not a thumbnail view
    if(dirop->viewType() != SQ_DirOperator::TypeThumbs)
        return;

    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(tv->updateRunning())
        return;

    tv->progressBox->show();

    QTimer::singleShot(30, this, SLOT(slotDelayedRecreateThumbnail()));

    qApp->processEvents();
}

void SQ_WidgetStack::slotDelayedRecreateThumbnail()
{
    SQ_Thumbnail thumb;

    KFileItemList *list = const_cast<KFileItemList *>(selectedItems());

    if(!list)
        return;

    KFileItem *item;

    thumbnailUpdateStart(list->count());

    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    qApp->processEvents();

    while((item = list->take()) != 0)
    {
        if(!item)
            continue;

        QString path = item->url().path();

        if(!SQ_ThumbnailLoadJob::loadThumbnail(path, thumb, true))
            continue;

        int biggestDimension = QMAX(thumb.thumbnail.width(), thumb.thumbnail.height());
        int thumbPixelSize = SQ_ThumbnailSize::instance()->pixelSize() - 2;

        if(biggestDimension > thumbPixelSize)
        {
            double scale = double(thumbPixelSize) / double(biggestDimension);
            thumb.thumbnail = thumb.thumbnail.smoothScale(int(thumb.thumbnail.width() * scale),
                int(thumb.thumbnail.height() * scale));
        }

        SQ_PixmapCache::instance()->removeEntryFull(path);

        SQ_PixmapCache::instance()->insert(path, thumb);

        thumbnailProcess();

        tv->setThumbnailPixmap(item, thumb);
    }

    thumbnailsUpdateEnded();
}

/*
 *  Save currently selected items' paths, if any.
 *
 *  Return true, if at least one url was saved.
 */
bool SQ_WidgetStack::prepare()
{
    // clear old files
    files.clear();

    KFileItemList *items = const_cast<KFileItemList *>(selectedItems());
    KFileItem *i;

    if(!items || items->isEmpty())
    {
        KMessageBox::information(this, i18n("No files to copy or move"));
        return false;
    }

    i = items->first();

    while(i)
    {
        files.append(i->url());
        i = items->next();
    }

    return true;
}

/*
 *  Slots for file actions: copy, move, cut...
 */
void SQ_WidgetStack::slotFileCopy()
{
    fileaction = (prepare()) ? SQ_WidgetStack::Copy : SQ_WidgetStack::Unknown;
}

void SQ_WidgetStack::slotFileCut()
{
    fileaction = (prepare()) ? SQ_WidgetStack::Cut : SQ_WidgetStack::Unknown;
}

// Paste files
void SQ_WidgetStack::slotFilePaste()
{
    // No files to copy ?
    if(files.isEmpty() || fileaction == SQ_WidgetStack::Unknown)
    {
        KMessageBox::information(this, i18n("No files to copy or move"));
        return;
    }

    // get current url
    KURL _url = url();

    // now copy or move files to current url
    if(fileaction == SQ_WidgetStack::Copy)
    {
        KIO::copy(files, _url);
    }
    else if(fileaction == SQ_WidgetStack::Cut)
    {
        KIO::move(files, _url);
    }
}

// Create links
void SQ_WidgetStack::slotFileLinkTo()
{
    if(!prepare())
        return;

    // select a directory
    QString s = KFileDialog::getExistingDirectory(QString::null, this);

    if(s.isEmpty())
        return;

    KURL url = s;

    // create symlinks
    KIO::link(files, url);
}

// Copy to...
void SQ_WidgetStack::slotFileCopyTo()
{
    if(!prepare())
        return;

    // select a directory
    QString s = KFileDialog::getExistingDirectory(QString::null, this);

    if(s.isEmpty())
        return;

    KURL url = s;

    // copy files to selected directory
    KIO::copy(files, url);
}

// Move to...
void SQ_WidgetStack::slotFileMoveTo()
{
    if(!prepare())
        return;

    // select a directory
    QString s = KFileDialog::getExistingDirectory(QString::null, this);

    if(s.isEmpty())
        return;

    KURL url = s;

    // move files to selected directory
    KIO::move(files, url);
}

/*
 *  User wants to select to deselect some files.
 */
void SQ_WidgetStack::slotSelectGroup()
{
    selectDeselectGroup(true);
}

void SQ_WidgetStack::slotDeselectGroup()
{
    selectDeselectGroup(false);
}

/*
 *  Execute SQ_SelectDeselectGroup dialog, and select or
 *  deselect files after it has been closed.
 */
void SQ_WidgetStack::selectDeselectGroup(bool select)
{
    QString mask;

    KFileView *local = dirop->view();

    SQ_SelectDeselectGroup sd(KSquirrel::app());

    sd.setCaption(select ? i18n("Select a group of files") : i18n("Deselect a group of files"));
    sd.pushMask->setText(select ? i18n("Select !") : i18n("Deselect !"));

    if(sd.exec(mask) == QDialog::Accepted)
    {
        if(select)
            local->clearSelection();

        KFileItemList *sd_files = const_cast<KFileItemList *>(items());
        KFileItem *i;
        QRegExp exp(mask);
        exp.setWildcard(true);

        if(!sd_files)
            return;

        i = sd_files->first();

        // go through all files
        while(i)
        {
            // name matched
            if(exp.exactMatch(i->name()))
                local->setSelected(i, select);

            // next file
            i = sd_files->next();
        }
    }
}

/*
 *  Deselect all files.
 */
void SQ_WidgetStack::slotDeselectAll()
{
    dirop->view()->clearSelection();
}

/*
 *  Select all files.
 */
void SQ_WidgetStack::slotSelectAll()
{
    KFileItemList *sd_files = const_cast<KFileItemList *>(items());
    KFileView *local = dirop->view();
    KFileItem *i;

    if(!sd_files)
        return;

    i = sd_files->first();

    // go through files and select them all
    while(i)
    {
        local->setSelected(i, true);
        i = sd_files->next();
    }
}

KAction* SQ_WidgetStack::action(const QString &name)
{
    return dirop->actionCollection()->action(name);
}
