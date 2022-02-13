/***************************************************************************
                          sq_thumbnailsunused.cpp  -  description
                             -------------------
    begin                : Fri Jul 20 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#include <kconfig.h>

#include "sq_thumbnailsunused.h"

SQ_ThumbnailsUnused * SQ_ThumbnailsUnused::m_instance = 0;

SQ_ThumbnailsUnused::SQ_ThumbnailsUnused(QObject *parent) : QObject(parent), QMap<KURL, time_t>()
{
    m_instance = this;

    cache = new KConfig("ksquirrel-unused-cache");

    load();
}

SQ_ThumbnailsUnused::~SQ_ThumbnailsUnused()
{
    save();
    delete cache;
}

bool SQ_ThumbnailsUnused::needUpdate(const KURL &u, time_t t)
{
//    printf("TU SIZE %d\n", size());
    iterator it = find(u);

    if(it == end())
    {
//        printf("TU NOTFOUND %s\n", u.url().ascii());
        return true;
    }

//    printf("TU COMPARE %s\n", (it.data() != t)?"!=":"=");

    return it.data() != t;
}

void SQ_ThumbnailsUnused::load()
{
    cache->setGroup("General");
    int count = cache->readNumEntry("count");

    if(count <= 0)
        return;

    QString is, s;
    time_t t;

    for(int i = 0;i < count;i++)
    {
        cache->setGroup("URL");

        is = QString::number(i);
        s = cache->readEntry(is);

        cache->setGroup("Time");
        t = cache->readNumEntry(is);

        insert(KURL::fromPathOrURL(s), t);
    }

//    printf("TU LOADED %d\n", size());
}

void SQ_ThumbnailsUnused::save()
{
    cache->deleteGroup("URL");
    cache->deleteGroup("Time");

    cache->setGroup("General");
    cache->writeEntry("count", size());

    iterator itEnd = end();
    QString is;
    int i = 0;

    for(iterator it = begin();it != itEnd;++it)
    {
        is = QString::number(i);
        cache->setGroup("URL");
        cache->writeEntry(is, it.key().prettyURL());

        cache->setGroup("Time");
        cache->writeEntry(is, it.data());

        i++;
    }
}
