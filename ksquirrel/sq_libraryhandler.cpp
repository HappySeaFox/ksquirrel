/***************************************************************************
                          sq_libraryhandler.cpp  -  description
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

#include <qmessagebox.h>
#include <qlibrary.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qfile.h>

#include <kstringhandler.h>
#include <kdebug.h>

#include "sq_libraryhandler.h"
#include "sq_config.h"

#include <csetjmp>

#include "fileio.h"
#include "fmt_codec_base.h"

#include "sq_thumbnailinfo.h"

static const int buffer_size = 10;

SQ_LibraryHandler * SQ_LibraryHandler::m_instance = NULL;

/*
 *  Convert a string, return by fmt_pixmap() to
 *  "normal" string.
 */

int convertMimeFromBits(const QString &s, QString &dest)
{
    int num;

    dest = "";

    QStringList list = QStringList::split(',', s);
    QStringList::iterator it = list.begin();
    QStringList::iterator lend = list.end();

    while(it != lend)
    {
        num = (*it).toInt();
        dest.append((unsigned char)num);
        ++it;
    }

    return list.count();
}

// SQ_LibraryHandler
SQ_LibraryHandler::SQ_LibraryHandler(QObject *parent, QStringList *foundLibraries) 
    : QObject(parent), QValueVector<SQ_LIBRARY>(), latest(NULL)
{
    m_instance = this;

    kdDebug() << "+SQ_LibraryHandler" << endl;

    if(foundLibraries)
        reInit(foundLibraries);
}

SQ_LibraryHandler::~SQ_LibraryHandler()
{
    clear();

    kdDebug() << "-SQ_LibraryHandler" << endl;
}

/*
 *  Find appropriate SQ_LIBRARY by filename. If
 *  not found, return NULL.
 */
SQ_LIBRARY* SQ_LibraryHandler::libraryForFile(const QString &full_path)
{
    // wrong parameter, or file doesn't exist
    if(full_path.isEmpty() || !QFile::exists(full_path))
        return NULL;

    SQ_LIBRARY *l, *found = NULL;

    QFile file(full_path);
    char buffer[buffer_size+1];

    if(!file.open(IO_ReadOnly))
        return NULL;

    // read some bytes from file, and compare it with
    // regexps. If regexp succeeded, we found library!
    int rr = file.readBlock(buffer, buffer_size);

    if(rr != buffer_size || file.status() != IO_Ok)
    {
        file.close();
        return NULL;
    }
    else
        file.close();

    for(int j = 0;j < buffer_size;j++)
        if(buffer[j] == '\0')
            buffer[j] = '\001';

    buffer[buffer_size] = '\0';

    QString read = QString::fromLatin1(buffer);

    // go through array and compare current regexp with 'read'
    for(QValueVector<SQ_LIBRARY>::iterator it = begin();it != end();++it)
    {
        l = &(*it);

        if(!l->regexp_str.isEmpty())
            if(read.find(l->regexp) == 0) // we found library!
            {
                found = l;
                latest = found;
                break;
            }
    }

    // we found something...
    if(found)
    {
        // can found library read files ?
        if(found->codec->fmt_readable())
        {
            // yes, print some debug info and return
            kdDebug() << "[1] SQ_LibraryHandler: \""
                      << KStringHandler::csqueeze(full_path, 22)
                      << "\" => \""
                      << found->quickinfo
                      << "\""
                      << endl;

            return found;
        }

        // oops, library can't read files !
        return NULL;
    }

    QFileInfo f(full_path);
    QString ext = "*.";
    ext += f.extension(false);
    ext += " ";

    // library still not found ...
    // let's find it by file extension
    for(QValueVector<SQ_LIBRARY>::iterator it = begin();it != end();++it)
    {
        l = &(*it);

        if(l->filter.find(ext, 0, false) != -1 && l->regexp_str.isEmpty())
        {
            kdDebug() << "[2] SQ_LibraryHandler: \""
                      << KStringHandler::csqueeze(full_path, 22)
                      << "\" => \""
                      << l->quickinfo
                      << "\""
                      << endl;

            latest = l;

            return ((l && l->codec->fmt_readable()) ? l : NULL);
        }
    }

    return NULL;
}

/*
 *  Does SQ_LibraryHandler support given file ?
 *
 *  Supports, if libraryForFile(path) is not NULL.
 */
