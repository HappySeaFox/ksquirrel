/***************************************************************************
                          sq_dir.h  -  description
                             -------------------
    begin                : ??? ??? 20 2004
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

#ifndef SQ_DIR_H
#define SQ_DIR_H

#include <qdir.h>

#include <ctime>

class KURL;

/*
 *  Class for managing application-specific data. It takes care
 *  of storing thumbnails, unpacked archives.
 *
 *  All data stored in ~/.ksquirrel (called 'storage').
 *
 *  Objects of this class are used by SQ_Archivehandler, SQ_PixmapCache,
 *  SQ_ThumbnailJob etc.
 */

class SQ_Dir : public QDir
{
    public:
        enum Prefix {

            // image basket cache
            Basket,

            // thumbnail cache (~/.ksquirrel/thumbnails).
            Thumbnails,

            // image categories (~/.ksquirrel/categories).
            Categories};

        SQ_Dir(Prefix);
        ~SQ_Dir();

        /*
         *  Create relative directory in storage.
         *
         *  For example, if prefix == Thumbnails,
         *  mkdir("/mnt/win_c") will create
         *  ~/.ksquirrel/thumbnails/mnt/win_c.
         *
         *  Return true, if operation succeded.
         */
        bool mkdir(const QString &relpath);

        /*
         *  Get current root directory.
         *
         *  For example, if prefix == Thumbnails, it will
         *  return "/home/krasu/.ksquirrel/thumbnails".
         */
        QString root() const;

        /*
         *  Get absolute path for relative path 'relpath'.
         *
         *  Reimplemented in SQ_DirThumbs to handle MD5-encoded
         *  urls.
         */
        virtual QString absPath(const KURL &relurl);

    private:
        /*
         *  Internal, used by mkdir().
         */
        virtual void setRoot(const QString &name);

    protected:
        QString m_root;
};

inline
QString SQ_Dir::root() const
{
    return m_root;
}

#endif
