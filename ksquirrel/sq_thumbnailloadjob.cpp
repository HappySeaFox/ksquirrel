/*
    copyright            : (C) 2004 by Baryshev Dmitry
    KSquirrel - image viewer for KDE
*/

/*  This file is part of the KDE project
    Copyright (C) 2000 David Faure <faure@kde.org>
                  2000 Carsten Pfeiffer <pfeiffer@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qdir.h>
#include <qimage.h>
#include <qpainter.h>
#include <qlabel.h>

#include <kfileitem.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

#include "sq_config.h"
#include "sq_widgetstack.h"
#include "sq_pixmapcache.h"
#include "sq_dir.h"
#include "sq_thumbnailloadjob.h"
#include "sq_libraryhandler.h"
#include "sq_thumbnailsize.h"
#include "sq_imageloader.h"

#include "libpixops/pixops.h"

#include <ksquirrel-libs/fmt_defs.h>

#include <string>

static const QString thumbFormat = "PNG";

SQ_ThumbnailLoadJob::SQ_ThumbnailLoadJob(const KFileItemList &items) : KIO::Job(false)
{
    mBrokenThumbnail.thumbnail = KGlobal::iconLoader()->loadIcon("file_broken", KIcon::Desktop, 48);
    mBrokenThumbnail.info.dimensions = mBrokenThumbnail.info.bpp = QString("0");
    mItems = items;

    dir = new SQ_Dir(SQ_Dir::Thumbnails);
}

SQ_ThumbnailLoadJob::~SQ_ThumbnailLoadJob()
{
    delete dir;
}

void SQ_ThumbnailLoadJob::start()
{
    if(mItems.isEmpty())
    {
        emit result(this);
        delete this;
        return;
    }

    determineNextIcon();
}

void SQ_ThumbnailLoadJob::appendItem(const KFileItem* item)
{
    mItems.append(item);
}

void SQ_ThumbnailLoadJob::itemRemoved(const KFileItem* item)
{
    mItems.removeRef(item);

    if(item == mCurrentItem)
    {
        subjobs.first()->kill();
        subjobs.removeFirst();
        determineNextIcon();
    }
}

void SQ_ThumbnailLoadJob::determineNextIcon()
{
    KFileItem* item;

    while(true)
    {
        item = mItems.first();

        if(!item)
            break;

        SQ_WidgetStack::instance()->thumbnailProcess();

        if(item->isDir() || !item->isReadable())
            mItems.removeFirst();
        else if(SQ_LibraryHandler::instance()->libraryForFile(item->url().path()))
            break;
        else
            mItems.removeFirst();
    }

    if(mItems.isEmpty())
    {
        emit result(this);
        delete this;
    }
    else
    {
        mState = STATE_STATORIG;
        mCurrentItem = mItems.first();
        mCurrentURL = mCurrentItem->url();
        addSubjob(KIO::stat(mCurrentURL, false));
        mItems.removeFirst();
    }
}

void SQ_ThumbnailLoadJob::slotResult(KIO::Job * job)
{
    subjobs.remove(job);
    Q_ASSERT(subjobs.isEmpty());

    switch(mState)
    {
        case STATE_STATORIG:
        {
            if(job->error())
            {
                emitThumbnailLoadingFailed();
                determineNextIcon();
                return;
            }

            KIO::UDSEntry entry = static_cast<KIO::StatJob*>(job)->statResult();
            KIO::UDSEntry::ConstIterator it= entry.begin();
            mOriginalTime = 0;

            for(; it!=entry.end(); ++it)
            {
                if ((*it).m_uds == KIO::UDS_MODIFICATION_TIME)
                {
                    mOriginalTime = (time_t)((*it).m_long);
                    break;
                }
            }

            mThumbURL.setPath(QDir::cleanDirPath(dir->root() + QDir::separator() + mCurrentURL.path()));
            mState = STATE_STATTHUMB;
            addSubjob(KIO::stat(mThumbURL, false));
            return;
        }

        case STATE_STATTHUMB:
            if(statResultThumbnail(static_cast<KIO::StatJob*>(job)))
                return;

            createThumbnail(mCurrentURL.path());
            determineNextIcon();
        return;

        case STATE_DELETETEMP:
            determineNextIcon();
        return;
    }
}

bool SQ_ThumbnailLoadJob::statResultThumbnail(KIO::StatJob * job)
{
    if(job->error())
        return false;

    SQ_LIBRARY     *lib;
    SQ_Thumbnail th;

    QString origPath = mThumbURL.path();
    origPath = QDir::cleanDirPath(origPath.right(origPath.length() - dir->root().length()));

//    printf("ORIGPATH %s\n", origPath.ascii());
//    origPath = QDir::cleanDirPath(origPath.replace(0, dir->root().length(), ""));

    lib = SQ_LibraryHandler::instance()->libraryForFile(origPath);

    if(!lib)
        return false;

    th.info.mime = lib->mime;

//    kdDebug() << "STAT searching \"" << origPath  << "\"..." << endl;

    if(SQ_PixmapCache::instance()->contains2(origPath, th))
    {
        emitThumbnailLoaded(th);
        determineNextIcon();
//        kdDebug() << "STAT found in cache \"" << origPath << "\"" << endl;
        return true;
    }

    KIO::UDSEntry entry = job->statResult();
    KIO::UDSEntry::ConstIterator it = entry.begin();
    time_t thumbnailTime = 0;

    for(; it != entry.end(); ++it)
    {
        if((*it).m_uds == KIO::UDS_MODIFICATION_TIME)
        {
            thumbnailTime = (time_t)((*it).m_long);
            break;
        }
    }

    if(thumbnailTime < mOriginalTime)
    {
//        kdDebug() << "STAT **** thumbnailTime < mOriginalTime ****" << endl;
        return false;
    }

    if(!th.thumbnail.load(mThumbURL.path(), thumbFormat))
    {
        return false;
    }

//    kdDebug() << "STAT loaded " << mThumbURL.path() << endl;

    th.info.type = th.thumbnail.text("sq_type");
    th.info.dimensions = th.thumbnail.text("sq_dimensions");
    th.info.bpp = th.thumbnail.text("sq_bpp");
    th.info.color = th.thumbnail.text("sq_color");
    th.info.compression = th.thumbnail.text("sq_compression");
    th.info.frames = th.thumbnail.text("sq_frames").toInt();
    th.info.uncompressed = th.thumbnail.text("sq_uncompressed");
    th.info.uncompressed = QString::fromUtf8(th.info.uncompressed.ascii());

    insertOrSync(origPath, th);

    emitThumbnailLoaded(th);
    determineNextIcon();

    return true;
}

void SQ_ThumbnailLoadJob::createThumbnail(const QString& pixPath)
{
    SQ_Thumbnail th;
    bool loaded = false;

    if(SQ_PixmapCache::instance()->contains2(pixPath, th))
    {
        emitThumbnailLoaded(th);
//        kdDebug() << "CREATE found in cache \"" << pixPath << "\"" << endl;
        return;
    }

    loaded = SQ_ThumbnailLoadJob::loadThumbnail(pixPath, th);

    if(loaded)
    {
//        kdDebug() << "CREATE loaded " << pixPath << endl;
        th.thumbnail = th.thumbnail.swapRGB();
        insertOrSync(pixPath, th);
        emitThumbnailLoaded(th);
    }
    else
        emitThumbnailLoadingFailed();
}

void SQ_ThumbnailLoadJob::insertOrSync(const QString &path, SQ_Thumbnail &th)
{
    if(!SQ_PixmapCache::instance()->full())
    {
        SQ_PixmapCache::instance()->insert(path, th);
//        kdDebug() << "IOSYNC inserting \"" << path << "\"" << endl;
    }
    else
    {
//        kdDebug() << "IOSYNC SQ_PixmapCache is full! Cache is ignored!" << endl;
        SQ_PixmapCache::instance()->syncEntry(path, th);
    }
}

bool SQ_ThumbnailLoadJob::loadThumbnail(const QString &pixPath, SQ_Thumbnail &t, bool processImage)
{
    fmt_info *finfo;

    RGBA *all;

    bool b = SQ_ImageLoader::instance()->loadImage(pixPath, true, 2, true);

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

    SQ_LIBRARY *lib = SQ_LibraryHandler::instance()->libraryForFile(pixPath);

    t.info.type = lib->quickinfo;
    t.info.dimensions = QString::fromLatin1("%1x%2").arg(finfo->image[0].w).arg(finfo->image[0].h);
    t.info.bpp = QString::fromLatin1("%1").arg(finfo->image[0].bpp);
    t.info.color = finfo->image[0].colorspace;
    t.info.compression = finfo->image[0].compression;
    t.info.frames = 0;
    t.info.mime = lib->mime;
    t.info.uncompressed = KIO::convertSize(finfo->image[0].w * finfo->image[0].h * sizeof(RGBA));
    t.info.frames = finfo->image.size();

    if(processImage)
        t.thumbnail = SQ_ThumbnailLoadJob::scaleImage((unsigned char *)all, finfo->image[0].w,
                finfo->image[0].h, SQ_ThumbnailSize::biggest());
    else
    {
        QImage image((unsigned char *)all, finfo->image[0].w, finfo->image[0].h, 32, 0, 0, QImage::LittleEndian);
        image.setAlphaBuffer(true);
        t.thumbnail = image.copy();
    }

    SQ_ImageLoader::instance()->cleanup();

    return true;
}

QImage SQ_ThumbnailLoadJob::scaleImage(unsigned char *im, int w, int h, int fitwithin)
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

void SQ_ThumbnailLoadJob::emitThumbnailLoaded(SQ_Thumbnail &t)
{
    int biggestDimension = QMAX(t.thumbnail.width(), t.thumbnail.height());
    int thumbPixelSize = SQ_ThumbnailSize::instance()->pixelSize();

    if(biggestDimension <= thumbPixelSize)
    {
        emit thumbnailLoaded(mCurrentItem, t);
        return;
    }

    t.thumbnail = SQ_ThumbnailLoadJob::scaleImage(
                        t.thumbnail.bits(),
                        t.thumbnail.width(),
                        t.thumbnail.height(),
                        thumbPixelSize);

    emit thumbnailLoaded(mCurrentItem, t);
}

void SQ_ThumbnailLoadJob::emitThumbnailLoadingFailed()
{
    emit thumbnailLoaded(mCurrentItem, mBrokenThumbnail);
}

void SQ_ThumbnailLoadJob::appendItems(const KFileItemList &items)
{
    KFileItem *item;
    KFileItemList *m_items = const_cast<KFileItemList *>(&items);

    if(!mItems.isEmpty() && !items.isEmpty())
    {
        for(item = m_items->first();item;item = m_items->next())
            mItems.append(item);
    }
}

#include "sq_thumbnailloadjob.moc"
