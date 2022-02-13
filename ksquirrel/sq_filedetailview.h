/***************************************************************************
                          sq_filedetailview.h  -  description
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

#ifndef SQ_FILEDETAILVIEW_H
#define SQ_FILEDETAILVIEW_H

#include <qwidget.h>
#include <kfiledetailview.h>

class SQ_FileListViewItem : public KFileListViewItem
{
	public:
		SQ_FileListViewItem(QListView *parent, KFileItem *fi);
		~SQ_FileListViewItem();

	protected:
		virtual void paintFocus(QPainter *, const QColorGroup &, const QRect &r);
};

class SQ_FileDetailView : public KFileDetailView
{
    Q_OBJECT

	public:
		SQ_FileDetailView(QWidget* parent, const char* name);
		~SQ_FileDetailView();

		virtual void insertItem(KFileItem *i);

	protected:
		QDragObject *dragObject();
		void initItem(SQ_FileListViewItem *item, const KFileItem *i);
		virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);
};

#endif
