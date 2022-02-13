/***************************************************************************
                          sq_mountview.cpp  -  description
                             -------------------
    begin                : ??? Nov 29 2005
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

#include <kmountpoint.h>
#include <kmimetype.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <klocale.h>

#include "sq_mountview.h"

SQ_MountView::SQ_MountView(QWidget *parent, const char *name) : KFileIconView(parent, name)
{
    actionCollection()->action("large rows")->activate();

    setAcceptDrops(false);
    setMaxItemWidth(200);

    new KAction(i18n("Reload"), Key_F5, this, SLOT(slotReload()), actionCollection(), "Reload devices");

    connect(this, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem *)));

    slotReload();
}

SQ_MountView::~SQ_MountView()
{}

void SQ_MountView::slotExecuted(QIconViewItem *i)
{
    if(!i)
        return;

    KFileIconViewItem *f = dynamic_cast<KFileIconViewItem *>(i);

    if(!f)
        return;

    KFileItem *fi = f->fileInfo();

    if(!fi)
        return;

    emit path(fi->url().path());
}

void SQ_MountView::slotReload()
{
    KIconView::clear();

    // get currently mounted filesystems
    KMountPoint::List mt = KMountPoint::currentMountPoints();

    KFileItem *i;
    KFileIconViewItem *fi;

    for(KMountPoint::List::iterator it = mt.begin();it != mt.end();++it)
    {
        if((*it)->mountedFrom().startsWith("/"))
        {
            i = new KFileItem((*it)->mountPoint(),
                        KMimeType::findByURL((*it)->mountedFrom())->name(), 0);

            fi = new KFileIconViewItem(this, (*it)->mountPoint(),
                        KMimeType::pixmapForURL((*it)->mountedFrom(), 0,
                        KIcon::Desktop, KIcon::SizeLarge), i);
        }
    }
}
