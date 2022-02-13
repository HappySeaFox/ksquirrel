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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qstring.h>
#include <qpainter.h>

#include <kwordwrap.h>

#include "sq_filethumbviewitem.h"

SQ_FileThumbViewItem::SQ_FileThumbViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi):
    KFileIconViewItem(parent, text, pixmap, fi), m_listed(false)
{}

SQ_FileThumbViewItem::~SQ_FileThumbViewItem()
{}

void SQ_FileThumbViewItem::paintItem(QPainter *p, const QColorGroup &cg)
{
    KIconViewItem::paintPixmap(p, cg);
    paintText(p, cg);
}

void SQ_FileThumbViewItem::paintText(QPainter *p, const QColorGroup &cg)
{
    QRect rectText = textRect(false);
    QRect rc = pixmapRect(false);

    rectText.setX(rc.x());
    rectText.setWidth(rc.width()-2);

    if(isSelected())
    {
        p->fillRect(rectText, cg.highlight());
        p->setPen(QPen(cg.highlightedText()));
    }
    else
    {
        if(iconView()->itemTextBackground() != NoBrush)
            p->fillRect(rectText, iconView()->itemTextBackground());

        p->setPen(cg.text());
    }

    wordWrap()->drawText(p, textRect(false).x(), textRect(false).y(), AlignCenter);
}

void SQ_FileThumbViewItem::paintFocus(QPainter *, const QColorGroup &)
{}
