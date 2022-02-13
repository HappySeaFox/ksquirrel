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
#include <kdeversion.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_treeview.h"

#if KDE_VERSION < 306
	#define SAME_URL(u1,u2,slash) u1.cmp(u2,slash)
#else
	#define SAME_URL(u1,u2,slash) u1.equals(u2,slash)
#endif

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

	setCurrentItem(root->root());
	root->setOpen(true);
	root->root()->setExpandable(true);

	connect(this, SIGNAL(spacePressed(QListViewItem*)), SIGNAL(executed(QListViewItem*)));
	connect(this, SIGNAL(returnPressed(QListViewItem*)), SIGNAL(executed(QListViewItem*)));
	connect(this, SIGNAL(executed(QListViewItem*)), SLOT(slotItemExecuted(QListViewItem*)));
	connect(this, SIGNAL(newURL(const KURL&)), SLOT(slotNewURL(const KURL&)));
		
	header()->hide();
}

SQ_TreeView::~SQ_TreeView()
{}

void SQ_TreeView::slotItemExecuted(QListViewItem *item)
{
	if(!item) return;

	int sync_type = sqConfig->readNumEntry("Fileview", "sync type", 0);

	if(sync_type == 2)
		return;

	KFileTreeViewItem *cur = (KFileTreeViewItem*)item;
	KURL Curl = cur->url();

	sqWStack->setURLfromtree(Curl);
}

void SQ_TreeView::emitNewURL(const KURL &url)
{
	emit newURL(url);
}

void SQ_TreeView::populateItem(KFileTreeViewItem *item)
{
	blockSignals(true);
	setCurrentItem(item);
	blockSignals(false);
	ensureItemVisible(item);
	item->setOpen(true);
}

void SQ_TreeView::slotNewURL(const KURL &url)
{
	QStringList folderParts;
	QStringList::Iterator folderIter,endFolderIter;
	KFileTreeViewItem* viewItem;
	KFileTreeViewItem* nextViewItem;

	QString path = url.path();
	viewItem = static_cast<KFileTreeViewItem*>(root->root());
	folderParts = QStringList::split('/', path);

	folderIter = folderParts.begin();
	endFolderIter = folderParts.end();
	for(;folderIter != endFolderIter;++folderIter)
	{
		nextViewItem = findViewItem(viewItem,*folderIter);
		
		if (nextViewItem)
		    viewItem = nextViewItem;
		else
		    break;
	}

	populateItem(viewItem);

	if(SAME_URL(viewItem->url(), url, true))
	{
//		setCurrentItem(viewItem);
//		ensureItemVisible(viewItem);
//		populateItem(viewItem);
	}
	else
	    slotSetNextUrlToSelect(url);
}

KFileTreeViewItem* SQ_TreeView::findViewItem(KFileTreeViewItem* parent,const QString& text)
{
	QListViewItem* item;

	for (item=parent->firstChild();item;item=item->nextSibling())
	{
		if (item->text(0) == text)
			return static_cast<KFileTreeViewItem*>(item);
	}

	return 0L;
}

void SQ_TreeView::slotNewTreeViewItems(KFileTreeBranch *branch, const KFileTreeViewItemList &)
{
	if(!branch)
		return;

//	printf("NEW url to select: %s\n", m_nextUrlToSelect.url().ascii());

	if(m_nextUrlToSelect.isEmpty())
		return;

//	KFileTreeViewItemListIterator it(itemList);

	QStringList paths;
	KURL k = m_nextUrlToSelect;
	
	while(true)
	{
		paths.prepend(k.path());
		k = k.upURL();

		if(k.path() == "/")
		{
		    paths.prepend("/");
		    break;
		}
	}

	const unsigned int count = paths.count();

	// ugly hack: I don't know make it works without it :(
	for(unsigned int i = 0;i < count;i++)
	    slotNewURL(m_nextUrlToSelect);
	    
	return;
/*	
	for(;it.current(); ++it)
	{
		KURL url = (*it)->url();

		printf("NEW grepping %s <=> %s\n", m_nextUrlToSelect.path().ascii(), (*it)->url().path().ascii());

		if(SAME_URL(m_nextUrlToSelect,url,true))
		{
			printf("NEW url to select: %s, it: %s\n", m_nextUrlToSelect.url().ascii(), (*it)->url().path().ascii());
			blockSignals(true);
			setCurrentItem(*it);
			blockSignals(false);
			ensureItemVisible(*it);
			(*it)->setOpen(true);
			m_nextUrlToSelect = KURL();
			return;
		}
	}
*/
}
