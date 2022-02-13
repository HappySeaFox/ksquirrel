/*
    copyright            : (C) 2004 by Baryshev Dmitry
    KSquirrel - image viewer for KDE
*/

/*  This file is part of the KDE project
    Copyright (C) 2000 David Faure <faure@kde.org>

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

#ifndef SQ_THUMBNAILLOADJOB_H
#define SQ_THUMBNAILLOADJOB_H

#include <qimage.h>

#include <kio/job.h>

#include "sq_thumbnailinfo.h"

class KFileItem;
class KTempFile;

class SQ_DirThumbs;

typedef QPtrList<KFileItem> KFileItemList;

/*
 *  SQ_ThumbnailLoadJob is a job for loading thumbnails
 *  in given directory.
 */

class SQ_ThumbnailLoadJob : public KIO::Job
{
    Q_OBJECT

    public:
        SQ_ThumbnailLoadJob(const KFileItemList &itemList);
        virtual ~SQ_ThumbnailLoadJob();

        void start();
        void itemRemoved(const KFileItem* item);
        void itemsRemoved(const KFileItemList &list);
        void appendItem(const KFileItem* item);
        void appendItems(const KFileItemList &items);
        void prependItems(const KFileItemList &items);

    private:
        void determineNextIcon();
        bool statResultThumbnail();
        void createThumbnail(const KURL &);
        void emitThumbnailLoaded(SQ_Thumbnail &);
        void emitThumbnailLoadingFailed();
        void insertOrSync(const KURL &, SQ_Thumbnail &th);

    signals:
        void thumbnailLoaded(const KFileItem* item, const SQ_Thumbnail &t);

    private slots:
        void slotResult(KIO::Job *job);
        void slotData(KIO::Job *job, const QByteArray &data);

    private:
        enum { STATE_DETERMINE, STATE_STATORIG, STATE_PREDOWNLOAD, STATE_DOWNLOAD } mState;

        KFileItemList mItems;
        KFileItem *mCurrentItem;
        KURL mCurrentURL;
        KURL mThumbURL;
        KURL mTempURL;
        KIO::filesize_t totalSize, size;
        KTempFile         *tmp;
        time_t mOriginalTime;
        QString mime;
        bool continueDownload;

        SQ_DirThumbs *dir;
        SQ_Thumbnail mBrokenThumbnail;
};

#endif
