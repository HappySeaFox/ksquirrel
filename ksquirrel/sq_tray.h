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
 *  SQ_SystemTray represents tray. It also can show popup menu.
 */

class SQ_Tray : public KSystemTray
{
    Q_OBJECT

    public:
        SQ_Tray(QWidget *parent = 0, const char *name = 0);
        ~SQ_Tray();

    protected:

        /*
         *  Reimplement mouse events. We need to show
         *  popup menu.
         */
        void mousePressEvent(QMouseEvent *);

        // TODO: remove ?
        void mouseReleaseEvent(QMouseEvent *);

    private slots:

        /*
         *  Show main window.
         */
        void slotActivate();

        /*
         *  Quit...
         */
        void slotClose();

     private:
        KPopupMenu  *rightMenu;
        KAction     *pAOpen, *pAExit;
};

#endif
