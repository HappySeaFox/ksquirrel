/***************************************************************************
                          sq_printpanelbase.h  -  description
                             -------------------
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

#ifndef SQ_PRINTPANELBASE_H
#define SQ_PRINTPANELBASE_H

#include <qframe.h>

#define SQ_K 6
#define SQ_M 2

#define PANEL_W_F 132
#define PANEL_H_F 152
#define PANEL_W PANEL_W_F-SQ_K
#define PANEL_H PANEL_H_F-SQ_K

class SQ_PrintPanelBase : public QFrame
{
    public:
        SQ_PrintPanelBase(QWidget *parent = 0, const char *name = 0);
        ~SQ_PrintPanelBase();
};

#endif
