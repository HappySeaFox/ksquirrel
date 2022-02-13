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

#include <qdir.h>
#include <qimage.h>
#include <qpainter.h>
#include <qlabel.h>

#include <kfileitem.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include "sq_config.h"
#include "sq_widgetstack.h"
#include "sq_pixmapcache.h"
#include "sq_dir.h"
#include "sq_thumbnailjob.h"
#include "sq_libraryhandler.h"
#include "sq_thumbnailsize.h"

#include <csetjmp>

#include "fileio.h"
#include "fmt_utils.h"
#include "fmt_codec_base.h"
#include "error.h"

#include <string>

using namespace fmt_utils;

#define SQ_FAIL_CLOSE(a) \
		if((a))                    \
		{                            \
			codeK->fmt_read_close();     \
			return false;                           \
		}

static const QString thumbFormat = "PNG";

SQ_ThumbnailLoadJob::SQ_ThumbnailLoadJob(const KFileItemList *items) : KIO::Job(false)
{
	mBrokenThumbnail.thumbnail = KGlobal::iconLoader()->loadIcon("file_broken", KIcon::NoGroup, SQ_ThumbnailSize::instance()->pixelSize());
	mBrokenThumbnail.info.dimensions = mBrokenThumbnail.info.bpp = QString("0");
	mItems = *items;

	if(mItems.isEmpty())
		return;

	dir = new SQ_Dir(SQ_Dir::Thumbnails);
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

		SQ_WidgetStack::instance()->thumbnailProcess();

		if(item->isDir())
			mItems.removeFirst();
		else if(SQ_LibraryHandler::instance()->supports(item->url().path()))
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

	lib = SQ_LibraryHandler::instance()->libraryForFile(origPath);

	if(!lib)
		return false;

	th.info.mime = lib->mime;

	kdDebug() << "STAT searching \"" << origPath  << "\"..." << endl;

	if(SQ_PixmapCache::instance()->contains2(origPath, th))
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

	if(SQ_PixmapCache::instance()->contains2(pixPath, th))
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
	if(!SQ_PixmapCache::instance()->full())
	{
		SQ_PixmapCache::instance()->insert(path, th);
		kdDebug() << "STAT inserting \"" << path << "\"" << endl;
	}
	else
	{
		kdDebug() << "STAT SQ_PixmapCache is full! Cache is ignored!" << endl;
		SQ_PixmapCache::instance()->syncEntry(path, th);
	}
}

bool SQ_ThumbnailLoadJob::loadThumbnail(const QString &pixPath, SQ_Thumbnail &t, bool processImage)
{
	SQ_LIBRARY 	*lib;
	static RGBA 		*all = NULL, *dumbscan = NULL;
	int 				w = 0, h = 0, res, current = 0, i, j;
	RGBA			*scan;
	fmt_info			finfo;
	fmt_codec_base	 *codeK;
	bool			b;

	lib = SQ_LibraryHandler::instance()->libraryForFile(pixPath);

	if(!lib || !lib->codec)
		return false;

	codeK = lib->codec;

#ifndef QT_NO_STL
	res = codeK->fmt_read_init(QString(pixPath.local8Bit()));
#else
	res = codeK->fmt_read_init(QString(pixPath.local8Bit()).ascii());
#endif

	if(res != SQE_OK)
		return false;

	while(true)
	{
		i = codeK->fmt_read_next();

		finfo = codeK->information();

		if(i != SQE_OK && i != SQE_NOTOK && !current)
		{
			codeK->fmt_read_close();
			return false;
		}
		else if((i != SQE_OK && i != SQE_NOTOK && current) || (i == SQE_NOTOK && current))
			break;

//		fprintf(stderr, "%dx%d@%d ...\n", finfo.image[current].w, finfo.image[current].h, finfo.image[current].bpp);

		if(!current)
		{
			w = finfo.image[current].w;
			h = finfo.image[current].h;
			const int S = w * h * sizeof(RGBA);

			all = (RGBA *)realloc(all, S);

			b = !all;
			SQ_FAIL_CLOSE(b)

			memset(all, 255, S);

			t.info.type = lib->quickinfo;
			t.info.dimensions = QString::fromLatin1("%1x%2").arg(w).arg(h);
			t.info.bpp = QString::fromLatin1("%1").arg(finfo.image[current].bpp);
#ifndef QT_NO_STL
			t.info.color = finfo.image[current].colorspace;
			t.info.compression = finfo.image[current].compression;
#else
			t.info.color = finfo.image[current].colorspace.c_str();
			t.info.compression = finfo.image[current].compression.c_str();
#endif
			t.info.frames = QString::fromLatin1("0");
			t.info.mime = lib->mime;
			t.info.uncompressed = KIO::convertSize(S);
		}
		else
		{
			dumbscan = (RGBA *)realloc(dumbscan, finfo.image[current].w * finfo.image[current].h * sizeof(RGBA));

			if(!dumbscan)
				break;
		}

		for(int pass = 0;pass < finfo.image[current].passes;pass++)
		{
			codeK->fmt_read_next_pass();

			if(!current)
			{
				for(j = 0;j < finfo.image[current].h;j++)
				{
					scan = all + j * finfo.image[current].w;
					i = codeK->fmt_read_scanline(scan);
					b = (i != SQE_OK);
					SQ_FAIL_CLOSE(b);
				}
			}
			else
			{
				for(j = 0;j < finfo.image[current].h;j++)
				{
					i = codeK->fmt_read_scanline(dumbscan);
					if(i != SQE_OK) goto E;
				}
			}
		}

		if(finfo.image[current].needflip && !current)
			fmt_utils::flipv((char *)all, finfo.image[current].w * sizeof(RGBA), finfo.image[current].h);

		current++;
	}

	E:

	t.info.frames = QString::fromLatin1("%1").arg(current);
	codeK->fmt_read_close();

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
	const int SZ = SQ_ThumbnailSize::biggest();

	if(image->width() > SZ || image->height() > SZ)
		return image->smoothScale(QSize(SZ, SZ), QImage::ScaleMin).swapRGB();
	else
		return  image->swapRGB();
}

void SQ_ThumbnailLoadJob::emitThumbnailLoaded(SQ_Thumbnail &t)
{
	int biggestDimension = QMAX(t.thumbnail.width(), t.thumbnail.height());
	int thumbPixelSize = SQ_ThumbnailSize::instance()->pixelSize() - 2;

	if(biggestDimension <= thumbPixelSize)
	{
		emit thumbnailLoaded(mCurrentItem, t);
		return;
	}

	double scale = double(thumbPixelSize) / double(biggestDimension);

	t.thumbnail = t.thumbnail.smoothScale(int(t.thumbnail.width() * scale), int(t.thumbnail.height() * scale));

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
