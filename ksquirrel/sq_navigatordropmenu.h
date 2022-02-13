/***************************************************************************
                          sq_navigatordropmenu.h  -  description
                             -------------------
    begin                : ??? Feb 23 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_NAVIGATORDROPMENU_H
#define SQ_NAVIGATORDROPMENU_H

#include <qobject.h>
#include <qpoint.h>

#include <kurl.h>

class KPopupMenu;

/**
  *@author Baryshev Dmitry
  */

class SQ_NavigatorDropMenu : public QObject
{
	Q_OBJECT

	public:
		SQ_NavigatorDropMenu();
		virtual ~SQ_NavigatorDropMenu();

		static SQ_NavigatorDropMenu* instance();

		void setupFiles(const KURL::List &l, const KURL &u);
		void exec(const QPoint &pos);

	protected slots:
		void slotCopy();
		void slotMove();
		void slotLink();

	private:
		KPopupMenu *dropmenu;
		KURL::List list;
		KURL url;

		static SQ_NavigatorDropMenu *app;
};

#endif
