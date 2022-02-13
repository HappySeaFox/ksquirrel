/***************************************************************************
                          sq_headlabel.cpp  -  description
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

#include <kcursor.h>
#include <kglobalsettings.h>

#include "sq_headlabel.h"

SQ_HeadLabel::SQ_HeadLabel(QWidget *parent, const char *name) : QLabel(parent, name)
{
    setPaletteBackgroundColor(KGlobalSettings::highlightColor());
    setPaletteForegroundColor(KGlobalSettings::highlightedTextColor());
    setCursor(KCursor::handCursor());

    QFont f = font();
    f.setBold(true);
    setFont(f);

    setMargin(4);
}

SQ_HeadLabel::~SQ_HeadLabel()
{}
