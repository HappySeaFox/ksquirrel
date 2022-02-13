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

#include <qpixmap.h>

#include <kio/job.h>

#include "sq_thumbnailsize.h"

class KFileItem;
class SQ_Dir;

typedef QPtrList<KFileItem> KFileItemList;

class SQ_ThumbnailLoadJob : public KIO::Job
{
	Q_OBJECT

	public:
		SQ_ThumbnailLoadJob(const KFileItemList* itemList, SQ_ThumbnailSize);
		virtual ~SQ_ThumbnailLoadJob();

		void start();
		void itemRemoved(const KFileItem* item);
		void appendItem(const KFileItem* item);
		void appendItems(const KFileItemList &items);
		static void deleteImageThumbnail(const KURL& url);
		QImage makeBigThumb(QImage *image);

	signals:
		void thumbnailLoaded(const KFileItem* item,const QPixmap&);

	private slots:
		void slotResult(KIO::Job *job);

	private:
		enum { STATE_STATORIG, STATE_STATTHUMB, STATE_DELETETEMP } mState;

		KFileItemList mItems;
		KFileItem *mCurrentItem;
		KURL mCurrentURL;
		time_t mOriginalTime;
		KURL mThumbURL;
		KURL mTempURL;
		QString mCacheDir;
		QPixmap mBrokenPixmap;
		SQ_ThumbnailSize mThumbnailSize;
		SQ_Dir *dir;
		
	private:
		void determineNextIcon();
		bool statResultThumbnail( KIO::StatJob * );
		void createThumbnail(const QString& path);

		bool loadThumbnail(const QString& pixPath, QPixmap&);
		void emitThumbnailLoaded(const QPixmap &pix);
		void emitThumbnailLoadingFailed();
};

#endif
