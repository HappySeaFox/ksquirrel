/***************************************************************************
                          sq_config.cpp  -  description
                             -------------------
    begin                : ??? ??? 14 2004
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

#include "sq_config.h"

SQ_Config::SQ_Config(const QString& fileName) : KConfig(fileName)
{}

SQ_Config::~SQ_Config()
{}

QString SQ_Config::readEntry(const QString &sgroup, const QString &key, const QString& aDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return readEntry(key, aDefault);
}

QString SQ_Config::readEntry(const char *sgroup, const QString &key, const QString& aDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readEntry(key, aDefault);
}

int SQ_Config::readNumEntry(const QString &sgroup, const QString &key, int nDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readNumEntry(key, nDefault);
}

int SQ_Config::readNumEntry(const char *sgroup, const QString &key, int nDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readNumEntry(key, nDefault);
}

bool SQ_Config::readBoolEntry(const QString &sgroup, const QString &pKey, const bool bDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readBoolEntry(pKey, bDefault);
}

bool SQ_Config::readBoolEntry(const char *sgroup, const QString &pKey, const bool bDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readBoolEntry(pKey, bDefault);
}
