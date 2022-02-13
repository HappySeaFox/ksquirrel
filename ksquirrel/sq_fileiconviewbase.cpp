/***************************************************************************
                          sq_fileiconviewbase.cpp  -  description
                             -------------------
    begin                : ??? Feb 22 2005
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

#include <qdragobject.h>
#include <qevent.h>
#include <qcursor.h>

#include <kapplication.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kio/job.h>

#include "sq_fileiconviewbase.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_navigatordropmenu.h"

SQ_FileIconViewBase::SQ_FileIconViewBase(QWidget *parent, const char *name)
    : KFileIconView(parent, name)
{
    setAcceptDrops(true);
    connect(this, SIGNAL(dropped(QDropEvent *, const KURL::List &, const KURL &)),
                this, SLOT(slotDropped(QDropEvent *, const KURL::List &, const KURL &)));
}

SQ_FileIconViewBase::~SQ_FileIconViewBase()
{}

/*
 *  Somebody dropped urls in viewport. Let's execute popup menu with
 *  file actions.
 */
void SQ_FileIconViewBase::slotDropped(QDropEvent *, const KURL::List &urls, const KURL &_url)
{
    KURL url = (_url.isEmpty()) ? SQ_WidgetStack::instance()->url() : _url;

    // setup and show popup menu with actions
    SQ_NavigatorDropMenu::instance()->setupFiles(urls, url);
    SQ_NavigatorDropMenu::instance()->exec(QCursor::pos());
}

// Accept drag
void SQ_FileIconViewBase::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(true);
}

/*
 *  Handle double clicks.
 */
void SQ_FileIconViewBase::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    QIconView::contentsMouseDoubleClickEvent(e);

    QIconViewItem *item = findItem(e->pos());

    // double click on item
    if(item)
    {
        if(e->button() == Qt::LeftButton && !SQ_WidgetStack::instance()->diroperator()->singleClick())
            emitExecute(item, e->globalPos());

        emit doubleClicked(item, e->globalPos());
    }
    // double click in viewport, lets invoke browser
    else
    {
        kapp->invokeBrowser(SQ_WidgetStack::instance()->url().path());
    }
}
