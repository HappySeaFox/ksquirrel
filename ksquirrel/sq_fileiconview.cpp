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

#include <qstrlist.h>
#include <qdragobject.h>

#include <kurldrag.h>
#include <kapplication.h>

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

SQ_FileIconView::SQ_FileIconView(QWidget *parent, const char *name) : KFileIconView(parent, name)
{
	disconnect(this, SIGNAL(clicked(QIconViewItem*, const QPoint&)), this, 0);
	setAcceptDrops(true);
	setSorting(QDir::IgnoreCase);
}

SQ_FileIconView::~SQ_FileIconView()
{}

void SQ_FileIconView::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept(true);
}

QDragObject* SQ_FileIconView::dragObject()
{
	const KFileItemList *list = KFileView::selectedItems();

	if(list->isEmpty())
		return 0;

	QPtrListIterator<KFileItem> it(*list);
	KURL::List urls;

	for(; it.current(); ++it)
		urls.append(it.current()->url());

	return (new KURLDrag(urls, viewport()));
}

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
	if(i->isDir() && sqConfig->readBoolEntry("Fileview", "disable_dirs", false))
		return;

	SQ_FileIconViewItem *item;

	item = new SQ_FileIconViewItem((QIconView*)this, i->text(), i->pixmap(iconSize()), i);

	initItem(item, i);

	i->setExtraData(this, item);
}

void SQ_FileIconView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
	QIconView::contentsMouseDoubleClickEvent(e);

	QIconViewItem *item = findItem(e->pos());

	if(item)
	{
		if(e->button() == Qt::LeftButton && !sqWStack->visibleWidget()->sing)
			emitExecute(item, e->globalPos());

		emit doubleClicked(item, e->globalPos());
	}
	else
	{
		kapp->invokeBrowser(sqWStack->getURL().path());
	}
}
