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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qdragobject.h>
#include <qcursor.h>

#include <kapplication.h>
#include <kglobalsettings.h>
#include <kaction.h>
#include <kfileitem.h>
#include <kio/job.h>

#include "sq_fileiconviewbase.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"

SQ_FileIconViewBase::SQ_FileIconViewBase(QWidget *parent, const char *name)
    : KFileIconView(parent, name)
{
    disconnect(this, SIGNAL(currentChanged(QIconViewItem *)), 0, 0);

    connect(this, SIGNAL(mouseButtonClicked(int, QIconViewItem *, const QPoint &)),
                    this, SLOT(slotMouseButtonClicked(int, QIconViewItem *)));
    connect(this, SIGNAL(doubleClicked(QIconViewItem *)),
                    this, SLOT(slotDoubleClicked(QIconViewItem *)));
    connect(this, SIGNAL(currentChanged(QIconViewItem *)),
                    this, SLOT(slotCurrentChanged(QIconViewItem *)));
}

SQ_FileIconViewBase::~SQ_FileIconViewBase()
{}

void SQ_FileIconViewBase::exec(QIconViewItem *i, bool single, bool hl)
{
    if(i)
    {
        KFileIconViewItem *kvi = static_cast<KFileIconViewItem *>(i);
        KFileItem *kfi = kvi->fileInfo();

        if(hl && kfi) // highlight all items
            emit highlighted(kfi);
        else if(single && kfi && kfi->isFile()) // execute only files
            emit launch(kfi);
    }
}

void SQ_FileIconViewBase::slotDoubleClicked(QIconViewItem *i)
{
    exec(i, !KGlobalSettings::singleClick());
}

void SQ_FileIconViewBase::slotMouseButtonClicked(int btn, QIconViewItem *i)
{
    if(btn == Qt::LeftButton)
        exec(i, KGlobalSettings::singleClick());
}

void SQ_FileIconViewBase::slotCurrentChanged(QIconViewItem *i)
{
    exec(i, true, true);
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
        emit doubleClicked(item, e->globalPos());

    // double click in viewport, lets invoke browser
    else
        kapp->invokeBrowser(SQ_WidgetStack::instance()->url().path());
}

#include "sq_fileiconviewbase.moc"
