/***************************************************************************
                          sq_popupmenu.cpp  -  description
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

#include "sq_popupmenu.h"

SQ_PopupMenu::SQ_PopupMenu(QWidget *parent, const char *name)
    : KPopupMenu(parent, name), title(0)
{}

SQ_PopupMenu::~SQ_PopupMenu()
{}

void SQ_PopupMenu::insertTitle(const QString &t)
{
    title = KPopupMenu::insertTitle(t);
}

void SQ_PopupMenu::changeTitle(const QString &t)
{
    KPopupMenu::changeTitle(title, t);
}
