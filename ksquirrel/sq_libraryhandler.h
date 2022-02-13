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
#include <qdatetime.h>
#include <qmap.h>
#include <qobject.h>

#include "sq_library.h"

class QStringList;

class KConfig;

/*
 *  SQ_LibraryHandler is a library manager. It's the main class
 *  in library loading mechanizm. 
 */

class SQ_LibraryHandler : public QObject, public QValueVector<SQ_LIBRARY>
{
    public:
        SQ_LibraryHandler(QObject *parent);
        ~SQ_LibraryHandler();

        /*
        *  Reload libraries from disk
        */
        void reload();

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
         *  Does any of found libraries handle given extension ?
         */
        bool knownExtension(const QString &ext);

        /*
         *  Fill 'filters' with all found filters, and
         *  'quick' with appropriate information.
         */
        void allFilters(QStringList &filters, QStringList &quick) const;

        void allWritableFilters(QStringList &filters, QStringList &quick) const;

        /*
         *  Get all filters as one string.
         */
        QString allFiltersString() const;

        /*
         *  Filters as one string suitable for KFileDialogs.
         *  If r == true, return readable codecs
         *  If allfiles == true, append *.* to result
         */
        QString allFiltersFileDialogString(bool r, bool allfiles = true) const;

        /*
         *  Remove and unload all libraries.
         */
        void clear();

        /*
         *  Print some information on found libraries.
         */
        void dump() const;

        static SQ_LibraryHandler* instance() { return m_instance; }

    private:

        void add(QStringList &foundLibraries);

        /*
         *  Load libraries from disk.
         */
        void load();
        /*
         *  Is library with name 'quick' already been handled ?
         */
        bool alreadyInMap(const QString &quick) const;

        void writeSettings(SQ_LIBRARY *lib);
        void readSettings(SQ_LIBRARY *lib);

        void debugInfo(const QString &symbol, const QString &path, const QString &lib, int);

    private:
        struct LibCacheEntry
        {
            LibCacheEntry() : library(0)
            {}

            LibCacheEntry(const QDateTime &dt, SQ_LIBRARY *l) : modified(dt), library(l)
            {}

            QDateTime modified;
            SQ_LIBRARY *library;
        };

        typedef QMap<QString, LibCacheEntry> LibCache;
        LibCache cache;

        KConfig *kconf;

        static SQ_LibraryHandler *m_instance;
};

#endif
