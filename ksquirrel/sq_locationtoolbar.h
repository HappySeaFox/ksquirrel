/***************************************************************************
                          sq_locationtoolbar.h  -  description
                             -------------------
    begin                : ??? Nov 11 2004
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

#ifndef SQ_LOCATIONTOOLBAR_H
#define SQ_LOCATIONTOOLBAR_H

#include <ktoolbar.h>

/*
 *  SQ_LocationToolbar is a simple wrapper of KToolBar.
 *
 *  Used by KSquirrel.
 */

class SQ_LocationToolbar : public KToolBar
{
    Q_OBJECT

    public: 
        SQ_LocationToolbar(QWidget *parent = 0, const char *name = 0);
        ~SQ_LocationToolbar();

        /*
         *  Is toolbar visible ?
         */
        bool configVisible() const;

    public slots:

        /*
         *  Reimplement setShown() to let us know, if toolbar
         *  shown.
         */
        void setShown(bool shown);

    private:
        bool vis;
};

inline
bool SQ_LocationToolbar::configVisible() const
{
    return vis;
}

#endif
