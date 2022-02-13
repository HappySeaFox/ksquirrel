/***************************************************************************
                          sq_locationtoolbar.cpp  -  description
                             -------------------
    begin                : ??? ??? 11 2004
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

#include "sq_locationtoolbar.h"

SQ_LocationToolbar::SQ_LocationToolbar(QWidget *parent, const char *name) : KToolBar(parent, name), vis(false)
{}

SQ_LocationToolbar::~SQ_LocationToolbar()
{}

/*
 *  Reimplement setShown() to let us know, if toolbar
 *  shown.
 */
void SQ_LocationToolbar::setShown(bool shown)
{
	vis = shown;

	KToolBar::setShown(shown);
}
