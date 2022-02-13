/***************************************************************************
                          sq_libraryhandler.h  -  description
                             -------------------
    begin                : Mar 5 2004
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
#include <qstringlist.h>

#include "sq_library.h"

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
