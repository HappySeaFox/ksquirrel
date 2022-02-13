/***************************************************************************
                          sq_storagefile.cpp  -  description
                             -------------------
    begin                : Sat Mar 3 2007
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

#include <qfile.h>
#include <qstring.h>
#include <qstring.h>

#include <kmdcodec.h>

#include "sq_storagefile.h"

void SQ_StorageFile::writeStorageFile(const QString &path, const KURL &inpath)
{
    KMD5 md5(QFile::encodeName(inpath.prettyURL()));
    QFile file(path + QString::fromLatin1(".") + QString(md5.hexDigest()));

    if(file.open(IO_WriteOnly))
    {
        QString k = inpath.prettyURL().utf8();
        file.writeBlock(k, k.length());
        file.close();
    }
}

KURL SQ_StorageFile::readStorageFile(const QString &path)
{
    QFile file(path);
    KURL url;

    if(file.open(IO_ReadOnly))
    {
        QByteArray ba = file.readAll();

        if(file.status() == IO_Ok)
        {
            QString k;
            k.append(ba);
            url = KURL::fromPathOrURL(QString::fromUtf8(k));
        }
    }

    file.close();

    return url;
}
