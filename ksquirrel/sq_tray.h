/***************************************************************************
                          sq_tray.h  -  description
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

#ifndef SQ_TRAY_H
#define SQ_TRAY_H

#include <ksystemtray.h>

class KAction;
class KPopupMenu;

/*
 *  SQ_SystemTray represents system tray.
 */

class SQ_Tray : public KSystemTray
{
    public:
        SQ_Tray(QWidget *parent = 0, const char *name = 0);
        ~SQ_Tray();
};

#endif
