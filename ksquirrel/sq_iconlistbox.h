/***************************************************************************
                          sq_iconlistbox.h  -  description
                             -------------------
    begin                : ??? ??? 19 2004
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

#ifndef SQ_ICONLISTBOX_H
#define SQ_ICONLISTBOX_H

#include <klistbox.h>

/* 
 *  SQ_IconListBox represents a listbox with fixed width.
 *
 *  SQ_IconListBox and SQ_IconListItem are used in SQ_Options.
 */

class SQ_IconListBox : public KListBox
{
    public:
        SQ_IconListBox(QWidget * = 0, const char * = 0, WFlags = 0);

        void updateAndInstall(QObject *);

    private:
        void updateWidth();
        void invalidateHeight();
        void invalidateWidth();

    private:
        bool mHeightValid;
        bool mWidthValid;
};

#endif
