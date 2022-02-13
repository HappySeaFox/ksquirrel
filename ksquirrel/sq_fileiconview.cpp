/***************************************************************************
                          sq_fileiconview.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
    email                : squirrel-sf@yandex.ru
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

#include "sq_fileiconview.h"

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

void SQ_FileIconView::dropEvent(QDropEvent *e)
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

	return KURLDrag::newDrag(urls, viewport());
}

void SQ_FileIconView::slotSelected(QIconViewItem *item, const QPoint &point)
{
	emit doubleClicked(item, point);
}

KFileIconViewItem* SQ_FileIconView::viewItem(KFileItem *item)
{
	return (item)?((KFileIconViewItem*)item->extraData(this)):(0L);
}
