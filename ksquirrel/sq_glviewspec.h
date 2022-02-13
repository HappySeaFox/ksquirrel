/***************************************************************************
                          sq_glviewspec.h  -  description
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

#ifndef SQ_GLVIEWSPEC_H
#define SQ_GLVIEWSPEC_H

#include "sq_glbase.h"

/**
  *@author CKulT
  */

class SQ_GLViewSpec : public SQ_GLBase
{
	public: 
		SQ_GLViewSpec(QWidget *parent = 0, const char *name = 0);
		~SQ_GLViewSpec();

	protected:
		virtual void create();

};

#endif
