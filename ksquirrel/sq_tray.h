/***************************************************************************
                          sq_tray.h  -  description
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

#ifndef SQ_TRAY_H
#define SQ_TRAY_H

#include <ksystemtray.h>
#include <kpopupmenu.h>

class KAction;

class SQ_SystemTray : public KSystemTray
{
       Q_OBJECT

	public:
		SQ_SystemTray(QWidget *_parent = 0, const char *name = "");

	protected:
		void mousePressEvent(QMouseEvent *);
		void mouseReleaseEvent(QMouseEvent *);

	private slots:
		void slotActivate();

	 private:
		KPopupMenu	*rightMenu;
		KAction 		*pAOpen;

};


#endif
