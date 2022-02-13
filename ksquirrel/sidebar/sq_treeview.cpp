/***************************************************************************
                          sq_treeview.cpp  -  description
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

#include <qdir.h>
#include <qheader.h>

#include "sq_iconloader.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_treeview.h"

SQ_TreeView * SQ_TreeView::m_instance = NULL;

SQ_TreeView::SQ_TreeView(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
    m_instance = this;

    // we should add at least one branch
    root = addBranch(KURL(QDir::rootDirPath()), " /", 
                SQ_IconLoader::instance()->loadIcon("hdd_mount", KIcon::Desktop, KIcon::SizeSmall));

    addColumn("Name");
    header()->hide();

    // don't show files
    setDirOnlyMode(root, true);

    // show '+'
    setRootIsDecorated(true);

    setCurrentItem(root->root());
    root->setChildRecurse(true);
    root->setOpen(true);

    // connect signals

    // Space and Return will open item
    connect(this, SIGNAL(spacePressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));

    connect(this, SIGNAL(executed(QListViewItem*)), this, SLOT(slotItemExecuted(QListViewItem*)));
    connect(this, SIGNAL(newURL(const KURL&)), this, SLOT(slotNewURL(const KURL&)));
    connect(root, SIGNAL(populateFinished(KFileTreeViewItem *)), this, SLOT(slotOpened(KFileTreeViewItem *)));

    itemsToClose = new KFileTreeViewItemList;

    SQ_Config::instance()->setGroup("Fileview");
    int sync_type = SQ_Config::instance()->readNumEntry("sync type", 0);

    // load url, if needed
    if(sync_type == 2 || sync_type == 0)
        emitNewURL(SQ_WidgetStack::instance()->url());
}

SQ_TreeView::~SQ_TreeView()
{
    delete itemsToClose;
}

/*
 *  Item executed. Let's pass its url to SQ_WidgetStack (if needed).
 */
void SQ_TreeView::slotItemExecuted(QListViewItem *item)
{
    if(!item) return;

    SQ_Config::instance()->setGroup("Fileview");
    int sync_type = SQ_Config::instance()->readNumEntry("sync type", 0);

    // current sychronization type doesn't require
    // passing url to SQ_WidgetStack - return
    if(sync_type == 2)
        return;

    KFileTreeViewItem *cur = static_cast<KFileTreeViewItem*>(item);
    KURL Curl = cur->url();

    // pass url to SQ_WidgetStack
    SQ_WidgetStack::instance()->setURLForCurrent(Curl, false);
}

void SQ_TreeView::emitNewURL(const KURL &url)
{
    // already selected?
    if(url.equals(currentURL(), true))
        return;

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
    setCurrentItem(item);
    ensureItemVisible(item);
    item->setOpen(true);
}

/*
 *  Close all last opened items.
 */
void SQ_TreeView::collapseOpened()
{
    paths.clear();

    KFileTreeViewItem *item;

    // go through array of items and close them all
    while((item = itemsToClose->getFirst()) != NULL)
    {
        item->setOpen(false);
        itemsToClose->removeFirst();
    }
}

/*
 *  Load url.
 */
void SQ_TreeView::slotNewURL(const KURL &url)
{
    collapseOpened();

    KURL k(url);
    k.adjustPath(1);

    KURL last = k;

    // divide url to paths.
    // for example, "/home/krasu/1/2" will be divided to
    //
    // "/home/krasu/1/2"
    // "/home/krasu/1"
    // "/home/krasu/"
    // "/home/"
    // "/"
    //
    while(true)
    {
        paths.prepend(k.path());
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
        return false;

    QValueList<QString>::iterator it = paths.begin();

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

    // save a pointer to this item for collapseOpened()
    itemsToClose->prepend(found);

    // done, but new subpaths are pending...
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

#include "sq_treeview.moc"
