/***************************************************************************
                          sq_filethumbviewitem.cpp  -  description
                             -------------------
    begin                : ??? ??? 12 2004
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

#include <qstring.h>
#include <qpainter.h>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "sq_filethumbviewitem.h"

SQ_FileThumbViewItem::SQ_FileThumbViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi):
		KFileIconViewItem(parent, text, pixmap, fi)
{}

SQ_FileThumbViewItem::~SQ_FileThumbViewItem()
{}

QString SQ_FileThumbViewItem::getFullInfo() const
{
	QString s = i18n("Type: %1\nDimensions: %2\nBits per pixel: %3\nColor model: %4\nCompression: %5\nNumber of frames: %6\nUncompressed size: %7")
			.arg(info.info.type)
			.arg(info.info.dimensions)
			.arg(info.info.bpp)
			.arg(info.info.color)
			.arg(info.info.compression)
			.arg(info.info.frames)
			.arg(info.info.uncompressed);

	return s;
}

void SQ_FileThumbViewItem::setInfo(const SQ_Thumbnail &t)
{
	info = t;
}

void SQ_FileThumbViewItem::paintItem(QPainter *p, const QColorGroup &cg)
{
	QIconView *view = iconView();
	QPixmap *unknown_icon = new QPixmap(KGlobal::iconLoader()->loadIcon("unkonwn", KIcon::Desktop, 32));
	QColor highlight = cg.highlight();

	if(!view)
		return;

	p->save();

	if(isSelected())
		p->setPen(cg.highlightedText());
	else
		p->setPen(cg.text());

	calcTmpText();

	bool textOnBottom = view->itemTextPos() == QIconView::Bottom;

	int dim;

	QPixmap *pix = pixmap() ? pixmap() : unknown_icon;

	QRect opr = pix->rect();

	if(textOnBottom)
		dim = pix->width();
	else
		dim = pix->height();

	if(isSelected())
	{
		if(textOnBottom)
			p->drawPixmap(x() + (width() - dim) / 2, y(), *pix, 0, 0, opr.width(), opr.height());
		else
			p->drawPixmap(x() , y() + (height() - dim) / 2, *pix, 0, 0, opr.width(), opr.height());

//		p->fillRect(pixmapRect(false), QBrush(highlight, QBrush::Dense4Pattern));
		p->setPen(QPen(highlight, 3));
		p->drawRect(pixmapRect(false));
	}
	else
	{
		if(textOnBottom)
			p->drawPixmap(x() + (width() - dim) / 2, y(), *pix);
		else
			p->drawPixmap(x() , y() + (height() - dim) / 2, *pix);
	}

	p->save();

	if(isSelected())
	{
		p->fillRect(textRect(false), highlight);
		p->setPen(QPen(cg.highlightedText()));
	}
	else
		if(view->itemTextBackground() != NoBrush)
					p->fillRect(textRect(false), view->itemTextBackground());

	int align = AlignHCenter;

	if(view->wordWrapIconText())
		align |= WordBreak | BreakAnywhere;

	p->drawText(textRect(false), align, (view->wordWrapIconText() ? text() : tempText()));

	p->restore();
	p->restore();
}

void SQ_FileThumbViewItem::paintFocus(QPainter *, const QColorGroup &)
{}
