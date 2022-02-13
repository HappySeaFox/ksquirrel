/***************************************************************************
                          sq_label.cpp  -  description
                             -------------------
    begin                : June 10 2005
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

#include <qpainter.h>

#include "sq_label.h"

#define MARGIN 15

SQ_Label::SQ_Label(QWidget *parent, const char *name) : QWidget(parent, name), single(false)
{}

SQ_Label::~SQ_Label()
{}

void SQ_Label::paintEvent(QPaintEvent *)
{
    if((single && ltext.isEmpty() && rtext.isEmpty()) || (!single && ltext.isEmpty()))
        return;

    QPainter paint(this);

    QFont font = paint.font();
    font.setBold(true);
    font.setPointSize(10);
    paint.setFont(font);

    if(!single)
    {
        paint.translate((width() + paint.fontMetrics().height()) / 2, height());
        paint.rotate(-90);
        paint.drawText(MARGIN, 0, ltext);

        paint.translate(height() - paint.fontMetrics().width(rtext), 0);
        paint.drawText(-MARGIN, 0, rtext);
    }
    else
    {
        paint.translate((width() + paint.fontMetrics().height()) / 2,
                        (height() + paint.fontMetrics().width(ltext)) / 2);
        paint.rotate(-90);
        paint.drawText(0, 0, ltext);
    }
}

void SQ_Label::setText(const QString &lt, const QString &rt)
{
    ltext = lt;
    rtext = rt;

    update();
}

void SQ_Label::setText(const QString &lt)
{
    ltext = lt;

    update();
}

void SQ_Label::setSingle(bool s)
{
    single = s;
}
