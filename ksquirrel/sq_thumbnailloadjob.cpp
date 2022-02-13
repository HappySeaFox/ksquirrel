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
#include <qdatetime.h>

#include <kfileitem.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kdebug.h>

#include "sq_config.h"
#include "sq_widgetstack.h"
#include "sq_pixmapcache.h"
#include "sq_dirthumbs.h"
#include "sq_thumbnailloadjob.h"
#include "sq_libraryhandler.h"
#include "sq_thumbnailsize.h"
#include "sq_thumbnailsunused.h"
#include "sq_utils.h"
#include "sq_filethumbview.h"
#include "sq_filethumbviewitem.h"
#include "sq_imageloader.h"

#include <ksquirrel-libs/fmt_defs.h>

#include <string>

#define SQ_PREDOWNLOAD_SIZE 20

SQ_ThumbnailLoadJob::SQ_ThumbnailLoadJob(const KFileItemList &items, SQ_FileThumbView *parnt) 
    : KIO::Job(false), parent(parnt)
{
    mBrokenThumbnail.thumbnail = KGlobal::iconLoader()->loadIcon("file_broken", KIcon::Desktop, SQ_ThumbnailSize::smallest());
    mItems = items;

    donothing = false;

    dir = new SQ_DirThumbs;
}

SQ_ThumbnailLoadJob::~SQ_ThumbnailLoadJob()
{
    delete dir;
}

