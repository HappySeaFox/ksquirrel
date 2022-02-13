/*
	copyright            : (C) 2004 by Baryshev Dmitry
	KSQuirrel - image viewer for KDE
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

#include <qdir.h>
#include <qimage.h>
#include <qpainter.h>
#include <qlabel.h>

#include <kfileitem.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_widgetstack.h"
#include "sq_pixmapcache.h"
#include "sq_dir.h"
#include "sq_thumbnailjob.h"
#include "sq_libraryhandler.h"
#include "defs.h"
#include "err.h"

static const QString thumbFormat = "PNG";

SQ_ThumbnailLoadJob::SQ_ThumbnailLoadJob(const KFileItemList *items, SQ_ThumbnailSize size) : KIO::Job(false), mThumbnailSize(size)
{
	mBrokenThumbnail.thumbnail = KGlobal::iconLoader()->loadIcon("file_broken", KIcon::NoGroup, SQ_ThumbnailSize(SQ_ThumbnailSize::Small).pixelSize());
	mBrokenThumbnail.info.dimensions = mBrokenThumbnail.info.bpp = QString("0");
	mItems = *items;

	if(mItems.isEmpty())
		return;

	dir = new SQ_Dir;
	dir->setRoot(QString::fromLatin1("thumbnails"));
}

SQ_ThumbnailLoadJob::~SQ_ThumbnailLoadJob()
{}

void SQ_ThumbnailLoadJob::start()
{
	if (mItems.isEmpty())
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

		sqWStack->thumbnailProcess();

		if(item->isDir())
			mItems.removeFirst();
		else if(sqLibHandler->supports(item->url().path()))
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

	switch (mState)
	{
		case STATE_STATORIG:
		{
			if (job->error())
			{
				emitThumbnailLoadingFailed();
				determineNextIcon();
				return;
			}

			KIO::UDSEntry entry = static_cast<KIO::StatJob*>(job)->statResult();
			KIO::UDSEntry::ConstIterator it= entry.begin();
			mOriginalTime = 0;

			for(; it!=entry.end(); it++)
			{
				if ((*it).m_uds == KIO::UDS_MODIFICATION_TIME)
				{
					mOriginalTime = (time_t)((*it).m_long);
					break;
				}
			}

			mThumbURL.setPath(QDir::cleanDirPath(dir->root() + "/" + mCurrentURL.path()));
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
	SQ_LIBRARY 	*lib;

	if(job->error())
	{
		return false;
	}

	SQ_Thumbnail th;

	QString origPath = mThumbURL.path();
	origPath = QDir::cleanDirPath(origPath.replace(0, dir->root().length(), ""));

	lib = sqLibHandler->libraryForFile(origPath);

	if(!lib)
		return false;

	th.info.mime = lib->mime;

	kdDebug() << "STAT searching \"" << origPath  << "\" ..." << endl;

	if(sqCache->contains2(origPath, th))
	{
		emitThumbnailLoaded(th);
		determineNextIcon();
		kdDebug() << "STAT found in cache \"" << origPath << "\"" << endl;
		return true;
	}

	KIO::UDSEntry entry = job->statResult();
	KIO::UDSEntry::ConstIterator it = entry.begin();
	time_t thumbnailTime = 0;

	for(; it != entry.end(); it++)
	{
		if((*it).m_uds == KIO::UDS_MODIFICATION_TIME)
		{
			thumbnailTime = (time_t)((*it).m_long);
			break;
		}
	}

	if(thumbnailTime < mOriginalTime)
	{
		kdDebug() << "STAT **** thumbnailTime < mOriginalTime ****" << endl;
		return false;
	}

	if(!th.thumbnail.load(mThumbURL.path(), thumbFormat))
	{
		return false;
	}

	kdDebug() << "Loaded thumbnail " << mThumbURL.path() << endl;

	th.info.type = th.thumbnail.text("sq_type");
	th.info.dimensions = th.thumbnail.text("sq_dimensions");
	th.info.bpp = th.thumbnail.text("sq_bpp");
	th.info.color = th.thumbnail.text("sq_color");
	th.info.compression = th.thumbnail.text("sq_compression");
	th.info.frames = th.thumbnail.text("sq_frames");
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

	if(sqCache->contains2(pixPath, th))
	{
		emitThumbnailLoaded(th);
		kdDebug() << "CREATE found in cache \"" << pixPath << "\"" << endl;
		return;
	}

	loaded = SQ_ThumbnailLoadJob::loadThumbnail(pixPath, th);

	if(loaded)
	{
		insertOrSync(pixPath, th);
		emitThumbnailLoaded(th);
	}
	else
		emitThumbnailLoadingFailed();
}

void SQ_ThumbnailLoadJob::insertOrSync(const QString &path, SQ_Thumbnail &th)
{
	if(!sqCache->full())
	{
		sqCache->insert(path, th);
		kdDebug() << "STAT inserting \"" << path << "\"" << endl;
	}
	else
	{
		kdDebug() << "STAT SQ_PixmapCache is full! Cache is ignored!" << endl;
		sqCache->syncEntry(path, th);
	}
}

bool SQ_ThumbnailLoadJob::loadThumbnail(const QString &pixPath, SQ_Thumbnail &t, bool processImage)
{
	SQ_LIBRARY 	*lib;
	static RGBA 	*all = 0L;
	QString 		dim;
	char 			*dump;
	int 				w, h, num;

	lib = sqLibHandler->libraryForFile(pixPath);

	if(!lib)
		return false;

	int res = lib->fmt_readimage((const char*)pixPath.local8Bit(), &all, &dump);

	if(res != SQERR_OK)
	{
		return false;
	}

	QStringList list = QStringList::split('\n', dump);

	QStringList::Iterator it = list.begin();
	t.info.type = *it; ++it;
	dim = *it; ++it;
	w = dim.toInt();
	dim = *it; ++it;
	h = dim.toInt();
	t.info.dimensions = QString("%1x%2").arg(w).arg(h);
	t.info.bpp = *it; ++it;
	t.info.color = *it; ++it;
	t.info.compression = *it; ++it;
	t.info.frames = *it; ++it;
	t.info.uncompressed = *it; ++it;
	t.info.mime = lib->mime;

	num = t.info.uncompressed.toInt();
	t.info.uncompressed = KIO::convertSize(num);

	QImage image((unsigned char *)all, w, h, 32, 0, 0, QImage::LittleEndian);
	image.setAlphaBuffer(true);

	if(processImage)
		t.thumbnail = SQ_ThumbnailLoadJob::makeBigThumb(&image);
	else
		t.thumbnail = image;

	return true;
}

QImage SQ_ThumbnailLoadJob::makeBigThumb(QImage *image)
{
	const int SZ = SQ_ThumbnailSize::biggest().pixelSize();

	if(image->width() > SZ || image->height() > SZ)
		/**image = */ return image->smoothScale(QSize(SZ, SZ), QImage::ScaleMin).swapRGB();
	else
		/**image =*/ return  image->swapRGB();

