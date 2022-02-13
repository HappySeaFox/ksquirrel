/***************************************************************************
                          sq_quickbrowser.cpp  -  description
                             -------------------
    begin                : ??? ??? 4 2004
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

#include <qtoolbutton.h>
#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_quickbrowser.h"
#include "sq_diroperatorbase.h"
#include "sq_fileiconview.h"
#include "sq_widgetstack.h"
#include "sq_glview.h"
#include "sq_glwidget.h"

#ifndef SQ_SECTION_LIST
#define SQ_SECTION_LIST "Navigator List"
#endif

SQ_QuickBrowser * SQ_QuickBrowser::view = NULL;
SQ_DirOperatorBase * SQ_QuickBrowser::op = NULL;

SQ_Header::SQ_Header(QWidget *parent) : KToolBar(parent), inMouse(false), p(parent)
{}

SQ_Header::~SQ_Header()
{}

void SQ_Header::mousePressEvent(QMouseEvent *e)
{
	e->accept();

	if(e->button() == Qt::LeftButton)
	{
		oldParentX = p->x();
		oldParentY = p->y();

		oldX = e->globalX();
		oldY = e->globalY();

		inMouse = true;
	}
}

void SQ_Header::mouseMoveEvent(QMouseEvent *e)
{
	e->accept();

	if(!inMouse)
		return;

	int offsetX = e->globalX() - oldX;
	int offsetY = e->globalY() - oldY;

	parentWidget()->move(oldParentX + offsetX, oldParentY	 + offsetY);
}

void SQ_Header::mouseReleaseEvent(QMouseEvent *e)
{
	e->accept();

	if(e->button() == Qt::LeftButton)
		inMouse = false;
}

SQ_SizeGrip::SQ_SizeGrip(QWidget *top, QWidget *parent, const char *name) : QSizeGrip(parent, name), p(top)
{}

SQ_SizeGrip::~SQ_SizeGrip()
{}

void SQ_SizeGrip::mousePressEvent(QMouseEvent *)
{
	mother = p->mapToGlobal(p->pos());
}

void SQ_SizeGrip::mouseMoveEvent(QMouseEvent *e)
{
	if(e->state() != Qt::LeftButton)
		return;

	int x = e->globalX();
	int y = e->globalY();
	int w = x - mother.x();
	int h = y - mother.y() + 33;

	if(w > 200 && h > 100)
		p->resize(w, h);
	else if(w > 200)
		p->resize(w, p->height());
	else if(h > 100)
		p->resize(p->width(), h);
}

SQ_QuickStatus::SQ_QuickStatus(QWidget *parent, const char *name) : QStatusBar(parent, name)
{
	setCursor(Qt::ArrowCursor);
}

SQ_QuickStatus::~SQ_QuickStatus()
{}

void SQ_QuickStatus::mousePressEvent(QMouseEvent *e)
{
	e->accept();
}

SQ_QuickBrowser::SQ_QuickBrowser(QWidget *parent, const char *name) : QVBox(parent, name)
{
	view = this;
	hide();
	SQ_Header *t = new SQ_Header(this);

	t->setIconSize(16);
	t->setPalette(QPalette(QColor(255,255,255), QColor(255,255,255)));

	op = quick = new SQ_DirOperatorBase(SQ_WidgetStack::instance()->getURL(), SQ_DirOperatorBase::TypeList, this);
	quick->readConfig(KGlobal::config(), SQ_SECTION_LIST);
	quick->setViewConfig(KGlobal::config(), SQ_SECTION_LIST);
	quick->setMode(KFile::Files);
//	quick->iv->setSelectionMode(KFile::Single);
	quick->iv->setIconSize(16);

	quick->iv->setCursor(Qt::ArrowCursor);

	quick->actionCollection()->action("back")->plug(t);
	quick->actionCollection()->action("forward")->plug(t);
	quick->actionCollection()->action("up")->plug(t);
	quick->actionCollection()->action("reload")->plug(t);
	quick->actionCollection()->action("home")->plug(t);
	quick->actionCollection()->action("mkdir")->plug(t);
	quick->actionCollection()->action("delete")->plug(t);
	(new KAction(i18n("Hide"), "exit", 0, this, SLOT(slotClose()), KSquirrel::app()->actionCollection(), "SQ GL THide2"))->plug(t);

	setStretchFactor(quick, 1);

	SQ_QuickStatus *status = new SQ_QuickStatus(this);
	status->setSizeGripEnabled(false);
	QLabel *fix = new QLabel(status);
	SQ_SizeGrip *grip = new SQ_SizeGrip(this, status);

	status->addWidget(fix, 1, true);
	status->addWidget(grip, 0, true);

	QRect r = SQ_Config::instance()->readRectEntry("GL view", "quickGeometry");

	if(r.isNull())
		r = QRect(0, 34, 250, 200);

	setGeometry(r);
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
	SQ_GLWidget::window()->pAToolQuick->animateClick();
}

void SQ_QuickBrowser::showEvent(QShowEvent *)
{
	KFileItem *f = quick->iv->currentFileItem();

	if(f)
		quick->setCurrentItem(f);
}

SQ_QuickBrowser* SQ_QuickBrowser::window()
{
	return view;
}

SQ_DirOperatorBase *SQ_QuickBrowser::quickOperator()
{
	return op;
}

