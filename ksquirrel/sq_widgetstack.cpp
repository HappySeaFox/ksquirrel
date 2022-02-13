/***************************************************************************
                          sq_widgetstack.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
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
#include <qapplication.h>

#include <kaction.h>
#include <kcombobox.h>
#include <kstandarddirs.h>
#include <kstdaccel.h>
#include <klocale.h>
#include <kio/job.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kstringhandler.h>

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
#include "sq_libraryhandler.h"
#include "sq_diroperator.h"
#include "sq_bookmarkowner.h"
#include "sq_thumbnailsize.h"
#include "sq_progressbox.h"
#include "sq_thumbnailloadjob.h"
#include "sq_pixmapcache.h"
#include "sq_selectdeselectgroup.h"
#include "sq_navigatordropmenu.h"

SQ_WidgetStack * SQ_WidgetStack::m_instance = 0;

SQ_WidgetStack::SQ_WidgetStack(QWidget *parent, const int id) : QObject(parent)
{
    m_instance = this;

    SQ_DirOperator::ViewT m_type = static_cast<SQ_DirOperator::ViewT>(id);

    KURL url;

    // Check Options for path
    SQ_Config::instance()->setGroup("Fileview");

    if(SQ_HLOptions::instance()->dir.isEmpty())
    {
        switch(SQ_Config::instance()->readNumEntry("set path", 0))
        {
            case 0: url = KURL::fromPathOrURL(SQ_Config::instance()->readEntry("last visited", "/")); break;
            case 1: url = KURL::fromPathOrURL(QDir::current().path()); break;
            case 2: url = KURL::fromPathOrURL(SQ_Config::instance()->readEntry("custom directory", "/")); break;

            default: url = KURL::fromPathOrURL("/");
        }
    }
    else // path from command line
        url = SQ_HLOptions::instance()->dir;

    dirop = new SQ_DirOperator(url, static_cast<SQ_DirOperator::ViewT>(id), parent);

    raiseWidget(m_type, false);

    connect(KSquirrel::app(), SIGNAL(thumbSizeChanged(const QString&)), dirop, SLOT(slotSetThumbSize(const QString&)));
    connect(dirop, SIGNAL(runSeparately(KFileItem *)), this, SLOT(slotRunSeparately()));

    KSquirrel::app()->historyCombo()->setEditText(url.isLocalFile() ? url.path() : url.prettyURL());

    timerShowProgress = new QTimer(this);
    connect(timerShowProgress, SIGNAL(timeout()), this, SLOT(slotDelayedShowProgress()));
}

SQ_WidgetStack::~SQ_WidgetStack()
{}

/*
 *  Get current url of visible diroperator.
 */
KURL SQ_WidgetStack::url() const
{
    return dirop->url();
}

void SQ_WidgetStack::init()
{
    SQ_Config::instance()->setGroup("Fileview");

    fileaction = static_cast<SQ_WidgetStack::FileAction>(SQ_Config::instance()->readNumEntry("last_action", SQ_WidgetStack::Unknown));
    KURL uu = KURL::fromPathOrURL(SQ_Config::instance()->readEntry("last_url"));

    connect(SQ_NavigatorDropMenu::instance(), SIGNAL(done(const KURL &, int)),
            this, SLOT(slotTreeMenuDone(const KURL &, int)));

    emitNewLastURL(uu);
}

void SQ_WidgetStack::slotTreeMenuDone(const KURL &url, int a)
{
    if(a == SQ_NavigatorDropMenu::Copy)
        fileaction = SQ_WidgetStack::Copy;
    else if(a == SQ_NavigatorDropMenu::Move)
        fileaction = SQ_WidgetStack::Cut;
    else if(a == SQ_NavigatorDropMenu::Link)
        fileaction = SQ_WidgetStack::Link;

    lastURL = url;

    emitNewLastURL(lastURL);
}

void SQ_WidgetStack::setURL(const KURL &newurl, bool parseTree)
{
    KURL url = newurl;
    url.adjustPath(+1);

    // update bookmarks' url
    if(SQ_BookmarkOwner::instance())
        SQ_BookmarkOwner::instance()->setURL(url);

    // update history combobox
    KSquirrel::app()->historyCombo()->setEditText(url.isLocalFile() ? url.path() : url.prettyURL());
    KSquirrel::app()->historyCombo()->addToHistory(url.isLocalFile() ? url.path() : url.prettyURL());

    // set url for file tree
    if(SQ_TreeView::instance() && parseTree)
    {
        SQ_Config::instance()->setGroup("Fileview");
        int sync_type = SQ_Config::instance()->readNumEntry("sync type", 0);

        if(sync_type != 2)
            SQ_TreeView::instance()->emitNewURL(url);
    }
}

/*
 *  Select next/prevous supported image in filemanager.
 *  Do nothing, if no more supported images found in given
 *  direction.
 */
