/***************************************************************************
                          sq_about.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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


#ifndef SQ_ABOUT_KSQUIRREL_H
#define SQ_ABOUT_KSQUIRREL_H

#include <kaboutdata.h>

/*
 *  'About' dialog
 */

static const char *description = I18N_NOOP("KSquirrel - image viewer for KDE");

#ifdef PACKAGE_VERSION
#define SQ_VERSION PACKAGE_VERSION
#else
#define SQ_VERSION "0.6.0-pre8"
#endif

static KAboutData aboutData(
			"ksquirrel",

#ifndef SQ_SMALL

			"KSquirrel",

#else

			"KSquirrel (light version)",

#endif

			SQ_VERSION, 
			description,
			KAboutData::License_GPL,
			"(c) 2003-2005 Baryshev Dmitry", 
			QString::null,
			"http://ksquirrel.sourceforge.net",
			QString::null);
    
#endif
