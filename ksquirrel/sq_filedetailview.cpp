/***************************************************************************
                          sq_filedetailview.cpp  -  description
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

#include "sq_filedetailview.h"

#include <kurldrag.h>

SQ_FileDetailView::SQ_FileDetailView(QWidget* parent, const char* name) : KFileDetailView(parent, name)
{
	setDragEnabled(true);
	disconnect(this, SIGNAL(clicked(QListViewItem *, const QPoint&, int)), this, 0);
}


SQ_FileDetailView::~SQ_FileDetailView()
{}

void SQ_FileDetailView::slotSelected(QListViewItem *item, const QPoint &point, int c)
{
	emit doubleClicked(item, point, c);
}

QDragObject* SQ_FileDetailView::dragObject()
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