//	return *image;
}

void SQ_ThumbnailLoadJob::emitThumbnailLoaded(SQ_Thumbnail &t)
{
	SQ_ThumbnailSize biggest = SQ_ThumbnailSize::biggest();

	if(mThumbnailSize == biggest)
	{
		emit thumbnailLoaded(mCurrentItem, t);
		return;
	}

	int biggestDimension = QMAX(t.thumbnail.width(), t.thumbnail.height());
	int thumbPixelSize = mThumbnailSize.pixelSize();

	if(biggestDimension <= thumbPixelSize)
	{
		emit thumbnailLoaded(mCurrentItem, t);
		return;
	}

	double scale = double(thumbPixelSize)/double(biggestDimension);
	QPixmap pix2(thumbPixelSize, thumbPixelSize);

	QPainter painter;
	painter.begin(&pix2);
	painter.eraseRect(0, 0, thumbPixelSize, thumbPixelSize);
	painter.scale(scale, scale);
	painter.drawImage((biggestDimension - t.thumbnail.width()) / 2, (biggestDimension - t.thumbnail.height()) / 2, t.thumbnail);
	painter.end();

	t.thumbnail = pix2;

	emit thumbnailLoaded(mCurrentItem, t);
}

void SQ_ThumbnailLoadJob::emitThumbnailLoadingFailed()
{
	emit thumbnailLoaded(mCurrentItem, mBrokenThumbnail);
}

void SQ_ThumbnailLoadJob::appendItems(const KFileItemList &items)
{
	KFileItem *item;
	KFileItemList *m_items = (KFileItemList *)&items;

	if(!mItems.isEmpty() && !items.isEmpty())
	{
		for(item = m_items->first();item;item = m_items->next())
			mItems.append(item);
	}
}
