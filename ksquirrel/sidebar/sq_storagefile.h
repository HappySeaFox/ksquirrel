/***************************************************************************
                          sq_storagefile.h  -  description
                             -------------------
    begin                : Sat Mar 3 2007
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

#ifndef SQ_STORAGEFILE_H
#define SQ_STORAGEFILE_H

#include <qstring.h>

#include <kurl.h>

namespace SQ_StorageFile
{
    void writeStorageFile(const QString &path, const KURL &inpath, KURL w = KURL());
    void writeStorageFileAsString(const QString &path, const KURL &inpath, QString content = QString::null);

    KURL readStorageFile(const QString &path);
    QString readStorageFileAsString(const QString &path);
};

#endif
