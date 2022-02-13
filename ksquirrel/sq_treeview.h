/***************************************************************************
                          sq_treeview.h  -  description
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

#ifndef SQ_TREEVIEW_H
#define SQ_TREEVIEW_H

#include <qstringlist.h>

#include <kfiletreeview.h>
#include <kurl.h>

class SQ_TreeView : public KFileTreeView
{
    Q_OBJECT

	public:
		SQ_TreeView(QWidget *parent = 0, const char *name = 0);
		~SQ_TreeView();

		void emitNewURL(const KURL &url);
		bool configVisible() const;

	protected:
		void showEvent(QShowEvent *);

	private:
		void populateItem(KFileTreeViewItem *);
		bool doSearch(KFileTreeBranch *);
		void collapseOpened();

	public slots:
		void slotItemExecuted(QListViewItem*);
		void slotNewURL(const KURL &url);
		void slotOpened(KFileTreeViewItem *);
		void setShown(bool shown);

	signals:
		void newURL(const KURL &url);

	private:
		KFileTreeBranch *root, *home;
		QStringList paths;
		KFileTreeViewItemList *itemsToClose;
		bool vis;
		KURL pendingURL;
};

#endif
