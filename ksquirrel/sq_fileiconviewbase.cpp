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
{}

SQ_FileIconViewBase::~SQ_FileIconViewBase()
{}

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
        kapp->invokeBrowser(SQ_WidgetStack::instance()->url().prettyURL());
}

void SQ_FileIconViewBase::updateView(const KFileItem *i)
{
    KFileIconViewItem *item = viewItem(i);

    if(item)
        initItemMy(item, i, true);
}

KFileIconViewItem* SQ_FileIconViewBase::viewItem(const KFileItem *item)
{
    return item ? reinterpret_cast<KFileIconViewItem *>((void *)item->extraData(this)) : 0;
}

void SQ_FileIconViewBase::initItemMy(KFileIconViewItem *item, const KFileItem *i, bool upd)
{
    if(upd)
        item->setText(i->text(), true, true);

    // determine current sorting type
    QDir::SortSpec spec = KFileView::sorting();

    if(spec & QDir::Time)
        item->setKey(sortingKey((unsigned long)i->time(KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
    else if(spec & QDir::Size)
        item->setKey(sortingKey(i->size(), i->isDir(), spec));
    else
        item->setKey(sortingKey(i->text(), i->isDir(), spec));
}

#include "sq_fileiconviewbase.moc"
