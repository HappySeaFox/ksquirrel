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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kmessagebox.h>
#include <kmountpoint.h>
#include <kglobal.h>
#include <klocale.h>
#include <kautomount.h>
#include <kpopupmenu.h>

#include "ksquirrel.h"
#include "sq_mountview.h"
#include "sq_mountviewitem.h"
#include "sq_iconloader.h"
#include "sq_config.h"

SQ_MountView * SQ_MountView::m_inst = 0;

SQ_MountView::SQ_MountView(QWidget *parent, const char *name) : KListView(parent, name), m_columns(-1)
{
    m_inst = this;

    popup = new KPopupMenu;
    id_mount = popup->insertItem(SQ_IconLoader::instance()->loadIcon("hdd_mount", KIcon::Desktop, KIcon::SizeSmall),
                i18n("Mount"), this, SLOT(slotMount()));
    id_unmount = popup->insertItem(SQ_IconLoader::instance()->loadIcon("hdd_unmount", KIcon::Desktop, KIcon::SizeSmall),
                i18n("Unmount"), this, SLOT(slotUnmount()));
    popup->insertItem(SQ_IconLoader::instance()->loadIcon("reload", KIcon::Desktop, KIcon::SizeSmall),
                i18n("Refresh"), this, SLOT(slotRefresh()));
    popup->insertSeparator();
    popup->insertItem(i18n("Cancel"));

    connect(this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
            this, SLOT(slotContextMenu(KListView *, QListViewItem *, const QPoint &)));
    setAcceptDrops(false);

    connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem *)));
    connect(this, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem *)));

    setShowSortIndicator(true);
    setAllColumnsShowFocus(true);
    setSelectionMode(QListView::Single);
    setItemsMovable(false);

    // "Name" column will always exist
    addColumn(i18n("Name"));

    setupColumns();
}

SQ_MountView::~SQ_MountView()
{}

void SQ_MountView::slotExecuted(QListViewItem *i)
{
    SQ_MountViewItem *mvi = static_cast<SQ_MountViewItem *>(i);

    if(!mvi)
        return;

    if(mvi->mounted())
        emit path(mvi->text(0));
    else
    {
        mountItem = mvi;
        KAutoMount *mounter = new KAutoMount(false, QString::null, mvi->device(), QString::null, QString::null, false);
        connect(mounter, SIGNAL(finished()), this, SLOT(slotMountFinished()));
    }
}

void SQ_MountView::slotMountFinished()
{
    mountItem->setMounted(true);
    emit path(mountItem->text(0));
}

void SQ_MountView::slotMountError()
{
    mountItem->setMounted(false);
}

void SQ_MountView::reload(bool current)
{
    // get currently mounted filesystems
    KMountPoint::List mt = current ?
        KMountPoint::currentMountPoints(KMountPoint::NeedMountOptions | KMountPoint::NeedRealDeviceName)
        : KMountPoint::possibleMountPoints(KMountPoint::NeedMountOptions | KMountPoint::NeedRealDeviceName);

    SQ_MountViewItem *fi;
    int colum;

    for(KMountPoint::List::iterator it = mt.begin();it != mt.end();++it)
    {
        colum = 1;

        if(mounted.find((*it)->mountPoint()) == mounted.end())
        {
            mounted.append((*it)->mountPoint());

            // filter out /proc, swap etc.
            if(!(*it)->mountedFrom().startsWith("/") || !(*it)->mountPoint().startsWith("/"))
                continue;

            fi = new SQ_MountViewItem(this, (*it)->mountPoint());
            fi->setMounted(current);
            fi->setDevice((*it)->realDeviceName());

            if(m_columns & OPT_COL_DEVICE)
                fi->setText(colum++, (*it)->realDeviceName());

            if(m_columns & OPT_COL_FSTYPE)
                fi->setText(colum++, (*it)->mountType());

            if(m_columns & OPT_COL_OPTIONS)
                fi->setText(colum, (*it)->mountOptions().join(QString::fromLatin1(", ")));
        }
    }
}

bool SQ_MountView::exists(const QString &name)
{
    QListViewItemIterator it(this);

    while(it.current())
    {
        SQ_MountViewItem *mvi = static_cast<SQ_MountViewItem *>(it.current());

        if(mvi && mvi->text(0) == name)
            return true;

        ++it;
    }

    return false;
}

void SQ_MountView::setColumns(int cols)
{
    if(m_columns == cols)
        return;

    m_columns = cols;

    int cur = columns() - 1;

    // remove old columns, 0 is always "Name"
    for(int i = 0;i < cur;i++)
        removeColumn(1);

    // add new
    if(m_columns & OPT_COL_DEVICE)
        addColumn(i18n("Device"));

    if(m_columns & OPT_COL_FSTYPE)
        addColumn(i18n("FS Type"));

    if(m_columns & OPT_COL_OPTIONS)
        addColumn(i18n("Options"));

    slotRefresh();
}

void SQ_MountView::setupColumns()
{
    SQ_Config::instance()->setGroup("Sidebar");
    int p = 0;

    if(SQ_Config::instance()->readBoolEntry("mount_options", false))
        p |= SQ_MountView::OPT_COL_OPTIONS;

    if(SQ_Config::instance()->readBoolEntry("mount_fstype", true))
        p |= SQ_MountView::OPT_COL_FSTYPE;

    if(SQ_Config::instance()->readBoolEntry("mount_device", false))
        p |= SQ_MountView::OPT_COL_DEVICE;

    setColumns(p);
}

void SQ_MountView::slotContextMenu(KListView *, QListViewItem *i, const QPoint &p)
{
    SQ_MountViewItem *mvi = static_cast<SQ_MountViewItem *>(i);

    citem = mvi;

    popup->setItemEnabled(id_mount, !!mvi);
    popup->setItemEnabled(id_unmount, !!mvi);

    popup->exec(p);
}

void SQ_MountView::slotUnmount()
{
    if(citem)
    {
        KAutoUnmount *mounter = new KAutoUnmount(citem->text(0), QString::null);
        connect(mounter, SIGNAL(finished()), this, SLOT(slotUnmountFinished()));
    }
}

void SQ_MountView::slotMount()
{
    if(citem)
    {
        KAutoMount *mounter = new KAutoMount(false, QString::null, citem->device(), QString::null, QString::null, false);
        connect(mounter, SIGNAL(finished()), this, SLOT(slotMountFinished2()));
//        connect(mounter, SIGNAL(error()), this, SLOT(slotMountError()));
    }
}

void SQ_MountView::slotUnmountFinished()
{
    citem->setMounted(false);
}

void SQ_MountView::slotMountFinished2()
{
    citem->setMounted(true);
}

void SQ_MountView::slotRefresh()
{
    clear();
    mounted.clear();
    reload();
    reload(false);
}

#include "sq_mountview.moc"
