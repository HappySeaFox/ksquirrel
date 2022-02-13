/***************************************************************************
                          sq_squirreloptions.h  -  description
                             -------------------
    begin                : Wed Mar 24 2004
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

#ifndef SQ_SQUIRRELOPTIONS_H
#define SQ_SQUIRRELOPTIONS_H

#include <qobject.h>

/**
  *@author ckult
  */

class KConfig;

class SQ_SquirrelOptions : public QObject
{
	private:
		KConfig *config;

	public: 
		SQ_SquirrelOptions(QObject * parent = 0, const char *name = 0);
		~SQ_SquirrelOptions();

		void init(KConfig *);
};

#endif
