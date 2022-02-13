/***************************************************************************
                          sq_splitter.cpp  -  description
                             -------------------
    begin                : June 2 2006
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sq_splitter.h"
#include "sq_rubberband.h"
#include "sq_multibar.h"

SQ_Splitter::SQ_Splitter(QWidget *parent, const char *name) 
    : QHBox(parent, name), first(0)
{
    setSpacing(0);
}

SQ_Splitter::~SQ_Splitter()
{}

void SQ_Splitter::addWidget(QWidget *w)
{
    if(!first)
    {
        first = w;
        w->reparent(this, QPoint());

        rb = new SQ_RubberBand(this);

        connect(rb, SIGNAL(newx(const int)), this, SLOT(slotNewX(const int)));
    }
    else
    {
        w->reparent(this, QPoint());
        setStretchFactor(w, 1);
    }
}

void SQ_Splitter::slotNewX(const int x)
{
    if(SQ_MultiBar::instance())
        SQ_MultiBar::instance()->updateWidth(x);
}

#include "sq_splitter.moc"
