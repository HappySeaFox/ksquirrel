/***************************************************************************
                          sq_dirthumbs.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UTIME_H
#include <sys/types.h>
#include <utime.h>
#endif

#include <qimage.h>
#include <qfileinfo.h>
#include <qdatetime.h>

#include <kurl.h>
#include <kmdcodec.h>

#include "sq_dirthumbs.h"
#include "sq_thumbnailinfo.h"

SQ_DirThumbs::SQ_DirThumbs() : SQ_Dir()
{
    setRoot();
}

SQ_DirThumbs::~SQ_DirThumbs()
{}

/*
 *  Save thumbnail to storage.
 */
void SQ_DirThumbs::saveThumbnail(const KURL &url, SQ_Thumbnail &thumb)
{
    QString ab = absPath(url);

    if(!thumb.thumbnail.isNull() && needUpdate(ab, thumb.originalTime))
    {
        // thumbnail standard tags
        thumb.thumbnail.setText("Thumb::Image::Width", 0, QString::number(thumb.w));
        thumb.thumbnail.setText("Thumb::Image::Height", 0, QString::number(thumb.h));
        thumb.thumbnail.setText("Thumb::URI", 0, url.prettyURL());
        thumb.thumbnail.setText("Thumb::MTime", 0, QString::number(thumb.originalTime));
        thumb.thumbnail.setText("Software", 0, "KSquirrel");

        thumb.thumbnail.save(ab, sqdirThumbFormat, sqdirThumbQuality);
    }

#ifdef HAVE_UTIME_H
    struct utimbuf ut;
    ut.actime = thumb.originalTime;
    ut.modtime = thumb.originalTime;
    utime((const char *)ab, &ut);
#endif
}

QString SQ_DirThumbs::absPath(const KURL &relurl)
{
    KMD5 md5(QFile::encodeName(relurl.prettyURL()));

    QString ext = QString::fromLatin1(".%1").arg(sqdirThumbFormat);

    return m_orig + QDir::separator() + QString(md5.hexDigest()) + ext.lower();
}

void SQ_DirThumbs::setRoot()
{
    m_orig = QDir::cleanDirPath(homeDirPath() + QDir::separator() + QString::fromLatin1(".thumbnails"));
    QDir::mkdir(m_orig);

    m_orig = QDir::cleanDirPath(m_orig + QDir::separator() + QString::fromLatin1("normal"));
    QDir::mkdir(m_orig);
}

void SQ_DirThumbs::removeFile(const KURL &url)
{
    // determine absolute path and remove file and mime icon
    QFile::remove(absPath(url));
}

bool SQ_DirThumbs::needUpdate(const QString &turl, time_t tm)
{
    QFileInfo fthumbpath(turl);

    QDateTime dt_orig;
    dt_orig.setTime_t(tm);

    return (dt_orig > fthumbpath.lastModified());
}
