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
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kwordwrap.h>

#include "sq_filethumbviewitem.h"

SQ_FileThumbViewItem::SQ_FileThumbViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi):
		KFileIconViewItem(parent, text, pixmap, fi)
{}

SQ_FileThumbViewItem::~SQ_FileThumbViewItem()
{}

QString SQ_FileThumbViewItem::getFullInfo() const
{
	QString s = i18n("Type: %1\nDimensions: %2\nBits per pixel: %3\nColor space: %4\nCompression: %5\nNumber of frames: %6\nUncompressed size: %7")
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
	QIconView* view = iconView();

	Q_ASSERT(view);

	if(!view) return;

	if(!wordWrap())
	{
		kdWarning() << "KIconViewItem::paintItem called but wordwrap not ready - calcRect not called, or aborted!" << endl;
		return;
	}

    p->save();

    paintPixmap(p, cg);
    paintText(p, cg);

    p->restore();
}

void SQ_FileThumbViewItem::paintPixmap(QPainter *p, const QColorGroup &cg)
{
	int iconX = pixmapRect(false).x();
	int iconY = pixmapRect(false).y();

	QPixmap *pix = pixmap();

	if(isSelected())
	{
		if(pix && !pix->isNull())
		{
			QPixmap selectedPix = KPixmapEffect::selectedPixmap(KPixmap(*pix), cg.highlight());
			p->drawPixmap(iconX, iconY, selectedPix);
//			p->setPen(QPen(cg.highlight(), 3));
//			p->drawRect(pixmapRect(false));
		}
	}
	else
	{
		p->drawPixmap(iconX, iconY, *pix);
	}
}

void SQ_FileThumbViewItem::paintText(QPainter *p, const QColorGroup &cg)
{
	calcTmpText();

	int textX = textRect(false).x();
	int textY = textRect(false).y();

	if(isSelected())
	{
		p->fillRect(textRect(false), cg.highlight());
		p->setPen(QPen(cg.highlightedText()));
	}
	else
	{
		if(iconView()->itemTextBackground() != NoBrush)
			p->fillRect(textRect(false), iconView()->itemTextBackground());

		p->setPen(cg.text());
	}

	int align = (iconView()->itemTextPos() == QIconView::Bottom) ? AlignHCenter : AlignAuto;
	wordWrap()->drawText(p, textX, textY, align);
}

void SQ_FileThumbViewItem::paintFocus(QPainter *, const QColorGroup &)
{}
