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

class SQ_ExternalTool : public QObject, public QPtrList<KDesktopFile>
{
	Q_OBJECT

	public: 
		SQ_ExternalTool();
		~SQ_ExternalTool();

		void addTool(const QString &pixmap, const QString &name, const QString &command);

		QString getToolPixmap(const int i);
		QString getToolName(const int i);
		QString getToolCommand(const int i);

		KPopupMenu* getNewPopupMenu();
		KPopupMenu* getConstPopupMenu() const;

		void writeEntries();

		static SQ_ExternalTool* instance();

	private slots:
		void slotAboutToShowMenu();

	private:
		KPopupMenu *menu;
		int title;
		SQ_Dir *dir;
		static SQ_ExternalTool *ext;
};

#endif
