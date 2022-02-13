/***************************************************************************
                          sq_iconloader.h  -  description
                             -------------------
    begin                : ??? Aug 20 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_ICONLOADER_H
#define SQ_ICONLOADER_H

#include <kiconloader.h>

/**
  *@author Baryshev Dmitry
  */

class SQ_IconLoader : public KIconLoader
{
	public: 
		SQ_IconLoader();
		~SQ_IconLoader();

	private:
		static SQ_IconLoader *sing;

	public:
		static SQ_IconLoader* instance();
};

#endif
