/***************************************************************************
                          sq_mountviewitem.cpp  -  description
                             -------------------
    begin                : ??? Feb 24 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
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

#include <qfileinfo.h>

#include "sq_mountviewitem.h"
#include "sq_iconloader.h"

SQ_MountViewItem::SQ_MountViewItem(KListView *parent, const QString &mpoint)
    : KListViewItem(parent, mpoint), m_mounted(false)
{}

SQ_MountViewItem::~SQ_MountViewItem()
{}

void SQ_MountViewItem::setMounted(bool b)
{
    m_mounted = b;
    QFileInfo inf(text(0));

    if(m_mounted)
        setPixmap(0, inf.isReadable() ? SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, KIcon::SizeMedium)
                : SQ_IconLoader::instance()->loadIcon("folder_locked", KIcon::Desktop, KIcon::SizeMedium));
    else
        setPixmap(0, SQ_IconLoader::instance()->loadIcon("folder_red", KIcon::Desktop, KIcon::SizeMedium));
}
