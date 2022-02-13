/***************************************************************************
                          sq_threaddirlister.cpp  -  description
                             -------------------
    begin                : Feb 10 2007
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <dirent.h>

#include <qfile.h>
#include <qdir.h>
#include <qobject.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qapplication.h>

#include <kconfig.h>
#include <kglobal.h>

#include "sq_threaddirlister.h"

SQ_ThreadDirLister::SQ_ThreadDirLister(QObject *o) 
    : QThread(), obj(o)
{
    cache = new KConfig("ksquirrel-tree-cache");
}

SQ_ThreadDirLister::~SQ_ThreadDirLister()
{
    // sync() & delete
    delete cache;
}

void SQ_ThreadDirLister::run()
{
    KURL      url;
    QString   path, name, filepath;
    DIR      *dir;
    dirent   *file;
    int       count_files, count_dirs;
    QDateTime dt, dt_def;
    QFileInfo fi;
    bool b_read;

    static const QString &dot    = KGlobal::staticQString(".");
    static const QString &dotdot = KGlobal::staticQString("..");

    while(true)
    {
        lock();
        count_files = todo.count();
        unlock();

        if(!count_files) break;

        lock();
        url = todo.first();
        unlock();

        path = url.path();
        count_files = count_dirs = 0;
        b_read = true;
        fi.setFile(path);

        if(cache->hasGroup(path))
        {
            cache->setGroup(path);
            dt = cache->readDateTimeEntry("modified", &dt_def);

            // compare cached time and real time
            if(dt.isValid() && fi.lastModified() == dt)
            {
                count_files = cache->readNumEntry("count_files", 0);
                count_dirs = cache->readNumEntry("count_dirs", 0);
                b_read = false;
            }
        }

        if(b_read)
        {
            dir = opendir(QFile::encodeName(path));
            dt = fi.lastModified(); // save directory last modified time

            if(dir)
            {
                while((file = readdir(dir)))
                {
                    name = QFile::decodeName(file->d_name);

                    if(name != dot && name != dotdot)
                    {
                       filepath = path + QDir::separator() + name;

                       fi.setFile(filepath);

                       if(fi.isDir())
                            count_dirs++;
                        else
                            count_files++;
                    }
                }

                closedir(dir);
            }
            else // opendir() failed, this value won't be cached
                b_read = false;
        }

        lock();
        todo.pop_front();
        unlock();

        // cache only new values
        if(b_read)
        {
            cache->setGroup(path);
            cache->writeEntry("modified", dt);
            cache->writeEntry("count_files", count_files);
            cache->writeEntry("count_dirs", count_dirs);
        }

        QApplication::postEvent(obj, new SQ_ItemsEvent(url, count_files, count_dirs));
    }// while
}
