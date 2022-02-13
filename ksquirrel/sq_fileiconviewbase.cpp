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

#include <kglobalsettings.h>
#include <kaction.h>
#include <kfileitem.h>
#include <kio/job.h>
#include <konq_filetip.h>

#include "ksquirrel.h"
#include "sq_fileiconviewbase.h"
#include "sq_config.h"

SQ_FileIconViewBase::SQ_FileIconViewBase(QWidget *parent, const char *name)
    : KFileIconView(parent, name)
{
    toolTip = new KonqFileTip(this);
    slotResetToolTip();

    disconnect(this, SIGNAL(onViewport()), this, 0);
    disconnect(this, SIGNAL(onItem(QIconViewItem *)), this, 0);
    connect(this, SIGNAL(onViewport()), this, SLOT(slotRemoveToolTip()));
    connect(this, SIGNAL(onItem(QIconViewItem *)), this, SLOT(slotShowToolTip(QIconViewItem *)));
}

SQ_FileIconViewBase::~SQ_FileIconViewBase()
{
    slotRemoveToolTip();
}

void SQ_FileIconViewBase::slotResetToolTip()
{
    SQ_Config::instance()->setGroup("Fileview");

    toolTip->setOptions(true,
                        SQ_Config::instance()->readBoolEntry("tooltips_preview", false),
                        SQ_Config::instance()->readNumEntry("tooltips_lines", 6));
}

// Show extended tooltip for item under mouse cursor
void SQ_FileIconViewBase::slotShowToolTip(QIconViewItem *item)
{
    SQ_Config::instance()->setGroup("Fileview");

    if(!SQ_Config::instance()->readBoolEntry("tooltips", false) ||
        (!KSquirrel::app()->isActiveWindow() && SQ_Config::instance()->readBoolEntry("tooltips_inactive", true)))
        return;

    // remove previous tootip and stop timer
    slotRemoveToolTip();

    KFileIconViewItem *fitem = dynamic_cast<KFileIconViewItem *>(item);

    if(!fitem) return;

    KFileItem *f = fitem->fileInfo();

    if(f) toolTip->setItem(f, fitem->rect(), fitem->pixmap());
}

bool SQ_FileIconViewBase::eventFilter(QObject *o, QEvent *e)
{
    if(o == viewport() || o == this)
    {
        int type = e->type();

        if(type == QEvent::Leave || type == QEvent::FocusOut || type == QEvent::Hide)
            slotRemoveToolTip();
    }

    return KFileIconView::eventFilter(o, e);
}

/*
 *  Remove tootip.
 */
void SQ_FileIconViewBase::slotRemoveToolTip()
{
    toolTip->setItem(0);
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
        emit invokeBrowser();
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
