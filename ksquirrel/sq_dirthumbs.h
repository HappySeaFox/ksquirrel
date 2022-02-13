/***************************************************************************
                          sq_dirthumbs.h  -  description
                             -------------------
    begin                : ??? Jul 18 2007
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

#ifndef SQ_DIR_THUMBS_H
#define SQ_DIR_THUMBS_H

#include "sq_dir.h"

struct SQ_Thumbnail;

class SQ_DirThumbs : public SQ_Dir
{
    public:
        SQ_DirThumbs();
        ~SQ_DirThumbs();

        virtual QString absPath(const KURL &relurl);

        /*
         *  Save thumbnail to storage.
         */
        void saveThumbnail(const KURL &url, SQ_Thumbnail &thumb);

        /*
         *  Remove file from storage
         */
        void removeFile(const KURL &url);

        /*
         *  Compare 'last modified' time of 'url'
         *  and thumbnail time.
         */
        bool needUpdate(const QString &turl, time_t tm);

    private:
        /*
         *  Thumbnail cache will have ~/.thumbnails/normal path
         */
        void setRoot();

    private:
        /*
         *  m_orig to save thumbnails
         */
        QString m_orig;
};

#endif
