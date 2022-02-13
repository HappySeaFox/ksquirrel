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

#include <qcursor.h>
#include <qapplication.h>

#include <kaction.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <kwin.h>

#include "ksquirrel.h"
#include "sq_tray.h"
#include "sq_glview.h"

SQ_SystemTray::SQ_SystemTray(QWidget *parent, const char *name) : KSystemTray(parent, name)
{
    // create popup menu
    rightMenu = new KPopupMenu;

    KActionSeparator *pASep = new KActionSeparator;

    pAOpen = KStdAction::open(this, SLOT(slotActivate()), KSquirrel::app()->actionCollection(), "Open SQ from tray");
    pAExit = KStdAction::quit(this, SLOT(slotClose()), KSquirrel::app()->actionCollection(), "SQ close from tray");

    // insert actions to popup menu
    pAOpen->plug(rightMenu);
    KSquirrel::app()->pAConfigure->plug(rightMenu);
    pASep->plug(rightMenu);
    pAExit->plug(rightMenu);

    // set pixmap
    //
    // TODO: load icon "ksquirrel" ?
    setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/tray.png")));
}

/*
 *  Reimplement mouse press event.
 */
void SQ_SystemTray::mousePressEvent(QMouseEvent *ev)
{
    // left button ?
    if(ev->button() == Qt::LeftButton)
    {
        // activate main window
        slotActivate();
    }
    // right button ?
    else if(ev->button() == Qt::RightButton)
    {
        // show popup menu
        rightMenu->exec(QCursor::pos());
    }
}

void SQ_SystemTray::mouseReleaseEvent(QMouseEvent *ev)
{
    KSystemTray::mouseReleaseEvent(ev);
}

/*
 *  Show main window.
 */
void SQ_SystemTray::slotActivate()
{
    if(SQ_GLView::window()->isSeparate() && !SQ_GLView::window()->isHidden())
        SQ_GLView::window()->show();

    KSquirrel::app()->show();
}

/*
 *  Quit...
 */
void SQ_SystemTray::slotClose()
{
    // save all parameters...
    KSquirrel::app()->finalActions();

    // quit
    qApp->quit();
}