bool SQ_LibraryHandler::supports(const QString &f)
{
    return libraryForFile(f) != NULL;
}

/*
 *  Get all filters as one string.
 */
QString SQ_LibraryHandler::allFiltersString() const
{
    QString ret;

    // construct string
    for(QValueVector<SQ_LIBRARY>::const_iterator it = begin();it != end();++it)
    {
        if(!(*it).filter.isEmpty())
            ret = ret + (*it).filter + " ";
    }

    return ret;
}

/*
 *  Fill 'filters' with all found filters, and
 *  'quick' with appropriate information.
 */
void SQ_LibraryHandler::allFilters(QStringList &filters, QStringList &quick) const
{
    // clear rubbish
    filters.clear();
    quick.clear();

    // no found libraries ?
    if(empty())
        return;

    // go through array and fill QStringLists
    for(QValueVector<SQ_LIBRARY>::const_iterator it = begin();it != end();++it)
        if(!(*it).filter.isEmpty())
        {
            filters.append((*it).filter);
            quick.append((*it).quickinfo);
        }
}

void SQ_LibraryHandler::allWritableFilters(QStringList &filters, QStringList &quick) const
{
    // clear rubbish
    filters.clear();
    quick.clear();

    // no libraries ?
    if(empty())
        return;

    // go through array and fill QStringLists
    for(QValueVector<SQ_LIBRARY>::const_iterator it = begin();it != end();++it)
        if((*it).writable && !(*it).filter.isEmpty())
        {
            filters.append((*it).filter);
            quick.append((*it).quickinfo);
        }
}

/*
 *  Remove and unload all libraries.
 */
void SQ_LibraryHandler::clear()
{
    kdDebug() << "SQ_LibraryHandler::clear()" << endl;

    // unload libraries on clear()
    for(QValueVector<SQ_LIBRARY>::iterator it = begin();it != end();++it)
    {
        (*it).fmt_codec_destroy((*it).codec);
        (*it).lib->unload();
    }

    QValueVector<SQ_LIBRARY>::clear();
}

/*
 *  Clear old libraries, and load new.
 */
void SQ_LibraryHandler::reInit(QStringList *foundLibraries)
{
    clear();
    add(foundLibraries);
}

/*
 *  Add new libraries.
 */
void SQ_LibraryHandler::add(QStringList *foundLibraries)
{
    QString mime_str;
    int mime_len;

    // hell, nothing to add ?
    if(foundLibraries->empty()) return;

    for(QValueList<QString>::iterator it = foundLibraries->begin();it != foundLibraries->end();++it)
    {
        QFileInfo ff(*it);

        // bad library file
        if(ff.isDir() || !ff.isReadable())
            continue;

        SQ_LIBRARY libtmp;
        QStringList formats;

        // create QLibrary object
        libtmp.lib = new QLibrary(*it);
        libtmp.libpath = *it;
        libtmp.lib->load();

        // resolve create() and destroy() functions
        libtmp.fmt_codec_create = (fmt_codec_base*(*)())(libtmp.lib)->resolve("fmt_codec_create");
        libtmp.fmt_codec_destroy = (void (*)(fmt_codec_base*))(libtmp.lib)->resolve(QString::fromLatin1("fmt_codec_destroy"));

        // couldn't resolve - corrupted library ?
        if(!libtmp.fmt_codec_create || !libtmp.fmt_codec_destroy)
        {
            libtmp.lib->unload();
            delete libtmp.lib;
        }
        else
        {
            // create codec !
            fmt_codec_base *codeK = libtmp.fmt_codec_create();

#ifndef QT_NO_STL
            QString q = codeK->fmt_quickinfo();
#else
            QString q = codeK->fmt_quickinfo().c_str();
#endif

            // Yet unknown library ?
            if(!alreadyInMap(q))
            {

#ifndef QT_NO_STL
                mime_len = convertMimeFromBits(codeK->fmt_pixmap(), mime_str);
#else
                mime_len = convertMimeFromBits(codeK->fmt_pixmap().c_str(), mime_str);
#endif

                libtmp.mime.loadFromData((const uchar *)mime_str.ascii(), mime_len, "PNG");
                libtmp.quickinfo = q;

#ifndef QT_NO_STL

                libtmp.filter = codeK->fmt_filter();
                libtmp.version = codeK->fmt_version();
                libtmp.regexp_str = codeK->fmt_mime();

#else

                libtmp.filter = codeK->fmt_filter().c_str();
                libtmp.version = codeK->fmt_version().c_str();
                libtmp.regexp_str = codeK->fmt_mime().c_str();

#endif

                libtmp.regexp.setPattern(libtmp.regexp_str);
                libtmp.regexp.setCaseSensitive(true);
                libtmp.writable = codeK->fmt_writable();
                libtmp.readable = codeK->fmt_readable();

                if(libtmp.writable)
                    codeK->fmt_getwriteoptions(&libtmp.opt);

                libtmp.codec = codeK;

                append(libtmp);
            }
            else // already known library
            {
                // destroy codec
                libtmp.fmt_codec_destroy(codeK);

                // unload library
                libtmp.lib->unload();

                // delete object
                delete libtmp.lib;

                kdDebug() << "Library \"" << q << "\" is already in map" << endl;
            }
        }
    }

    // print some information
    dump();
}

