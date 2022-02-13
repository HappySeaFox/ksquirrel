/***************************************************************************
                          sq_glview.cpp  -  description
                             -------------------
    begin                : ??? ??? 5 2004
    copyright            : (C) 2004 by Baryshev Dmitry
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

#include <qlayout.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qhbox.h>

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

SQ_GLView * SQ_GLView::sing = NULL;

SQ_GLView::SQ_GLView(QWidget *parent, const char *name) : QVBox(parent, name)
{
	sing = this;

	separate = (parent) ? false : true;

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

	QString fourlines = QString::fromLatin1("----");
	QHBox *sqSBDecodedBox = new QHBox;
	sqSBDecodedBox->setSpacing(2);
	QLabel *sqSBDecodedI = new QLabel(QString::fromLatin1("--"), sqSBDecodedBox, "SBDecodedI");
	names["SBDecodedI"] = sqSBDecodedI;
	QLabel *sqSBDecoded = new QLabel(fourlines, sqSBDecodedBox, "SBDecoded");
	names["SBDecoded"] = sqSBDecoded;

	QLabel *sqSBGLZoom = new QLabel(fourlines, 0, "SBGLZoom");
	names["SBGLZoom"] = sqSBGLZoom;
	QLabel *sqSBGLAngle = new QLabel(fourlines, 0, "SBGLAngle");
	names["SBGLAngle"] = sqSBGLAngle;
	QLabel *sqSBGLCoord = new QLabel(fourlines, 0, "SBGLCoord");
	names["SBGLCoord"] = sqSBGLCoord;
	QLabel *sqSBLoaded = new QLabel(fourlines, 0, "SBLoaded");
	names["SBLoaded"] = sqSBLoaded;
	QLabel *sqSBFrame = new QLabel(QString::fromLatin1("0/0"), 0, "SBFrame");
	names["SBFrame"] = sqSBFrame;
	QLabel *sqSBFile = new QLabel(QString::null, NULL, "SBFile");
	names["SBFile"] = sqSBFile;

	sqSBFrame->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter | Qt::ExpandTabs);
	sqSBFile->setAlignment(Qt::AlignRight | Qt::AlignTop);
	QFont font = sqSBFile->font();
	font.setBold(true);
	sqSBFile->setFont(font);

	sbar->addWidget(sqSBDecodedBox, 0, true);
//	sbar->addWidget(sqSBDecoded, 0, true);
	sbar->addWidget(sqSBFrame, 0, true);
	sbar->addWidget(sqSBLoaded, 0, true);
	sbar->addWidget(sqSBGLZoom, 0, true);
	sbar->addWidget(sqSBGLAngle, 0, true);
	sbar->addWidget(sqSBGLCoord, 0, true);
	sbar->addWidget(sqSBFile, 1, true);

	sbar->setShown(SQ_Config::instance()->readBoolEntry("GL view", "statusbar", true));

	if(!separate)
		return;

	QRect rect(0,0,320,200);
	QRect geom = SQ_Config::instance()->readRectEntry("GL view", "geometry", &rect);

	setGeometry(geom);

	gl->matrixChanged();
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
	KSquirrel::app()->slotCloseGLWidget();
}

void SQ_GLView::reparent(QWidget *parent, const QPoint &p, bool showIt)
{
	QVBox::reparent(parent, p, showIt);

	separate = (parent) ? false : true;
}

bool SQ_GLView::eventFilter(QObject *watched, QEvent *e)
{
	if(watched == this)
	{
		if(/*e->type() == QEvent::WindowDeactivate || */e->type() == QEvent::Hide)
		{
			gl->stopAnimation();
			return true;
		}
		else if(/*e->type() == QEvent::WindowActivate || */e->type() == QEvent::Show)
		{
			if(!gl->manualBlocked())
				gl->startAnimation();

			return true;
		}
		else
			return QVBox::eventFilter(watched, e);
	}
	else
		return QVBox::eventFilter(watched, e);
}

QLabel* SQ_GLView::sbarWidget(const QString &name) const
{
	return names[name];
}

SQ_GLView* SQ_GLView::window()
{
	return sing;
}
