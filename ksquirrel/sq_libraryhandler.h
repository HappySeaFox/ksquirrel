/***************************************************************************
                          sq_libraryhandler.h  -  description
                             -------------------
    begin                :  Mar 5 2004
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

#ifndef SQ_LIBRARY_HANDLER_H
#define SQ_LIBRARY_HANDLER_H

#include <qvaluevector.h>
#include <qimage.h>
#include <qregexp.h>
#include <qstringlist.h>

#include "defs.h"

class QLibrary;

const int buffer_size = 14;

class SQ_LIBRARY
{
	public:
		SQ_LIBRARY() {}
		~SQ_LIBRARY() {}

		QLibrary	*lib;
		QString	libpath;
		QRegExp	regexp;
		QString	filter, regexp_str;
		QString	quickinfo, version;

		int 		(*fmt_init)(fmt_info *, const char *);
		int 		(*fmt_read_scanline)(fmt_info *, RGBA *);
		int		(*fmt_readimage)(const char*, RGBA **, char **);
		int	 	(*fmt_close)();
		int		(*fmt_next)(fmt_info *);
		int		(*fmt_next_pass)(fmt_info *);

		const char* 	(*fmt_version)();
		const char* 	(*fmt_quickinfo)();
		const char*	(*fmt_pixmap)();
		const char* 	(*fmt_mime)();
		const char* 	(*fmt_filter)();

		QImage 	mime;
		int			mime_len;
};

class SQ_LibraryHandler : public QValueVector<SQ_LIBRARY>
{
	public:
		SQ_LibraryHandler(QStringList *foundLibraries = 0);
		~SQ_LibraryHandler();

		SQ_LIBRARY* libraryForFile(const QString &) const;
		bool supports(const QString &) const;
		QStringList allFilters() const;
		QString allFiltersString() const;

		void clear();
		void dump() const;
		void reInit(QStringList *foundLibraries);

		void add(QStringList *foundLibraries);
		void remove(QStringList *foundLibraries);

	private:
		bool alreadyInMap(const QString &quick) const;
};

#endif
