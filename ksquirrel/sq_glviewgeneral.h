/***************************************************************************
                          sq_glviewgeneral.h  -  description
                             -------------------
    begin                : ??? ??? 5 2004
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

#ifndef SQ_GLVIEWGENERAL_H
#define SQ_GLVIEWGENERAL_H

#include "sq_glbase.h"

/**
  *@author CKulT
  */

class SQ_GLViewGeneral : public SQ_GLBase
{
	public: 
		SQ_GLViewGeneral(QWidget *parent = 0, const char *name = 0);
		~SQ_GLViewGeneral();

	protected:
		virtual void create();

};

#endif
