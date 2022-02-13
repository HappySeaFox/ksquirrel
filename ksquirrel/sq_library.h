/***************************************************************************
                          sq_library.h  -  description
                             -------------------
    begin                : ðÎÄ éÀÎ 27 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_LIBRARY_H
#define SQ_LIBRARY_H

#include <qstring.h>
#include <qregexp.h>
#include <qpixmap.h>

#include <ksquirrel-libs/fmt_defs.h>
#include <ksquirrel-libs/settings.h>

class QLibrary;

class KTempFile;

class fmt_codec_base;

/*
 *  SQ_LIBRARY represents a library. It contains all information
 *  needed by other classes, e.g. pointer to codec, mime image,
 *  version string, filter string, etc.
 *
 *  Used by SQ_LibraryHandler.
 */

struct SQ_LIBRARY
{
    SQ_LIBRARY() : lib(0), codec(0), tmp(0)
    {}

    // pointer to library
    QLibrary    *lib;
    
    // path to a library on disk
    QString    libpath;

    // converted regular expression
    QRegExp    regexp;

    // filter for a filemanager
    QString    filter;

    // path to config file (.ui)
    QString    config;

    fmt_settings settings;

    // regular expression as string
    QString    regexp_str;

    // information on codec
    QString    quickinfo;

    // codec's version
    QString    version;

    // pointer to a codec
    fmt_codec_base    *codec;

    // 'create' and 'destroy' functions.
    // should exist in library!
    fmt_codec_base*     (*codec_create)();
    void                (*codec_destroy)(fmt_codec_base*);

    QPixmap     mime;

    // options for writers.
    fmt_writeoptionsabs    opt;

    bool        writestatic, writeanimated;
    bool        readable;
    bool        canbemultiple, needtempfile;
    
    KTempFile *tmp;
};

#endif
