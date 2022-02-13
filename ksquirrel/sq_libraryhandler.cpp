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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qlibrary.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qdir.h>

#include <kstringhandler.h>
#include <ktempfile.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>

#include "sq_libraryhandler.h"
#include "sq_config.h"

#include <ksquirrel-libs/fmt_codec_base.h>

static const int buffer_size = 10;

SQ_LibraryHandler * SQ_LibraryHandler::m_instance = 0;

// SQ_LibraryHandler
SQ_LibraryHandler::SQ_LibraryHandler(QObject *parent) 
    : QObject(parent), QValueVector<SQ_LIBRARY>(), latest(0)
{
    m_instance = this;

    kdDebug() << "+SQ_LibraryHandler" << endl;

    kconf = new KConfig("ksquirrel-codec-settings");

    load();
}

SQ_LibraryHandler::~SQ_LibraryHandler()
{
    clear();

    delete kconf;

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
        return 0;

    QTime tm;
    tm.start();

    QFileInfo fi(full_path);

    // Cache support since 0.7.0 :)
    LibCache::iterator it = cache.find(full_path);

    if(it != cache.end())
    {
        if(fi.lastModified().isValid() && fi.lastModified() == it.data().modified)
        {
            latest = it.data().library;

            debugInfo("[C]", full_path, latest->quickinfo, tm.elapsed());

            return latest;
        }
    }

    SQ_LIBRARY *l, *found = 0;

    QFile file(full_path);
    char buffer[buffer_size+1];

    if(!file.open(IO_ReadOnly))
        return 0;

    // read some bytes from file, and compare it with
    // regexps. If regexp succeeded, we found library!
    int rr = file.readBlock(buffer, buffer_size);

    if(rr != buffer_size || file.status() != IO_Ok)
    {
        file.close();
        return 0;
    }
    else
        file.close();

    for(int j = 0;j < buffer_size;j++)
        if(buffer[j] == '\0')
            buffer[j] = '\001';

    buffer[buffer_size] = '\0';

    QString read = QString::fromLatin1(buffer);

    iterator itEnd = end();

    // go through array and compare current regexp with 'read'
    for(iterator it = begin();it != itEnd;++it)
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
        if(found->readable)
        {
            debugInfo("[1]", full_path, found->quickinfo, tm.elapsed());

            if(fi.lastModified().isValid())
                cache.insert(full_path, LibCacheEntry(fi.lastModified(), found));

            return found;
        }

        // oops, library can't read files !
        return 0;
    }

    QString ext = "*.";
    ext += fi.extension(false);
    ext += " ";

    // library still not found ...
    // let's find it by file extension
    for(iterator it = begin();it != itEnd;++it)
    {
        l = &(*it);

        if(l->filter.find(ext, 0, false) != -1 && l->regexp_str.isEmpty())
        {
            debugInfo("[2]", full_path, l->quickinfo, tm.elapsed());

            latest = l;

            if(l && l->readable)
            {
                if(fi.lastModified().isValid())
                    cache.insert(full_path, LibCacheEntry(fi.lastModified(), l));

                return l;
            }

            return 0;
        }
    }

    return 0;
}

/*
 *  Does SQ_LibraryHandler support given file ?
 *
 *  Supports, if libraryForFile(path) is not NULL.
 */
bool SQ_LibraryHandler::supports(const QString &f)
{
    return libraryForFile(f) != 0;
}

/*
 *  Get all filters as one string.
 */
QString SQ_LibraryHandler::allFiltersString() const
{
    QString ret;

    const_iterator itEnd = end();

    // construct string
    for(const_iterator it = begin();it != itEnd;++it)
    {
        if(!(*it).filter.isEmpty())
            ret = ret + (*it).filter + " ";
    }

    return ret;
}

