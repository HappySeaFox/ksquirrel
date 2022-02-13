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

/*
 *  SQ_PixmapCache represents a simple memory cache, which
 *  stores pixmaps. It doesn't use neither QPixmapCache nor QCache.
 *
 *  Uses SQ_Dir(Thumbnails) to store thumbnails on disk.
 */

class SQ_PixmapCache : public QMap<QString, SQ_Thumbnail>
{
	public:
		SQ_PixmapCache(int limit = 5*1024);
		~SQ_PixmapCache();

		/*
		 *  Get maximum cache limit.
		 */
		int  cacheLimit();

		/*
		 *  Set cache limit, in kilobytes.
		 *
		 *  200 means ~200 Kb, 1024 - 1Mb
		 */
		void setCacheLimit(int);

		/*
		 *  Write all entries to disk and clear cache.
		 */
		void sync();

		/*
		 *  Write one entry to disk and remove it from cache.
		 */
		void syncEntry(const QString &key, SQ_Thumbnail &thumb);

		/*
		 *  Insert new entry to cache
		 */
		void insert(const QString &key, const SQ_Thumbnail &thumb);

		/*
		 *  Check if pixmap, represented by 'key', is already in cache.
		 */
		bool contains2(const QString &key, SQ_Thumbnail &th);

		/*
		 *  Calculate total size used by cache (not exact!)
		 */
		int totalSize();

		/*
		 *  Is cache full ?
		 */
		bool full();

		/*
		 *  Clear cache.
		 */
		void clear();

		/*
		 *  Get root directory, where all thumbnails are saved.
		 */
		QString root() const;

		/*
		 *  Remove entry from cache.
		 */
		void removeEntry(const QString &key);

		/*
		 *  Remove entry from cache and from disk.
		 */
		void removeEntryFull(const QString &key);

		/*
		 *  Calculate cache-related size of given thumbnail.
		 */
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
