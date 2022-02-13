/***************************************************************************
                          sq_dir.h  -  description
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

#ifndef SQ_DIR_H
#define SQ_DIR_H

#include <qdir.h>
#include <qpixmap.h>
#include <qfileinfo.h>

/**
  *@author CKulT
  */

class SQ_Dir : public QDir
{
	public: 
		SQ_Dir();
		~SQ_Dir();

		bool mkdir(const QString &relpath);
		void setRoot(const QString &name);
		void rewind();
		QString getRoot();
		QString getAbsPath(const QString relpath);
		void savePixmap(const QString &path, const QPixmap &pixmap);
		bool fileExists(const QString &file, QString &file);
		bool updateNeeded(const QString &file);

	private:
		QString root;
};

#endif
