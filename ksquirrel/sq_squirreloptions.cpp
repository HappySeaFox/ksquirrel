/***************************************************************************
                          sq_squirreloptions.cpp  -  description
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

#include "sq_squirreloptions.h"
#include "ksquirrel.h"

SQ_SquirrelOptions::SQ_SquirrelOptions(QObject * parent, const char *name) : QObject(parent, name)
{
}

SQ_SquirrelOptions::~SQ_SquirrelOptions()
{}

void SQ_SquirrelOptions::init(KConfig *conf)
{
	config = conf;

	//@todo init options here
}