QString SQ_LibraryHandler::allFiltersFileDialogString(bool r, bool allfiles) const
{
    QString ret;

    const_iterator itEnd = end();

    // construct string
    for(const_iterator it = begin();it != itEnd;++it)
    {
        if(!r)
            if((*it).writestatic)
                ret = ret + (*it).filter + "|" + (*it).quickinfo + "\n";
            else;
        else if((*it).readable)
            ret = ret + (*it).filter + "|" + (*it).quickinfo + "\n";
    }

    return allfiles ? (ret + "*.*|" + i18n("All files")) : ret.left(ret.length() - 1);
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

    const_iterator itEnd = end();

    // go through array and fill QStringLists
    for(const_iterator it = begin();it != itEnd;++it)
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

    const_iterator itEnd = end();

    // go through array and fill QStringLists
    for(const_iterator it = begin();it != itEnd;++it)
        if((*it).writestatic && !(*it).filter.isEmpty())
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

    cache.clear();

    iterator itEnd = end();

    // unload libraries on clear()
    for(iterator it = begin();it != itEnd;++it)
    {
        writeSettings(&(*it));

        // delete temp file
        if((*it).needtempfile)
            delete (*it).tmp;

        (*it).codec_destroy((*it).codec);
        delete (*it).lib;
        (*it).lib = 0;
    }

    QValueVector<SQ_LIBRARY>::clear();
}

/*
 *  Add new libraries.
 */
