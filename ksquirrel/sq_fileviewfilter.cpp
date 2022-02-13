/***************************************************************************
                          sq_fileviewfilter.cpp  -  description
                             -------------------
    begin                : ??? ??? 6 2004
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

#include "sq_fileviewfilter.h"
#include "ksquirrel.h"
#include "sq_config.h"

SQ_FileviewFilter::SQ_FileviewFilter() : QValueVector<FILTER> ()
{
	both = sqConfig->readBoolEntry("Filters", "menuitem both", true);
}

SQ_FileviewFilter::~SQ_FileviewFilter()
{}

void SQ_FileviewFilter::addFilter(const FILTER &filter)
{
	append(filter);
}

void SQ_FileviewFilter::addFilter(const QString &name, const QString &ext)
{
	FILTER filt = {name, ext};

	append(filt);
}

QString SQ_FileviewFilter::getFilterName(const int i)
{
	return (*this)[i].name;
}

QString SQ_FileviewFilter::getFilterExt(const int i)
{
	return (*this)[i].filter;
}

void SQ_FileviewFilter::writeEntries()
{
	int ncount = count(), cur = 1;
	QString num;

	sqConfig->deleteGroup("Filters");
	sqConfig->deleteGroup("Filters ext");

	for(int i = 0;i < ncount;i++,cur++)
	{
		sqConfig->setGroup("Filters");
		num.sprintf("%d", cur);
		sqConfig->writeEntry(num, getFilterName(i));
		sqConfig->setGroup("Filters ext");
		sqConfig->writeEntry(num, getFilterExt(i));
	}
}

void SQ_FileviewFilter::setShowBoth(bool newboth)
{
	both = newboth;
}

bool SQ_FileviewFilter::getShowBoth()
{
	return both;
}
