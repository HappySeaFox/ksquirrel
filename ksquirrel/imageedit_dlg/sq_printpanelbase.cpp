/***************************************************************************
                          sq_printpanelbase.cpp  -  description
                             -------------------
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

#include <qpixmap.h>
#include <qpainter.h>

#include "sq_printpanelbase.h"

SQ_PrintPanelBase::SQ_PrintPanelBase(QWidget *parent, const char *name) : QFrame(parent, name)
{
    setFixedWidth(PANEL_W_F);
    setFixedHeight(PANEL_H_F);

    QPixmap pix(PANEL_W_F, PANEL_H_F, -1, QPixmap::BestOptim);

    pix.fill(colorGroup().background());

    QPainter paint(&pix);

    paint.fillRect(SQ_K, SQ_K, PANEL_W, PANEL_H, QBrush(darkGray, Qt::Dense1Pattern));
    paint.fillRect(0, 0, PANEL_W, PANEL_H, QBrush(colorGroup().base()));

    setPaletteBackgroundPixmap(pix);
}

SQ_PrintPanelBase::~SQ_PrintPanelBase()
{}
