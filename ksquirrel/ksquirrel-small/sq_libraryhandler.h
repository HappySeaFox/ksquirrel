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

/*
 *  SQ_LibraryHandler is a library manager. It's the main class
 *  in library loading mechanizm. 
 *
 *  See http://ksquirrel.sf.net/development.php for more.
 */

class SQ_LibraryHandler : public QValueVector<SQ_LIBRARY>
{
	public:
		SQ_LibraryHandler(QStringList *foundLibraries = 0);
		~SQ_LibraryHandler();

		/*
		 *  Find appropriate SQ_LIBRARY by filename. If
		 *  not found, return NULL.
		 */
		SQ_LIBRARY* libraryForFile(const QString &path);

		/*
		 *  Find appropriate SQ_LIBRARY by its name. If
		 *  not found, return NULL.
		 *
		 *  Name is a string, returned by fmt_quickinfo()
		 */
		SQ_LIBRARY* libraryByName(const QString &name);

		/*
		 *  Find appropriate codec for given filename. If
		 *  not found, return NULL.
		 */
		fmt_codec_base* codecForFile(const QString &path);

		/*
		 *  Find appropriate codec for given library name. If
		 *  not found, return NULL.
		 */
		fmt_codec_base* codecByName(const QString &name);

		/*
		 *  Does SQ_LibraryHandler support given file ?
		 *
		 *  It supports, if libraryForFile(path) is not NULL.
		 */
		bool supports(const QString &path);

		/*
		 *  Does any of found libraries handle given extension ?
		 */
		bool knownExtension(const QString &ext);

		/*
		 *  Fill 'filters' with all found filters, and
		 *  'quick' with appropriate information.
		 */
		void allFilters(QStringList &filters, QStringList &quick) const;
		
		/*
		 *  Get all filters as one string.
		 */
		QString allFiltersString() const;

		/*
		 *  Remove and unload all libraries.
		 */
		void clear();

		/*
		 *  Print some information on found libraries.
		 */
		void dump() const;

		/*
		 *  Clear old libraries, and load new.
		 */
		void reInit(QStringList *foundLibraries);

		/*
		 *  Add new libraries.
		 */
		void add(QStringList *foundLibraries);

		/*
		 *  Remove given libraries.
		 */
		void remove(QStringList *foundLibraries);

		/*
		 *  supports() saves last found library.
		 *  You can access it via latestLibrary().
		 */
		SQ_LIBRARY *latestLibrary();

		/*
		 *  -//- but return codec.
		 */
		fmt_codec_base *latestCodec();

		static SQ_LibraryHandler* instance();

	private:

		/*
		 *  Is library with name 'quick' already been handled ?
		 */
		bool alreadyInMap(const QString &quick) const;

	private:
		/*
		 *  Last found library by libraryForFile().
		 */
		SQ_LIBRARY *latest;

		static SQ_LibraryHandler *hand;
};

inline
SQ_LIBRARY* SQ_LibraryHandler::latestLibrary()
{
    return latest;
}

#endif
