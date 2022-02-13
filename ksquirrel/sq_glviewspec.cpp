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

#include "sq_glviewspec.h"
#include "sq_glviewwidget.h"
#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_widgetstack.h"

SQ_GLViewSpec::SQ_GLViewSpec(QWidget *parent, const char *name) : SQ_GLBase(parent, name)
{
	separate = true;
	create();
}

SQ_GLViewSpec::~SQ_GLViewSpec()
{
	sqConfig->setGroup("GL view");
	if(sqConfig->readBoolEntry("GL view", "save pos", true))
	{
		sqConfig->writeEntry("posX", x());
		sqConfig->writeEntry("posY", y());
	}

	if(sqConfig->readBoolEntry("GL view", "save size", true))
	{
		sqConfig->writeEntry("sizeX", width());
		sqConfig->writeEntry("sizeY", height());
	}
}

void SQ_GLViewSpec::create()
{
	QGridLayout *l;
	int posX, posY, sizeX, sizeY;

	l = new QGridLayout(this, 3, 1, 1, 1, "SQ_GLBASE_LAYOUT");

	KToolBar *toolbar = new KToolBar(this);
	KStatusBar *statusbar = new KStatusBar(this);
	statusbar->addWidget(new QLabel("test", statusbar));
	statusbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	gl = new SQ_GLViewWidget(this);

	QToolButton *pBAbstract;

	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/previous.png"))), "Previous", "", sqWStack, SLOT(emitPreviousSelected()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/next.png"))), "Next", "", sqWStack, SLOT(emitNextSelected()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/zoom+.png"))), gl->pAZoomPlus->text(), "", gl, SLOT(slotZoomPlus()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract->setAutoRepeat(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/zoom-.png"))), gl->pAZoomMinus->text(), "", gl, SLOT(slotZoomMinus()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract->setAutoRepeat(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/rotate_left.png"))), gl->pARotateLeft->text(), "", gl, SLOT(slotRotateLeft()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract->setAutoRepeat(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/rotate_right.png"))), gl->pARotateRight->text(), "", gl, SLOT(slotRotateRight()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract->setAutoRepeat(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/flip_v.png"))), gl->pAFlipV->text(), "", gl, SLOT(slotFlipV()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract->setAutoRepeat(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/flip_h.png"))), gl->pAFlipH->text(), "", gl, SLOT(slotFlipH()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract->setAutoRepeat(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/reset.png"))), gl->pAReset->text(), "", gl, SLOT(slotMatrixReset()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract->setAutoRepeat(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/help.png"))), gl->pAHelp->text(), "", gl, SLOT(slotSomeHelp()), toolbar);
	pBAbstract->setUsesBigPixmap(true);
	pBAbstract = new QToolButton(QIconSet(QPixmap::fromMimeSource(locate("appdata", "actions/close.png"))), gl->pAClose->text(), "", gl, SLOT(slotCloseGLView()), toolbar);
	pBAbstract->setUsesBigPixmap(true);

	l->addWidget(toolbar, 0, 0);
	l->addWidget(gl, 1, 0);
	l->addWidget(statusbar, 2, 0);

	if(sqConfig->readBoolEntry("GL view", "save pos", true))
	{
		posX = sqConfig->readNumEntry("GL view", "posX", 0);
		posY = sqConfig->readNumEntry("GL view", "posY", 0);
	}
	else
		posX = posY = 0;

	if(sqConfig->readBoolEntry("GL view", "save size", true))
	{
		sizeX = sqConfig->readNumEntry("GL view", "sizeX", 0);
		sizeY = sqConfig->readNumEntry("GL view", "sizeY", 0);
	}
	else
	{
		sizeX = 320;
		sizeY = 200;
	}

	move(posX, posY);
	resize(sizeX, sizeY);
}