int SQ_WidgetStack::moveTo(Direction direction, KFileItem *it, bool useSupported)
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

    if(useSupported)
    {
        while(true)
        {
            if(item->isFile())
                // supported image type ?
//                if(SQ_LibraryHandler::instance()->libraryForFile(item->url().path()))
                if(SQ_LibraryHandler::instance()->maybeSupported(item->url()) != SQ_LibraryHandler::No)
                    break;

            item = (direction == SQ_WidgetStack::Next)?
                    (local->nextItem(item)):
                    (local->prevItem(item));

            if(!item)
                return moveFailed;
        }
    }

    // set current file, select it
    dirop->setCurrentItem(item);

    return moveSuccess;
}

/*
 *  Set filter.
 */
void SQ_WidgetStack::setNameFilter(const QString &f)
{
    dirop->setNameFilter(f);
    dirop->actionCollection()->action("reload")->activate();
}

/*
 *  Get current filter.
 */
QString SQ_WidgetStack::nameFilter() const
{
    return dirop->nameFilter();
}

/*
 *  Change view type. See SQ_DirOperator::ViewT for more.
 */
void SQ_WidgetStack::raiseWidget(SQ_DirOperator::ViewT id, bool doUpdate)
{
    if(doUpdate) dirop->stopThumbnailUpdate();

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

            if(id == SQ_DirOperator::TypeList)
                iv->actionCollection()->action("small columns")->activate();
            else
                iv->actionCollection()->action("large rows")->activate();
        }
        break;

        case SQ_DirOperator::TypeDetailed:
        {
            SQ_FileDetailView *dv = dynamic_cast<SQ_FileDetailView *>(dirop->preparedView());

            dirop->setPreparedView();
            dv->setSelectionMode(KFile::Extended);
        }
        break;

        case SQ_DirOperator::TypeThumbs:
        {
            SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->preparedView());
            tv->actionCollection()->action("large rows")->activate();
            dirop->setPreparedView();
            action("short view")->activate();
            tv->setSelectionMode(KFile::Extended);
            updateGrid(true);
            if(doUpdate) dirop->startOrNotThumbnailUpdate();
        }
        break;
    }

    // enable/disable menu with thumbnail sizes
    KSquirrel::app()->enableThumbsMenu(id == SQ_DirOperator::TypeThumbs);
    dirop->enableThumbnailActions(id == SQ_DirOperator::TypeThumbs);
    dirop->selectOld();
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
    {
        KFileItem *fi = dirop->view()->currentFileItem();
        SQ_GLWidget::window()->setExpectedURL(fi->url());
        dirop->execute(fi);
    }
}

void SQ_WidgetStack::emitPreviousSelected()
{
    if(moveTo(SQ_WidgetStack::Previous) == SQ_WidgetStack::moveSuccess)
    {
        KFileItem *fi = dirop->view()->currentFileItem();
        SQ_GLWidget::window()->setExpectedURL(fi->url());
        dirop->execute(fi);
    }
}

void SQ_WidgetStack::slotSelectFirstFile()
{
    moveToFirstLast(SQ_WidgetStack::Next, false);
}

void SQ_WidgetStack::slotSelectLastFile()
{
    moveToFirstLast(SQ_WidgetStack::Previous, false);
}

// Go to first file
void SQ_WidgetStack::firstFile()
{
    moveToFirstLast(SQ_WidgetStack::Next);
}

// Go to last file
void SQ_WidgetStack::lastFile()
{
    moveToFirstLast(SQ_WidgetStack::Previous);
}

void SQ_WidgetStack::moveToFirstLast(Direction direct, bool exec)
{
    KFileView *local_view = dirop->view();
    KFileItemList *list = const_cast<KFileItemList*>(local_view->items());

    if(list)
    {
        KFileItem *item = (direct == SQ_WidgetStack::Next) ? list->first() : list->last();

        if(!item || moveTo(direct, item) == moveFailed)
            return;

        item = local_view->currentFileItem();

        if(item)
        {
            dirop->setCurrentItem(item);
            if(exec) dirop->execute(item);
        }
    }
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
    int newgrid = SQ_ThumbnailSize::instance()->extended() ?
                    SQ_ThumbnailSize::instance()->extendedSize().width()
                    : SQ_ThumbnailSize::instance()->pixelSize();

    newgrid += (SQ_Config::instance()->readNumEntry("margin", 2) + 2);

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
    if(dirop->viewType() != SQ_DirOperator::TypeThumbs)
        return;

    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(!tv) return;

    timerShowProgress->stop();
    tv->progressBox()->hide();
    tv->progressBox()->flush();
    tv->progressBox()->startButtonPixmap();
}

void SQ_WidgetStack::thumbnailUpdateStart(int count)
{
    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(!tv) return;

    tv->progressBox()->setTotalSteps(count);
    tv->progressBox()->stopButtonPixmap();

    timerShowProgress->start(1000, true);
}

