/***************************************************************************
                          sq_dir.cpp  -  description
                             -------------------
    begin                : ??? ??? 20 2004
    copyright            : (C) 2004 by Baryshev Dmitry
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qstringlist.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qfileinfo.h>

#include <kurl.h>

#include "sq_dir.h"
#include "sq_thumbnailinfo.h"

SQ_Dir::SQ_Dir() : QDir()
{}

SQ_Dir::SQ_Dir(Prefix p) : QDir()
{
    switch(p)
    {
        case SQ_Dir::DirectoryBasket:
            setRoot("directorybasket");
        break;
        case SQ_Dir::Basket:
            setRoot("basket");
        break;
        case SQ_Dir::Categories:
            setRoot("categories");
        break;
    }
}

SQ_Dir::~SQ_Dir()
{}

/*
 *  Create relative directory in storage.
 *
 *  For example, if prefix == Thumbnails,
 *  mkdir("/mnt/win_c") will create
 *  ~/.ksquirrel/thumbnails/mnt/win_c.
 */
bool SQ_Dir::mkdir(const QString &relpath)
{
    QStringList paths = QStringList::split('/', QDir::cleanDirPath(relpath));

    cd(m_root);

    // recursively create directories
    for(QStringList::iterator it = paths.begin();it != paths.end();++it)
    {
        if(!exists(*it, false))
            if(!QDir::mkdir(*it))
                return false;

        cd(*it);
    }

    return true;
}

void SQ_Dir::setRoot(const QString &name)
{
    m_root = QDir::cleanDirPath(homeDirPath() + QDir::separator() + QString::fromLatin1(".ksquirrel"));
    QDir::mkdir(m_root);

    m_root = QDir::cleanDirPath(m_root + QDir::separator() + name);
    QDir::mkdir(m_root);
}

QString SQ_Dir::absPath(const KURL &relurl)
{
    return m_root + QDir::separator() + relurl.path();
}
