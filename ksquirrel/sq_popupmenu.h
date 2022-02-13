/***************************************************************************
                          sq_popupmenu.h  -  description
                             -------------------
    begin                : ??? June 3 2006
    copyright            : (C) 2006 by Baryshev Dmitry
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

#ifndef SQ_POPUPMENU_H
#define SQ_POPUPMENU_H

#include <kpopupmenu.h>

/**
  *@author Baryshev Dmitry
  */

class SQ_PopupMenu : public KPopupMenu
{
    public: 
	SQ_PopupMenu(QWidget *parent = 0, const char *name = 0);
    	~SQ_PopupMenu();

        void insertTitle(const QString &t);
        void changeTitle(const QString &t);

     private:
        int title;
};

#endif
