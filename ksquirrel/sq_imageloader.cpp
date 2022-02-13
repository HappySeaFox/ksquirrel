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

#include <kdebug.h>

#include "sq_imageloader.h"
#include "sq_libraryhandler.h"

#include "fileio.h"
#include "fmt_utils.h"
#include "fmt_codec_base.h"
#include "error.h"

SQ_ImageLoader * SQ_ImageLoader::m_instance = NULL;

SQ_ImageLoader::SQ_ImageLoader(QObject *parent) : QObject(parent), m_errors(0)
{
    m_instance = this;

    kdDebug() << "+SQ_ImageLoader" << endl;

    m_image = NULL;
    dumbscan = NULL;
    finfo = new fmt_info;
}

SQ_ImageLoader::~SQ_ImageLoader()
{
    kdDebug() << "-SQ_ImageLoader" << endl;

    delete finfo;

    if(dumbscan) free(dumbscan);
    if(m_image) free(m_image);
}

/*
 *  Try to determine image dimensions.
 */
bool SQ_ImageLoader::tasteImage(const QString &path, int *w, int *h)
{
    SQ_LIBRARY      *lib;
    int             res;
    fmt_codec_base  *codeK;

    // determine library for file
    lib = SQ_LibraryHandler::instance()->libraryForFile(path);

    if(!lib || !lib->codec)
        return false;

    // determine codec
    codeK = lib->codec;

#ifndef QT_NO_STL

    // init...
    res = codeK->fmt_read_init(QString(path.local8Bit()));

#else

    res = codeK->fmt_read_init(QString(path.local8Bit()).ascii());

#endif

    // error in init()!
    if(res != SQE_OK)
    {
        codeK->fmt_read_close();
        return false;
    }

    // advance to next page
    res = codeK->fmt_read_next();

    if(res != SQE_OK)
    {
        codeK->fmt_read_close();
        return false;
    }
    else
    {
        fmt_image *image;

        image = codeK->image(0);
        *w = image->w;
        *h = image->h;
        codeK->fmt_read_close();

        return true;
    }
}

/*
 *  Try to load image and store a pointer to decoded image data in m_image. Aslo
 *  store information about the image in finfo.
 */
bool SQ_ImageLoader::loadImage(const QString &pixPath, bool multi)
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
    codeK->fmt_read_close();

    // reset error count
    m_errors = 0;

    // determine library for file
    lib = SQ_LibraryHandler::instance()->libraryForFile(pixPath);

    if(!lib || !lib->codec)
        return false;

    // determine codec
    codeK = lib->codec;

#ifndef QT_NO_STL

    // init...
    res = codeK->fmt_read_init(QString(pixPath.local8Bit()));

#else

    res = codeK->fmt_read_init(QString(pixPath.local8Bit()).ascii());

#endif

    // error in init()!
    if(res != SQE_OK)
    {
        codeK->fmt_read_close();
        return false;
    }

    // read all pages in image...
    while(true)
    {
        // error occured while decoding previuos page...
        if(m_errors)
            break;

        // advance to next page
        i = codeK->fmt_read_next();

        // error occured while decoding first page
        if(i != SQE_OK && i != SQE_NOTOK && !current)
        {
            codeK->fmt_read_close();
            return false;
        }
        else if((i != SQE_OK && i != SQE_NOTOK && current) || (i == SQE_NOTOK && current) || (!multi && current))
            break;

        image = codeK->image(current);

        // realloc memory
        if(!current)
        {
            const int S =image->w * image->h * sizeof(RGBA);

            m_image = (RGBA *)realloc(m_image, S);

            if(!m_image)
            {
                codeK->fmt_read_close();
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
            codeK->fmt_read_next_pass();

            if(!current)
            {
                for(j = 0;j < image->h;j++)
                {
                    scan = m_image + j * image->w;
                    i = codeK->fmt_read_scanline(scan);
                    m_errors += (i != SQE_OK);
                }
            }
            else
            {
                for(j = 0;j < image->h;j++)
                {
                    i = codeK->fmt_read_scanline(dumbscan);

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
void SQ_ImageLoader::cleanup()
{
    if(dumbscan) free(dumbscan);
    if(m_image) free(m_image);

    dumbscan = NULL;
    m_image = NULL;
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
