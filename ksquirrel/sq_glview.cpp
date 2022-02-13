/***************************************************************************
                          sq_glviewspec.cpp  -  description
                             -------------------
    begin                : ??? ??? 5 2004
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

#include <qlayout.h>
#include <qlabel.h>
#include <qtoolbutton.h>

#include <kaction.h>
#include <kstatusbar.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kstatusbar.h>

#include "ksquirrel.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_config.h"
#include "sq_widgetstack.h"

SQ_GLView::SQ_GLView(QWidget *parent, const char *name) : QVBox(parent, name)
{
	separate = ((parent == 0L)?true:false);

	createContent();

	installEventFilter(this);
}

SQ_GLView::~SQ_GLView()
{}

void SQ_GLView::createContent()
{
	gl = new SQ_GLWidget(this);
	gl->glInitA();

	setStretchFactor(gl, 1);

	sbar = new KStatusBar(this);

	sbar->addWidget(sqSBDecodedBox, 0, true);
	sbar->addWidget(sqSBLoaded, 0, true);
	sbar->addWidget(sqSBGLZoom, 0, true);
	sbar->addWidget(sqSBGLAngle, 0, true);
	sbar->addWidget(sqSBGLCoord, 0, true);
	QLabel *levak = new QLabel(sbar);
	sbar->addWidget(levak, 1, true);

	sbar->setShown(sqConfig->readBoolEntry("GL view", "statusbar", true));

	if(!separate)
		return;

	QRect rect(0,0,320,200);
	QRect geom = sqConfig->readRectEntry("GL view", "geometry", &rect);

	setGeometry(geom);
}

bool SQ_GLView::isSeparate() const
{
	return separate;
}

KStatusBar* SQ_GLView::statusbar()
{
	return sbar;
}

void SQ_GLView::closeEvent(QCloseEvent *e)
{
	e->ignore();
	lower();
}

void SQ_GLView::reparent(QWidget *parent, const QPoint &p, bool showIt)
{
	QVBox::reparent(parent, p, showIt);

	separate = (parent) ? false : true;
}

bool SQ_GLView::eventFilter(QObject *watched, QEvent *e)
{/*
	if(e->type() == QEvent::WindowDeactivate || e->type() == QEvent::Hide)
	{
		gl->stopAnimation();
		return true;
	}
	else if(e->type() == QEvent::WindowActivate || e->type() == QEvent::Show)
	{
		if(!gl->manualBlocked())
			gl->startAnimation();

		return true;
	}
	else*/
		return QVBox::eventFilter(watched, e);
}
