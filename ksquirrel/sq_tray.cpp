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

#include "ksquirrel.h"
#include "sq_tray.h"

#include <qapplication.h>

SQ_SystemTray::SQ_SystemTray(QWidget *_parent, const char *name) : KSystemTray(_parent,name), parent(_parent)
{
	rightMenu = new KPopupMenu;

        KAction *pAOpen = new KAction("Open SQ", sqLoader->loadIcon("ok", KIcon::Desktop, KIcon::SizeSmall), 0, sqApp, SLOT(show()), sqApp->actionCollection(), "Open SQ");
        KAction *pAQuit = new KAction("Quit SQ", sqLoader->loadIcon("exit", KIcon::Desktop, KIcon::SizeSmall), 0, qApp, SLOT(quit()), sqApp->actionCollection(), "Quit SQ");
        KAction *pAOptions = new KAction("Options SQ", sqLoader->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall), 0, sqApp, SLOT(slotOptions()), sqApp->actionCollection(), "Options SQ");

	pAOpen->plug(rightMenu);
	pAOptions->plug(rightMenu);
	(new KActionSeparator)->plug(rightMenu);
	pAQuit->plug(rightMenu);
}

void SQ_SystemTray::mousePressEvent(QMouseEvent *ev)
{
	if(ev->button() == Qt::LeftButton)
	{
		sqApp->show();
	}
	else if(ev->button() == Qt::RightButton)
	{
		rightMenu->exec(QCursor::pos());
	}
    
//    KSystemTray::mousePressEvent(ev);
}

void SQ_SystemTray::mouseReleaseEvent(QMouseEvent *ev)
{
    KSystemTray::mouseReleaseEvent(ev);
}
