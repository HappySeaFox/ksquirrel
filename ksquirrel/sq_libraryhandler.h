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

template <class T> class QValueVector;

class SQ_LIBRARY
{
	public:
		SQ_LIBRARY() {}
		~SQ_LIBRARY() {}

		QLibrary *lib;
		QString libpath;
		QString sinfo;

		int (*fmt_init)(fmt_info **, const char *);
		int (*fmt_read_info)(fmt_info *);
		int (*fmt_read_scanline)(fmt_info *, RGBA *);
		char* (*fmt_version)();
		char* (*fmt_quickinfo)();
		char* (*fmt_extension)();
		int* (*fmt_close)(fmt_info *);
};
  
class SQ_LibraryHandler : public QObject
{
	private:
		QValueVector<SQ_LIBRARY>*libs;
		SQ_LIBRARY				*currentlib;

	public:
		SQ_LibraryHandler(QStringList *foundLibraries = 0, QObject *parent = 0, const char *name = 0);
		~SQ_LibraryHandler();

		void setCurrentLibrary(const QString &name);
		SQ_LIBRARY* getCurrentLibrary();

		SQ_LIBRARY getLibByIndex(const int &i);
		int count() const;

		bool supports(const QString &format) const;
		QString allSupportedForFilter() const;

		void clear();
		void reInit(QStringList *foundLibraries);

		void add(QStringList *foundLibraries);
		void remove(QStringList *foundLibraries);
};

#endif
