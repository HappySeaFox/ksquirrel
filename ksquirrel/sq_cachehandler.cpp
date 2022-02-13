/***************************************************************************
                          sq_cachehandler.cpp  -  description
                             -------------------
    begin                : ??? ??? 26 2004
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

#include "sq_cachehandler.h"

SQ_CacheHandler::SQ_CacheHandler()
{}

SQ_CacheHandler::~SQ_CacheHandler()
{}

QString SQ_CacheHandler::getPath() const
{
	return path;
}

void SQ_CacheHandler::setPath(const QString &new_path)
{
	path = new_path;
}
