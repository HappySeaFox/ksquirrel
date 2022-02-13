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

#include <kiconloader.h>
#include <kdeversion.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_treeview.h"

SQ_TreeView * SQ_TreeView::tree = 0L;

SQ_TreeView::SQ_TreeView(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
	tree = this;
	vis = false;

	QPixmap homePix = KSquirrel::loader()->loadIcon("gohome", KIcon::Desktop, KIcon::SizeSmall);
	QPixmap rootPix = KSquirrel::loader()->loadIcon("hdd_mount", KIcon::Desktop, KIcon::SizeSmall);   

	root = addBranch(KURL(QDir::rootDirPath()), " /", rootPix);
	home = addBranch(KURL(QDir().home().absPath()), " Home", homePix);

	addColumn("Name");
	header()->hide();

	setDirOnlyMode(root, true);
	setDirOnlyMode(home, true);

	setRootIsDecorated(true);
	setShowSortIndicator(true);

	setCurrentItem(root->root());
	root->setOpen(true);

	connect(this, SIGNAL(spacePressed(QListViewItem*)), SIGNAL(executed(QListViewItem*)));
	connect(this, SIGNAL(returnPressed(QListViewItem*)), SIGNAL(executed(QListViewItem*)));
	connect(this, SIGNAL(executed(QListViewItem*)), SLOT(slotItemExecuted(QListViewItem*)));
	connect(this, SIGNAL(newURL(const KURL&)), SLOT(slotNewURL(const KURL&)));
	connect(root, SIGNAL(populateFinished(KFileTreeViewItem *)), SLOT(slotOpened(KFileTreeViewItem *)));

	itemsToClose = new KFileTreeViewItemList;

	int sync_type = SQ_Config::instance()->readNumEntry("Fileview", "sync type", 0);

	if(sync_type == 2 || sync_type == 0)
		emitNewURL(SQ_WidgetStack::instance()->getURL());
}

SQ_TreeView::~SQ_TreeView()
{}

void SQ_TreeView::slotItemExecuted(QListViewItem *item)
{
	if(!item) return;

	int sync_type = SQ_Config::instance()->readNumEntry("Fileview", "sync type", 0);

	if(sync_type == 2)
		return;

	KFileTreeViewItem *cur = static_cast<KFileTreeViewItem*>(item);
	KURL Curl = cur->url();

	SQ_WidgetStack::instance()->setURLForCurrent(Curl);
}

void SQ_TreeView::emitNewURL(const KURL &url)
{
	if(!vis)
	{
		pendingURL = url;
		pendingURL.adjustPath(1);
		return;
	}
	else
		pendingURL = KURL();

	emit newURL(url);
}

void SQ_TreeView::populateItem(KFileTreeViewItem *item)
{
	setCurrentItem(item);
	ensureItemVisible(item);
	item->setOpen(true);
}

void SQ_TreeView::collapseOpened()
{
	paths.clear();

	KFileTreeViewItem *item;

	while((item = itemsToClose->getFirst()) != 0L)
	{
		item->setOpen(false);
		itemsToClose->removeFirst();
	}
}

void SQ_TreeView::slotNewURL(const KURL &url)
{
	collapseOpened();

	KURL k(url);
	k.adjustPath(1);

	KURL last = k;

	while(true)
	{
		paths.prepend(k.path());
		k = k.upURL();

		if(k.equals(last, true))
			break;

		last = k;
	}

	while(doSearch(root))
	{}
}

void SQ_TreeView::slotOpened(KFileTreeViewItem *item)
{
	if(!item) return;

	doSearch(root);
}

bool SQ_TreeView::doSearch(KFileTreeBranch *branch)
{
	if(paths.empty())
		return false;

	QValueList<QString>::iterator it = paths.begin();

	KFileTreeViewItem *found = findItem(branch, *it);

	if(!found)
		return false;

	paths.erase(it);
	populateItem(found);

	itemsToClose->prepend(found);

	return true;
}

bool SQ_TreeView::configVisible() const
{
	return vis;
}

void SQ_TreeView::setShown(bool shown)
{
	vis = shown;

	KFileTreeView::setShown(shown);
}

void SQ_TreeView::showEvent(QShowEvent *)
{
	if(!pendingURL.isEmpty())
	{
		KURL url = pendingURL;

		pendingURL = KURL();

		emit newURL(url);
	}
}

SQ_TreeView* SQ_TreeView::instance()
{
	return tree;
}
