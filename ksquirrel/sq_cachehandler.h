/***************************************************************************
                          sq_cachehandler.h  -  description
                             -------------------
    begin                : ??? ??? 26 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@uandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_CACHEHANDLER_H
#define SQ_CACHEHANDLER_H

#include <qstring.h>

#include "sq_zfstream.h"

/**
  *@author CKulT
  */

class SQ_CacheHandler
{
	private:
		QString		path;
		gzofstream	outf;
		gzifstream	inf;

	public: 
		SQ_CacheHandler();
		~SQ_CacheHandler();

		QString getPath() const;
		void setPath(const QString &new_path);
};

#endif
