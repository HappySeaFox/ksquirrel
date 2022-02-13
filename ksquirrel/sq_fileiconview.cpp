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

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_fileiconview.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"

SQ_FileIconViewItem::SQ_FileIconViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi):
		KFileIconViewItem(parent, text, pixmap, fi)
{}

SQ_FileIconViewItem::~SQ_FileIconViewItem()
{}

void SQ_FileIconViewItem::paintFocus(QPainter *, const QColorGroup &)
{}

SQ_FileIconView::SQ_FileIconView(QWidget *parent, const char *name) : SQ_FileIconViewBase(parent, name)
{
	QString n = name;
	disconnect(this, SIGNAL(clicked(QIconViewItem*, const QPoint&)), this, 0);
	setSorting(QDir::IgnoreCase);
	dirPix = KSquirrel::loader()->loadIcon("folder", KIcon::Desktop, (n == "icon view") ? KIcon::SizeMedium : KIcon::SizeSmall);
}

SQ_FileIconView::~SQ_FileIconView()
{}

void SQ_FileIconView::slotSelected(QIconViewItem *item, const QPoint &point)
{
	emit doubleClicked(item, point);
}

SQ_FileIconViewItem* SQ_FileIconView::viewItem(const KFileItem *item)
{
	return (item)?((SQ_FileIconViewItem*)item->extraData(this)):(0L);
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
		}while(item != 0L);
	}
}

void SQ_FileIconView::updateView(const KFileItem *i)
{
	SQ_FileIconViewItem *item = viewItem(i);

	if(item)
		initItem(item, i);
}

void SQ_FileIconView::initItem(SQ_FileIconViewItem *item, const KFileItem *i)
{
	QDir::SortSpec spec = KFileView::sorting();

	if(spec & QDir::Time)
		item->setKey(sortingKey((unsigned long)i->time(KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
	else if(spec & QDir::Size)
		item->setKey(sortingKey(i->size(), i->isDir(), spec));
	else
		item->setKey(sortingKey(i->text(), i->isDir(), spec));
}

void SQ_FileIconView::insertItem(KFileItem *i)
{
	if(i->isDir() && SQ_Config::instance()->readBoolEntry("Fileview", "disable_dirs", false))
		return;

	SQ_FileIconViewItem *item;

	item = new SQ_FileIconViewItem((QIconView*)this, i->text(), i->pixmap(iconSize()), i);

	initItem(item, i);

	i->setExtraData(this, item);
}

void SQ_FileIconView::insertCdUpItem(const KURL &base)
{
	KFileItem *fi = new KFileItem(base.upURL(), QString::null, KFileItem::Unknown);

	SQ_FileIconViewItem *item = new SQ_FileIconViewItem(this, QString::fromLatin1(".."), dirPix, fi);

	item->setSelectable(false);

	fi->setExtraData(this, item);
}

void SQ_FileIconView::clearView()
{
	KIconView::clear();

	insertCdUpItem(SQ_WidgetStack::instance()->getURL());
}

void SQ_FileIconView::listingCompleted()
{
/*	printf("LISTING, items: ");

	KFileItem *f = firstFileItem();

	while(f)
	{
		printf("%s, ", f->name().ascii());

		f = nextItem(f);
	}printf("\n\n");*/

	arrangeItemsInGrid();
}
