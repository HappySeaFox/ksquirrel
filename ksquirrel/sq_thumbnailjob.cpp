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
#include <qfile.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qlabel.h>

#include <kfileitem.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_widgetstack.h"
#include "sq_pixmapcache.h"
#include "sq_dir.h"
#include "sq_thumbnailjob.h"
#include "sq_libraryhandler.h"
#include "defs.h"
#include "err.h"

void flip(unsigned char *, unsigned int, unsigned int);

SQ_ThumbnailLoadJob::SQ_ThumbnailLoadJob(const KFileItemList *items, SQ_ThumbnailSize size) : KIO::Job(false), mThumbnailSize(size)
{
	mBrokenPixmap = KGlobal::iconLoader()->loadIcon("file_broken", KIcon::NoGroup, SQ_ThumbnailSize(SQ_ThumbnailSize::Small).pixelSize());

	mItems = *items;

	if(mItems.isEmpty())
		return;

	dir = new SQ_Dir;
	dir->setRoot("thumbnails");
}

SQ_ThumbnailLoadJob::~SQ_ThumbnailLoadJob()
{}

void SQ_ThumbnailLoadJob::deleteImageThumbnail(const KURL& url)
{
//	QDir dir();
//	dir.remove(url.fileName());
}

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

	if (item == mCurrentItem)
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

		QFileInfo fitem(item->name());

		sqWStack->thumbnailProcess();

		if(item->isDir())
			mItems.removeFirst();
		else if(sqLibHandler->supports(fitem.extension(false)))
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

			mThumbURL.setPath(QDir::cleanDirPath(dir->getRoot() + "/" + mCurrentURL.path()));
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
	{
		printf("STAT ******************** JOB ERROR! ***********************\n");
		return false;
	}

	QPixmap pix;

	QString origPath = mThumbURL.path();
	origPath = QDir::cleanDirPath(origPath.replace(0, dir->getRoot().length(), ""));

	printf("STAT searching \"%s\" ...\n", origPath.ascii());

	if(sqCache->contains2(origPath, pix))
	{
		emitThumbnailLoaded(pix);
		determineNextIcon();
		printf("STAT found in cache \"%s\"\n", origPath.ascii());
		return true;
	}

	KIO::UDSEntry entry = job->statResult();
	KIO::UDSEntry::ConstIterator it = entry.begin();
	time_t thumbnailTime = 0;

	for (; it != entry.end(); it++)
	{
		if ((*it).m_uds == KIO::UDS_MODIFICATION_TIME)
		{
			thumbnailTime = (time_t) ((*it).m_long);
			break;
		}
	}

	if(thumbnailTime < mOriginalTime)
	{
		printf("STAT **** thumbnailTime < mOriginalTime ****\n");
		return false;
	}

	if(!pix.load(mThumbURL.path()))
	{
		return false;
	}

	sqCache->insert(origPath, pix);
	printf("STAT inserting \"%s\"\n", origPath.ascii());

	emitThumbnailLoaded(pix);
	determineNextIcon();

	return true;
}

void SQ_ThumbnailLoadJob::createThumbnail(const QString& pixPath)
{
	QPixmap pix;
	bool loaded = false;

	if(sqCache->contains2(pixPath, pix))
	{
		emitThumbnailLoaded(pix);
		printf("CREATE found in cache \"%s\"\n", pixPath.ascii());
		return;
	}

	loaded = loadThumbnail(pixPath, pix);

	if(loaded)
	{
		sqCache->insert(pixPath, pix);
		emitThumbnailLoaded(pix);
		printf("CREATE inserting \"%s\"\n", pixPath.ascii());
	}
	else
		emitThumbnailLoadingFailed();
}

bool SQ_ThumbnailLoadJob::loadThumbnail(const QString &pixPath, QPixmap &pix)
{
	fmt_info *finfo;
	SQ_LIBRARY *lib;
	static RGBA *all = 0L;
	QFileInfo info(pixPath);

	lib = sqLibHandler->setCurrentLibrary(info.extension(false));
	lib->fmt_init(&finfo, (const char*)pixPath.local8Bit());

	if(lib->fmt_read_info(finfo) != SQERR_OK)
	{
		free(finfo);
		return false;
	}

	all = (RGBA*)realloc(all, finfo->w * finfo->h * 4);

	lib->fmt_readimage(finfo, all);
	lib->fmt_close(finfo);

	if(finfo->needflip)
		flip((unsigned char*)all, finfo->w * 4, finfo->h);

	QImage image((unsigned char *)all, finfo->w, finfo->h, 32, 0, 0, QImage::LittleEndian);
	image.setAlphaBuffer(true);

	pix = QPixmap(makeBigThumb(&image));

	return true;
}

QImage SQ_ThumbnailLoadJob::makeBigThumb(QImage *image)
{
	const int SZ = SQ_ThumbnailSize::biggest().pixelSize();

	if(image->width() > SZ || image->height() > SZ)
		*image = image->scale(QSize(SZ, SZ), QImage::ScaleMin).swapRGB();
	else
		*image = image->swapRGB();

	int im_w = image->width(), im_h = image->height();

	if(im_w == im_h == SZ)
		return *image;

	static RGBA thumb[128*128];
	memset(thumb, 0, 4 * 128 * 128);

	RGBA *thumb_next = thumb + ((SZ - im_w) / 2) + ((SZ - im_h) / 2)*SZ;

	for(int i = 0;i < im_h;i++)
	{
		memcpy(thumb_next + i*SZ, image->scanLine(i), im_w * 4);
	}

	QImage image2((unsigned char *)thumb, SZ, SZ, 32, 0, 0, QImage::LittleEndian);
	image2.setAlphaBuffer(true);

	*image = image2;

	return *image;
}

void SQ_ThumbnailLoadJob::emitThumbnailLoaded(const QPixmap& pix)
{
	SQ_ThumbnailSize biggest = SQ_ThumbnailSize::biggest();

	if(mThumbnailSize == biggest)
	{
		emit thumbnailLoaded(mCurrentItem, pix);
		return;
	}

	int biggestDimension = QMAX(pix.width(), pix.height());
	int thumbPixelSize = mThumbnailSize.pixelSize();

	if(biggestDimension <= thumbPixelSize)
	{
		emit thumbnailLoaded(mCurrentItem, pix);
		return;
	}

	double scale = double(thumbPixelSize)/double(biggestDimension);
	QPixmap pix2(thumbPixelSize, thumbPixelSize);

	QPainter painter;
	painter.begin(&pix2);
	painter.eraseRect(0, 0, thumbPixelSize, thumbPixelSize);
	painter.scale(scale, scale);
	painter.drawPixmap((biggestDimension-pix.width()) / 2, (biggestDimension-pix.height()) / 2, pix);
	painter.end();

	emit thumbnailLoaded(mCurrentItem, pix2);
}

void SQ_ThumbnailLoadJob::emitThumbnailLoadingFailed()
{
	emit thumbnailLoaded(mCurrentItem, mBrokenPixmap);
}

/*  utility  */
void flip(unsigned char * image, unsigned int w, unsigned int h)
{
	unsigned int i;
	unsigned char *hptr;

	if((hptr = (unsigned char *)malloc(w)) == 0)
		return;

	for (i = 0; i < h/2; i++)
	{
		memcpy(hptr, image + i * w, w);
		memcpy(image + i * w, image + (h - i - 1) * w, w);
		memcpy(image + (h - i - 1) * w, hptr, w);
	}

	free(hptr);
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
