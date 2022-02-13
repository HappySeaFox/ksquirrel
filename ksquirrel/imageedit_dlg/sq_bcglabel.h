/***************************************************************************
                          sq_bcglabel.h  -  description
                             -------------------
    begin                : ??? May 4 2005
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

#ifndef SQ_BCGLABEL_H
#define SQ_BCGLABEL_H

#include <qlabel.h>

/**
  *@author Baryshev Dmitry
  */

class SQ_BCGLabel : public QLabel
{
	public: 
		SQ_BCGLabel(QWidget *parent, const char *name = 0, WFlags f = 0);
		~SQ_BCGLabel();

                static int fixedWidth();
};

#endif
