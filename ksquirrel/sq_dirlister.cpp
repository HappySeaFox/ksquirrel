/***************************************************************************
                          sq_dirlister.cpp  -  description
                             -------------------
    begin                : ??? ??? 27 2004
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

#include "sq_dirlister.h"

SQ_DirLister::SQ_DirLister(bool delayed) : KDirLister(delayed)
{}

SQ_DirLister::~SQ_DirLister()
{}

bool SQ_DirLister::openURL(const KURL &url, bool _keep, bool _reload)
{
	_reload = true; // I want KDirLister to reload directory every time.

	KDirLister::openURL(url, _keep, _reload);
}
