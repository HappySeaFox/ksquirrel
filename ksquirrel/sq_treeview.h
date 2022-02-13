/***************************************************************************
                          sq_treeview.h  -  description
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

#ifndef SQ_TREEVIEW_H
#define SQ_TREEVIEW_H

#include <kfiletreeview.h>


class SQ_TreeView : public KFileTreeView
{
    Q_OBJECT

	private:
        
	public:
		SQ_TreeView(QWidget *parent = 0, const char *name = 0);
		~SQ_TreeView();

	public slots:
		void slotItemExecuted(QListViewItem*);
};

#endif
