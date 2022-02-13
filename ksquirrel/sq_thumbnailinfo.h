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

#include <ctime>

// default thumbnail format
#define sqdirThumbFormat "PNG"

// default quality
#define sqdirThumbQuality 85

#define sqdirMimeFormat "XPM"

/*
 *  Represents thumbnail.
 */
struct SQ_Thumbnail
{
    // mime icon from image codec
    QPixmap mime;

    // 'last modifiled' time of original file.
    // we should store it for non-local urls
    time_t originalTime;

    int w, h;

    QImage thumbnail;
};

#endif