void SQ_WidgetStack::slotDelayedShowProgress()
{
    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(tv)
        tv->progressBox()->show();
}

void SQ_WidgetStack::thumbnailProcess()
{
    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(tv)
        tv->progressBox()->advance();
}

void SQ_WidgetStack::setURLForCurrent(const QString &path, bool parseTree)
{
    KURL url = KURL::fromPathOrURL(path);

    setURLForCurrent(url, parseTree);
}

void SQ_WidgetStack::setURLForCurrent(const KURL &url, bool parseTree)
{
    setURL(url, parseTree);
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

    while((item = it.current()))
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

    tv->progressBox()->show();

    QTimer::singleShot(1, this, SLOT(slotDelayedRecreateThumbnail()));
}

void SQ_WidgetStack::slotDelayedRecreateThumbnail()
{
    SQ_Thumbnail thumb;

    KFileItemList *list = const_cast<KFileItemList *>(selectedItems());
    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(!list || !tv)
        return;

    KFileItemListIterator it(*list);
    KFileItem *item;

    while((item = it.current()))
    {
        SQ_PixmapCache::instance()->removeEntryFull(item->url());
        ++it;
    }

    tv->doStartThumbnailUpdate(*list);
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
        KMessageBox::information(dirop, i18n("No files to copy or move"));
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
        KMessageBox::information(dirop, i18n("No files to copy or move"));
        return;
    }

    // get current url
    KURL _url = url();
    emitNewLastURL(_url);
    KIO::Job *job;

    // now copy or move files to current url
    job = (fileaction == SQ_WidgetStack::Copy) ? KIO::copy(files, _url) : KIO::move(files, _url);
    connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotJobResult(KIO::Job *)));
}

void SQ_WidgetStack::slotJobResult(KIO::Job *job)
{
    if(job && job->error())
        job->showErrorDialog(KSquirrel::app());
}

void SQ_WidgetStack::emitNewLastURL(const KURL &u)
{
    lastURL = u;
    QString text;

    QString targ = KStringHandler::csqueeze(u.isLocalFile() ? u.path() : u.prettyURL(), 25);

    if(fileaction == SQ_WidgetStack::Copy)
        text = i18n("Repeat (copy to %1)").arg(targ);
    else if(fileaction == SQ_WidgetStack::Cut)
        text = i18n("Repeat (move to %1)").arg(targ);
    else if(fileaction == SQ_WidgetStack::Link)
        text = i18n("Repeat (link to %1)").arg(targ);

    if(!text.isEmpty())
        emit newLastURL(text);
}

void SQ_WidgetStack::repeat()
{
    if(fileaction == SQ_WidgetStack::Copy || fileaction == SQ_WidgetStack::Cut)
        prepare();
    else
        return;

    // No files to copy ?
    if(lastURL.isEmpty() || files.isEmpty())
        return;

    KIO::Job *job = (fileaction == SQ_WidgetStack::Copy) ? KIO::copy(files, lastURL) : KIO::move(files, lastURL);
    connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotJobResult(KIO::Job *)));
}

// Create links
void SQ_WidgetStack::slotFileLinkTo()
{
    if(!prepare())
        return;

    // select a directory
    KURL url = KFileDialog::getExistingURL(lastURL.url(), dirop);

    if(url.isEmpty())
        return;

    fileaction = SQ_WidgetStack::Link;
    emitNewLastURL(url);

    // create symlinks
    KIO::link(files, url);
}

// Copy to...
void SQ_WidgetStack::slotFileCopyTo()
{
    if(!prepare())
        return;

    // select a directory
    KURL url = KFileDialog::getExistingURL(lastURL.url(), dirop);

    if(url.isEmpty())
        return;

    fileaction = SQ_WidgetStack::Copy;
    emitNewLastURL(url);

    // copy files to selected directory
    KIO::copy(files, url);
}

// Move to...
void SQ_WidgetStack::slotFileMoveTo()
{
    if(!prepare())
        return;

    // select a directory
    KURL url = KFileDialog::getExistingURL(lastURL.url(), dirop);

    if(url.isEmpty())
        return;

    fileaction = SQ_WidgetStack::Cut;
    emitNewLastURL(url);

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

bool SQ_WidgetStack::updateRunning() const
{
    if(dirop->viewType() != SQ_DirOperator::TypeThumbs)
        return false;

    SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(dirop->view());

    if(!tv)
        return false;

    return tv->updateRunning();
}

void SQ_WidgetStack::saveState()
{
    SQ_Config::instance()->writeEntry("last visited", dirop->url().prettyURL());
    SQ_Config::instance()->writeEntry("last_action", static_cast<int>(fileaction));
    SQ_Config::instance()->writeEntry("last_url", lastURL.prettyURL());

    dirop->saveConfig();
}

#include "sq_widgetstack.moc"
