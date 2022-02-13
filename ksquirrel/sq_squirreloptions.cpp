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

#include <qvaluevector.h>

SQ_SquirrelOptions::SQ_SquirrelOptions(QObject * parent, const char *name) : QObject(parent, name)
{
	num = 6;
}

SQ_SquirrelOptions::~SQ_SquirrelOptions()
{}

void SQ_SquirrelOptions::init(KConfig *conf)
{
	config = conf;

	l = new QValueVector<SquirrelOption> * [num];

	for(int i = 0;i < num;i++)
		l[i] = new QValueVector<SquirrelOption>;
}

void SQ_SquirrelOptions::reinit(KConfig *conf)
{
	for(int i = 0;i < num;i++)
	{
		l[i]->clear();
		delete [] l[i];
	}

	delete l;

	init(conf);
}

QString SQ_SquirrelOptions::readStringEntry(const int group, const int value, QString &def)
{
	return def;
}

bool SQ_SquirrelOptions::readBoolEntry(const int group, const int value, bool def)
{
	return def;
}

int SQ_SquirrelOptions::readNumEntry(const int group, const int value, int &def)
{
	return def;
}
