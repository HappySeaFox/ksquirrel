/***************************************************************************
                          sq_libraryhandler.h  -  description
                             -------------------
    begin                :  Mar 5 2004
    copyright            : (C) 2004 by CKulT
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

#ifndef SQ_LIBRARY_HANDLER_H
#define SQ_LIBRARY_HANDLER_H

#include <qobject.h>
#include <qlibrary.h>
#include <qstringlist.h>

/**
  *@author CKulT
  */

#include "defs.h"

class SQ_LIBRARY
{
	public:
		SQ_LIBRARY() {}
		~SQ_LIBRARY() {}

		QLibrary *lib;
		QString libpath;
		int (*readformat)(const char *name, PICTURE **);
		int (*writeformat)(const char *name, PICTURE **);
		int (*readable)();
		int (*writeable)();
		char* (*version)();
		char* (*info)();
		char* (*extension)();
		QString sinfo;
};
  
class SQ_LibraryHandler : public QObject
{
	private:
		QValueList<SQ_LIBRARY>	*libs;
		SQ_LIBRARY				*currentlib;

	public:
		SQ_LibraryHandler(QStringList &foundLibraries, QObject *parent = 0, const char *name = 0);
		~SQ_LibraryHandler();

		void setCurrentLibrary(const QString &name);
		int callReadFormat(const QString &file, PICTURE **pic);

		SQ_LIBRARY getLibByIndex(const int &i);
		int count() const { return libs->count(); }

		bool supports(const QString &format) const;
		QString allSupportedForFilter() const;
};

#endif
