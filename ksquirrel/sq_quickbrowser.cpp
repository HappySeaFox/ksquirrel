/***************************************************************************
                          sq_quickbrowser.cpp  -  description
                             -------------------
    begin                : ??? ??? 4 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@yandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwidgetstack.h>
#include <qcursor.h>
#include <qvbox.h>

#include <ktoolbar.h>
#include <kiconloader.h>
#include <klocale.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_quickbrowser.h"
#include "sq_diroperator.h"
#include "sq_widgetstack.h"
#include "sq_glview.h"
#include "sq_glwidget.h"

#ifndef SQ_SECTION_NAME
#define SQ_SECTION_NAME ("squirrel image viewer file browser")
#endif

SQ_QuickBrowser::SQ_QuickBrowser(QWidget *parent, const char *name) : QVBox(parent, name)
{
	hide();
	KToolBar *t = new KToolBar(this);
	QWidgetStack *s = new QWidgetStack(this);

	t->setIconSize(16);
	t->setFrameShape(QFrame::GroupBoxPanel);

	quick = new SQ_DirOperator("/", SQ_DirOperator::TypeList);
//	quick->readConfig(KGlobal::config(), SQ_SECTION_NAME);
	quick->setViewConfig(KGlobal::config(), SQ_SECTION_NAME);
	quick->setMode(KFile::Files);
//	quick->setView(KFile::Simple);
//	quick->view()->actionCollection()->action(2)->activate();
	quick->setURL(sqWStack->getURL(), false);
	quick->setIconSize(16);

	quick->actionCollection()->action("back")->plug(t);
	quick->actionCollection()->action("forward")->plug(t);
	quick->actionCollection()->action("up")->plug(t);
	quick->actionCollection()->action("reload")->plug(t);
	quick->actionCollection()->action("home")->plug(t);
	quick->actionCollection()->action("mkdir")->plug(t);
	quick->actionCollection()->action("delete")->plug(t);
	(new KAction(i18n("Hide"), sqLoader->loadIcon("exit", KIcon::Desktop, 16), 0, this, SLOT(slotClose()), sqApp->actionCollection(), "SQ GL THide2"))->plug(t);
	quick->setCursor(QCursor(Qt::PointingHandCursor));

	s->addWidget(quick, 0);
	setGeometry(sqConfig->readRectEntry("GL view", "quickGeometry"));
}

SQ_QuickBrowser::~SQ_QuickBrowser()
{}

void SQ_QuickBrowser::closeEvent(QCloseEvent *e)
{
	hide();
	e->ignore();
}

void SQ_QuickBrowser::slotClose()
{
	sqGLWidget->pAQuick->activate();
}

