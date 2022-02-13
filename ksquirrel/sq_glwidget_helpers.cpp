/***************************************************************************
                          sq_glwidget_helpers.cpp  -  description
                             -------------------
    begin                : Пнд Ноя 7 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#include <qwmatrix.h>

#include <ktoolbar.h>

#include "sq_glwidget_helpers.h"

#ifdef SQ_HAVE_KEXIF
#include <libkexif/kexifdata.h>
#include <qrect.h>
#include <qpoint.h>
#include <qwmatrix.h>
#include <qpointarray.h>
#include <algorithm>
#endif

SQ_ToolButtonPopup::SQ_ToolButtonPopup(const QPixmap &pix, const QString &textLabel, KToolBar *parent)
    : KToolBarButton(pix, -1, parent, 0, textLabel)
{
    setFixedWidth(SQ_ToolButton::fixedWidth());
}

SQ_ToolButtonPopup::~SQ_ToolButtonPopup()
{}

SQ_ToolButton::SQ_ToolButton(const QIconSet &iconSet, const QString &textLabel,
    QObject *receiver, const char *slot, KToolBar *parent, const char *name)
    : QToolButton(iconSet, textLabel, QString::null, receiver, slot, parent, name)
{
    setFixedWidth(SQ_ToolButton::fixedWidth());
}

SQ_ToolButton::~SQ_ToolButton()
{}

bool normalizeSelection(int &sx, int &sy, int &sw, int &sh, int w, int h, const QWMatrix &wm)
{
#ifdef SQ_HAVE_KEXIF
    if(!wm.isIdentity())
    {
//        printf("NORMALIZE1 %d,%d %dx%d\n", sx, sy, sw, sh);
        int ax = -w/2 + sx;
        int ay = h/2  - sy;

//        printf("NORMALIZE2 %d,%d %dx%d\n", ax, ay, sw, sh);

        QPointArray pa(4), pb;

        pa.setPoint(0, ax,    ay-sh);
        pa.setPoint(1, ax+sw, ay-sh);
        pa.setPoint(2, ax+sw, ay);
        pa.setPoint(3, ax,    ay);

        pb = wm.map(pa);
/*
        printf("P %d,%d\n", pb.point(0).x(), pb.point(0).y());
        printf("P %d,%d\n", pb.point(1).x(), pb.point(1).y());
        printf("P %d,%d\n", pb.point(2).x(), pb.point(2).y());
        printf("P %d,%d\n", pb.point(3).x(), pb.point(3).y());
*/
        int fx, fy, fx2, fy2;
        fx = std::min(std::min(pb.point(0).x(), pb.point(1).x()), std::min(pb.point(2).x(), pb.point(3).x()));
        fy = std::max(std::max(pb.point(0).y(), pb.point(1).y()), std::max(pb.point(2).y(), pb.point(3).y()));

        fx2 = std::max(std::max(pb.point(0).x(), pb.point(1).x()), std::max(pb.point(2).x(), pb.point(3).x()));
        fy2 = std::min(std::min(pb.point(0).y(), pb.point(1).y()), std::min(pb.point(2).y(), pb.point(3).y()));

//        printf("FF %d %d %d %d\n", fx, fy, fx2, fy2);

        sx = fx;
        sy = fy;
        sw = fx2 - fx;
        sh = fy - fy2;

//        printf("RECT %d,%d %dx%d\n", sx, sy, sw, sh);

        sx += w/2;
        sy = h/2-sy;

//        printf("*** SEL %d,%d %dx%d\n", sx, sy, sw, sh);
    }
#endif

    if(sx > w || sy > h || sx + sw < 0 || sy + sh < 0)
        return false;

    if(sx < 0) { sw = sw+sx; sx = 0; }
    if(sy < 0) { sh = sh+sy; sy = 0; }

    if(sx + sw > w) sw = w - sx;
    if(sy + sh > h) sh = h - sy;

    return (sw && sh);
}
