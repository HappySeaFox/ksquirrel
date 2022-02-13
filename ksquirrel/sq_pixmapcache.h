/***************************************************************************
                          sq_pixmapcache.h  -  description
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

#ifndef SQ_PIXMAPCACHE_H
#define SQ_PIXMAPCACHE_H

#include <qmap.h>
#include <qpixmap.h>

class SQ_Dir;

class SQ_PixmapCache : public QMap<QString, QPixmap>
{
	public:
		SQ_PixmapCache(int limit = 1024);
		~SQ_PixmapCache();

		int  cacheLimit();
		void setCacheLimit(int);
		void sync();
		void insert(const QString &key, const QPixmap &pixmap);
		bool contains2(const QString &key, QPixmap &pixmap);

	private:
		bool removeLast(int bytes);

	private:
		int cache_limit;
		int current;
};

#endif
