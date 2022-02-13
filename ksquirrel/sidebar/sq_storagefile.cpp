/***************************************************************************
                          sq_storagefile.cpp  -  description
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

#include <qfile.h>
#include <qstring.h>
#include <qstring.h>

#include <kmdcodec.h>

#include "sq_storagefile.h"

void SQ_StorageFile::writeStorageFile(const QString &path, const KURL &inpath, KURL w)
{
    SQ_StorageFile::writeStorageFileAsString(path, inpath, w.prettyURL());
}

void SQ_StorageFile::writeStorageFileAsString(const QString &path, const KURL &inpath, QString content)
{
    if(content.isEmpty())
        content = inpath.prettyURL();

    KMD5 md5(QFile::encodeName(inpath.prettyURL()));
    QFile file(path + QString::fromLatin1(".") + QString(md5.hexDigest()));

    if(file.open(IO_WriteOnly))
    {
        QString k = content.utf8();
        file.writeBlock(k, k.length());
        file.close();
    }
}

KURL SQ_StorageFile::readStorageFile(const QString &path)
{
    QString n = SQ_StorageFile::readStorageFileAsString(path);

    int ind = n.find('\n');

    if(ind != -1)
        n.truncate(ind);

    return KURL::fromPathOrURL(n);
}

QString SQ_StorageFile::readStorageFileAsString(const QString &path)
{
    QFile file(path);
    QString str;

    if(file.open(IO_ReadOnly))
    {
        QByteArray ba = file.readAll();

        if(file.status() == IO_Ok)
        {
            QString k;
            str.append(ba);
            str = QString::fromUtf8(str);
        }
    }

    file.close();

    return str;
}
