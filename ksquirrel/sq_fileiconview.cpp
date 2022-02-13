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

#include <qpoint.h>

#include <kdebug.h>

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
    kdDebug() << "+SQ_FileIconView" << endl;

    QString n = name;
    disconnect(this, SIGNAL(clicked(QIconViewItem*, const QPoint&)), this, 0);
    setSorting(QDir::IgnoreCase);
    dirPix = SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, (n == "icon view")
                    ? KIcon::SizeMedium : KIcon::SizeSmall);
}

SQ_FileIconView::~SQ_FileIconView()
{
    kdDebug() << "-SQ_FileIconView" << endl;
}

void SQ_FileIconView::slotSelected(QIconViewItem *item, const QPoint &point)
{
    emit doubleClicked(item, point);
}

/*
 *  Get SQ_FileIconViewItem by KFileItem. All KFileItems store
 *  a pointer to appropriate SQ_FileIconViewItem as extra data.
 *  See also KFileItem::setExtraData() and insertItem().
 */
SQ_FileIconViewItem* SQ_FileIconView::viewItem(const KFileItem *item)
{
    return item ? ((SQ_FileIconViewItem*)item->extraData(this)) : NULL;
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

void SQ_FileIconView::updateView(const KFileItem *i)
{
    SQ_FileIconViewItem *item = viewItem(i);

    if(item)
        initItem(item, i);
}

/*
 *  Internal. Set item's sorting key.
 */
void SQ_FileIconView::initItem(SQ_FileIconViewItem *item, const KFileItem *i)
{
    // determine current sorting type
    QDir::SortSpec spec = KFileView::sorting();

    if(spec & QDir::Time)
        item->setKey(sortingKey((unsigned long)i->time(KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
    else if(spec & QDir::Size)
        item->setKey(sortingKey(i->size(), i->isDir(), spec));
    else
        item->setKey(sortingKey(i->text(), i->isDir(), spec));
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

    initItem(item, i);

    i->setExtraData(this, item);
}

/*
 *  Insert ".." item.
 */
void SQ_FileIconView::insertCdUpItem(const KURL &base)
{
    KFileItem *fi = new KFileItem(base.upURL(), QString::null, KFileItem::Unknown);

    // create ".." item
    SQ_FileIconViewItem *item = new SQ_FileIconViewItem(this, QString::fromLatin1(".."), dirPix, fi);

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
