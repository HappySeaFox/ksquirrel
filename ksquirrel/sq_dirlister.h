/***************************************************************************
                          sq_dirlister.h  -  description
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

#ifndef SQ_DIRLISTER_H
#define SQ_DIRLISTER_H

#include <kdirlister.h>

/**
  *@author CKulT
  */

class SQ_DirLister : public KDirLister
{
	public:
		SQ_DirLister(bool = false);
		virtual ~SQ_DirLister();

		virtual bool openURL(const KURL &url, bool _keep = false, bool _reload = true);
};

#endif