void SQ_LibraryHandler::add(QStringList &foundLibraries)
{
    codec_options o;

    QStringList::iterator itEnd = foundLibraries.end();

    for(QStringList::iterator it = foundLibraries.begin();it != itEnd;++it)
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
        libtmp.codec_create = (fmt_codec_base*(*)())(libtmp.lib)->resolve(QString::fromLatin1("codec_create"));
        libtmp.codec_destroy = (void (*)(fmt_codec_base*))(libtmp.lib)->resolve(QString::fromLatin1("codec_destroy"));

        // couldn't resolve - corrupted library ?
        if(!libtmp.codec_create || !libtmp.codec_destroy)
        {
            libtmp.lib->unload();
            delete libtmp.lib;
        }
        else
        {
            // create codec !
            fmt_codec_base *codeK = libtmp.codec_create();

            // read options
            codeK->options(&o);

            QString q = o.name;

            // Yet unknown library ?
            if(!alreadyInMap(q))
            {

                // get all information on codec: filter, mime icon, mime regexp,
                libtmp.mime = QPixmap(reinterpret_cast<const char **>(o.pixmap));
                libtmp.quickinfo = q;
                libtmp.filter = o.filter;
                libtmp.version = o.version;
                libtmp.regexp_str = o.mime;
                libtmp.config = o.config;
                libtmp.regexp.setPattern(libtmp.regexp_str);
                libtmp.regexp.setCaseSensitive(true);
                libtmp.writestatic = o.writestatic;
                libtmp.writeanimated = o.writeanimated;
                libtmp.readable = o.readable;
                libtmp.canbemultiple = o.canbemultiple;
                libtmp.needtempfile = o.needtempfile;
                libtmp.tmp = 0;

                if(libtmp.needtempfile)
                {
                    libtmp.tmp = new KTempFile;
                    libtmp.tmp->setAutoDelete(true);
                    libtmp.tmp->close();
                    codeK->settempfile(libtmp.tmp->name());
                }

                if(libtmp.writestatic)
                    codeK->getwriteoptions(&libtmp.opt);

                libtmp.codec = codeK;

                readSettings(&libtmp);

                append(libtmp);
            }
            else // already known library
            {
                // destroy codec
                libtmp.codec_destroy(codeK);

                // unload library
                libtmp.lib->unload();

                delete libtmp.lib;
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
    const_iterator itEnd = end();

    // go through array and find 'quick'
    for(const_iterator it = begin();it != itEnd;++it)
        if((*it).quickinfo == quick)
            return true;

    return false;
}

/*
 *  Print some information on found libraries.
 */
void SQ_LibraryHandler::dump() const
{
    kdDebug() << "SQ_LibraryHandler: memory dump (total " << count() << ")" << endl;

    const_iterator itEnd = end();

    for(const_iterator it = begin();it != itEnd;++it)
    {
        kdDebug() << KStringHandler::csqueeze(QFileInfo((*it).libpath).fileName(), 30) << "  [" 
                  << KStringHandler::rsqueeze((*it).quickinfo, 45)
                  << "]"
                  << endl;
    }
}

/*
 *  Does any of found libraries handle given extension ?
 */
bool SQ_LibraryHandler::knownExtension(const QString &ext)
{
    iterator itEnd = end();

    // go through array and compare extensions
    for(iterator it = begin();it != itEnd;++it)
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

    iterator itEnd = end();

    // go through array and compare names
    for(iterator it = begin();it != itEnd;++it)
    {
        l = &(*it);

        if(l->quickinfo == name)
            return l;
    }

    return 0;
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
    return (lib && lib->codec) ? lib->codec : 0;
}

/*
 *  Find appropriate codec for given library name. If
 *  not found, return NULL.
 */
fmt_codec_base* SQ_LibraryHandler::codecByName(const QString &name)
{
    SQ_LIBRARY *lib = libraryByName(name);

    // return result
    return (lib && lib->codec) ? lib->codec : 0;
}

fmt_codec_base* SQ_LibraryHandler::latestCodec()
{
    return (latest && latest->codec) ? latest->codec : 0;
}

void SQ_LibraryHandler::writeSettings(SQ_LIBRARY *lib)
{
    // no config - no settings
    if(lib->config.isEmpty())
        return;

    kconf->setGroup(lib->quickinfo);

    fmt_settings::iterator itEnd = lib->settings.end();

    QString k;

    for(fmt_settings::iterator it = lib->settings.begin();it != itEnd;++it)
    {
        k = (*it).first;

        if((*it).second.type == settings_value::v_bool) // boolean
        {
           k.prepend("b");
           kconf->writeEntry(k, (*it).second.bVal);
        }
        else if((*it).second.type == settings_value::v_int) // integer
        {
            k.prepend("i");
            kconf->writeEntry(k, (*it).second.iVal);
        }
        else if((*it).second.type == settings_value::v_double) // double
         {
             k.prepend("d");
             kconf->writeEntry(k, (*it).second.dVal);
         }
        else // string
        {
            k.prepend("s");
            kconf->writeEntry(k, (*it).second.sVal);
        }
    }
}

void SQ_LibraryHandler::readSettings(SQ_LIBRARY *lib)
{
    // no config - no settings
    if(lib->config.isEmpty())
        return;

    QMap<QString, QString> map = kconf->entryMap(lib->quickinfo);

    if(!map.size())
    {
        lib->codec->fill_default_settings();
        lib->settings = lib->codec->settings();
        return;
    }

    QMap<QString, QString>::iterator mapEnd = map.end();
    fmt_settings &sett = lib->settings;
    QString d, k;
    settings_value val;

    for(QMap<QString, QString>::iterator mapIt = map.begin();mapIt != mapEnd;++mapIt)
    {
        k = mapIt.key();
        d = mapIt.data();

        if(k.startsWith("i"))
        {
            val.type = settings_value::v_int;
            val.iVal = d.toInt();
        }
        else if(k.startsWith("d"))
        {
            val.type = settings_value::v_double;
            val.dVal = d.toDouble();
        }
        else if(k.startsWith("b"))
        {
            val.type = settings_value::v_bool;
            val.bVal = (d == "true");
        }
        else // all other values are treated as strings
        {
            val.type = settings_value::v_string;
            val.sVal = d.ascii();
        }

        k = k.right(k.length() - 1);
        sett[k.ascii()] = val;
    }

    lib->codec->set_settings(sett);
}

void SQ_LibraryHandler::debugInfo(const QString &symbol, const QString &path, const QString &lib, int ms)
{
    kdDebug() << symbol
              << " SQ_LibraryHandler: \""
              << KStringHandler::lsqueeze(path, 20)
              << "\" => \""
              << lib
              << "\" ["
              << ms
              << " ms.]"
              << endl;
}

void SQ_LibraryHandler::reload()
{
    clear();
    load();
}

void SQ_LibraryHandler::load()
{
    QStringList libs;

    QDir dir(SQ_KLIBS, QString::null, QDir::Unsorted, QDir::Files);

    const QFileInfoList *list = dir.entryInfoList();
    QFileInfoListIterator it(*list);
    QFileInfo *fi;

    while((fi = it.current()) != 0)
    {
        libs.append(fi->absFilePath());
        ++it;
    }

    add(libs);
}
