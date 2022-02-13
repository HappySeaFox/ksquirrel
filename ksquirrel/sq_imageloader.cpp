/***************************************************************************
                          sq_imageloader.cpp  -  description
                             -------------------
    begin                : Tue Sep 20 2005
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qfile.h>

#include "sq_imageloader.h"
#include "sq_libraryhandler.h"
#include "sq_codecsettings.h"

#include <ksquirrel-libs/fileio.h>
#include <ksquirrel-libs/fmt_utils.h>
#include <ksquirrel-libs/fmt_codec_base.h>
#include <ksquirrel-libs/error.h>

SQ_ImageLoader * SQ_ImageLoader::m_instance = 0;

SQ_ImageLoader::SQ_ImageLoader(QObject *parent) : QObject(parent), m_errors(0)
{
    m_instance = this;

    m_image = 0;
    dumbscan = 0;
    finfo = new fmt_info;
}

SQ_ImageLoader::~SQ_ImageLoader()
{
    delete finfo;

    if(dumbscan) free(dumbscan);
    if(m_image) free(m_image);
}

/*
 *  Try to determine image dimensions.
 */
bool SQ_ImageLoader::tasteImage(const QString &path, int *w, int *h, SQ_LIBRARY *_lib)
{
    SQ_LIBRARY      *lib;
    int             res;
    fmt_codec_base  *codeK;

    // determine library for file
    lib = _lib ? _lib : SQ_LibraryHandler::instance()->libraryForFile(path);

    if(!lib || !lib->codec)
        return false;

    // determine codec
    codeK = lib->codec;

    // init...
    res = codeK->read_init(QString(QFile::encodeName(path)));

    // error in init()!
    if(res != SQE_OK)
    {
        codeK->read_close();
        return false;
    }

    // advance to next page
    res = codeK->read_next();

    if(res != SQE_OK)
    {
        codeK->read_close();
        return false;
    }
    else
    {
        fmt_image *image;

        image = codeK->image(0);
        *w = image->w;
        *h = image->h;
        codeK->read_close();

        return true;
    }
}

/*
 *  Try to load image and store a pointer to decoded image data in m_image. Aslo
 *  store information about the image in finfo.
 */
bool SQ_ImageLoader::loadImage(const QString &pixPath, bool multi, int nomorethan, bool changeSettings)
{
    SQ_LIBRARY      *lib;
    int    res, current = 0, i, j;
    RGBA     *scan;
    fmt_codec_base  *codeK;
    fmt_image       *image;

    finfo->image.clear();
    finfo->meta.clear();

#define SQ_SAVE_INF \
    *finfo = codeK->information(); \
    codeK->read_close();

    // reset error count
    m_errors = 0;

    // determine library for file
    lib = SQ_LibraryHandler::instance()->libraryForFile(pixPath);

    if(!lib || !lib->codec)
        return false;

    // determine codec
    codeK = lib->codec;

    // init...
    res = codeK->read_init(QString(QFile::encodeName(pixPath)));

    // error in init()!
    if(res != SQE_OK)
    {
        codeK->read_close();
        return false;
    }

    if(changeSettings)
        SQ_CodecSettings::applySettings(lib, SQ_CodecSettings::ThumbnailLoader);

    // read all pages in image...
    while(true)
    {
        if(m_errors || (!multi && current) || (multi && current == nomorethan))
            break;

        // advance to next page
        i = codeK->read_next();

        // error occured while decoding first page
        if(i != SQE_OK && i != SQE_NOTOK && !current)
        {
            codeK->read_close();
            return false;
        }
        else if((i != SQE_OK && i != SQE_NOTOK && current) || (i == SQE_NOTOK && current))
            break;

        image = codeK->image(current);

        // realloc memory
        if(!current)
        {
            const int S = image->w * image->h * sizeof(RGBA);

            m_image = (RGBA *)realloc(m_image, S);

            if(!m_image)
            {
                codeK->read_close();
                return false;
            }

            memset(m_image, 255, S);
        }
        else
        {
            dumbscan = (RGBA *)realloc(dumbscan, image->w * image->h * sizeof(RGBA));

            if(!dumbscan)
            {
                SQ_SAVE_INF
                return false;
            }
        }

        // read all passes
        for(int pass = 0;pass < image->passes;pass++)
        {
            codeK->read_next_pass();

            if(!current)
            {
                for(j = 0;j < image->h;j++)
                {
                    scan = m_image + j * image->w;
                    i = codeK->read_scanline(scan);
                    m_errors += (i != SQE_OK);
                }
            }
            else
            {
                for(j = 0;j < image->h;j++)
                {
                    i = codeK->read_scanline(dumbscan);

                    if(i != SQE_OK)
                    {
                        SQ_SAVE_INF
                        return false;
                    }
                }
            }
        }

        // flip decoded image, if needed
        if(image->needflip && !current)
            fmt_utils::flipv((char *)m_image, image->w * sizeof(RGBA), image->h);

        current++;
    }

    SQ_SAVE_INF

    // all done!
    return true;
}

// Delete buffers...
void SQ_ImageLoader::cleanup(bool del)
{
    if(dumbscan) free(dumbscan);
    if(del && m_image) free(m_image);

    dumbscan = 0;
    m_image = 0;
}

/*
 *  Return decoded image as QImage.
 */
QImage SQ_ImageLoader::image() const
{
    QImage image((unsigned char*)bits(), finfo->image[0].w, finfo->image[0].h, 32, 0, 0, QImage::LittleEndian);

    // if the image has alpha channel, let QImage know it
    if(finfo->image[0].hasalpha)
        image.setAlphaBuffer(true);

    return image.swapRGB();
}
