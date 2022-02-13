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

#include <qstring.h>
#include <qregexp.h>
#include <qimage.h>

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
