/***************************************************************************
                          sq_rubberband.cpp  -  description
                             -------------------
    begin                : June 2 2006
    copyright            : (C) 2006 by Baryshev Dmitry
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

#include <kcursor.h>

#include <qpainter.h>
#include <qstyle.h>

#include "sq_rubberband.h"

SQ_RubberBand::SQ_RubberBand(QWidget *parent, const char *name)
    : QWidget(parent, name), pressed(false), virt_x(-1)
{
    setCursor(KCursor::SizeHorCursor);
    setFixedWidth(6);
}

SQ_RubberBand::~SQ_RubberBand()
{}

void SQ_RubberBand::mousePressEvent(QMouseEvent *e)
{
    pressed = true;
    virt_x = mapToParent(e->pos()).x();
}

void SQ_RubberBand::mouseMoveEvent(QMouseEvent *e)
{
    mapped_x = mapToParent(e->pos()).x();

    if(pressed && mapped_x > 0 && mapped_x < parentWidget()->width()-width() && 
        (virt_x - mapped_x > width() || mapped_x - virt_x > width()))
    {
        emit newx(mapped_x);
        virt_x = mapped_x;
    }
}

void SQ_RubberBand::mouseReleaseEvent(QMouseEvent *)
{
    pressed = false;
    virt_x = -1;
}

void SQ_RubberBand::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    style().drawPrimitive(QStyle::PE_Splitter, &p, rect(),
                                       colorGroup(), QStyle::Style_Horizontal);
}

#include "sq_rubberband.moc"
