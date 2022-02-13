/***************************************************************************
                          sq_archivehandler.h  -  description
                             -------------------
    begin                : ??? ??? 26 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
  ***************************************************************************/

/* Originally taken from ShowImg */

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
#include <qmap.h>

class KFileItem;

class SQ_Dir;

#undef SQ_AH
#define SQ_AH SQ_ArchiveHandler

class SQ_ArchiveHandler : public QObject
{
	public: 
		SQ_ArchiveHandler(QObject * parent = 0, const char *name = 0);
		~SQ_ArchiveHandler();

		int findProtocolByName(const QString &prot);
		int findProtocolByFile(KFileItem *item);

		void setFile(KFileItem *);
		QString getPath() const;

		QString getExtractedPath() const;

		bool unpack();
		bool needClean();
		void clean(QString s);

	private:
		QString	extracteddir, fullextracteddir;
		QString	fullpath;
		QString	file;
		SQ_Dir	*dir;
		QMap<QString, int> protocols;
		KFileItem *item;
};

#endif
