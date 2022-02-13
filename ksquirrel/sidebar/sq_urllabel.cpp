/***************************************************************************
                          sq_urllabel.cpp  -  description
                             -------------------
    begin                : Jan 2 2006
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

#include <kcursor.h>
#include <kglobalsettings.h>

#include "sq_urllabel.h"

SQ_URLLabel::SQ_URLLabel(QWidget *parent, const char *name) : QLabel(parent, name)
{
    setPaletteForegroundColor(KGlobalSettings::textColor());
    setPaletteBackgroundColor(KGlobalSettings::baseColor());
    setCursor(KCursor::handCursor());
}

SQ_URLLabel::~SQ_URLLabel()
{}

void SQ_URLLabel::enterEvent(QEvent *)
{
    QFont f = font();
    f.setUnderline(true);
    setFont(f);
}

void SQ_URLLabel::leaveEvent(QEvent *)
{
    QFont f = font();
    f.setUnderline(false);
    setFont(f);
}

void SQ_URLLabel::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        emit clicked();
}

#include "sq_urllabel.moc"
