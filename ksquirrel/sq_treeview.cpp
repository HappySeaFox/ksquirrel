/***************************************************************************
                          sq_treeview.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
    email                : squirrel-sf@yandex.ru
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

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_treeview.h"
#include "sq_specialpage.h"


SQ_TreeView::SQ_TreeView(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
	QPixmap homePix = sqLoader->loadIcon("gohome", KIcon::Desktop, KIcon::SizeSmall);
	QPixmap rootPix = sqLoader->loadIcon("hdd_mount", KIcon::Desktop, KIcon::SizeSmall);   

	root = addBranch(KURL(QDir::rootDirPath()), " /", rootPix);
	home = addBranch(KURL(QDir().home().absPath()), " Home", homePix);

	addColumn("Name");

	setDirOnlyMode(root, true);
	setDirOnlyMode(home, true);

	setRootIsDecorated(true);
	setShowSortIndicator(true);

	setCurrentItem(home->root());
	home->setOpen(true);

	connect(this, SIGNAL(spacePressed(QListViewItem*)), SIGNAL(executed(QListViewItem*)));
	connect(this, SIGNAL(returnPressed(QListViewItem*)), SIGNAL(executed(QListViewItem*)));

	if(sqConfig->readNumEntry("Fileview", "sync type", 0) == 1)
		connect(this, SIGNAL(executed(QListViewItem*)), SLOT(slotItemExecuted(QListViewItem*)));

	connect(this, SIGNAL(newURL(const KURL&)), SLOT(slotNewURL(const KURL&)));
		
	header()->hide();
}

SQ_TreeView::~SQ_TreeView()
{}

void SQ_TreeView::slotItemExecuted(QListViewItem *item)
{
	if(!item) return;

	KFileTreeViewItem *cur = (KFileTreeViewItem*)item;
	KURL Curl = cur->url();

	sqWStack->setURLfromtree(Curl);
}

void SQ_TreeView::emitNewURL(const KURL &url)
{
	emit newURL(url);
}

void SQ_TreeView::slotNewURL(const KURL &url)
{
//	KFileTreeViewItem *item = sqTree->findItem(root, url.path());

//	if(!item) return;

	root->populate(url, root->root());
}
