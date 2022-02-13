/***************************************************************************
                          sq_dir.cpp  -  description
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

#include <qstringlist.h>
#include <qimage.h>
#include <qfile.h>

#include <kdebug.h>

#include "sq_dir.h"

// default thumbnail format
static const QString sqdirThumbFormat = "PNG";

// default quality
static const int thumbQuality = 85;

SQ_Dir::SQ_Dir(Prefix p) : QDir()
{
    switch(p)
    {
        case SQ_Dir::Thumbnails:
            setRoot("thumbnails");
        break;
        case SQ_Dir::Extracts:
            setRoot("extracts");
        break;
        case SQ_Dir::Tmp:
            setRoot("tmp");
        break;
    }

    kdDebug() << "+SQ_Dir" << " [" << m_root << "]" << endl;
}

SQ_Dir::~SQ_Dir()
{
    kdDebug() << "-SQ_Dir" << " [" << m_root << "]" << endl;
}

/*
 *  Create relative directory in storage.
 *
 *  For example, if prefix == Thumbnails,
 *  mkdir("/mnt/win_c") will create
 *  ~/.ksquirrel/thumbnails/mnt/win_c.
 */
bool SQ_Dir::mkdir(const QString &relpath)
{
    QString _relpath = QDir::cleanDirPath(relpath);

    QStringList paths = QStringList::split('/', _relpath);

    cd(m_root);

    // recursively create directories
    for(QStringList::iterator it = paths.begin();it != paths.end();++it)
    {
        if(!exists(*it, false))
            if(!QDir::mkdir(*it))
                return false;

        cd(*it);
    }

    return true;
}

void SQ_Dir::setRoot(const QString &name)
{
    m_root = QDir::cleanDirPath(homeDirPath() + QString::fromLatin1("/.ksquirrel/"));
    QDir::mkdir(m_root);

    m_root = QDir::cleanDirPath(m_root + QString::fromLatin1("/") + name);
    QDir::mkdir(m_root);
}

/*
 *  Change current directory to m_root directory.
 *
 *  For example, if prefix == Thumbnails, it will
 *  cd to "/home/krasu/.ksquirrel/thumbnails".
 */
void SQ_Dir::rewind()
{
    cd(m_root);
}

/*
 *  Get current root directory.
 *
 *  For example, if prefix == Thumbnails, it will
 *  return "/home/krasu/.ksquirrel/thumbnails".
 */
QString SQ_Dir::root() const
{
    return m_root;
}

/*
 *  Save thumbnail to storage.
 */
void SQ_Dir::saveThumbnail(const QString &path, SQ_Thumbnail &thumb)
{
    if(thumb.thumbnail.isNull())
        return;

    QString fullpath(m_root + path), s;
    QFileInfo fpath(path), ffullpath(fullpath);

    if(fpath.lastModified() < ffullpath.lastModified())
        return;

    if(!mkdir(fpath.dirPath(true)))
    {
        kdDebug() << "SQ_Dir::saveThumbnail: mkdir() falied" << endl;
        return;
    }

    QString k = thumb.info.uncompressed.utf8();
    thumb.thumbnail.setText("sq_type", 0, thumb.info.type);
    thumb.thumbnail.setText("sq_dimensions", 0, thumb.info.dimensions);
    thumb.thumbnail.setText("sq_bpp", 0, thumb.info.bpp);
    thumb.thumbnail.setText("sq_color", 0, thumb.info.color);
    thumb.thumbnail.setText("sq_compression", 0, thumb.info.compression);
    thumb.thumbnail.setText("sq_frames", 0, thumb.info.frames);
    thumb.thumbnail.setText("sq_uncompressed", 0, k);

    thumb.thumbnail.save(fullpath, sqdirThumbFormat, thumbQuality);
}

/*
 *  Check if file exists. If exists, set 'fullpath'
 *  to its full path.
 */
bool SQ_Dir::fileExists(const QString &file, QString &fullpath)
{
    QFileInfo f(m_root + file);

    // file exists ?
    bool b = f.exists();

    // yes!
    if(b)
        fullpath = m_root + file;

    return b;
}

/*
 *  Check if file needs to be updated.
 *
 *  For example, yesterday you unpacked /opt/arc.zip with KSquirrel. SQ_Dir created
 *  ~/.ksquirrel/extracts/opt/arc.zip and SQ_ArchiveHandler unpacked this
 *  archive to it. Today you replaced /opt/arc.zip with newer version. Now
 *  updateNeeded("/opt/arc.zip") will return true, and SQ_Archivehandler will clean
 *  "~/.ksquirrel/extracts/opt/arc.zip" and unpack it once more.
 */
bool SQ_Dir::updateNeeded(const QString &file)
{
    // file doesn't exist in storage, update needed!
    if(!QFile::exists(absPath(file)))
        return true;

    QFileInfo fpath(file), ffullpath(absPath(file));

    // compare "last modified" time
    return fpath.lastModified() > ffullpath.lastModified();
}

/*
 *  Remove file from storage
 */
void SQ_Dir::removeFile(const QString &file)
{
    // determine absolute path and remove file
    QFile::remove(absPath(file));
}
