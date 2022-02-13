/***************************************************************************
                          sq_iconlistitem.h  -  description
                             -------------------
    begin                : ??? ??? 19 2004
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

#ifndef SQ_ICONLISTITEM_H
#define SQ_ICONLISTITEM_H

#include <qpainter.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <klistview.h>

class QListBox;

class SQ_IconListItem : public QListBoxItem
{
	public:
		SQ_IconListItem(QListBox *listbox, const QPixmap &pixmap, const QString &text);

		virtual int height(const QListBox *lb) const;
		virtual int width(const QListBox *lb) const;
		int expandMinimumWidth(int width);

	protected:
		const QPixmap &defaultPixmap();
		void paint(QPainter *painter);

	private:
		QPixmap mPixmap;
		int mMinimumWidth;
};

#endif
