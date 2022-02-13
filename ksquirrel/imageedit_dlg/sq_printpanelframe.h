/***************************************************************************
                          sq_printpanelframe.h  -  description
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

#ifndef SQ_PRINTPANELFRAME_H
#define SQ_PRINTPANELFRAME_H

#include <qframe.h>

class SQ_PrintPanelFrame : public QFrame
{
    public:
        SQ_PrintPanelFrame(QWidget *parent = 0, const char *name = 0);
        ~SQ_PrintPanelFrame();

    protected:
        virtual void mousePressEvent(QMouseEvent *e);
};

#endif
