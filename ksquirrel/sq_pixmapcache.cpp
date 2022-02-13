/***************************************************************************
                          sq_pixmapcache.cpp  -  description
                             -------------------
    begin                :  Sep 28 2004
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

#include <kdebug.h>

#include "sq_pixmapcache.h"
#include "sq_dir.h"

SQ_PixmapCache::SQ_PixmapCache(int limit) : QMap<QString, SQ_Thumbnail>()
{
	cache_limit = limit << 10;

	dir = new SQ_Dir;
	dir->setRoot("thumbnails");

	valid_full = false;
}

SQ_PixmapCache::~SQ_PixmapCache()
{}

int SQ_PixmapCache::cacheLimit()
{
	return cache_limit;
}

void SQ_PixmapCache::setCacheLimit(int n)
{
	cache_limit = n << 10;
}

void SQ_PixmapCache::sync()
{
	if(empty())
		return;

	QMapIterator<QString, SQ_Thumbnail> BEGIN = begin();
	QMapIterator<QString, SQ_Thumbnail>    END = end();

	kdDebug() << "Syncing " << size() << " entries ..." << endl;

	for(QMapIterator<QString, SQ_Thumbnail> it = BEGIN;it != END;it++)
	{
		syncEntry(it.key(), it.data());
	}

	clear();
}

void SQ_PixmapCache::syncEntry(const QString &key, SQ_Thumbnail &thumb)
{
	kdDebug() << "syncing single entry \"" << key << "\" ..." << endl;
	dir->saveThumbnail(key, thumb);
}

void SQ_PixmapCache::insert(const QString &key, const SQ_Thumbnail &thumb)
{
	if(thumb.thumbnail.isNull())
		return;

	last_full += SQ_PixmapCache::entrySize(thumb);

	(*this)[key] = thumb;
}

void SQ_PixmapCache::removeEntry(const QString &key)
{
	SQ_Thumbnail thumb = (*this)[key];

	last_full -= SQ_PixmapCache::entrySize(thumb);

	QMap<QString, SQ_Thumbnail>::remove(key);
}

void SQ_PixmapCache::removeEntryFull(const QString &key)
{
	removeEntry(key);

	dir->removeFile(key);
}

bool SQ_PixmapCache::contains2(const QString &key, SQ_Thumbnail &th)
{
	if(this->contains(key))
	{
		th = (*this)[key];
		return true;
	}

	return false;
}

int SQ_PixmapCache::totalSize()
{
	if(valid_full)
		return last_full;

	QMapConstIterator<QString, SQ_Thumbnail> BEGIN = constBegin();
	QMapConstIterator<QString, SQ_Thumbnail>    END = constEnd();

	SQ_Thumbnail t;
	int total = 0;

	for(QMapConstIterator<QString, SQ_Thumbnail> it = BEGIN;it != END;it++)
	{
		t = it.data();

		total += SQ_PixmapCache::entrySize(t);
	}

	last_full = total;
	valid_full = true;

	return total;
}

int SQ_PixmapCache::entrySize(const SQ_Thumbnail &t)
{
	int  total = (((t.thumbnail.width() * t.thumbnail.height() * t.thumbnail.depth()) >> 3)
					+ t.info.bpp.length()
					+ t.info.color.length()
					+ t.info.compression.length()
					+ t.info.dimensions.length()
					+ t.info.frames.length()
					+ t.info.type.length()
					+ t.info.uncompressed.length()
					+ ((t.info.mime.width() * t.info.mime.height() * t.info.mime.depth()) >> 3));

	return total;
}

bool SQ_PixmapCache::full()
{
	return cache_limit <= totalSize();
}

void SQ_PixmapCache::clear()
{
	valid_full = false;

	QMap<QString, SQ_Thumbnail>::clear();
}

QString SQ_PixmapCache::root() const
{
	return dir->root();
}