void SQ_ThumbnailLoadJob::start()
{
    if(mItems.isEmpty())
    {
        emit done();
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

    nextFile(item == mCurrentItem);
}

void SQ_ThumbnailLoadJob::itemsRemoved(const KFileItemList &items)
{
    KFileItem *item;
    bool next = false;
    KFileItemList *m_items = const_cast<KFileItemList *>(&items);

    if(!mItems.isEmpty() && !items.isEmpty())
    {
        for(item = m_items->first();item;item = m_items->next())
        {
            mItems.removeRef(item);

            if(item == mCurrentItem)
                next = true;
        }
    }

    nextFile(next);
}

void SQ_ThumbnailLoadJob::pop(const KFileItemList &items)
{
    KFileItem *item;
    bool next = false;
    KFileItemList *m_items = const_cast<KFileItemList *>(&items);

    if(!mItems.isEmpty() && !items.isEmpty())
    {
        for(item = m_items->first();item;item = m_items->next())
        {
            mItems.removeRef(item);

            if(item == mCurrentItem)
                next = true;
        }

        for(item = m_items->last();item;item = m_items->prev())
            mItems.prepend(item);
    }

    nextFile(next);
}

void SQ_ThumbnailLoadJob::nextFile(bool b)
{
    if(b)
    {
        KIO::Job *j = subjobs.first();

        if(j)
        {
            j->kill();
            subjobs.removeFirst();
        }

        determineNextIcon();
    }
}

void SQ_ThumbnailLoadJob::determineNextIcon()
{
    if(donothing)
        return;

    KFileItem *item = 0;
    SQ_FileThumbViewItem *tfi;

    while(true)
    {
        item = mItems.first();

        if(!item)
        {
            emit done();
            delete this;
            return;
        }

        SQ_WidgetStack::instance()->thumbnailProcess();

        tfi = reinterpret_cast<SQ_FileThumbViewItem *>(item->extraData(parent));

        // 1) local urls that are 100% supported, or
        // 2) remote urls that are 100% supported or _maybe_ supported (application/octet-stream)
        if(item->isReadable() && SQ_LibraryHandler::instance()->maybeSupported(item->url(), item->mimetype()) != SQ_LibraryHandler::No)
            break;
        else
        {
            mItems.removeFirst();
            tfi->setListed(true);
        }
    }

    if(mItems.isEmpty() || !item)
    {
        emit done();
        delete this;
        return;
    }
    else
    {
        mState = STATE_STATORIG;
        mCurrentItem = mItems.first();
        mCurrentURL = mCurrentItem->url();
        mItems.removeFirst();

        KIO::StatJob *job = KIO::stat(mCurrentURL, false);
        job->setSide(true);
        job->setDetails(0);
        addSubjob(job);
    }
}

void SQ_ThumbnailLoadJob::slotResult(KIO::Job *job)
{
    subjobs.remove(job);
//    Q_ASSERT(subjobs.isEmpty());

    switch(mState)
    {
        case STATE_STATORIG:
        {
            KIO::UDSEntry entry = mCurrentItem->entry();
            KIO::UDSEntry::ConstIterator it = entry.begin();
            mOriginalTime = 0;
            size = totalSize = 0;
            QString suff;

            for(; it != entry.end(); ++it)
            {
                if((*it).m_uds == KIO::UDS_MODIFICATION_TIME)
                {
                    mOriginalTime = (time_t)((*it).m_long);
                }
                else if((*it).m_uds == KIO::UDS_SIZE)
                {
                    totalSize = (KIO::filesize_t)((*it).m_long);
                }
                else if((*it).m_uds == KIO::UDS_NAME)
                {
                    suff = (*it).m_str;
                }
            }

            mThumbURL.setPath(dir->absPath(mCurrentURL));

            if(mCurrentURL.isLocalFile())
            {
                if(!statResultThumbnail())
                    createThumbnail(mCurrentURL);

                determineNextIcon();
            }
            else if(statResultThumbnail())
                determineNextIcon();
            else if(SQ_ThumbnailsUnused::instance()->needUpdate(mCurrentURL, mOriginalTime))
            {
                mState = STATE_PREDOWNLOAD;
                continueDownload = false;

                tmp = new KTempFile(QString::null, QString::fromLatin1(".%1").arg(suff), 0600);
                tmp->setAutoDelete(true);
                tmp->close();

                mTempURL.setPath(tmp->name());

                KIO::Job *cpjob = KIO::get(mCurrentURL, false, false);

                connect(cpjob, SIGNAL(data(KIO::Job *, const QByteArray &)),
                        this, SLOT(slotData(KIO::Job *, const QByteArray &)));

                addSubjob(cpjob);
            }
            else
                determineNextIcon();
        }
        break;

        case STATE_PREDOWNLOAD:
        {
            // error
            if(job->error() && job->error() != KIO::ERR_USER_CANCELED)
                emitThumbnailLoadingFailed();

            // no errors, or canceled
            delete tmp;
            determineNextIcon();
        }
        break;

        case STATE_DOWNLOAD:
        {
            if(job->error())
                emitThumbnailLoadingFailed();
            else if(SQ_LibraryHandler::instance()->libraryForFile(mTempURL.path()))
                createThumbnail(mTempURL);
            else
                SQ_ThumbnailsUnused::instance()->insert(mCurrentURL, mOriginalTime);

            delete tmp;
            determineNextIcon();
        }
        break;
    }
}

void SQ_ThumbnailLoadJob::slotData(KIO::Job *job, const QByteArray &data)
{
    if(!data.size())
        return;

    size += data.size();

    QFile f(mTempURL.path());

    if(f.open(IO_WriteOnly | IO_Append))
    {
        f.writeBlock(data);
        f.close();
    }

    // 20 bytes are enough to determine file type
    if(size >= SQ_PREDOWNLOAD_SIZE && !continueDownload)
    {
        // cancel download (file type is not supported)
        if(totalSize != size
            && !SQ_LibraryHandler::instance()->libraryForFile(mTempURL.path()))
        {
            SQ_ThumbnailsUnused::instance()->insert(mCurrentURL, mOriginalTime);
            job->kill(false);          // kill job & emit result
        }
        else if(!statResultThumbnail())
        {
            // nice, we can open this image - 
            // go to 'STATE_DOWNLOAD' stage and continue download
            continueDownload = true;
            mState = STATE_DOWNLOAD;
        }
        else if(size != totalSize)
        {
            job->kill(true);          // kill job
            subjobs.remove(job);
            determineNextIcon();
        }
    }
}

bool SQ_ThumbnailLoadJob::statResultThumbnail()
{
    SQ_Thumbnail th;

    if(SQ_PixmapCache::instance()->contains2(mCurrentURL, th))
    {
        emitThumbnailLoaded(th);
        return true;
    }

    if(dir->needUpdate(mThumbURL.path(), mOriginalTime))
    {
//        kdDebug() << "STAT **** thumbnailTime < mOriginalTime ****" << endl;
        return false;
    }

    if(!th.thumbnail.load(mThumbURL.path(), sqdirThumbFormat))
        return false;

    SQ_LIBRARY *lib = SQ_LibraryHandler::instance()->libraryForFile(mCurrentURL);

    th.w = th.thumbnail.text("Thumb::Image::Width").toInt();
    th.h = th.thumbnail.text("Thumb::Image::Height").toInt();

    if((!th.w || !th.h) && mCurrentURL.isLocalFile())
        SQ_ImageLoader::instance()->tasteImage(mCurrentURL.path(), &th.w, &th.h, lib);

    th.mime = lib->mime;
    th.originalTime = mOriginalTime;
    th.mime.load(dir->absPath(mCurrentURL), sqdirMimeFormat);

    insertOrSync(mCurrentURL, th);

    emitThumbnailLoaded(th);

    return true;
}

void SQ_ThumbnailLoadJob::createThumbnail(const KURL &pixPath)
{
    SQ_Thumbnail th;
    bool loaded = false;

    if(SQ_PixmapCache::instance()->contains2(pixPath, th))
    {
        emitThumbnailLoaded(th);
//        kdDebug() << "CREATE found in cache \"" << pixPath << "\"" << endl;
        return;
    }

    loaded = SQ_Utils::loadThumbnail(pixPath, th);

    if(loaded)
    {
        th.originalTime = mOriginalTime;
        th.thumbnail = th.thumbnail.swapRGB();
        insertOrSync(mCurrentURL, th);
        emitThumbnailLoaded(th);
    }
    else
        emitThumbnailLoadingFailed();
}

void SQ_ThumbnailLoadJob::insertOrSync(const KURL &url, SQ_Thumbnail &th)
{
    if(!SQ_PixmapCache::instance()->full())
    {
        SQ_PixmapCache::instance()->insert(url, th);
//        kdDebug() << "IOSYNC inserting \"" << url.url() << "\"" << endl;
    }
    else
    {
//        kdDebug() << "IOSYNC SQ_PixmapCache is full! Cache is ignored!" << endl;
        SQ_PixmapCache::instance()->syncEntry(url, th);
    }
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

    t.thumbnail = SQ_Utils::scaleImage(
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

void SQ_ThumbnailLoadJob::prependItems(const KFileItemList &items)
{
    KFileItem *item;
    KFileItemList *m_items = const_cast<KFileItemList *>(&items);

    if(!mItems.isEmpty() && !items.isEmpty())
    {
        for(item = m_items->last();item;item = m_items->prev())
            mItems.prepend(item);
    }
}

void SQ_ThumbnailLoadJob::kill(bool q)
{
    donothing = true;

    KIO::Job::kill(q);
}

#include "sq_thumbnailloadjob.moc"
