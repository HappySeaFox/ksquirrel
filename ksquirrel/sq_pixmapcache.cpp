/***************************************************************************
                          sq_pixmapcache.cpp  -  description
                             -------------------
    begin                :  Sep 28 2004
    copyright            : (C) 2004 by ckult
    email                : squirrel-sf@yandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sq_pixmapcache.h"
#include "sq_dir.h"

SQ_PixmapCache::SQ_PixmapCache(int limit) : QMap<QString, QPixmap>()
{
	cache_limit = limit << 10;
	current = 0;
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

	SQ_Dir *dir = new SQ_Dir();
	dir->setRoot("thumbnails");

	QMapIterator<QString, QPixmap> BEGIN = begin();
	QMapIterator<QString, QPixmap>    END = end();

	printf("Syncing %d entries ...\n", size());

	for(QMapIterator<QString, QPixmap> it = BEGIN;it != END;it++)
	{
		printf("syncing \"%s\" ...\n", it.key().ascii());
		dir->savePixmap(it.key(), it.data());
	}

	clear();

	delete dir;
}

// LIFO method is mush better, than FIFO
// so we'll remove _last_ element in cache
bool SQ_PixmapCache::removeLast(int bytes)
{
	if(current + bytes <= cache_limit)
		return false;

	QMapIterator<QString, QPixmap> it = end();

	QPixmap *pixmap;

	pixmap = &it.data();

	if(pixmap->isNull())
		return false;

	int size2 = (pixmap->width() * pixmap->height() * pixmap->depth()) >> 3;

	erase(it);

	current -= size2;

	if(!size())
	{
		current = 0;
		return false;
	}

	return (current + bytes) > cache_limit;
}

void SQ_PixmapCache::insert(const QString &key, const QPixmap &pixmap)
{
	int size = (pixmap.width() * pixmap.height() * pixmap.depth()) >> 3;

	while(removeLast(size))
	{}

	current += size;

	(*this)[key] = pixmap;
}

bool SQ_PixmapCache::contains2(const QString &key, QPixmap &pixmap)
{
	if(this->contains(key))
	{
		pixmap = (*this)[key];
		return true;
	}

	return false;
}
