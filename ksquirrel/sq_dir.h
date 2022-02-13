/***************************************************************************
                          sq_dir.h  -  description
                             -------------------
    begin                : Mon Mar 2 2004
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

/**
  *@author CKulT
  */

class SQ_Dir : public QDir
{
	public: 
		SQ_Dir();
		SQ_Dir(const QString & path, const QString & nameFilter = QString::null, int sortSpec = Name | IgnoreCase, int filterSpec = All);
		SQ_Dir(const QDir & d);
		~SQ_Dir();

		QString operator[] (int index) const;
};

#endif
