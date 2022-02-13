/***************************************************************************
                          sq_imageloader.h  -  description
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

#ifndef SQ_IMAGELOADER_H
#define SQ_IMAGELOADER_H

#include <qstring.h>
#include <qimage.h>
#include <qobject.h>

struct fmt_info;
struct RGBA;

struct SQ_LIBRARY;

/*
 *  SQ_ImageLoader namespace represents functions to decode images. Uses
 *  SQ_LibraryHandler. Used by SQ_EditBase (preview image) and SQ_ThumbnailLoadJob
 *  (for generating thumbnails).
 */

class SQ_ImageLoader : public QObject
{
    public:
        SQ_ImageLoader(QObject *parent);
        ~SQ_ImageLoader();

        /*
         *  Try to load image and store a pointer to decoded image data in m_image. Aslo
         *  store information about the image in finfo.
         *
         *  If 'multi' is true, read all image pages from file.
         */
        bool loadImage(const QString &path, bool multi = true, int nomorethan = -1, bool changeSettings = false);

        /*
         *  Try to determine image dimensions.
         */
        bool tasteImage(const QString &path, int *w, int *h, SQ_LIBRARY *_lib = 0);

        /*
         *  Remove any previously saved data.
         */
        void cleanup(bool del = true);

        /*
         *  Direct access to image bits...
         */
        RGBA* bits() const;

        /*
         *  Return decoded image as QImage.
         */
        QImage image() const;

        /*
         *  Get a pointer to structure with information
         *  on decoded image (number of pages, width, height...)
         */
        fmt_info* info() const;

        int errors() const;

        static SQ_ImageLoader* instance() { return m_instance; }

    private:
        RGBA *m_image, *dumbscan;
        fmt_info *finfo;
        int m_errors;

        static SQ_ImageLoader *m_instance;
};

inline
RGBA* SQ_ImageLoader::bits() const
{
    return m_image;
}

inline
fmt_info* SQ_ImageLoader::info() const
{
    return finfo;
}

inline
int SQ_ImageLoader::errors() const
{
    return m_errors;
}

#endif
