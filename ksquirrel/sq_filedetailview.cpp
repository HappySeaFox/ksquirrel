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

#include <qheader.h>

#include <kurldrag.h>
#include <kapplication.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"

SQ_FileListViewItem::SQ_FileListViewItem(QListView *parent, KFileItem *fi)
													: KFileListViewItem(parent, fi)
{}

SQ_FileListViewItem::~SQ_FileListViewItem()
{}

void SQ_FileListViewItem::paintFocus(QPainter *, const QColorGroup &, const QRect &)
{}

SQ_FileDetailView::SQ_FileDetailView(QWidget* parent, const char* name) : KFileDetailView(parent, name)
{
//	disconnect(this, SIGNAL(clicked(QListViewItem *, const QPoint&, int)), 0, 0);
//	disconnect(this, SIGNAL(doubleClicked(QListViewItem *, const QPoint&, int)), 0, 0);
}

SQ_FileDetailView::~SQ_FileDetailView()
{}

QDragObject* SQ_FileDetailView::dragObject()
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

void SQ_FileDetailView::insertItem(KFileItem *i)
{
	if(i->isDir() && sqConfig->readBoolEntry("Fileview", "disable_dirs", false))
		return;

	SQ_FileListViewItem *item = new SQ_FileListViewItem(this, i);

	initItem(item, i);

	i->setExtraData(this, item);
}

void SQ_FileDetailView::initItem(SQ_FileListViewItem *item, const KFileItem *i)
{
	QDir::SortSpec spec = KFileView::sorting();

	if(spec & QDir::Time)
		item->setKey(sortingKey(i->time( KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
	else if(spec & QDir::Size)
		item->setKey(sortingKey(i->size(), i->isDir(), spec));
	else
		item->setKey(sortingKey(i->text(), i->isDir(), spec));
}

void SQ_FileDetailView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
	QPoint vp = contentsToViewport(e->pos());
	
	QListViewItem *item = itemAt(vp);

	emit QListView::doubleClicked(item);

	int col = item ? header()->mapToLogical(header()->cellAt(vp.x())) : -1;

	if(item)
	{
		if(e->button() == Qt::LeftButton && !sqWStack->visibleWidget()->sing)
			emitExecute(item, e->globalPos(), col);

		emit doubleClicked(item, e->globalPos(), col);
	}
	else
	{
		kapp->invokeBrowser(sqWStack->getURL().path());
	}
}
