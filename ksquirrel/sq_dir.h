/***************************************************************************
                          sq_dir.h  -  description
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

#ifndef SQ_DIR_H
#define SQ_DIR_H

#include <qdir.h>
#include <qpixmap.h>
#include <qfileinfo.h>

#include "sq_thumbnailinfo.h"

class SQ_Dir : public QDir
{
	public: 
		enum Prefix { Thumbnails, Extracts, Desktops, Tmp };

		SQ_Dir(Prefix);
		~SQ_Dir();

		bool mkdir(const QString &relpath);
		void setRoot(const QString &name);
		void rewind();
		QString root() const;
		QString getAbsPath(const QString relpath);
		void saveThumbnail(const QString &path, SQ_Thumbnail &thumb);
		bool fileExists(const QString &file, QString &file);
		bool updateNeeded(const QString &file);
		void removeFile(const QString &file);

	private:
		QString m_root;
		QString def_prot;
};

#endif
