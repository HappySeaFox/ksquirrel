/***************************************************************************
                          sq_externaltool.h  -  description
                             -------------------
    begin                : ??? ??? 12 2004
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

#ifndef SQ_EXTERNALTOOL_H
#define SQ_EXTERNALTOOL_H

#include <qptrlist.h>
#include <qstring.h>
#include <qobject.h>

#include <kdesktopfile.h>

class KPopupMenu;

class SQ_Dir;

/*
 *  Class which manages external tools. It store all available external tools
 *  in memory, and create popup menu where external tools been inserted.
 */

class SQ_ExternalTool : public QObject, public QPtrList<KDesktopFile>
{
	Q_OBJECT

	public: 
		SQ_ExternalTool();
		~SQ_ExternalTool();

		/*
		 *  Add new tool with name, pixmap and external command.
		 */
		void addTool(const QString &pixmap, const QString &name, const QString &command);

		/*
		 *  Get pixmap, name or command of external tool.
		 */
		QString toolPixmap(const int i);
		QString toolName(const int i);
		QString toolCommand(const int i);

		/*
		 *  Recreate current popop menu.
		 */
		KPopupMenu* newPopupMenu();

		/*
		 *  Get current popup menu.
		 */
		KPopupMenu* constPopupMenu() const;

		/*
		 *  Write current state to config file
		 */
		void writeEntries();

		static SQ_ExternalTool* instance();

	private slots:
		/*
		 *  Invoked, when user executed popup menu with external tools.
		 *  This slot will do some useful stuff.
		 */
		void slotAboutToShowMenu();

	private:
		/*
		 *  Popup menu.
		 */
		KPopupMenu *menu;

		/*
		 *  Id of menu's title.
		 */
		int title;

		/*
		 *  For saving .desktop files.
		 */
		SQ_Dir *dir;

		static SQ_ExternalTool *ext;
};

#endif
