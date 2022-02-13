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

#include "sq_fileiconview.h"

#include <qstrlist.h>

#include <kurldrag.h>
#include <qdragobject.h>

SQ_FileIconView::SQ_FileIconView(QWidget *parent, const char *name) : KFileIconView(parent, name)
{
	disconnect(this, SIGNAL(clicked(QIconViewItem*, const QPoint&)), this, 0);
	setAcceptDrops(true);
//	connect(this, SIGNAL(dropped(QDropEvent*, const QValueList<QIconDragItem>&)), this, SLOT(slotDrop(QDropEvent*, const QValueList<QIconDragItem>&)));
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
