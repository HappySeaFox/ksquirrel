/***************************************************************************
                          sq_label.cpp  -  description
                             -------------------
    begin                : June 10 2005
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

#include <qpainter.h>

#include "sq_label.h"

SQ_Label::SQ_Label(QWidget *parent, const char *name) : QWidget(parent, name)
{}

SQ_Label::~SQ_Label()
{}

void SQ_Label::paintEvent(QPaintEvent *)
{
	if(text.isEmpty())
		return;

	QPainter paint(this);

	QFont font = paint.font();
	font.setBold(true);
	font.setPointSize(8);
	paint.setFont(font);

	paint.translate((width() + paint.fontMetrics().height()) / 2, (height() + paint.fontMetrics().width(text)) / 2);
      paint.rotate(-90);
	paint.drawText(0, 0, text);
}

void SQ_Label::setText(const QString &ntext)
{
	if(ntext == text)
		return;

	text = ntext;

	update();
}
