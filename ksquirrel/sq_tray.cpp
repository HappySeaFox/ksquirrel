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

#include <qcursor.h>
#include <qapplication.h>

#include <kaction.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kpopupmenu.h>
#include <kwin.h>
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_tray.h"
#include "sq_glview.h"

SQ_Tray::SQ_Tray(QWidget *parent, const char *name) : KSystemTray(parent, name)
{
    kdDebug() << "+SQ_Tray" << endl;

    // create popup menu
    rightMenu = new KPopupMenu;

    pAOpen = KStdAction::open(this, SLOT(slotActivate()), KSquirrel::app()->actionCollection(), "Open SQ from tray");
    pAExit = KStdAction::quit(this, SLOT(slotClose()), KSquirrel::app()->actionCollection(), "SQ close from tray");

    // insert actions to popup menu
    pAOpen->plug(rightMenu);
    KSquirrel::app()->actionCollection()->action("SQ Configure")->plug(rightMenu);
    rightMenu->insertSeparator();
    pAExit->plug(rightMenu);

    // set pixmap
    //
    // TODO: load icon "ksquirrel" ?
    setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/tray.png")));
}

SQ_Tray::~SQ_Tray()
{
    kdDebug() << "-SQ_Tray" << endl;

    delete rightMenu;
}

/*
 *  Reimplement mouse press event.
 */
void SQ_Tray::mousePressEvent(QMouseEvent *ev)
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

void SQ_Tray::mouseReleaseEvent(QMouseEvent *ev)
{
    KSystemTray::mouseReleaseEvent(ev);
}

/*
 *  Show main window.
 */
void SQ_Tray::slotActivate()
{
    if(SQ_GLView::window()->isSeparate() && !SQ_GLView::window()->isHidden())
        SQ_GLView::window()->show();

    KSquirrel::app()->show();
}

/*
 *  Quit...
 */
void SQ_Tray::slotClose()
{
    // save all parameters...
    KSquirrel::app()->finalActions();

    // quit
    qApp->quit();
}

#include "sq_tray.moc"
