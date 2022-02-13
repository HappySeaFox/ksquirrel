/***************************************************************************
                          sq_treeview.cpp  -  description
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

#include <qdir.h>
#include <qheader.h>
#include <qtimer.h>
#include <qcursor.h>

#include <klocale.h>
#include <kurldrag.h>
#include <kurl.h>
#include <kdirwatch.h>

#include "ksquirrel.h"
#include "sq_iconloader.h"
#include "sq_widgetstack.h"
#include "sq_filethumbview.h"
#include "sq_config.h"
#include "sq_treeview.h"
#include "sq_treeviewitem.h"
#include "sq_threaddirlister.h"
#include "sq_navigatordropmenu.h"
#include "sq_treeviewmenu.h"
#include "sq_hloptions.h"

#include "sq_directorybasket.h"

SQ_TreeView * SQ_TreeView::m_instance = 0;

SQ_FileTreeViewBranch::SQ_FileTreeViewBranch(KFileTreeView *parent, const KURL &url,
        const QString &name, const QPixmap &pix) : KFileTreeBranch(parent, url, name, pix)
{}

SQ_FileTreeViewBranch::~SQ_FileTreeViewBranch()
{}

KFileTreeViewItem* SQ_FileTreeViewBranch::createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem)
{
    KFileTreeViewItem *tvi = 0;

    if(parent && fileItem)
        tvi = new SQ_TreeViewItem(parent, fileItem, this);

    return tvi;
}

/******************************************************************************************************/

