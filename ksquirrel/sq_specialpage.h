/***************************************************************************
                          sq_specialpage.h  -  description
                             -------------------
    begin                :  Mar 12 2004
    copyright            : (C) 2004 by ckult
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_SPECIALPAGE_H
#define SQ_SPECIALPAGE_H

#include <qwidget.h>

#include <kparts/part.h>
#include <klibloader.h>

/**
  *@author ckult
  */

class SQ_SpecialPage : public QWidget
{
	private:
		int Type;
		KParts::ReadOnlyPart *part;
		KLibFactory *factory;
		
	public: 
		SQ_SpecialPage(int type = 0xff, QWidget * parent = 0, const char * name = 0);
		~SQ_SpecialPage();

		int getType() const;
		QWidget* getWidget() const;
};

#endif
