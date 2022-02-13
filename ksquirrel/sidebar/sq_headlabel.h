/***************************************************************************
                          sq_headlabel.h  -  description
                             -------------------
    begin                : ??? Nov 28 2005
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

#ifndef SQ_HEADLABEL_H
#define SQ_HEADLABEL_H

#include <qlabel.h>

class SQ_HeadLabel : public QLabel
{
    public:
        SQ_HeadLabel(QWidget *parent = 0, const char *name = 0);
        ~SQ_HeadLabel();
};

#endif
