/***************************************************************************
                          sq_bookmarks.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
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

#ifndef SQ_BOOKMARKS_H
#define SQ_BOOKMARKS_H

#include <qobject.h>

/**
  *@author ckult
  */

class SQ_Bookmarks : public QObject
{
	public: 
		SQ_Bookmarks();
		~SQ_Bookmarks();
};

#endif
