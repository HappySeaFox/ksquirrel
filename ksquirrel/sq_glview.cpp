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

	create();
}

SQ_GLView::~SQ_GLView()
{}

void SQ_GLView::create()
{
	int posX, posY, sizeX, sizeY;

	gl = new SQ_GLWidget(this);
	gl->glInit();
	gl->createDecodingThread();

	setStretchFactor(gl, 1);

	if(separate)
	{
		KStatusBar *sbar = new KStatusBar(this);
		sqSBDecoded->reparent(sbar, QPoint(), true);
		sqSBGLreport->reparent(sbar, QPoint(), true);
		sbar->addWidget(sqSBDecoded, 0, true);
		sbar->addWidget(sqSBGLreport, 0, true);
		QLabel *levak = new QLabel(sbar);
		sbar->addWidget(levak, 1, true);
	}

	if(!separate)
		return;

	if(sqConfig->readBoolEntry("GL view", "save pos", true))
	{
		posX = sqConfig->readNumEntry("GL view", "posX", 0);
		posY = sqConfig->readNumEntry("GL view", "posY", 0);
	}
	else
		posX = posY = 40;

	if(sqConfig->readBoolEntry("GL view", "save size", true))
	{
		sizeX = sqConfig->readNumEntry("GL view", "sizeX", 320);
		sizeY = sqConfig->readNumEntry("GL view", "sizeY", 200);
	}
	else
	{
		sizeX = 320;
		sizeY = 200;
	}

	move(posX, posY);
	resize(sizeX, sizeY);
}
