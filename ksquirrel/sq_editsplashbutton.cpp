/***************************************************************************
                          sq_editsplashbutton.cpp  -  description
                             -------------------
    begin                : ??? May 5 2005
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

#include "sq_editsplashbutton.h"

#define SQ_QUAD_W 118
#define SQ_QUAD_H  140
#define SQ_PIXMAP_DIM 96

SQ_EditSplashButton::SQ_EditSplashButton(QWidget *parent, const char *name )
						: QWidget(parent,name), on(false), pressed(false)
{
	setFixedWidth(SQ_QUAD_W);
	setFixedHeight(SQ_QUAD_H);
}

SQ_EditSplashButton::~SQ_EditSplashButton()
{}

void SQ_EditSplashButton::mouseReleaseEvent(QMouseEvent *e)
{
	e->accept();

	pressed = false;

	repaint(false);

	if(on)
		emit clicked();
}

void SQ_EditSplashButton::mousePressEvent(QMouseEvent *e)
{
	e->accept();

	pressed = true;

	repaint(false);
}

void SQ_EditSplashButton::enterEvent(QEvent *)
{
	on = true;

	repaint(false);
}

void SQ_EditSplashButton::leaveEvent(QEvent *)
{
	on = false;

	repaint(false);
}

void SQ_EditSplashButton::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	p.setFont(font());

	QColor h = colorGroup().highlight();

	if(!on && !pressed)
		p.fillRect(rect(), h);
	else if(on && !pressed)
	{
		p.setBrush(h.dark(120));
		p.setPen(h.dark(190));
		p.drawRoundRect(rect(), 20, 20);
	}
	else// if(on && pressed)
	{
		p.setBrush(h.dark(140));
		p.setPen(h.dark(190));
		p.drawRoundRect(rect(), 20, 20);
	}

	if(!pix.isNull())
	{
		int delta = (SQ_QUAD_W - SQ_PIXMAP_DIM) / 2;
		p.drawPixmap(delta, delta, pix);
	}

	if(!text.isEmpty())
	{
		QRect rr(0, SQ_QUAD_W, SQ_QUAD_W, SQ_QUAD_H-SQ_QUAD_W);
		p.setPen(colorGroup().highlightedText());
		p.drawText(rr, Qt::AlignCenter, text);
	}
}

void SQ_EditSplashButton::setPixmap(const QPixmap &_pix)
{
	pix = QPixmap();

	if(_pix.isNull())
		return;

	if(_pix.width() != SQ_PIXMAP_DIM || _pix.height() != SQ_PIXMAP_DIM)
		return;

	pix = _pix;
}
