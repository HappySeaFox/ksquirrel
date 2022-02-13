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

#include <kfiledetailview.h>

/*
 *  SQ_FileDetailView represents detailed view in filemanager.
 */

/*
 *  SQ_FileListViewItem represents one item in detaild view.
 */
class SQ_FileListViewItem : public KFileListViewItem
{
	public:
		SQ_FileListViewItem(QListView *parent, KFileItem *fi);
		SQ_FileListViewItem(QListView *parent, const QString &text, const QPixmap &icon, KFileItem *fi);
		~SQ_FileListViewItem();

	protected:
		/*
		 *  Reimplement paintFocus() to ignore painting focus.
		 */
		virtual void paintFocus(QPainter *, const QColorGroup &, const QRect &r);
};

class SQ_FileDetailView : public KFileDetailView
{
    Q_OBJECT

	public:
		SQ_FileDetailView(QWidget* parent, const char* name);
		~SQ_FileDetailView();

		/*
		 *  Reimplement insertItem() to enable/disable inserting
		 *  directories (depends on current settings).
		 */
		virtual void insertItem(KFileItem *i);

		/*
		 *  Clear current view and insert "..".
		 */
		virtual void clearView();

		/*
		 *  Insert ".." item.
		 */
		void insertCdUpItem(const KURL &baseurl);

	protected:
		/*
		 *  Internal. Set item's sorting key.
		 */
		void initItem(SQ_FileListViewItem *item, const KFileItem *i);

		/*
		 *  On double click execute item or
		 *  invoke default browser in current url.
		 */
		virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);

		/*
		 *  Accept dragging.
		 */
		virtual void dragEnterEvent(QDragEnterEvent *);

	protected slots:

		/*
		 *  Somebody dropped urls in viewport. Let's execute popup menu with
		 *  file actions.
		 */
		void slotDropped(QDropEvent *e, const KURL::List &urls, const KURL &url);

	private:
		QPixmap	dirPix;
};

#endif
