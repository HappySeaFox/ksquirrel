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

#include "sq_treeview.h"
#include "sq_specialpage.h"

#include <qdir.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"

#include <qheader.h>

SQ_TreeView::SQ_TreeView(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
	QPixmap homePix = sqLoader->loadIcon("gohome", KIcon::Desktop, KIcon::SizeSmall);
	QPixmap rootPix = sqLoader->loadIcon("hdd_mount", KIcon::Desktop, KIcon::SizeSmall);   

	KFileTreeBranch *root = addBranch(KURL(QDir::rootDirPath()), " /", rootPix);
	KFileTreeBranch *home = addBranch(KURL(QDir().home().absPath()), " Home", homePix);
	addColumn("Name");

	setDirOnlyMode(root, true);
	setDirOnlyMode(home, true);

	setRootIsDecorated(true);
	setShowSortIndicator(true);

	setCurrentItem(home->root());
	home->setOpen(true);

	connect(this, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotSetupClick(QListViewItem*)));
	connect(this, SIGNAL(spacePressed(QListViewItem*)), SIGNAL(executed(QListViewItem*)));
	connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotDoubleClicked(QListViewItem*)));

	header()->hide();
}

SQ_TreeView::~SQ_TreeView()
{}

void SQ_TreeView::slotSetupClick(QListViewItem *item)
{
	if(!item) return;

	KFileTreeViewItem *cur = static_cast<KFileTreeViewItem*>(item);
	KURL Curl = cur->url();

	sqWStack->setURLfromtree(Curl);
}

void SQ_TreeView::slotDoubleClicked(QListViewItem *item)
{
	if(!item) return;

	slotSetupClick(item);
	emit executed(item);
}
