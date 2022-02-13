/***************************************************************************
                          sq_tray.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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
#include <qcursor.h>
#include <qapplication.h>

#include <kaction.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <kwin.h>

#include "ksquirrel.h"
#include "sq_tray.h"
#include "sq_glview.h"

SQ_SystemTray::SQ_SystemTray(QWidget *parent, const char *name) : KSystemTray(parent, name)
{
	rightMenu = new KPopupMenu;

	KActionSeparator *pASep = new KActionSeparator;

	pAOpen = KStdAction::open(this, SLOT(slotActivate()), KSquirrel::app()->actionCollection(), "Open SQ from tray");
	pAExit = KStdAction::quit(this, SLOT(slotClose()), KSquirrel::app()->actionCollection(), "SQ close from tray");

	pAOpen->plug(rightMenu);
	KSquirrel::app()->pAConfigure->plug(rightMenu);
	pASep->plug(rightMenu);
	pAExit->plug(rightMenu);

	setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/tray.png")));
}

void SQ_SystemTray::mousePressEvent(QMouseEvent *ev)
{
	if(ev->button() == Qt::LeftButton)
	{
		slotActivate();
	}
	else if(ev->button() == Qt::RightButton)
	{
		rightMenu->exec(QCursor::pos());
	}
	else;
}

void SQ_SystemTray::mouseReleaseEvent(QMouseEvent *ev)
{
	KSystemTray::mouseReleaseEvent(ev);
}

void SQ_SystemTray::slotActivate()
{
	if(SQ_GLView::window()->isSeparate() && !SQ_GLView::window()->isHidden())
		SQ_GLView::window()->show();

	KSquirrel::app()->show();
}

void SQ_SystemTray::slotClose()
{
	KSquirrel::app()->finalActions();
	qApp->quit();
}
