/***************************************************************************
                          sq_bcglabel.cpp  -  description
                             -------------------
    begin                : Sun Sep 25 2005
    copyright            : (C) 2005 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sq_pushbutton.h"

SQ_PushButton::SQ_PushButton(QWidget *parent, const char *name) : QPushButton(parent, name)
{
    init();
}

SQ_PushButton::SQ_PushButton(const QString &text, QWidget *parent, const char *name) : QPushButton(text, parent, name)
{
    init();
}

SQ_PushButton::~SQ_PushButton()
{}

void SQ_PushButton::init()
{
    setFixedWidth(24);
    setFixedHeight(24);
}

void SQ_PushButton::setPopup(QPopupMenu *popup)
{
    if(popup)
    {
        setFixedWidth(36);
        QPushButton::setPopup(popup);
    }
}
