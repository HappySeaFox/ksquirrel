/***************************************************************************
                          sq_thumbnailsunused.h  -  description
                             -------------------
    begin                : Fri Jul 20 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_THUMBNAILSUNUSED_H
#define SQ_THUMBNAILSUNUSED_H

#include <qmap.h>
#include <qobject.h>

#include <kurl.h>

#include <ctime>

class KConfig;

/*
 *  This is small helper class for thumbnail loader. It is used only on
 *  non-local filesystems to cache information on unsupported file
 *  types to avoid downloading them from [possibly] slow kio-slave
 *  (for example, smb:/ on slow network).
 */

class SQ_ThumbnailsUnused : public QObject, public QMap<KURL, time_t>
{
    public:
        SQ_ThumbnailsUnused(QObject *parent = 0);
        ~SQ_ThumbnailsUnused();

        bool needUpdate(const KURL &u, time_t t);

        static SQ_ThumbnailsUnused* instance() { return m_instance; };

    private:
        void load();
        void save();

    private:
        static SQ_ThumbnailsUnused *m_instance;
        KConfig *cache;
};

#endif
