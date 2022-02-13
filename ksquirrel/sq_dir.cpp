/***************************************************************************
                          sq_dir.cpp  -  description
                             -------------------
    begin                : ??? ??? 20 2004
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

#include <qstringlist.h>
#include <qimage.h>
#include <qfile.h>

#include <kdebug.h>

#include "sq_dir.h"

static const QString thumbFormat = "PNG";
static const int thumbQuality = 80;

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

	cd(m_root);

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
	m_root = cleanDirPath(homeDirPath() + QString::fromLatin1("/.ksquirrel/"));
	QDir::mkdir(m_root);

	m_root = cleanDirPath(m_root + QString::fromLatin1("/") + name);
	QDir::mkdir(m_root);
}

void SQ_Dir::rewind()
{
	cd(m_root);
}

QString SQ_Dir::root() const
{
	return m_root;
}

void SQ_Dir::saveThumbnail(const QString &path, SQ_Thumbnail &thumb)
{
	if(thumb.thumbnail.isNull())
	{
		kdDebug() << "Thumbnail is NULL!" << endl;
		return;
	}

	QString fullpath(m_root + path), s;
	QFileInfo fpath(path), ffullpath(fullpath);

	if(fpath.lastModified() < ffullpath.lastModified())
	{
		kdDebug() << "equal => skipping writing ..." << endl;
		return;
	}

	kdDebug() << "writing accepted ..." << endl;

	if(!mkdir(fpath.dirPath(true)))
	{
		kdDebug() << "Saving thumbnail: mkdir() falied" << endl;
		return;
	}

	QString k = thumb.info.uncompressed.utf8();
	thumb.thumbnail.setText("sq_type", 0, thumb.info.type);
	thumb.thumbnail.setText("sq_dimensions", 0, thumb.info.dimensions);
	thumb.thumbnail.setText("sq_bpp", 0, thumb.info.bpp);
	thumb.thumbnail.setText("sq_color", 0, thumb.info.color);
	thumb.thumbnail.setText("sq_compression", 0, thumb.info.compression);
	thumb.thumbnail.setText("sq_frames", 0, thumb.info.frames);
	thumb.thumbnail.setText("sq_uncompressed", 0, k);

	thumb.thumbnail.save(fullpath, thumbFormat, thumbQuality);
}

bool SQ_Dir::fileExists(const QString &file, QString &fullpath)
{
	QFileInfo f(m_root + file);

	bool b = f.exists();

	if(b)
		fullpath = m_root + file;

	return b;
}

QString SQ_Dir::getAbsPath(const QString relpath)
{
	return m_root + "/" + relpath;
}

bool SQ_Dir::updateNeeded(const QString &file)
{
	QFileInfo fpath(file), ffullpath(m_root + file);

	return fpath.lastModified() > ffullpath.lastModified();
}

void SQ_Dir::removeFile(const QString &file)
{
	QString full = getAbsPath(file);

	QFile::remove(full);
}

