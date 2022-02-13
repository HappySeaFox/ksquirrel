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

SQ_FileviewFilter::SQ_FileviewFilter() : QValueVector<FILTER> ()
{
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
