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

#ifndef SQ_GLVIEW_H
#define SQ_GLVIEW_H

#include "qvbox.h"

class SQ_GLWidget;

/**
  *@author CKulT
  */

class SQ_GLView : public QVBox
{
	public: 
		SQ_GLView(QWidget *parent = 0, const char *name = 0);
		~SQ_GLView();

		SQ_GLWidget *gl;

	protected:
		void create();

	private:
		bool separate;
};

#endif
