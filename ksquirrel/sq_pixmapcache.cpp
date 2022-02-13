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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sq_pixmapcache.h"
#include "sq_dir.h"

SQ_PixmapCache * SQ_PixmapCache::m_instance = 0;

SQ_PixmapCache::SQ_PixmapCache(QObject *parent, int limit) 
    : QObject(parent), QMap<QString, SQ_Thumbnail>()
{
    m_instance = this;
    cache_limit = limit << 10;

    dir = new SQ_Dir(SQ_Dir::Thumbnails);

    valid_full = false;
}

SQ_PixmapCache::~SQ_PixmapCache()
{
    delete dir;
}

/*
 *  Write all entries to disk and clear cache.
 */
void SQ_PixmapCache::sync()
{
    // nothing to sync
    if(empty())
        return;

    iterator itEnd = end();

    // go through array and sync each entry
    for(iterator it = begin();it != itEnd;++it)
        syncEntry(it.key(), it.data());

    // remove all entries from cache
    clear();
}

/*
 *  Write one entry to disk and remove it from cache.
 */
void SQ_PixmapCache::syncEntry(const QString &key, SQ_Thumbnail &thumb)
{
    // let SQ_Dir save thumbnail
    dir->saveThumbnail(key, thumb);
}

/*
 *  Insert new entry to cache
 */
void SQ_PixmapCache::insert(const QString &key, const SQ_Thumbnail &thumb)
{
    // thumbnail is null ?
    if(thumb.thumbnail.isNull())
        return;

    // calc new cache size
    last_full += SQ_PixmapCache::entrySize(thumb);

    // add new entry
    QMap<QString, SQ_Thumbnail>::insert(key, thumb);
}

/*
 *  Remove entry from cache.
 */
void SQ_PixmapCache::removeEntry(const QString &key)
{
    iterator it = find(key);

    // no item to remove ?
    if(it == end())
        return;

    last_full -= SQ_PixmapCache::entrySize(it.data());

    QMap<QString, SQ_Thumbnail>::remove(key);
}

/*
 *  Remove entry from cache and from disk.
 */
void SQ_PixmapCache::removeEntryFull(const QString &key)
{
    // remove from memory
    removeEntry(key);

    // remove from disk
    dir->removeFile(key);
}

/*
 *  Check if pixmap, represented by 'key', is already in cache.
 */
bool SQ_PixmapCache::contains2(const QString &key, SQ_Thumbnail &th)
{
    iterator it = find(key);

    // item found
    if(it != end())
    {
        th = it.data();
        return true;
    }

    // not found
    return false;
}

/*
 *  Calculate total size used by cache (not exact!)
 */
int SQ_PixmapCache::totalSize()
{
    if(valid_full)
        return last_full;

    int total = 0;

    const_iterator itEnd = constEnd();

    // go through rray and calculate total size
    for(const_iterator it = constBegin();it != itEnd;++it)
        total += SQ_PixmapCache::entrySize(it.data());

    last_full = total;
    valid_full = true;

    return total;
}

/*
 *  Calculate cache-related size of given thumbnail (not exactly).
 */
int SQ_PixmapCache::entrySize(const SQ_Thumbnail &t)
{
    int  total = (((t.thumbnail.width() * t.thumbnail.height() * t.thumbnail.depth()) >> 3)
/*
                + t.info.bpp.length()
                + t.info.color.length()
                + t.info.compression.length()
                + t.info.dimensions.length()
                + t.info.frames.length()
                + t.info.type.length()
                + t.info.uncompressed.length()
*/
                + ((t.info.mime.width() * t.info.mime.height() * t.info.mime.depth()) >> 3));

    return total;
}

QString SQ_PixmapCache::root() const
{
    return dir->root();
}

void SQ_PixmapCache::clear()
{
    valid_full = false;

    QMap<QString, SQ_Thumbnail>::clear();
}
