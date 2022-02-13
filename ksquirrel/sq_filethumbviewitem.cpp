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

#include <klocale.h>
#include <kglobalsettings.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kwordwrap.h>

#include "sq_filethumbviewitem.h"

SQ_FileThumbViewItem::SQ_FileThumbViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi):
    KFileIconViewItem(parent, text, pixmap, fi)
{}

SQ_FileThumbViewItem::~SQ_FileThumbViewItem()
{}

/*
 *  Get additional information as QString object.
 */
QString SQ_FileThumbViewItem::fullInfo() const
{
    QString s = i18n("<tr><td align=left>Type:</td><td align=left>%1</td></tr><tr><td align=left>Dimensions:</td><td align=left>%2</td></tr><tr><td align=left>Bits per pixel:</td><td align=left>%3</td></tr><tr><td align=left>Color space:</td><td align=left>%4</td></tr><tr><td align=left>Compression:</td><td align=left>%5</td></tr><tr><td align=left>Number of frames:</td><td align=left>%6</td></tr><tr><td align=left>Uncompressed size:</td><td align=left>%7</td></tr>")
            .arg(info.info.type)
            .arg(info.info.dimensions)
            .arg(info.info.bpp)
            .arg(info.info.color)
            .arg(info.info.compression)
            .arg((info.info.frames > 1 ? i18n("at least 2") : "1")) // since 0.7.0 thumbnail loader doesn't load
                                                                                                      // more than 2 pages (performance reason)
            .arg(info.info.uncompressed);

    return s;
}

/*
 *  Set additional information.
 */
void SQ_FileThumbViewItem::setInfo(const SQ_Thumbnail &t)
{
    info = t;
}

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
