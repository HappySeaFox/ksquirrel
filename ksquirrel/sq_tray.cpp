/***************************************************************************
                          sq_tray.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
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
#include <qcursor.h>

#include <kaction.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <kdeversion.h>
#include <kwin.h>

#include "ksquirrel.h"
#include "sq_tray.h"

#if KDE_IS_VERSION(3,2,0)
	#define SQ_ACTIVATE_WINDOW(id) KWin::activateWindow(id)
#else
	#define SQ_ACTIVATE_WINDOW(id) KWin::setActiveWindow(id)
#endif


SQ_SystemTray::SQ_SystemTray(QWidget *parent, const char *name) : KSystemTray(parent, name)
{
	rightMenu = new KPopupMenu;

	KActionSeparator *pASep = new KActionSeparator;

	pAOpen = KStdAction::open(this, SLOT(slotActivate()), sqApp->actionCollection(), "Open SQ from tray");

	pAOpen->plug(rightMenu);
	sqApp->pAConfigure->plug(rightMenu);
	pASep->plug(rightMenu);
	sqApp->pAExit->plug(rightMenu);

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
	sqApp->show();
	SQ_ACTIVATE_WINDOW(sqApp->winId());
}
