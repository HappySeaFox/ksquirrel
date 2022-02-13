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

#include <qstring.h>
#include <qpixmap.h>
#include <qimage.h>

/*
 *  Information on thumbnail, where all parameters are given as strings.
 */
struct SQ_ThumbInfo
{
    QString type, dimensions, bpp, color, compression, uncompressed;
    int frames;
    QPixmap mime;
};

/*
 *  Represents thumbnail.
 */
struct SQ_Thumbnail
{
    SQ_ThumbInfo info;
    QImage thumbnail;
};

#endif
