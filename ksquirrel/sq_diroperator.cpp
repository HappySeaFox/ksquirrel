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
#include <qfileinfo.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <qclipboard.h>

#include <kapplication.h>
#include <kstringhandler.h>
#include <klocale.h>
#include <kprogress.h>
#include <kglobal.h>
#include <kstdaccel.h>
#include <kdiskfreesp.h>
#include <konq_operations.h>
#include <kiconeffect.h>

#include "ksquirrel.h"
#include "sq_archivehandler.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filethumbviewitem.h"
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
#include "sq_progress.h"
#include "sq_downloader.h"
#include "sq_directorybasket.h"

static const int  SQ_MAX_WORD_LENGTH = 50;

SQ_DirOperator::SQ_DirOperator(const KURL &url, ViewT type_, QWidget *parent, const char *name)
    : KDirOperator(url, parent, name), type(type_)
{
    usenew = false;

    totalSize = static_cast<KIO::filesize_t>(0);

    // create and insert new actions in context menu
    setupActionsMy();

    connect(this, SIGNAL(finishedLoading()), this, SLOT(slotFinishedLoading()));
    connect(this, SIGNAL(updateInformation(int,int)), this, SLOT(slotUpdateInformation(int,int)));
    connect(this, SIGNAL(urlEntered(const KURL&)), this, SLOT(slotUrlEntered(const KURL&)));
    connect(this, SIGNAL(dropped(const KFileItem *, QDropEvent*, const KURL::List&)),
            this, SLOT(slotDropped(const KFileItem *, QDropEvent*, const KURL::List&)));
    connect(this, SIGNAL(fileSelected(const KFileItem *)), this, SLOT(slotExecutedConst(const KFileItem *)));

    connect(dirLister(), SIGNAL(deleteItem(KFileItem *)), this, SLOT(slotItemDeleted(KFileItem *)));
    connect(dirLister(), SIGNAL(newItems(const KFileItemList &)), this, SLOT(slotNewItems(const KFileItemList &)));
    connect(dirLister(), SIGNAL(refreshItems(const KFileItemList &)), this, SLOT(slotRefreshItems(const KFileItemList &)));

    timer_preview = new QTimer(this);
    connect(timer_preview, SIGNAL(timeout()), this, SLOT(slotPreview()));

    setAcceptDrops(true);

    fileview = 0;
    oldCurrentItem = 0;

    fireDiskSize(url);

    down = new SQ_Downloader(this, "SQ_Downloader [dirop]");

    connect(down, SIGNAL(result(const KURL &)), this, SLOT(slotDownloaderResult(const KURL &)));
    connect(SQ_ArchiveHandler::instance(), SIGNAL(unpack(const KURL &)), this, SLOT(slotSetURL(const KURL &)));

    // read sorting settings
    SQ_Config::instance()->setGroup("Fileview");
    int sorting = 0;

    if(SQ_Config::instance()->readBoolEntry("sorting_name", true))      sorting |= QDir::Name;
    if(SQ_Config::instance()->readBoolEntry("sorting_time", false))     sorting |= QDir::Time;
    if(SQ_Config::instance()->readBoolEntry("sorting_size", false))     sorting |= QDir::Size;
    if(SQ_Config::instance()->readBoolEntry("sorting_dirs", true))      sorting |= QDir::DirsFirst;
    if(SQ_Config::instance()->readBoolEntry("sorting_reverse", false))  sorting |= QDir::Reversed;
    if(SQ_Config::instance()->readBoolEntry("sorting_ignore", false))   sorting |= QDir::IgnoreCase;

    actionHidden = dynamic_cast<KToggleAction *>(actionCollection()->action("show hidden"));

    if(SQ_Config::instance()->readBoolEntry("show hidden", false) && actionHidden)
    {
        actionHidden->setChecked(false);
        actionHidden->activate();
    }

    setSorting(static_cast<QDir::SortSpec>(sorting));
    setEnableDirHighlighting(true);
}

SQ_DirOperator::~SQ_DirOperator()
{}