/*
 *  Is library named 'quick' already been handled ?
 */
bool SQ_LibraryHandler::alreadyInMap(const QString &quick) const
{
    // go through array and find 'quick'
    for(QValueVector<SQ_LIBRARY>::const_iterator it = begin();it != end();++it)
        if((*it).quickinfo == quick)
            return true;

    return false;
}

/*
 *  Remove given libraries.
 */
void SQ_LibraryHandler::remove(QStringList *foundLibraries)
{
    for(QValueList<QString>::iterator it = foundLibraries->begin();it != foundLibraries->end();++it)
    {
        for(QValueVector<SQ_LIBRARY>::iterator vit = begin();vit != end();++vit)
        {
            if(*it == (*vit).libpath)
            {
                // destroy codec
                (*vit).fmt_codec_destroy((*vit).codec);

                // unload & delete library
                (*vit).lib->unload();
                delete (*vit).lib;

                // remove this entry
                erase(vit);
                break;
            }
        }
    }

    dump();
}

/*
 *  Print some information on found libraries.
 */
void SQ_LibraryHandler::dump() const
{
#ifndef SQ_SMALL

    kdDebug() << "SQ_LibraryHandler: memory dump (total " << count() << ")" << endl;

    for(QValueVector<SQ_LIBRARY>::const_iterator it = begin();it != end();++it)
    {
        kdDebug() << KStringHandler::csqueeze(QFileInfo((*it).libpath).fileName(), 30) << "  [" 
                  << KStringHandler::rsqueeze((*it).quickinfo, 45) << "]" << endl;
    }

#else

    kdDebug() << "SQ_LibraryHandler: memory dump (total " << count() << ")" << endl;

#endif
}

/*
 *  Does any of found libraries handle given extension ?
 */
bool SQ_LibraryHandler::knownExtension(const QString &ext)
{
    // go through array and compare extensions
    for(QValueVector<SQ_LIBRARY>::iterator it = begin();it != end();++it)
    {
        if((*it).filter.contains(ext, false))
            return true;
    }

    return false;
}

/*
 *  Find appropriate SQ_LIBRARY by its name. If
 *  not found, return NULL.
 *
 *  Name is a string, returned by fmt_quickinfo()
 */
SQ_LIBRARY* SQ_LibraryHandler::libraryByName(const QString &name)
{
    SQ_LIBRARY *l;

    // go through array and compare names
    for(QValueVector<SQ_LIBRARY>::iterator it = begin();it != end();++it)
    {
        l = &(*it);

        if(l->quickinfo == name)
            return l;
    }

    return NULL;
}

/*
 *  Find appropriate codec for given filename. If
 *  not found, return NULL.
 */
fmt_codec_base* SQ_LibraryHandler::codecForFile(const QString &file)
{
    // find SQ_LIBRARY by filename
    SQ_LIBRARY *lib = libraryForFile(file);

    //  return result
    return (lib && lib->codec) ? lib->codec : NULL;
}

/*
 *  Find appropriate codec for given library name. If
 *  not found, return NULL.
 */
fmt_codec_base* SQ_LibraryHandler::codecByName(const QString &name)
{
    SQ_LIBRARY *lib = libraryByName(name);

    // return result
    return (lib && lib->codec) ? lib->codec : NULL;
}

fmt_codec_base *SQ_LibraryHandler::latestCodec()
{
    return (latest && latest->codec) ? latest->codec : NULL;
}
