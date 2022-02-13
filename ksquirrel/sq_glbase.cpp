/***************************************************************************
                          sq_glbase.cpp  -  description
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

#include "sq_glbase.h"

SQ_GLBase::SQ_GLBase(QWidget *parent, const char *name ) : QWidget(parent,name)
{
	setCaption("KSQuirrel: OpenGL viewer");
}

SQ_GLBase::~SQ_GLBase()
{}

void SQ_GLBase::closeEvent(QCloseEvent *e)
{
	e->ignore();
	hide();
}
