/***************************************************************************
                          sq_hloptions.h  -  description
                             -------------------
    begin                : ??? ??? 30 2004
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

#ifndef SQ_HLOPTIONS_H
#define SQ_HLOPTIONS_H

#include <qobject.h>
#include <qstring.h>


/**
  *@author CKulT
  */

class SQ_HLOptions : public QObject
{
	public: 
		SQ_HLOptions();
		~SQ_HLOptions();

	public:
		QString	path;
		bool showLibsAndExit;
};

#endif
