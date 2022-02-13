/***************************************************************************
                          sq_fileiconview.cpp  -  description
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

#include <qpoint.h>

#include "sq_config.h"
#include "sq_iconloader.h"
#include "sq_fileiconview.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"

SQ_FileIconViewItem::SQ_FileIconViewItem(QIconView *parent, const QString &text,
     const QPixmap &pixmap, KFileItem *fi)
    : KFileIconViewItem(parent, text, pixmap, fi)
{}

SQ_FileIconViewItem::~SQ_FileIconViewItem()
{}

void SQ_FileIconViewItem::paintFocus(QPainter *, const QColorGroup &)
{}

SQ_FileIconView::SQ_FileIconView(QWidget *parent, const char *name) : SQ_FileIconViewBase(parent, name)
{
    QString n = name;

    setSorting(QDir::IgnoreCase);

    dirPix = SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, (n == "icon view")
                    ? KIcon::SizeMedium : KIcon::SizeSmall);
}

SQ_FileIconView::~SQ_FileIconView()
{}

void SQ_FileIconView::slotSelected(QIconViewItem *item, const QPoint &point)
{
    emit doubleClicked(item, point);
}

void SQ_FileIconView::updateView(bool b)
{
    if(!b)
        return;

    SQ_FileIconViewItem *item = static_cast<SQ_FileIconViewItem*>(QIconView::firstItem());

    if(item)
    {
        do
        {
            item->setPixmap((item->fileInfo())->pixmap(iconSize()));
            item = static_cast<SQ_FileIconViewItem*>(item->nextItem());
        }while(item);
    }
}

/*
 *  Reimplement insertItem() to enable/disable inserting
 *  directories (depends on current settings).
 */
void SQ_FileIconView::insertItem(KFileItem *i)
{
    SQ_Config::instance()->setGroup("Fileview");

    // directores disabled ?
    if(i->isDir() && SQ_Config::instance()->readBoolEntry("disable_dirs", false))
        return;

    SQ_FileIconViewItem *item;

    // add new item
    item = new SQ_FileIconViewItem((QIconView*)this, i->text(), i->pixmap(iconSize()), i);

    initItemMy(item, i);

    i->setExtraData(this, item);
}

/*
 *  Insert ".." item.
 */
void SQ_FileIconView::insertCdUpItem(const KURL &base)
{
    static const QString &dirup = KGlobal::staticQString("..");

    KFileItem *fi = new KFileItem(base.upURL(), "inode/directory", S_IFDIR);

    // create ".." item
    SQ_FileIconViewItem *item = new SQ_FileIconViewItem(this, dirup, dirPix, fi);

    item->setSelectable(false);

    fi->setExtraData(this, item);
}

/*
 *  Clear current view and insert "..".
 */
void SQ_FileIconView::clearView()
{
    // call default clearing method
    KFileIconView::clearView();

    // insert ".."
    insertCdUpItem(SQ_WidgetStack::instance()->url());
}

void SQ_FileIconView::listingCompleted()
{
    arrangeItemsInGrid();
}

#include "sq_fileiconview.moc"
