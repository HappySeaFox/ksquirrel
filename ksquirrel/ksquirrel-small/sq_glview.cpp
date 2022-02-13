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

#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_config.h"

#ifndef SQ_SMALL

#include "ksquirrel.h"
#include "sq_widgetstack.h"

#else

#include <kurl.h>
#include <kwin.h>

#include "sq_hloptions.h"
#include "sq_libraryhandler.h"
#include "sq_librarylistener.h"
#include "sq_iconloader.h"

#endif

SQ_GLView * SQ_GLView::sing = NULL;

SQ_GLView::SQ_GLView(QWidget *parent, const char *name) : QVBox(parent, name)
{
	sing = this;

#ifndef SQ_SMALL

	separate = (parent) ? false : true;

	createContent();

	installEventFilter(this);

#else

	new SQ_IconLoader;

	libl = new SQ_LibraryListener;
	libh = new SQ_LibraryHandler;

	kconf = new SQ_Config("ksquirrelrc");

	connect(libl, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));

	const QString libPrefix = "/usr/lib/ksquirrel-libs/";

	KURL url = libPrefix;

	createContent();

	// SQ_LibraryListener will find libraries
	// and SQ_LibraryHandler will load them
	libl->slotOpenURL(url, false, true);

#endif

}

SQ_GLView::~SQ_GLView()
{}

void SQ_GLView::createContent()
{
	gl = new SQ_GLWidget(this);
	gl->glInitA();

	setStretchFactor(gl, 1);

	sbar = new KStatusBar(this);

	// create QLabels, fill 'names' with pointers
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

	// fill statusbar
	sbar->addWidget(sqSBDecodedBox, 0, true);
//	sbar->addWidget(sqSBDecoded, 0, true);
	sbar->addWidget(sqSBFrame, 0, true);
	sbar->addWidget(sqSBLoaded, 0, true);
	sbar->addWidget(sqSBGLZoom, 0, true);
	sbar->addWidget(sqSBGLAngle, 0, true);
	sbar->addWidget(sqSBGLCoord, 0, true);
	sbar->addWidget(sqSBFile, 1, true);

	sbar->setShown(SQ_Config::instance()->readBoolEntry("GL view", "statusbar", true));

#ifndef SQ_SMALL

	if(!separate)
		return;

#endif

	// restore geometry from config file
	QRect rect(0,0,660,480);
	QRect geom = SQ_Config::instance()->readRectEntry("GL view", "geometry", &rect);

	setGeometry(geom);

	gl->matrixChanged();
}

/*
 *  Get a pointer to statusbar.
 */
KStatusBar* SQ_GLView::statusbar()
{
	return sbar;
}

void SQ_GLView::closeEvent(QCloseEvent *e)
{

#ifndef SQ_SMALL

	// ignore event
	e->ignore();

	// let KSquirrel close SQ_GLView, since it can be built-in
	KSquirrel::app()->slotCloseGLWidget();

#else

	// in 'small' version accept close event,
	// and save geometry to config file
	kconf->setGroup("GL view");
	kconf->writeEntry("geometry", geometry());
	kconf->writeEntry("statusbar", gl->pAStatus->isChecked());
	kconf->writeEntry("ignore", gl->pAIfLess->isChecked());
	kconf->writeEntry("zoom type", gl->zoomType());
	kconf->writeEntry("actions", gl->actions());
	kconf->writeEntry("toolbars_hidden", gl->actionsHidden());

	e->accept();

#endif

}

#ifndef SQ_SMALL

/*
 *  Is this widget separate ?
 */
bool SQ_GLView::isSeparate() const
{
	return separate;
}

/*
 *  Make widget built-in with reparenting it.
 */
void SQ_GLView::reparent(QWidget *parent, const QPoint &p, bool showIt)
{
	// reparent
	QVBox::reparent(parent, p, showIt);

	// store current state
	separate = (parent) ? false : true;
}

bool SQ_GLView::eventFilter(QObject *watched, QEvent *e)
{
	if(watched == this)
	{
		// stop animation, if SQ_GLView is hidden (animation becomes useless)
		if(/*e->type() == QEvent::WindowDeactivate || */e->type() == QEvent::Hide)
		{
			gl->stopAnimation();
			return true;
		}
		// resume animation, if needed
		else if(/*e->type() == QEvent::WindowActivate || */e->type() == QEvent::Show)
		{
			if(!gl->manualBlocked())
				gl->startAnimation();

			return true;
		}
		else // call to default eventFilter()
			return QVBox::eventFilter(watched, e);
	}
	else
		return QVBox::eventFilter(watched, e);
}

void SQ_GLView::slotContinueLoading()
{}

void SQ_GLView::slotFullScreen(bool)
{}

#else

void SQ_GLView::slotContinueLoading()
{
	// now all libraries are loaded, we can
	// show an image
	show();

	disconnect(libl, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));

	if(!SQ_HLOptions::instance()->path.isEmpty())
	{
		// if it is known image type - let's load it
		if(SQ_LibraryHandler::instance()->supports(SQ_HLOptions::instance()->path))
			SQ_GLWidget::window()->slotStartDecoding(SQ_HLOptions::instance()->path, true);
	}
}

/*
 *  Goto fullscreen. If current version is NOT
 *  'small', fullscreen state will be managed by KSquirrel.
 */
void SQ_GLView::slotFullScreen(bool full)
{
	WId id = winId();
	// hide statusbar in fullscreen mode ?
	if(sbar && kconf->readBoolEntry("GL view", "hide_sbar", true))
	{
		sbar->setShown(!full);
		SQ_GLWidget::window()->pAStatus->setChecked(!full);
	}

	// goto fullscreen
	if(full)
		KWin::setState(id, NET::FullScreen);
	else // leave fullscreen
		KWin::clearState(id, NET::FullScreen);
}

#endif

/*
 *  Get a pointer to a widget in statusbar by name.
 */
QLabel* SQ_GLView::sbarWidget(const QString &name) const
{
	return names[name];
}

SQ_GLView* SQ_GLView::window()
{
	return sing;
}