void SQ_DirOperator::slotUrlEntered(const KURL &url)
{
    usenew = false;

    totalSize = static_cast<KIO::filesize_t>(0);

    clearListers();

    SQ_WidgetStack::instance()->setURLForCurrent(url);

    fireDiskSize(url);
}

void SQ_DirOperator::fireDiskSize(const KURL &url)
{
    if(url.isLocalFile())
    {
        KDiskFreeSp *sp = KDiskFreeSp::findUsageInfo(url.path());

        connect(sp, SIGNAL(foundMountPoint(const unsigned long&,
                                        const unsigned long&,
                                        const unsigned long&,
                                        const QString&)),
                                        this, SLOT(slotFoundMountPoint(const unsigned long&,
                                        const unsigned long&,
                                        const unsigned long&,
                                        const QString&)));
    }
    else
    {
        KSquirrel::app()->diskProgress()->flush();
        KSquirrel::app()->diskProgress()->setTotalSteps(1);
        KSquirrel::app()->diskProgress()->setIndicator(0);
        KSquirrel::app()->diskProgress()->update();
        KSquirrel::app()->sbarWidget("diskSpace")->setText(i18n("non-local filesystem"));
        QToolTip::remove(KSquirrel::app()->sbarWidget("diskSpace"));
    }
}

void SQ_DirOperator::slotFoundMountPoint(const unsigned long &kbSize,
                    const unsigned long &kbUsed,
                    const unsigned long &kbAvail,
                    const QString &mount)
{
    // clear any previous drawing
    KSquirrel::app()->diskProgress()->flush();

    KSquirrel::app()->diskProgress()->setTotalSteps(kbSize);
    KSquirrel::app()->diskProgress()->setIndicator(kbSize-kbAvail); // kbSize != kbAvail+kbUsed
    KSquirrel::app()->sbarWidget("diskSpace")->setText(QString::fromLatin1("%1: %2/%3")
                                                        .arg(mount)
                                                        .arg(KIO::convertSizeFromKB(kbAvail))
                                                        .arg(KIO::convertSizeFromKB(kbSize)));

    QToolTip::add(KSquirrel::app()->sbarWidget("diskSpace"), i18n("<table><tr><td>Mount point:</td><td>%1</td></tr><tr><td>Total size:</td><td>%2</td></tr><tr><td>Used:</td><td>%3</td></tr><tr><td>Available:</td><td>%4</td></tr></table>")
                                                                .arg(mount)
                                                                .arg(KIO::convertSizeFromKB(kbSize))
                                                                .arg(KIO::convertSizeFromKB(kbUsed))
                                                                .arg(KIO::convertSizeFromKB(kbAvail))
                                                                );
}

void SQ_DirOperator::slotExecutedConst(const KFileItem *fi)
{
    execute(const_cast<KFileItem *>(fi));
}

void SQ_DirOperator::execute(KFileItem *fi)
{
    down->kill();

    SQ_GLWidget::window()->setOriginalURL(fi->url());

    if(fi->url().isLocalFile())
        executePrivate(fi);
    else if(SQ_LibraryHandler::instance()->maybeSupported(fi->url(), fi->mimetype()) != SQ_LibraryHandler::No)
        down->start(fi);
}

void SQ_DirOperator::slotDownloaderResult(const KURL &url)
{
    if(url.isEmpty())
        return;    

    KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, url);

    executePrivate(&fi);
}

