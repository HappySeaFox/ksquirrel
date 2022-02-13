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

#include "fmt_types.h"
#include "fmt_defs.h"

class QLibrary;

class fmt_codec_base;

struct SQ_LIBRARY
{
	SQ_LIBRARY() : lib(0), codec(0)
	{}

	QLibrary	*lib;
	QString	libpath;
	QRegExp	regexp;
	QString	filter;
	QString	regexp_str;
	QString	quickinfo;
	QString	version;

	fmt_codec_base	*codec;

	fmt_codec_base* 	(*fmt_codec_create)();
	void				(*fmt_codec_destroy)(fmt_codec_base*);

	QImage 	mime;
	int			mime_len;

	fmt_writeoptionsabs	opt;
	bool		writable;
};

class SQ_LibraryHandler : public QValueVector<SQ_LIBRARY>
{
	public:
		SQ_LibraryHandler(QStringList *foundLibraries = 0);
		~SQ_LibraryHandler();

		SQ_LIBRARY* libraryForFile(const QString &);
		SQ_LIBRARY* libraryByName(const QString &);

		fmt_codec_base* codecForFile(const QString &);
		fmt_codec_base* codecByName(const QString &);

		bool supports(const QString &);
		bool knownExtension(const QString &ext);
		void allFilters(QStringList &filters, QStringList &quick) const;
		QString allFiltersString() const;

		void clear();
		void dump() const;
		void reInit(QStringList *foundLibraries);

		void add(QStringList *foundLibraries);
		void remove(QStringList *foundLibraries);

		SQ_LIBRARY *latestLibrary();
		fmt_codec_base *latestCodec();

		static SQ_LibraryHandler* instance();

	private:
		bool alreadyInMap(const QString &quick) const;

	private:
		SQ_LIBRARY *latest;

		static SQ_LibraryHandler *hand;
};

inline
SQ_LIBRARY* SQ_LibraryHandler::latestLibrary()
{
    return latest;
}

#endif
