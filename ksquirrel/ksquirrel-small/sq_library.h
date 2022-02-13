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
#include <qimage.h>

#include "fmt_types.h"
#include "fmt_defs.h"

class QLibrary;

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
    SQ_LIBRARY() : lib(0), codec(0)
    {}

    // pointer to library
    QLibrary    *lib;
    
    // path to library on disk
    QString    libpath;

    // converted regular expression
    QRegExp    regexp;

    // filter for filemanager (returned by fmt_filter())
    QString    filter;

    // regular expression as string (returned by fmt_mime())
    QString    regexp_str;

    // information on codec (returned by fmt_quickinfo())
    QString    quickinfo;

    // codec's version  (returned by fmt_version())
    QString    version;

    // pointer to codec
    fmt_codec_base    *codec;

    // 'create' and 'destroy' functions.
    // should exist in library!
    fmt_codec_base*     (*fmt_codec_create)();
    void                (*fmt_codec_destroy)(fmt_codec_base*);

    // mime image (returned by fmt_pixmap())
    QImage     mime;

    // options for writers.
    fmt_writeoptionsabs    opt;

    // readable ?  (returned by fmt_readable())
    bool        writable;

    // writeable ?  (returned by fmt_writeable())
    bool        readable;
};

#endif