SQ_TreeView::SQ_TreeView(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
    m_instance = this;

    itemToBeOpened = 0;
    m_animTimer = new QTimer(this);
    scanTimer = new QTimer(this);
    m_ignoreClick = false;

    connect(m_animTimer, SIGNAL(timeout()), this, SLOT(slotAnimation()));
    connect(scanTimer, SIGNAL(timeout()), this, SLOT(slotDelayedScan()));

    dw = 0;
    m_recurs = No;
    lister = new SQ_ThreadDirLister(this);
    setupRecursion();

    setFrameShape(QFrame::NoFrame);
    addColumn("Name");
    addColumn("Check");

    header()->hide();
    header()->moveSection(1, 0);
    setColumnWidthMode(1, QListView::Manual);
    setColumnWidth(1, 18);

    root = new SQ_FileTreeViewBranch(this, QDir::rootDirPath(), QString::null,
        SQ_IconLoader::instance()->loadIcon("folder_red", KIcon::Desktop, KIcon::SizeSmall));

    // some hacks to create our SQ_TreeViewItem as root item
    SQ_TreeViewItem  *ritem = new SQ_TreeViewItem(this,
                                        new KFileItem(QDir::rootDirPath(),
                                        "inode/directory", S_IFDIR),
                                        root);

    ritem->setText(0, i18n("root"));
    ritem->setExpandable(true);

    // oops :)
    delete root->root();

    // set new root
    root->setRoot(ritem);

    addBranch(root);

    // don't show files
    setDirOnlyMode(root, true);

    // show '+'
    setRootIsDecorated(true);

    // connect signals

    // Space and Return will open item
    connect(this, SIGNAL(spacePressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(slotCurrentChanged(QListViewItem*)));

    connect(this, SIGNAL(executed(QListViewItem*)), this, SLOT(slotItemExecuted(QListViewItem*)));
    connect(this, SIGNAL(newURL(const KURL&)), this, SLOT(slotNewURL(const KURL&)));
    connect(root, SIGNAL(populateFinished(KFileTreeViewItem *)), this, SLOT(slotOpened(KFileTreeViewItem *)));
    connect(root, SIGNAL(deleteItem(KFileItem *)), this, SLOT(slotDeleteItemMy(KFileItem *)));

    setCurrentItem(root->root());
    root->setChildRecurse(false);

    SQ_Config::instance()->setGroup("Fileview");
    int sync_type = SQ_Config::instance()->readNumEntry("sync type", 0);

    // load url, if needed
    if(sync_type != 1)
        emitNewURL(SQ_WidgetStack::instance()->url());

    setAcceptDrops(true);

    connect(this, SIGNAL(dropped(QDropEvent *, QListViewItem *, QListViewItem *)),
            this, SLOT(slotDropped(QDropEvent *, QListViewItem *, QListViewItem *)));

    connect(this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
            this, SLOT(slotContextMenu(KListView *, QListViewItem *, const QPoint &)));

    menu = new SQ_TreeViewMenu(this);

    if(SQ_HLOptions::instance()->have_directorybasket)
    {
        menu->insertSeparator();
        menu->insertItem(i18n("Add to Folder Basket"), this, SLOT(slotAddToFolderBasket()));
    }
}

SQ_TreeView::~SQ_TreeView()
{
    lister->terminate();
    lister->wait();

    delete lister;
}

void SQ_TreeView::slotCurrentChanged(QListViewItem *item)
{
    SQ_TreeViewItem *tvi = static_cast<SQ_TreeViewItem *>(item);

    if(tvi) cURL = tvi->url();
}

void SQ_TreeView::setRecursion(int b)
{
    // avoid duplicate calls
    if(m_recurs == b)
        return;

    QListViewItemIterator it(this);
    SQ_TreeViewItem *tvi;

    // ignore root item
    ++it;

    // turn recursion on
    if(m_recurs == No && b)
    {
        dw = new KDirWatch(this);
        connect(dw, SIGNAL(dirty(const QString &)), this, SLOT(slotDirty(const QString &)));

        dw->blockSignals(true);
        lister->lock();
        while(it.current())
        {
            tvi = static_cast<SQ_TreeViewItem *>(it.current());

            if(tvi)
            {
                tvi->setParams((m_recurs == Files || m_recurs == FilesDirs),
                               (m_recurs == Dirs || m_recurs == FilesDirs));

                lister->appendURL(tvi->url());
                dw->addDir(tvi->path());
            }

            ++it;
        }
        lister->unlock();
        dw->blockSignals(false);

        m_recurs = b;

        if(!lister->running())
            scanTimer->start(1, true);
    }
    // turn recursion off
    else
    {
        delete dw;
        dw = 0;

        m_recurs = b;
        while(it.current())
        {
            tvi = static_cast<SQ_TreeViewItem *>(it.current());

            // reset item names
            if(tvi)
            {
                tvi->setParams((m_recurs == Files || m_recurs == FilesDirs),
                               (m_recurs == Dirs || m_recurs == FilesDirs));
                tvi->setCount(tvi->files(), tvi->dirs());
            }

            ++it;
        }
    }
}

void SQ_TreeView::slotClearChecked()
{
    QListViewItemIterator it(this);
    SQ_TreeViewItem *tvi;

    while(it.current())
    {
        tvi = static_cast<SQ_TreeViewItem *>(it.current());

        if(tvi && tvi->checked())
            tvi->setChecked(false);

        ++it;
    }
}

/*
 *  Item executed. Let's pass its url to SQ_WidgetStack (if needed).
 */
void SQ_TreeView::slotItemExecuted(QListViewItem *item)
{
    if(!item) return;

    item->setOpen(true);

    SQ_Config::instance()->setGroup("Fileview");
    int sync_type = SQ_Config::instance()->readNumEntry("sync type", 0);

    // current sychronization type doesn't require
    // passing url to SQ_WidgetStack - return
    if(sync_type == 1)
        return;

    KFileTreeViewItem *cur = static_cast<KFileTreeViewItem*>(item);
    KURL Curl = cur->url();

    // pass url to SQ_WidgetStack
    SQ_WidgetStack::instance()->setURLForCurrent(Curl, false);
}

void SQ_TreeView::emitNewURL(const KURL &url)
{
    // already selected?
    if(!url.isLocalFile() || url.equals(currentURL(), true) || url.equals(cURL, true))
        return;

    cURL = url;

    // tree is invisible.
    // save url for future use
    if(!isVisible())
    {
        pendingURL = url;
        pendingURL.adjustPath(1);
        return;
    }
    else
        pendingURL = KURL();

    emit newURL(url);
}

/*
 *  Set given item visible, current, and populate it.
 */
void SQ_TreeView::populateItem(KFileTreeViewItem *item)
{
    if(item->url().equals(cURL, true))
    {
        setSelected(item, true);
        setCurrentItem(item);

        // 1/5 of height give above items
        // 4/5 of height give below items
        setContentsPos(contentsX(), itemPos(item)-visibleHeight()/5);
    }

    itemToBeOpened = item;
    item->setOpen(true);
}

/*
 *  Load url.
 */
void SQ_TreeView::slotNewURL(const KURL &url)
{
    KURL k(url);
    k.adjustPath(1);

    KURL last = k;

    QString s;

    // divide url to paths.
    // for example, "/home/krasu/1/2" will be divided to
    //
    // "/home/krasu/1/2"
    // "/home/krasu/1"
    // "/home/krasu"
    // "/home"
    // "/"
    //
    while(true)
    {
        s = k.path(); // remove "/"
        s.remove(0, 1);

        paths.prepend(s);
        k = k.upURL();

        if(k.equals(last, true))
            break;

        last = k;
    }

    while(doSearch())
    {}
}

/*
 *  New item is opened. Try to continue loading url.
 */
void SQ_TreeView::slotOpened(KFileTreeViewItem *item)
{
    if(!item) return;

    // continue searhing...
    if(item == itemToBeOpened)
        doSearch();
}

/*
 *  Search first available url in variable 'paths'. Open found item.
 *  If item was found return true.
 */
bool SQ_TreeView::doSearch()
{
    // all items are opened
    if(paths.empty())
    {
        itemToBeOpened = 0;
        return false;
    }

    QStringList::iterator it = paths.begin();

    KFileTreeViewItem *found = findItem(root, *it);

    // item not found. It means that
    //
    //   * we loaded all subpaths - nothing to do
    //   * item we needed is not loaded yet. populateItem() _now_ is trying to open
    //     new subpath, and we will continue searching in slotOpened().
    //
    if(!found)
        return false;

    // ok, item is found
    //
    // remove first entry from 'paths'
    // and try to open item
    paths.erase(it);
    populateItem(found);

    // done, but subpaths are pending...
    return true;
}

/*
 *  On show event load saved url, if any. See emitNewURL().
 */
void SQ_TreeView::showEvent(QShowEvent *)
{
    // if pending url is valid
    if(!pendingURL.isEmpty())
    {
        // set pending url to current url
        KURL url = pendingURL;

        // reset pending url
        pendingURL = KURL();

        // finally, load url
        emit newURL(url);
    }
}

void SQ_TreeView::slotNewTreeViewItems(KFileTreeBranch *, const KFileTreeViewItemList &list)
{
    if(m_recurs == No)
        return;

//  uuuuuuuggggggghhhhhhh :)
    KFileTreeViewItemListIterator it(list);
    KFileTreeViewItem *item;

    lister->lock();
    while((item = it.current()))
    {
        lister->appendURL(item->url());
        dw->addDir(item->path());
        ++it;
    }
    lister->unlock();

    if(!lister->running())
        scanTimer->start(1, true);
}

void SQ_TreeView::slotDelayedScan()
{
    if(!lister->running())
        lister->start();
}

void SQ_TreeView::customEvent(QCustomEvent *e)
{
    if(!e || e->type() != SQ_ItemsEventId)
        return;

    SQ_ItemsEvent *ie = static_cast<SQ_ItemsEvent *>(e);

    SQ_TreeViewItem *tvi = static_cast<SQ_TreeViewItem *>(root->findTVIByURL(ie->url()));

    if(tvi)
    {
        tvi->setParams((m_recurs == Files || m_recurs == FilesDirs),
                       (m_recurs == Dirs || m_recurs == FilesDirs));
        tvi->setCount(ie->files(), ie->dirs());
    }
}

void SQ_TreeView::slotAnimation()
{
    KFileTreeViewItem *it = m_mapFolders.first();

    for(;it;it = m_mapFolders.next())
        it->setPixmap(0, SmallIcon("clock"));
}

void SQ_TreeView::startAnimation(KFileTreeViewItem *item, const char *, uint)
{
    if(!item)
        return;

    m_mapFolders.append(item);

    if(!m_animTimer->isActive())
        m_animTimer->start(50, true);
}

void SQ_TreeView::stopAnimation(KFileTreeViewItem *item)
{
    if(!item)
        return;

    int f = m_mapFolders.find(item);

    if(f != -1)
    {
        item->setPixmap(0, itemIcon(item));
        m_mapFolders.remove(item);
    }

    m_animTimer->stop();
}

void SQ_TreeView::viewportResizeEvent(QResizeEvent *)
{
    setColumnWidth(0, viewport()->width() - columnWidth(1));
    triggerUpdate();
}

void SQ_TreeView::clearSelection()
{
    if(!m_ignoreClick) KFileTreeView::clearSelection();
}

void SQ_TreeView::setSelected(QListViewItem *item, bool selected)
{
    if(!m_ignoreClick) KFileTreeView::setSelected(item, selected);
}

void SQ_TreeView::setCurrentItem(QListViewItem *item)
{
    if(!m_ignoreClick) KFileTreeView::setCurrentItem(item);
}

void SQ_TreeView::setOpen(QListViewItem *item, bool open)
{
    if(!m_ignoreClick) KFileTreeView::setOpen(item, open);
}

void SQ_TreeView::contentsMousePressEvent(QMouseEvent *e)
{
    QListViewItem *item;

    QPoint point = e->pos();
    point.setY(point.y() - contentsY());

    if(header()->sectionAt(point.x()) && (item = itemAt(point)))
    {
        SQ_TreeViewItem *m = static_cast<SQ_TreeViewItem *>(item);

        if(m)
        {
            int state = e->state();

            if(!state)
                toggle(m, true);
            else
            {
                QListViewItemIterator it(this);

                // toggle parent item
                if(state == Qt::ShiftButton)        toggle(m, true);
                else if(state == Qt::ControlButton) toggle(m, false, true);
                else if(state == Qt::AltButton)     toggle(m, false, false);

                SQ_TreeViewItem *tvi = static_cast<SQ_TreeViewItem *>(m->firstChild());

                // toggle all child items
                while(tvi)
                {
                    if(state == Qt::ShiftButton)               toggle(tvi, false, m->checked());
                    else if(state == Qt::ControlButton) toggle(tvi, false, true);
                    else if(state == Qt::AltButton)          toggle(tvi, false, false);

                    tvi = static_cast<SQ_TreeViewItem *>(tvi->nextSibling());
                }
            }
        }

        m_ignoreClick = true;
    }

    KFileTreeView::contentsMousePressEvent(e);

    m_ignoreClick = false;
}

void SQ_TreeView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    if(header()->sectionAt(e->x()))
        m_ignoreClick = true;

    KFileTreeView::contentsMouseDoubleClickEvent(e);

    m_ignoreClick = false;
}

