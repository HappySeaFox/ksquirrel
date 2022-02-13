/***************************************************************************
                          sq_filedetailview.cpp  -  description
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

#include <qheader.h>
#include <qcursor.h>

#include <kpopupmenu.h>
#include <kfileitem.h>
#include <kglobalsettings.h>
#include <konq_filetip.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_iconloader.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_dragprovider.h"

SQ_FileListViewItem::SQ_FileListViewItem(QListView *parent, KFileItem *fi) : KFileListViewItem(parent, fi)
{}

SQ_FileListViewItem::SQ_FileListViewItem(QListView *parent, const QString &text, const QPixmap &icon, KFileItem *fi)
    : KFileListViewItem(parent, text, icon, fi)
{}

SQ_FileListViewItem::~SQ_FileListViewItem()
{}

SQ_FileDetailView::SQ_FileDetailView(QWidget* parent, const char* name)
    : KFileDetailView(parent, name)
{
    // pixmap for directory item
    dirPix = SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, KIcon::SizeSmall);

    toolTip = new KonqFileTip(this);
    slotResetToolTip();

    disconnect(this, SIGNAL(onViewport()), this, 0);
    disconnect(this, SIGNAL(onItem(QListViewItem *)), this, 0);
    connect(this, SIGNAL(onViewport()), this, SLOT(slotRemoveToolTip()));
    connect(this, SIGNAL(onItem(QListViewItem *)), this, SLOT(slotShowToolTip(QListViewItem *)));
}

SQ_FileDetailView::~SQ_FileDetailView()
{
    slotRemoveToolTip();
}

void SQ_FileDetailView::slotResetToolTip()
{
    SQ_Config::instance()->setGroup("Fileview");

    toolTip->setOptions(true,
                        SQ_Config::instance()->readBoolEntry("tooltips_preview", false),
                        SQ_Config::instance()->readNumEntry("tooltips_lines", 6));
}

// Show extended tooltip for item under mouse cursor
void SQ_FileDetailView::slotShowToolTip(QListViewItem *item)
{
    SQ_Config::instance()->setGroup("Fileview");

    if(!SQ_Config::instance()->readBoolEntry("tooltips", false) ||
        (!KSquirrel::app()->isActiveWindow() && SQ_Config::instance()->readBoolEntry("tooltips_inactive", true)))
        return;

    // remove previous tootip and stop timer
    slotRemoveToolTip();

    KFileListViewItem *fitem = dynamic_cast<KFileListViewItem *>(item);

    if(!fitem) return;

    KFileItem *f = fitem->fileInfo();

    if(f) toolTip->setItem(f, fitem->rect(), fitem->pixmap(0));
}

bool SQ_FileDetailView::eventFilter(QObject *o, QEvent *e)
{
    if(o == viewport() || o == this)
    {
        int type = e->type();

        if(type == QEvent::Leave || type == QEvent::FocusOut || type == QEvent::Hide)
            slotRemoveToolTip();
    }

    return KFileDetailView::eventFilter(o, e);
}

/*
 *  Remove tootip.
 */
void SQ_FileDetailView::slotRemoveToolTip()
{
    toolTip->setItem(0);
}

/*
 *  Reimplement insertItem() to enable/disable inserting
 *  directories (depends on current settings).
 */
void SQ_FileDetailView::insertItem(KFileItem *i)
{
    SQ_Config::instance()->setGroup("Fileview");

    // directores disabled ?
    if(i->isDir() && SQ_Config::instance()->readBoolEntry("disable_dirs", false))
        return;

    // add new item
    setUpdatesEnabled(false);
    SQ_FileListViewItem *item = new SQ_FileListViewItem(this, i);

    initItem(item, i);
    setUpdatesEnabled(true);

    i->setExtraData(this, item);
}

/*
 *  Internal. Set item's sorting key.
 */
void SQ_FileDetailView::initItem(SQ_FileListViewItem *item, const KFileItem *i)
{
    // determine current sorting type
    QDir::SortSpec spec = KFileView::sorting();

    if(spec & QDir::Time)
        item->setKey(sortingKey(i->time(KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
    else if(spec & QDir::Size)
        item->setKey(sortingKey(i->size(), i->isDir(), spec));
    else
        item->setKey(sortingKey(i->text(), i->isDir(), spec));
}

/*
 *  On double click execute item or
 *  invoke default browser in current url.
 */
void SQ_FileDetailView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    QPoint vp = contentsToViewport(e->pos());

    QListViewItem *item = itemAt(vp);

    emit QListView::doubleClicked(item);

    int col = item ? header()->mapToLogical(header()->cellAt(vp.x())) : -1;

    // double click on item
    if(item)
        emit doubleClicked(item, e->globalPos(), col);

    // double click was in viewport, let's invoke browser
    else
        emit invokeBrowser();
}

// Accept dragging
void SQ_FileDetailView::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(true);
}

/*
 *  Insert ".." item.
 */
void SQ_FileDetailView::insertCdUpItem(const KURL &base)
{
    static const QString &dirup = KGlobal::staticQString("..");

    KFileItem *fi = new KFileItem(base.upURL(), "inode/directory", S_IFDIR);

    SQ_FileListViewItem *item = new SQ_FileListViewItem(this, dirup, dirPix, fi);

    item->setSelectable(false);
    item->setKey(sortingKey("..", true, QDir::Name|QDir::DirsFirst));

    fi->setExtraData(this, item);
}

/*
 *  Clear current view and insert "..".
 */
void SQ_FileDetailView::clearView()
{
    // call default clearing method
    KFileDetailView::clearView();

    // insert ".."
    insertCdUpItem(SQ_WidgetStack::instance()->url());
}

void SQ_FileDetailView::startDrag()
{
    SQ_Config::instance()->setGroup("Fileview");

    if(SQ_Config::instance()->readBoolEntry("drag", true))
    {
        SQ_DragProvider::instance()->setParams(this, *KFileView::selectedItems(), SQ_DragProvider::Icons);
        SQ_DragProvider::instance()->start();
    }
    else
        KFileDetailView::startDrag();
}

#include "sq_filedetailview.moc"
