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
#include <qobject.h>

#include <kurl.h>

#include "sq_thumbnailinfo.h"

class SQ_DirThumbs;

/*
 *  SQ_PixmapCache represents a simple memory cache, which
 *  stores pixmaps. It doesn't use neither QPixmapCache nor QCache.
 *
 *  Uses SQ_Dir(Thumbnails) to store thumbnails on disk.
 */

class SQ_PixmapCache : public QObject, public QMap<KURL, SQ_Thumbnail>
{
    public:
        SQ_PixmapCache(QObject *parent, int limit = 20*1024);
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
        void syncEntry(const KURL &key, SQ_Thumbnail &thumb);

        /*
         *  Insert new entry to cache
         */
        void insert(const KURL &key, const SQ_Thumbnail &thumb);

        /*
         *  Check if pixmap, represented by 'key', is already in cache.
         */
        bool contains2(const KURL &key, SQ_Thumbnail &th);

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
         *  Remove entry from cache.
         */
        void removeEntry(const KURL &key);

        /*
         *  Remove entry from cache and from disk.
         */
        void removeEntryFull(const KURL &key);

        static SQ_PixmapCache* instance() { return m_instance; }

    private:
        int entrySize(const SQ_Thumbnail &th) const;

    private:
        int cache_limit;
        int last_full;
        bool valid_full;
        SQ_DirThumbs *dir;

        static SQ_PixmapCache *m_instance;
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

inline
bool SQ_PixmapCache::full()
{
    // when 'cache_limit' is 0, pixmap cache is always full
    return cache_limit ? (cache_limit <= totalSize()) : true;
}

#endif
