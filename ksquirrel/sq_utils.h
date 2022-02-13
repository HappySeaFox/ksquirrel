/***************************************************************************
                          sq_utils.h  -  description
                             -------------------
    begin                : Thu Aug 2 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#ifndef SQ_UTILS_H
#define SQ_UTILS_H

#include <qimage.h>
#include <qwmatrix.h>

class KURL;

class QString;

class SQ_Thumbnail;

/*
 *  Helper class for SQ_GLWidget and SQ_ThumbnailLoadJob:
 *
 *  thumbnail loader, rotater, mmx scaler
 */

namespace SQ_Utils
{
    /*
     *  Scale given image to fit it within 'fitwithin'
     */
    QImage scaleImage(unsigned char *im, int w, int h, int fitwithin);

    /*
     *  Create and save thumbnail for 'pixPath'
     */
    bool loadThumbnail(const KURL &pixPath, SQ_Thumbnail&);

    /*
     * determine EXIF rotation and rotate image if needed
     */
    void exifRotate(const QString &file, QImage &i, int o = -1);

    QWMatrix exifGetMatrix(const QString &file, int o = -1);
};

#endif
