/***************************************************************************
                          sq_utils.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qstring.h>

#include <kurl.h>
#include <kio/global.h>

#include <ksquirrel-libs/fmt_defs.h>

#include "sq_thumbnailinfo.h"
#include "sq_thumbnailsize.h"
#include "sq_libraryhandler.h"
#include "sq_imageloader.h"
#include "libpixops/pixops.h"
#include "sq_utils.h"

#ifdef SQ_HAVE_KEXIF
#include <libkexif/kexifdata.h>
#endif

void SQ_Utils::exifRotate(const QString &file, QImage &im, int o)
{
#ifdef SQ_HAVE_KEXIF
    im = im.xForm(SQ_Utils::exifGetMatrix(file, o));
#endif
}

QWMatrix SQ_Utils::exifGetMatrix(const QString &file, int o)
{
    QWMatrix matrix;

#ifdef SQ_HAVE_KEXIF
    int O;

    if(o == -1)
    {
        KExifData data;
        data.readFromFile(file);
        O = data.getImageOrientation();
    }
    else
        O = o;

    switch(O)
    {
        case KExifData::HFLIP:        matrix.scale(-1,1); break;
        case KExifData::ROT_180:      matrix.rotate(180); break;
        case KExifData::VFLIP:        matrix.scale(1,-1); break;
        case KExifData::ROT_90_HFLIP: matrix.scale(-1,1); matrix.rotate(90); break;
        case KExifData::ROT_90:       matrix.rotate(90);  break;
        case KExifData::ROT_90_VFLIP: matrix.scale(1,-1); matrix.rotate(90); break;
        case KExifData::ROT_270:      matrix.rotate(270); break;

        // normal rotation or unspecified
        default: ;
    }

#endif

    return matrix;
}

QImage SQ_Utils::scaleImage(unsigned char *im, int w, int h, int fitwithin)
{
    if(w <= fitwithin && h <= fitwithin)
    {
        QImage scaled(im, w, h, 32, 0, 0, QImage::LittleEndian);
        scaled.setAlphaBuffer(true);
        return scaled.copy();
    }

    QSize sz(w, h);
    sz.scale(fitwithin, fitwithin, QSize::ScaleMin);
    QImage scaled(sz.width(), sz.height(), 32);
    scaled.setAlphaBuffer(true);

    pixops_scale(scaled.bits(), 0, 0, scaled.width(), scaled.height(), scaled.width() * 4, 4, true,
                    im, w, h, w * 4, 4, true,
                    (double)sz.width() / w, (double)sz.height() / h,
                    PIXOPS_INTERP_BILINEAR);

    return scaled;
}

bool SQ_Utils::loadThumbnail(const KURL &pixPath, SQ_Thumbnail &t)
{
    fmt_info *finfo;

    RGBA *all;

    bool b = SQ_ImageLoader::instance()->loadImage(pixPath.path(), true, 2, true);

    //printf("LOAD %s = %d\n", pixPath.prettyURL().ascii(), b);

    finfo = SQ_ImageLoader::instance()->info();
    all = SQ_ImageLoader::instance()->bits();

    // memory allocation failed in SQ_ImageLoader::loadImage()
    if(!all)
        return false;

    // another error occured...
    if(!b)
    {
        // if our image is partially corrupted - show it. The image
        // is partially corrupted, if number of errors < number of scanlines
        // and at least one page was loaded.
        if(!finfo->image.size()
            || (SQ_ImageLoader::instance()->errors() == finfo->image[0].h && finfo->image.size() == 1))
        {
            SQ_ImageLoader::instance()->cleanup();
            return false;
        }
    }

    SQ_LIBRARY *lib = SQ_LibraryHandler::instance()->libraryForFile(pixPath.path());

    t.w = finfo->image[0].w;
    t.h = finfo->image[0].h;

    t.mime = lib->mime;
    t.thumbnail = SQ_Utils::scaleImage((unsigned char *)all, finfo->image[0].w,
            finfo->image[0].h, SQ_ThumbnailSize::biggest());

    SQ_ImageLoader::instance()->cleanup();

    // finally, rotate thumbnail using EXIF
    SQ_Utils::exifRotate(pixPath.path(), t.thumbnail);

    return true;
}