void SQ_DirOperator::prepareView(ViewT t)
{
    type = t;

    if(fileview)
    {
        oldSelected = *fileview->selectedItems();
        oldCurrentItem = fileview->currentFileItem();
    }

    switch(type)
    {
        case  SQ_DirOperator::TypeDetailed:
        {
            SQ_FileDetailView *dv;
            fileview = dv = new SQ_FileDetailView(this, "detail view");

            connect(dv, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(slotCurrentChanged(QListViewItem *)));
            connect(dv, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
            connect(dv, SIGNAL(invokeBrowser()), this, SLOT(slotInvokeBrowser()));
            connect(KSquirrel::app(), SIGNAL(resetToolTip()), dv, SLOT(slotResetToolTip()));
        }
        break;

        case  SQ_DirOperator::TypeList:
        case  SQ_DirOperator::TypeIcons:
        {
            SQ_FileIconView *iv;
            fileview = iv = new SQ_FileIconView(this, (type == SQ_DirOperator::TypeIcons) ? "icon view":"list view");

            connect(iv, SIGNAL(currentChanged(QIconViewItem *)), this, SLOT(slotCurrentChanged(QIconViewItem *)));
            connect(iv, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
            connect(iv, SIGNAL(invokeBrowser()), this, SLOT(slotInvokeBrowser()));
            connect(KSquirrel::app(), SIGNAL(resetToolTip()), iv, SLOT(slotResetToolTip()));

            disableSpecificActions(iv);
        }
        break;

        case  SQ_DirOperator::TypeThumbs:
        {
            SQ_FileThumbView *tv;
            fileview = tv = new SQ_FileThumbView(this, "thumb view");

            connect(tv, SIGNAL(currentChanged(QIconViewItem *)), this, SLOT(slotCurrentChanged(QIconViewItem *)));
            connect(tv, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
            connect(tv, SIGNAL(invokeBrowser()), this, SLOT(slotInvokeBrowser()));
            connect(dirLister(), SIGNAL(canceled()), tv, SLOT(stopThumbnailUpdate()));
            connect(KSquirrel::app(), SIGNAL(resetToolTip()), tv, SLOT(slotResetToolTip()));

            disableSpecificActions(tv);
        }
        break;
    }
}

void SQ_DirOperator::slotCurrentChanged(QListViewItem *)
{
    timer_preview->start(SQ_PreviewWidget::instance()->delay(), true);
}

void SQ_DirOperator::slotCurrentChanged(QIconViewItem *)
{
    timer_preview->start(SQ_PreviewWidget::instance()->delay(), true);
}

void SQ_DirOperator::slotSelectionChanged()
{
    QString str = i18n("no files selected");
    QPixmap px;
    KIO::filesize_t sz = static_cast<KIO::filesize_t>(0);
    int add = 0;
    KFileItem *fi = 0;

    // which items are selected ?
    for(KFileItem *it = fileview->firstFileItem();it;it = fileview->nextItem(it))
    {
        if(fileview->isSelected(it))
        {
            // _current_ fileitem and currently _selected_ fileitem
            // may differ, so lets count only first _selected_ item
            if(!fi)
                fi = it;

            add++;

            // count only files
            if(it->isFile())
                sz += it->size();
        }
    }

    if(fi)
    {
        // determine pixmap
        px = fi->pixmap(KIcon::SizeSmall);

        // costruct name and size
        str = QString("%1 %2 %3")
                .arg(KStringHandler::csqueeze(fi->name(), SQ_MAX_WORD_LENGTH))
                .arg(KIO::convertSize(fi->size()))
                .arg(add > 1 ? QString("<b>[%1/%2]</b>").arg(KIO::convertSize(sz)).arg(add):"");
    }

    // update statusbar
    KSquirrel::app()->sbarWidget("fileIcon")->setPixmap(px);
    KSquirrel::app()->sbarWidget("fileName")->setText(str);
}

void SQ_DirOperator::setPreparedView()
{
    setView(fileview);

    fileview->setOnlyDoubleClickSelectsFiles(true);
}

void SQ_DirOperator::slotDropped(const KFileItem *i, QDropEvent*, const KURL::List &urls)
{
    KURL u = i ? i->url() : url();

    // setup and show popup menu with actions
    SQ_NavigatorDropMenu::instance()->setupFiles(urls, u);
    SQ_NavigatorDropMenu::instance()->exec(QCursor::pos());
}

// Insert new actions in context menu.
void SQ_DirOperator::setupActionsMy()
{
    new KAction(i18n("Edit file type"), 0, 0, this, SLOT(slotEditMime()), actionCollection(), "dirop_edit_mime");
    new KAction(i18n("Add to Basket"), "folder_image", CTRL+Qt::Key_B, this, SLOT(slotAddToBasket()), actionCollection(), "dirop_tobasket");
    new KAction(i18n("Add to Folder Basket"), "folder_image", CTRL+Qt::Key_D, this, SLOT(slotAddToDirectoryBasket()), actionCollection(), "dirop_todirbasket");

    KActionMenu *file = new KActionMenu(i18n("File actions"), actionCollection(), "dirop_file_menu");
    KAction *prop = actionCollection()->action("properties");

    KAction *reload = actionCollection()->action("mkdir");
    if(reload && reload->shortcut().isNull()) reload->setShortcut(KShortcut(CTRL+Qt::Key_N));
    reload = actionCollection()->action("reload");

    connect(prop, SIGNAL(enabled(bool)), this, SLOT(slotEnableFileActions(bool)));

    KAction *sep = actionCollection()->action("separator");

    file->insert(new KAction(i18n("Copy"), "editcopy", KStdAccel::copy(), SQ_WidgetStack::instance(), SLOT(slotFileCopy()), actionCollection(), "dirop_copy"));
    file->insert(new KAction(i18n("Cut"), "editcut", KStdAccel::cut(), SQ_WidgetStack::instance(), SLOT(slotFileCut()), actionCollection(), "dirop_cut"));
    file->insert(new KAction(i18n("Paste"), "editpaste", KStdAccel::paste(), SQ_WidgetStack::instance(), SLOT(slotFilePaste()), actionCollection(), "dirop_paste"));
    file->insert(sep);
    file->insert(new KAction(i18n("Copy to..."), 0, Qt::Key_F5, SQ_WidgetStack::instance(), SLOT(slotFileCopyTo()), actionCollection(), "dirop_copyto"));
    file->insert(new KAction(i18n("Move to..."), 0, Qt::Key_F6, SQ_WidgetStack::instance(), SLOT(slotFileMoveTo()),actionCollection(), "dirop_cutto"));
    file->insert(new KAction(i18n("Link to..."), 0, Qt::Key_F7, SQ_WidgetStack::instance(), SLOT(slotFileLinkTo()), actionCollection(), "dirop_linkto"));
    file->insert(sep);
    file->insert(new KAction(i18n("Run"), "launch", CTRL+Key_J, SQ_WidgetStack::instance(), SLOT(slotRunSeparately()), actionCollection(), "dirop_runsepar"));
    file->insert(new KAction(i18n("Repeat (nothing to repeat)"), "down", Qt::Key_F10, KSquirrel::app(), SLOT(slotRepeat()), actionCollection(), "dirop_repeat"));
    file->insert(new KAction(i18n("Convert..."), 0, CTRL+Qt::Key_K, SQ_Converter::instance(), SLOT(slotStartEdit()), actionCollection(), "dirop_convert"));
    file->insert(sep);
    file->insert(new KAction(i18n("Recreate selected thumbnails"), "reload", CTRL+Qt::Key_R, SQ_WidgetStack::instance(),
        SLOT(slotRecreateThumbnail()), actionCollection(), "dirop_recreate_thumbnails"));
    file->insert(sep);
    file->insert(new KAction(i18n("Copy file path"), 0, Qt::Key_F8, this, SLOT(slotCopyPath()), actionCollection(), "dirop_copypath"));
    file->insert(new KAction(i18n("Copy file url"), 0, Qt::Key_F9, this, SLOT(slotCopyURL()), actionCollection(), "dirop_copyurl"));

    if(reload) reload->setShortcut(CTRL+Qt::Key_F5);
}

void SQ_DirOperator::slotEnableFileActions(bool e)
{
    actionCollection()->action("dirop_tobasket")->setEnabled(e);
    actionCollection()->action("dirop_todirbasket")->setEnabled(e);

    actionCollection()->action("dirop_copy")->setEnabled(e);
    actionCollection()->action("dirop_cut")->setEnabled(e);
    actionCollection()->action("dirop_copyto")->setEnabled(e);
    actionCollection()->action("dirop_cutto")->setEnabled(e);
    actionCollection()->action("dirop_linkto")->setEnabled(e);

    actionCollection()->action("dirop_runsepar")->setEnabled(e);
    actionCollection()->action("dirop_convert")->setEnabled(e);

    actionCollection()->action("dirop_copypath")->setEnabled(e);
    actionCollection()->action("dirop_copyurl")->setEnabled(e);

    actionCollection()->action("dirop_recreate_thumbnails")->setEnabled(type == SQ_DirOperator::TypeThumbs && e);
    SQ_ExternalTool::instance()->constPopupMenu()->setEnabled(e);
    SQ_CategoriesBox::instance()->popupMenu()->setEnabled(e);
}

void SQ_DirOperator::slotCopyPath()
{
    KFileItem *f = fileview->currentFileItem();

    if(f)
        QApplication::clipboard()->setText(f->url().path());
}

void SQ_DirOperator::slotCopyURL()
{
    KFileItem *f = fileview->currentFileItem();

    if(f)
        QApplication::clipboard()->setText(f->url().prettyURL());
}

void SQ_DirOperator::slotEditMime()
{
    KFileItem *f = fileview->currentFileItem();

    if(f)
        KonqOperations::editMimeType(f->mimetype());
}

void SQ_DirOperator::slotAddToBasket()
{
    SQ_ImageBasket::instance()->add(*selectedItems());
}

void SQ_DirOperator::slotAddToDirectoryBasket()
{
    SQ_DirectoryBasket::instance()->add(*selectedItems());
}

void SQ_DirOperator::slotFinishedLoading()
{
    // force sort if invisible
    if(!isVisible())
    {
        // derived from KFileDetailView
        if(type ==  SQ_DirOperator::TypeDetailed)
        {
            KFileDetailView *dv = dynamic_cast<KFileDetailView *>(fileview);
            if(dv) dv->sort();
        }
        // derived from KFileIconView
        else
        {
            KFileIconView *tv = dynamic_cast<KFileIconView *>(fileview);
            if(tv) tv->sort(!tv->isReversed());
        }
    }

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
        m_pending = QString::null;
        KSquirrel::app()->sbarWidget("fileIcon")->clear();
        KSquirrel::app()->sbarWidget("fileName")->clear();
        return;
    }

    // setting current file directly doesn't work. let
    // events to be processed and only then set current file
    QTimer::singleShot(0, this, SLOT(slotDelayedFinishedLoading()));
}

void SQ_DirOperator::slotDelayedFinishedLoading()
{
    // select pending file if any
    if(!m_pending.isEmpty())
    {
        if(fileview)
        {
            fileview->clearSelection();
            fileview->setCurrentItem(m_pending);
            setCurrentItem(fileview->currentFileItem());
        }

        m_pending = QString::null;
        startOrNotThumbnailUpdate();

        return;
    }

    KURL up_url = url().upURL();

    KFileItem *first = fileview->firstFileItem();

    if(!first) return;

    // ignore ".." item
    if(up_url.equals(first->url(), true))
        first = fileview->nextItem(first);

    // hell!
    if(!first) return;

    SQ_Config::instance()->setGroup("Fileview");

    bool goto_first = false;

    // SQ_WidgetStack will select first supported image for us
    if(SQ_Config::instance()->readBoolEntry("tofirst", true))
        goto_first = SQ_WidgetStack::instance()->moveTo(SQ_WidgetStack::Next, first) == SQ_WidgetStack::moveFailed;
    else
        goto_first = true;

    if(goto_first)
    {
        KFileItem *fi = fileview->currentFileItem();

        if(fi && !up_url.equals(fi->url(), true))
            setCurrentItem(fileview->currentFileItem());
        else
            setCurrentItem(first);
    }

    startOrNotThumbnailUpdate();
}

void SQ_DirOperator::slotUpdateInformation(int files, int dirs)
{
    int total = dirs + files;

    SQ_Config::instance()->setGroup("Fileview");

    QString str = i18n("Total %1 in %2 (%3, %4)")
                    .arg(KIO::convertSize(totalSize))
                    .arg(i18n("1 item", "%n items", total))
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
    oldSelected = *fileview->selectedItems();
    oldCurrentItem = fileview->currentFileItem();

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

    selectOld();
}

void SQ_DirOperator::selectOld()
{
    // set current item...
    fileview->setCurrentItem(oldCurrentItem);

    // restore selection
    KFileItemListIterator it(oldSelected);

    fileview->ensureItemVisible(it.current());

    for(; it.current();++it)
        fileview->setSelected(it.current(), true);

    oldSelected.clear();

    stopPreview();
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

        // start thumbnail update only when this
        // diroperator is visible
        if(tv)
        {
            if(tv->isVisible())
                tv->startThumbnailUpdate();
            else
                tv->waitForShowEvent();
        }
    }
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

    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv && tv->updateRunning())
            tv->itemsRemoved(list);
    }

    SQ_Config::instance()->setGroup("Fileview");
    bool c = SQ_Config::instance()->readBoolEntry("calculate", false);

    for(KFileItem *itit_tvoyu_mats = list.first();itit_tvoyu_mats;itit_tvoyu_mats = list.next())
    {
        itemDeleted(itit_tvoyu_mats);

        if(c) totalSize -= itit_tvoyu_mats->size();
    }

    slotUpdateInformation(numFiles(), numDirs());

    // remove this lister from map
    listers.remove(url);

    delete lister;

    // update view
    KFileIconView *vi = dynamic_cast<KFileIconView *>(fileview);

    if(vi)
       vi->arrangeItemsInGrid();
}

