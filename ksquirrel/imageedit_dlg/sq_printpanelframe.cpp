/***************************************************************************
                          sq_printpanelframe.cpp  -  description
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

#include "sq_printpanelframe.h"
#include "sq_printpanel.h"

#define DIM 36

SQ_PrintPanelFrame::SQ_PrintPanelFrame(QWidget *parent, const char *name) : QFrame(parent, name)
{
    setFixedWidth(DIM);
    setFixedHeight(DIM);
    setFrameShape(QFrame::Box);
}

SQ_PrintPanelFrame::~SQ_PrintPanelFrame()
{}

void SQ_PrintPanelFrame::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    SQ_PrintPanel::instance()->toggleCurrentFrameColor(this);
}
