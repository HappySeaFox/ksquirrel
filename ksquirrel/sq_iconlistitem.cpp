/***************************************************************************
                          sq_iconlistitem.cpp  -  description
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

#include <qlistbox.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qpixmap.h>

#include "sq_iconlistitem.h"

SQ_IconListItem::SQ_IconListItem(QListBox *listbox, const QPixmap &pixmap, const QString &text)
		: QListBoxItem(listbox)
{
	mPixmap = pixmap;

	if(mPixmap.isNull())
		mPixmap = defaultPixmap();

	setText(text);
	mMinimumWidth = 0;
}

int SQ_IconListItem::expandMinimumWidth( int width )
{
	mMinimumWidth = QMAX(mMinimumWidth, width);

	return mMinimumWidth;
}

const QPixmap& SQ_IconListItem::defaultPixmap()
{
	static QPixmap *pix=0;

	if(pix == 0)
	{
		pix = new QPixmap( 32, 32 );
		QPainter p(pix);
		p.eraseRect(0, 0, pix->width(), pix->height());
		p.setPen(Qt::red);
		p.drawRect(0, 0, pix->width(), pix->height());
		p.end();

		QBitmap mask(pix->width(), pix->height(), true);
		mask.fill(Qt::black);
		p.begin(&mask);
		p.setPen(Qt::white);
		p.drawRect(0, 0, pix->width(), pix->height());
		p.end();

		pix->setMask(mask);
	}

	return *pix;
}

void SQ_IconListItem::paint(QPainter *painter)
{
	QFontMetrics fm = painter->fontMetrics();
	int ht = fm.boundingRect(0, 0, 0, 0, Qt::AlignCenter, text()).height();
	int wp = mPixmap.width();
	int hp = mPixmap.height();

	painter->drawPixmap((mMinimumWidth-wp)/2, 5, mPixmap);

	if(text().isEmpty() == false)
		painter->drawText(0, hp+7, mMinimumWidth, ht, Qt::AlignCenter, text());
}

int SQ_IconListItem::height( const QListBox *lb ) const
{
	if(text().isEmpty() == true)
		return mPixmap.height();
	else
	{
		int ht = lb->fontMetrics().boundingRect(0, 0, 0, 0, Qt::AlignCenter, text()).height();
		return (mPixmap.height() + ht + 10);
	}
}

int SQ_IconListItem::width( const QListBox *lb ) const
{
	int wt = lb->fontMetrics().boundingRect(0, 0, 0, 0, Qt::AlignCenter, text()).width() + 10;
	int wp = mPixmap.width() + 10;
	int w  = QMAX(wt, wp);

	return QMAX(w, mMinimumWidth);
}
