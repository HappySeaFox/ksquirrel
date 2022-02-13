/*
	copyright            : (C) 2004 by Baryshev Dmitry
	KSQuirrel - image viewer for KDE
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
class SQ_Dir;

typedef QPtrList<KFileItem> KFileItemList;

class SQ_ThumbnailLoadJob : public KIO::Job
{
	Q_OBJECT

	public:
		SQ_ThumbnailLoadJob(const KFileItemList* itemList);
		virtual ~SQ_ThumbnailLoadJob();

		void start();
		void itemRemoved(const KFileItem* item);
		void appendItem(const KFileItem* item);
		void appendItems(const KFileItemList &items);

		static QImage makeBigThumb(QImage *image);
		static bool loadThumbnail(const QString& pixPath, SQ_Thumbnail&, bool = true);

	private:
		void determineNextIcon();
		bool statResultThumbnail(KIO::StatJob *);
		void createThumbnail(const QString& path);
		void emitThumbnailLoaded(SQ_Thumbnail &);
		void emitThumbnailLoadingFailed();
		void insertOrSync(const QString &path, SQ_Thumbnail &th);

	signals:
		void thumbnailLoaded(const KFileItem* item, const SQ_Thumbnail &t);

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
		QString mCacheDir, mime;
		SQ_Dir *dir;
		SQ_Thumbnail mBrokenThumbnail;
};

#endif
