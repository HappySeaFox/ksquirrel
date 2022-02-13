/***************************************************************************
                          sq_fileiconview.h  -  description
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

#ifndef SQ_FILEICONVIEW_H
#define SQ_FILEICONVIEW_H

#include <kfileiconview.h>

#include <qwidget.h>
#include <qpoint.h>

class SQ_FileIconViewItem : public KFileIconViewItem
{
	public:
		SQ_FileIconViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi);
		~SQ_FileIconViewItem();

	protected:
		virtual void paintFocus(QPainter *p, const QColorGroup &cg);
};

class SQ_FileIconView : public KFileIconView
{
    Q_OBJECT

	public:
		SQ_FileIconView(QWidget *parent = 0, const char *name = "");
		~SQ_FileIconView();

		SQ_FileIconViewItem* viewItem(const KFileItem *item);

		virtual void updateView(const KFileItem *i);
		virtual void updateView(bool b);
		virtual void insertItem(KFileItem *i);

	protected:
		QDragObject *dragObject();
		void dragEnterEvent(QDragEnterEvent *);
		virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);

	private:
		void initItem(SQ_FileIconViewItem *item, const KFileItem *i);

	protected slots:
		void slotSelected(QIconViewItem *item, const QPoint &point);

};


#endif
