/***************************************************************************
                          sq_thumbnailinfo.h  -  description
                             -------------------
    begin                : ðÎÄ ïËÔ 11 2004
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

#ifndef SQ_THUMBNAILINFO_H
#define SQ_THUMBNAILINFO_H

#include <qstringlist.h>
#include <qimage.h>

/*
 *  Information on thumbnail, where all parameters are given as strings.
 */
struct SQ_ThumbInfo
{
	QString type, dimensions, bpp, color, compression, frames, uncompressed;
	QImage mime;
};

/*
 *  Represents thumbnail.
 */
struct SQ_Thumbnail
{
	SQ_ThumbInfo info;
	QImage thumbnail;
};

#ifdef SQ_HAVE_MIMESTRING

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

#endif

#endif