void SQ_TreeView::setupRecursion()
{
    SQ_Config::instance()->setGroup("Sidebar");

    setRecursion(SQ_Config::instance()->readNumEntry("recursion_type", No));
}

void SQ_TreeView::toggle(SQ_TreeViewItem *item, bool togg, bool set)
{
    if(togg)
        item->setChecked(!item->checked());
    else
        item->setChecked(set);

    if(item->checked())
        emit urlAdded(item->url());
    else
        emit urlRemoved(item->url());
}

void SQ_TreeView::slotDropped(QDropEvent *e, QListViewItem *parent, QListViewItem *item)
{
    if(!item) item = parent;

    KFileTreeViewItem *cur = static_cast<KFileTreeViewItem *>(item);

    if(!cur) return;

    KURL::List list;
    KURLDrag::decode(e, list);

    SQ_NavigatorDropMenu::instance()->setupFiles(list, cur->url());
    SQ_NavigatorDropMenu::instance()->exec(QCursor::pos(), true);
}

void SQ_TreeView::slotContextMenu(KListView *, QListViewItem *item, const QPoint &point)
{
    if(item)
    {
        KFileTreeViewItem *kfi = static_cast<KFileTreeViewItem*>(item);
        menu->updateDirActions(kfi->isDir());
        menu->setURL(kfi->url());
        menu->exec(point);
    }
}

void SQ_TreeView::slotDirty(const QString &path)
{
    KURL url;
    url.setPath(path);

    lister->lock();

    // we don't need update urls that are not yet updated
    // by threaded lister
    if(!lister->hasURL(url))
        lister->appendURL(url);
    else if(lister->isCurrent(url))
    {
        lister->terminate();
        lister->wait();
        lister->closeDir();
    }

    lister->unlock();

    if(!lister->running())
        scanTimer->start(1000, true);
}

void SQ_TreeView::slotDeleteItemMy(KFileItem *fi)
{
    if(m_recurs != No && fi)
        dw->removeDir(fi->url().path());
}

void SQ_TreeView::slotAddToFolderBasket()
{
    KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, menu->url());

    KFileItemList list;
    list.append(&fi);

    SQ_DirectoryBasket::instance()->add(list);
}

#include "sq_treeview.moc"
