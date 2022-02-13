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

    QImage orig(im, w, h, 32, 0, 0, QImage::LittleEndian);
    orig.setAlphaBuffer(true);

    // return scaled image
    return SQ_Utils::scale(orig, fitwithin, fitwithin, SQ_Utils::SMOOTH_FAST, QImage::ScaleMin);
}

bool SQ_Utils::loadThumbnail(const KURL &pixPath, SQ_Thumbnail &t)
{
    SQ_LIBRARY *lib = 0;

#ifdef SQ_HAVE_KEXIF
    lib = SQ_LibraryHandler::instance()->libraryForFile(pixPath.path());
    bool th = false;

    if(lib)
    {
        KExifData data;
        data.readFromFile(pixPath.path());
        QImage im = data.getThumbnail();

        if(!im.isNull())
        {
            SQ_Utils::exifRotate(QString::null, im, data.getImageOrientation());

            th = true;
            t.w = 0;
            t.h = 0;
            t.mime = lib->mime;

            QString w, h;
            w = im.text("Thumb::Image::Width");
            h = im.text("Thumb::Image::Height");
            t.w = w.toInt();
            t.h = h.toInt();

            if(!t.w || !t.h)
                SQ_ImageLoader::instance()->tasteImage(pixPath.path(), &t.w, &t.h, lib);

            t.thumbnail = SQ_Utils::scaleImage((unsigned char *)im.bits(), im.width(),
                im.height(), SQ_ThumbnailSize::biggest());
            t.thumbnail = t.thumbnail.swapRGB();
        }
    }
    else
        return false;

    // thumbnail loaded - nothing to do,
    // or load thumbnail by hands otherwise.
    if(th)
        return true;
#endif

    fmt_info *finfo;

    RGBA *all;

    bool b = SQ_ImageLoader::instance()->loadImage(pixPath.path(), SQ_CodecSettings::ThumbnailLoader);

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

    if(!lib) lib = SQ_LibraryHandler::instance()->libraryForFile(pixPath.path());

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
