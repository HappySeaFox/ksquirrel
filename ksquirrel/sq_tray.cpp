/***************************************************************************
                          sq_tray.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qpixmap.h>

#include <kstandarddirs.h>

#include "sq_tray.h"

SQ_Tray::SQ_Tray(QWidget *parent, const char *name) : KSystemTray(parent, name)
{
    setPixmap(QPixmap(locate("appdata", "images/tray.png")));
}

SQ_Tray::~SQ_Tray()
{}
