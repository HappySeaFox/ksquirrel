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
#include <qpixmap.h>
#include <qfileinfo.h>

#include "sq_thumbnailinfo.h"

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

            // thumbnail cache (~/.ksquirrel/thumbnails).
            Thumbnails,

            // SQ_ArchiveHandler cache (~/.ksquirrel/extracts).
            Extracts,

            // temporary usage (~/.ksquirrel/tmp).
            Tmp };

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
         *  Change current directory to m_root directory.
         *
         *  For example, if prefix == Thumbnails, it will
         *  cd to "~/.ksquirrel/thumbnails".
         */
        void rewind();

        /*
         *  Get current root directory.
         *
         *  For example, if prefix == Thumbnails, it will
         *  return "/home/krasu/.ksquirrel/thumbnails".
         */
        QString root() const;

        /*
         *  Get absolute path for relative path 'relpath'.
         */
        QString absPath(const QString &relpath);

        /*
         *  Save thumbnail to storage.
         */
        void saveThumbnail(const QString &path, SQ_Thumbnail &thumb);

        /*
         *  Check if file exists. If exists, set 'fullpath'
         *  to its full path.
         */
        bool fileExists(const QString &file, QString &fullpath);

        /*
         *  Check if file needs to be updated.
         *
         *  For example, yesterday you unpacked /opt/arc.zip with KSquirrel. SQ_Dir created
         *  ~/.ksquirrel/extracts/opt/arc.zip and SQ_ArchiveHandler unpacked this
         *  archive in it. Today you replaced /opt/arc.zip with newer version. Now
         *  updateNeeded("/opt/arc.zip") will return true, and SQ_Archivehandler will clean
         *  "~/.ksquirrel/extracts/opt/arc.zip" and unpack this archive once more.
         */
        bool updateNeeded(const QString &file);

        /*
         *  Remove file from storage
         */
        void removeFile(const QString &file);

    private:
        /*
         *  Internal, used by mkdir().
         */
        void setRoot(const QString &name);

private:
        QString m_root;
};

inline
QString SQ_Dir::absPath(const QString &relpath)
{
    return m_root + "/" + relpath;
}

inline
QString SQ_Dir::root() const
{
    return m_root;
}

#endif
