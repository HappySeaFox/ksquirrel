/***************************************************************************
                          sq_application.h  -  description
                             -------------------
    begin                : Mon Mar 1 2004
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

#ifndef SQ_APPLICATION_H
#define SQ_APPLICATION_H

#include <kapplication.h>

/**
  *@author CKulT
  */

class SQ_Application : public KApplication
{
	public: 
		SQ_Application(bool allowS = true, bool GUIenabled = true);
		~SQ_Application();
};

#endif
