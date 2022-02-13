/***************************************************************************
                          sq_config.cpp  -  description
                             -------------------
    begin                : ??? ??? 14 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
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

SQ_Config * SQ_Config::sing = NULL;

SQ_Config::SQ_Config(const QString& fileName) : KConfig(fileName)
{
	sing = this;
}

SQ_Config::~SQ_Config()
{}

QString SQ_Config::readEntry(const QString &sgroup, const QString &key, const QString& aDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readEntry(key, aDefault);
}

int SQ_Config::readNumEntry(const QString &sgroup, const QString &key, int nDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readNumEntry(key, nDefault);
}

bool SQ_Config::readBoolEntry(const QString &sgroup, const QString &pKey, const bool bDefault)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readBoolEntry(pKey, bDefault);
}

QStringList SQ_Config::readListEntry(const QString &sgroup, const QString &key, char sep)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readListEntry(key, sep);
}

QValueList<int> SQ_Config::readIntListEntry(const QString &sgroup, const QString &key)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readIntListEntry(key);
}

QRect SQ_Config::readRectEntry(const QString &sgroup, const QString &key, const QRect *def)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readRectEntry(key, def);
}

QFont SQ_Config::readFontEntry(const QString &sgroup, const QString &key, const QFont *pd)
{
	if(group() != sgroup) setGroup(sgroup);

	return KConfig::readFontEntry(key, pd);
}

SQ_Config* SQ_Config::instance()
{
	return sing;
}
