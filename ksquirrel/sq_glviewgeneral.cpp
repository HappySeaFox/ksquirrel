/***************************************************************************
                          sq_glviewgeneral.cpp  -  description
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

#include <qlayout.h>

#include "sq_glviewgeneral.h"
#include "sq_glviewwidget.h"

SQ_GLViewGeneral::SQ_GLViewGeneral(QWidget *parent, const char *name) : SQ_GLBase(parent, name)
{
	separate = false;
	create();
}

SQ_GLViewGeneral::~SQ_GLViewGeneral()
{}

void SQ_GLViewGeneral::create()
{
	QGridLayout *l = new QGridLayout(this, 1, 1, 0, 0, "SQ_GLBASE_LAYOUT");

	gl = new SQ_GLViewWidget(this);

	l->addWidget(gl, 0, 0);
}
