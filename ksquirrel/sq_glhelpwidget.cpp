/***************************************************************************
                          sq_glhelpwidget.cpp  -  description
                             -------------------
    begin                : ??? ??? 14 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@uandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sq_glhelpwidget.h"

SQ_GLHelpWidget::SQ_GLHelpWidget(const QString & text, QWidget * parent, const char * name)
	: QLabel(text, parent, name, Qt::WStyle_Customize|Qt::WStyle_NoBorder)
{
	setFrameShape(QFrame::GroupBoxPanel);
	setFocusPolicy(QWidget::WheelFocus);
}

SQ_GLHelpWidget::~SQ_GLHelpWidget()
{}

void SQ_GLHelpWidget::keyPressEvent(QKeyEvent *e)
{
	hide();
	e->ignore();
}

void SQ_GLHelpWidget::mousePressEvent(QMouseEvent *e)
{
	hide();
	e->ignore();
}

void SQ_GLHelpWidget::focusOutEvent(QFocusEvent *e)
{
	hide();
}
