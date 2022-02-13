/***************************************************************************
                          sq_pixmapcache.h  -  description
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

#ifndef SQ_PIXMAPCACHE_H
#define SQ_PIXMAPCACHE_H

#include <qmap.h>

#include "sq_thumbnailinfo.h"

class SQ_Dir;

class SQ_PixmapCache : public QMap<QString, SQ_Thumbnail>
{
	public:
		SQ_PixmapCache(int limit = 5*1024);
		~SQ_PixmapCache();

		int  cacheLimit();
		void setCacheLimit(int);
		void sync();
		void syncEntry(const QString &key, SQ_Thumbnail &thumb);
		void insert(const QString &key, const SQ_Thumbnail &thumb);
		bool contains2(const QString &key, SQ_Thumbnail &th);
		int totalSize();
		bool full();
		void clear();
		QString root() const;
		void removeEntry(const QString &key);
		void removeEntryFull(const QString &key);

		static int entrySize(const SQ_Thumbnail &t);
		static SQ_PixmapCache* instance();

		typedef QMapIterator<QString, SQ_Thumbnail> cache_iterator;

	private:
		int cache_limit;
		int last_full;
		bool valid_full;
		SQ_Dir *dir;
		static SQ_PixmapCache *cache;
};

inline
int SQ_PixmapCache::cacheLimit()
{
    return cache_limit;
}

inline
void SQ_PixmapCache::setCacheLimit(int n)
{
    cache_limit = n << 10;
}

#endif
