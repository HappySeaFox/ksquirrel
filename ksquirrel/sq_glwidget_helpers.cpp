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

#include <qpainter.h>

#include <ktoolbar.h>

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

bool normalizeSelection(int &sx, int &sy, int &sw, int &sh, int w, int h, bool flip)
{
    if(sx > w || sy > h || sx + sw < 0 || sy + sh < 0)
        return false;

    if(sx < 0) { sw = sw+sx; sx = 0; }
    if(sy < 0) { sh = sh+sy; sy = 0; }

    if(sx + sw > w) sw = w - sx;
    if(sy + sh > h) sh = h - sy;

    if(flip)
        sy = h - sy - sh;

    return (sw && sh);
}
