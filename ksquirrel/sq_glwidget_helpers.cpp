/***************************************************************************
                          sq_glwidget_helpers.cpp  -  description
                             -------------------
    begin                : Пнд Ноя 7 2005
    copyright            : (C) 2005 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
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
#include <qrect.h>
#include <qpoint.h>
#include <qpointarray.h>

#include <algorithm>

#include <ktoolbar.h>

#include <ksquirrel-libs/fmt_defs.h>

#ifdef SQ_HAVE_KEXIF
#include <libkexif/kexifdata.h>
#endif

#include "sq_glwidget_helpers.h"

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

int SQ_GLHelpers::roundAngle(int curangle)
{
    int sign = (curangle < 0 ? -1 : 1);
    curangle = std::abs(curangle);

    if((curangle > 0 && curangle < 45) || (curangle >= 315 && curangle < 360))
        curangle = 0;
    else if(curangle >= 45 && curangle < 135)
        curangle = 90;
    else if(curangle >= 135 && curangle < 225)
        curangle = 180;
    else if(curangle >= 225 && curangle < 315)
        curangle = 270;

    curangle *= sign;

    return curangle;
}

void SQ_GLHelpers::subRotation(QWMatrix &wm, int curangle, int orient)
{
    curangle = SQ_GLHelpers::roundAngle(curangle);

#ifdef SQ_HAVE_KEXIF
    switch(orient)
    {
        case KExifData::ROT_180:      curangle -= 180; break;
        case KExifData::ROT_90_HFLIP:
        case KExifData::ROT_90:
        case KExifData::ROT_90_VFLIP: curangle -= 90; break;
        case KExifData::ROT_270:      curangle -= 270; break;

        default: ;
    }
#endif

    switch(curangle)
    {
        case -180:
        case 180: wm.rotate(180); break;

        case -270:
        case 90:  wm.rotate(90);  break;

        case -90:
        case 270: wm.rotate(270); break;

        default: ;
    }
}

bool SQ_GLHelpers::normalizeSelection(int &sx, int &sy, int &sw, int &sh, int w, int h, const QWMatrix &matr, int curangle, int orient)
{
    QWMatrix wm = matr;

    SQ_GLHelpers::subRotation(wm, curangle, orient);

    if(!wm.isIdentity())
    {
        int ax = -w/2 + sx;
        int ay = h/2  - sy;

        QPointArray pa(4), pb;

        pa.setPoint(0, ax,    ay-sh);
        pa.setPoint(1, ax+sw, ay-sh);
        pa.setPoint(2, ax+sw, ay);
        pa.setPoint(3, ax,    ay);

        pb = wm.map(pa);

        int fx, fy, fx2, fy2;
        fx = std::min(std::min(pb.point(0).x(), pb.point(1).x()), std::min(pb.point(2).x(), pb.point(3).x()));
        fy = std::max(std::max(pb.point(0).y(), pb.point(1).y()), std::max(pb.point(2).y(), pb.point(3).y()));

        fx2 = std::max(std::max(pb.point(0).x(), pb.point(1).x()), std::max(pb.point(2).x(), pb.point(3).x()));
        fy2 = std::min(std::min(pb.point(0).y(), pb.point(1).y()), std::min(pb.point(2).y(), pb.point(3).y()));

        sx = fx;
        sy = fy;
        sw = fx2 - fx;
        sh = fy - fy2;

        sx += w/2;
        sy = h/2-sy;
    }

    if(sx > w || sy > h || sx + sw < 0 || sy + sh < 0)
        return false;

    if(sx < 0) { sw = sw+sx; sx = 0; }
    if(sy < 0) { sh = sh+sy; sy = 0; }

    if(sx + sw > w) sw = w - sx;
    if(sy + sh > h) sh = h - sy;

    return (sw && sh);
}

void SQ_GLHelpers::scanLine0(RGBA *data, RGBA *scan, int rw, int w, int h, int y, int flip)
{
    if(flip == 1)
    {
        data = data + rw*y + w-1;

        for(int i = 0;i < w;i++)
        {
            *scan = *data;

            scan++;
            data--;
        }
    }
    else if(flip == 2)
    {
        data = data + rw*(h-1-y);

        for(int i = 0;i < w;i++)
        {
            *scan = *data;

            scan++;
            data++;
        }
    }
    else
        memcpy(scan, data + rw*y, w * sizeof(RGBA));
}

void SQ_GLHelpers::scanLine90(RGBA *data, RGBA *scan, int rw, int w, int h, int y, int flip)
{
    if(flip == 2)
    {
        data = data + y;

        for(int i = 0;i < h;i++)
        {
            *scan = *data;

            scan++;
            data += rw;
        }
    }
    else
    {
        data = flip == 1 ? (data + rw*(h-1) + w-y-1) : (data + rw*(h-1) + y);

        for(int i = 0;i < h;i++)
        {
            *scan = *data;

            scan++;
            data -= rw;
        }
    }
}

void SQ_GLHelpers::scanLine180(RGBA *data, RGBA *scan, int rw, int w, int h, int y, int flip)
{
    if(flip == 1)
    {
        data = data + rw*(h-1-y);

        memcpy(scan, data, w * sizeof(RGBA));
    }
    else
    {
        data = flip == 2 ? (data + rw*y + w-1) : (data + rw*(h-1-y) + w-1);

        for(int i = 0;i < w;i++)
        {
            *scan = *data;

            scan++;
            data--;
        }
    }
}

void SQ_GLHelpers::scanLine270(RGBA *data, RGBA *scan, int rw, int w, int h, int y, int flip)
{
    if(flip == 2)
    {
        data = data + rw*(h-1) + w-y-1;

        for(int i = 0;i < h;i++)
        {
            *scan = *data;

            scan++;
            data -= rw;
        }
    }
    else
    {
        data = flip == 1 ? (data + y) : (data + w-y-1);

        for(int i = 0;i < h;i++)
        {
            *scan = *data;

            scan++;
            data += rw;
        }
    }
}