void SQ_DirOperator::executePrivate(KFileItem *fi)
{
    KURL url = fi->url();
    QString fullpath = url.path();
    QFileInfo fm(fullpath);

    // Ok, this file is image file. Let's try to load it
    if(SQ_LibraryHandler::instance()->libraryForFile(fullpath))
        SQ_GLWidget::window()->startDecoding(fullpath);
    else
    {
        SQ_Config::instance()->setGroup("Fileview");

        // archive ?
        if(!SQ_ArchiveHandler::instance()->findProtocolByFile(fi).isEmpty())
            if(SQ_Config::instance()->readBoolEntry("archives", true))
                SQ_ArchiveHandler::instance()->tryUnpack(fi);
            else;
        // not image, not archive. Read settings and run this file separately
        // with default application (if needed).
        else if(SQ_Config::instance()->readBoolEntry("run unknown", false))
            emit runSeparately(fi);
    }
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
    KFileItem *fi = fileview ? fileview->currentFileItem() : 0;

    if(fi) SQ_PreviewWidget::instance()->load(fi->url());
}

void SQ_DirOperator::slotNewItems(const KFileItemList &list)
{
    SQ_Config::instance()->setGroup("Fileview");

    if(SQ_Config::instance()->readBoolEntry("calculate", false))
    {
        KFileItemListIterator it(list);
        KFileItem *fi;

        while((fi = it.current()) != 0)
        {
            ++it;
            if(fi->isFile()) totalSize += fi->size();
        }
    }

    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs && usenew)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv)
            tv->addItemsToJob(list);
    }
}

