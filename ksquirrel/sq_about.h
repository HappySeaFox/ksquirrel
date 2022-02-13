/***************************************************************************
                          sq_about.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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


#ifndef _ABOUT_SQUIRREL_H
#define _ABOUT_SQUIRREL_H

#include <kaboutdata.h>

static const char *description = "KSquirrel - image viewer for KDE with dynamic format support.";

static KAboutData aboutData(
			"ksquirrel", 
			"KSquirrel (build 5200)",
			"0.2.8", 
			description,
			KAboutData::License_GPL,
			"(c) 2004, CKulT", 
			"",
			"http://ksquirrel.sourceforge.net",
			"");
    
#endif
