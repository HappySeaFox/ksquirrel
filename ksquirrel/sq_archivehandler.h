/***************************************************************************
                          sq_archivehandler.h  -  description
                             -------------------
    begin                : ??? ??? 26 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@uandex.ru
 ***************************************************************************/
/***************************************************************************
                          extract.h  -  description
                             -------------------
    begin                : Sat Dec 1 2001
    copyright            : (C) 2001 by Richard Groult, 2003 OGINO Tomonori
    email                : rgroult@jalix.org ogino@nn.iij4u.or.jp
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_ARCHIVEHANDLER_H
#define SQ_ARCHIVEHANDLER_H

#include <qobject.h>
#include <qstring.h>

class SQ_Dir;

#define SQ_AH SQ_ArchiveHandler

/**
  *@author CKulT
  */

class SQ_ArchiveHandler : public QObject
{
	public: 
		SQ_ArchiveHandler(QObject * parent = 0, const char *name = 0);
		~SQ_ArchiveHandler();

		static bool supported(const QString filename);

		void setPath(const QString);
		QString getPath() const;

		QString getExtractedPath() const;

		bool unpack();
		bool needClean();
		void clean(QString s = QString::null);

	private:
		QString	extracteddir, fullextracteddir;
		QString	fullpath;
		QString	file;
		SQ_Dir	*dir;
};

#endif