void SQ_DirOperator::slotRefreshItems(const KFileItemList &list)
{
    KFileItemListIterator it(list);
    KFileItem *fci = fileview ? fileview->currentFileItem() : 0;
    KFileItem *fi;

    // refresh image been previewed
    while((fi = it.current()))
    {
        if(fi == fci)
        {
            timer_preview->start(SQ_PreviewWidget::instance()->delay(), true);
            break;
        }

        ++it;
    }

    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv)
        {
            KFileItemListIterator it(list);
            SQ_FileThumbViewItem *tfi;

            if(tv->updateRunning())
                tv->itemsRemoved(list);

            while((fi = it.current()))
            {
                // force thumbnail to be updated by setting listed = false
                tfi = reinterpret_cast<SQ_FileThumbViewItem *>(fi->extraData(tv));

                if(tfi)
                    tfi->setListed(false);

                // remove thumbnail from pixmap cache and from disk
                SQ_PixmapCache::instance()->removeEntryFull(fi->url().path());
                ++it;
            }

            tv->addItemsToJob(list, false);
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

    itemKill(item);

    // remove thumbnail from cache,
    SQ_PixmapCache::instance()->removeEntryFull(item->url());
}

void SQ_DirOperator::itemKill(KFileItem *item)
{
    if(!item) return;

    SQ_Config::instance()->setGroup("Fileview");
    if(SQ_Config::instance()->readBoolEntry("calculate", false))
        totalSize -= item->size();

    slotUpdateInformation(numFiles(), numDirs());

    // start delayed thumbnail update, if needed
    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv && tv->updateRunning())
            tv->itemRemoved(item);
    }
}

