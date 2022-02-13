/***************************************************************************
                          sq_dir.cpp  -  description
                             -------------------
    begin                : ??? ??? 20 2004
    copyright            : (C) 2004 by CKulT
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

#include <qstringlist.h>

#include "sq_dir.h"

SQ_Dir::SQ_Dir() : QDir()
{}

SQ_Dir::~SQ_Dir()
{}

bool SQ_Dir::mkdir(const QString &relpath)
{
	QString _relpath = cleanDirPath(relpath);

	QStringList paths = QStringList::split('/', _relpath);

	QStringList::iterator BEGIN = paths.begin();
	QStringList::iterator    END = paths.end();

	cd(root);

	for(QStringList::iterator it = BEGIN;it != END;it++)
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
	root = cleanDirPath(homeDirPath() + "/.ksquirrel/");
       QDir::mkdir(root);

	root = cleanDirPath(root + "/" + name);
	QDir::mkdir(root);
}

void SQ_Dir::rewind()
{
	cd(root);
}

QString SQ_Dir::getRoot()
{
	return root;
}

void SQ_Dir::savePixmap(const QString &path, const QPixmap &pixmap)
{
	QString fullpath(root + path), s;
	QFileInfo fpath(path), ffullpath(fullpath);

	if(fpath.lastModified() < ffullpath.lastModified())
	{
		printf("equal => skipping writing ...\n"); return;
	}

	printf("writing accepted ...\n");

	if(!mkdir(fpath.dirPath(true))) return;

	pixmap.save(fullpath, "PNG");
}

bool SQ_Dir::fileExists(const QString &file, QString &fullpath)
{
	QFileInfo f(root + file);

	bool b = f.exists();

	if(b)
		fullpath = root + file;

	return b;
}

QString SQ_Dir::getAbsPath(const QString relpath)
{
	return root + relpath;
}

bool SQ_Dir::updateNeeded(const QString &file)
{
	QFileInfo fpath(file), ffullpath(root + file);

	return fpath.lastModified() > ffullpath.lastModified();
}