void SQ_DirOperator::stopPreview()
{
    timer_preview->stop();
}

void SQ_DirOperator::activatedMenu(const KFileItem *, const QPoint &pos)
{
    // remove "View" submenu, since we will insert our's one.
    setupMenu(KDirOperator::AllActions ^ KDirOperator::ViewActions);
    updateSelectionDependentActions();

    KActionMenu *pADirOperatorMenu = dynamic_cast<KActionMenu *>(actionCollection()->action("popupMenu"));

    pADirOperatorMenu->insert(actionCollection()->action("dirop_edit_mime"));
    pADirOperatorMenu->insert(actionCollection()->action("separator"));
    pADirOperatorMenu->popupMenu()->insertItem(i18n("Add to &Category"), SQ_CategoriesBox::instance()->popupMenu());
    pADirOperatorMenu->insert(actionCollection()->action("dirop_tobasket"));
    pADirOperatorMenu->insert(actionCollection()->action("dirop_todirbasket"));

    pADirOperatorMenu->popupMenu()->insertItem(i18n("File actions"), dynamic_cast<KActionMenu *>(actionCollection()->action("dirop_file_menu"))->popupMenu(), -1, 0);
    pADirOperatorMenu->popupMenu()->insertItem(i18n("&External tools"), SQ_ExternalTool::instance()->constPopupMenu(), -1, 1);
    pADirOperatorMenu->popupMenu()->insertSeparator(2);

    KFileItemList *items = const_cast<KFileItemList *>(selectedItems());

    if(items)
        SQ_ExternalTool::instance()->setItems(*items);

    pADirOperatorMenu->popup(pos);
}

void SQ_DirOperator::slotSetURL(const KURL &url)
{
    setURL(url, true);
}

void SQ_DirOperator::calcTotalSize()
{
    totalSize = static_cast<KIO::filesize_t>(0);

    KFileItemList *list = const_cast<KFileItemList *>(fileview->items());

    if(!list) return;

    KFileItemListIterator it(*list);
    KFileItem *fi;

    while((fi = it.current()) != 0)
    {
        ++it;

        if(fi->isFile())
            totalSize += fi->size();
    }

    slotUpdateInformation(numFiles(), numDirs());
}

void SQ_DirOperator::saveConfig()
{
    QDir::SortSpec sort = sorting();

    SQ_Config::instance()->writeEntry("sorting_name", KFile::isSortByName(sort));
    SQ_Config::instance()->writeEntry("sorting_time", KFile::isSortByDate(sort));
    SQ_Config::instance()->writeEntry("sorting_size", KFile::isSortBySize(sort));
    SQ_Config::instance()->writeEntry("sorting_dirs", KFile::isSortDirsFirst(sort));
    SQ_Config::instance()->writeEntry("sorting_reverse", (sort & QDir::Reversed) == QDir::Reversed);
    SQ_Config::instance()->writeEntry("sorting_ignore", KFile::isSortCaseInsensitive(sort));

    SQ_Config::instance()->writeEntry("show hidden", showHiddenFiles());
}

void SQ_DirOperator::setLazy(bool l, int delay, int rows)
{
    if(type == SQ_DirOperator::TypeThumbs)
    {
        SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

        if(tv)
        {
            if(tv->lazy() != l)
                tv->setLazy(l, delay);

            tv->setAddRows(rows);

            if(tv->updateRunning())
                tv->startThumbnailUpdate();
        }
    }
}

void SQ_DirOperator::slotInvokeBrowser()
{
    kapp->invokeBrowser(url().prettyURL());
}

void SQ_DirOperator::enableThumbnailActions(bool enable)
{
    actionCollection()->action("dirop_recreate_thumbnails")->setEnabled(enable);
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

#include "sq_diroperator.moc"
