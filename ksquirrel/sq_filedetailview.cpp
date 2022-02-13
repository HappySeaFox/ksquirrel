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
#include <qcursor.h>

#include <kapplication.h>
#include <kpopupmenu.h>

#include "sq_config.h"
#include "sq_iconloader.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_navigatordropmenu.h"

SQ_FileListViewItem::SQ_FileListViewItem(QListView *parent, KFileItem *fi) : KFileListViewItem(parent, fi)
{}

SQ_FileListViewItem::SQ_FileListViewItem(QListView *parent, const QString &text, const QPixmap &icon, KFileItem *fi)
		: KFileListViewItem(parent, text, icon, fi)
{}

SQ_FileListViewItem::~SQ_FileListViewItem()
{}

void SQ_FileListViewItem::paintFocus(QPainter *, const QColorGroup &, const QRect &)
{}

SQ_FileDetailView::SQ_FileDetailView(QWidget* parent, const char* name)
		: KFileDetailView(parent, name)
{
	setAcceptDrops(true);
	connect(this, SIGNAL(dropped(QDropEvent *, const KURL::List &, const KURL &)), this, SLOT(slotDropped(QDropEvent *, const KURL::List &, const KURL &)));
	dirPix = SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, KIcon::SizeSmall);
}

SQ_FileDetailView::~SQ_FileDetailView()
{}

void SQ_FileDetailView::insertItem(KFileItem *i)
{
	if(i->isDir() && SQ_Config::instance()->readBoolEntry("Fileview", "disable_dirs", false))
		return;

	SQ_FileListViewItem *item = new SQ_FileListViewItem(this, i);

	initItem(item, i);

	i->setExtraData(this, item);
}

void SQ_FileDetailView::initItem(SQ_FileListViewItem *item, const KFileItem *i)
{
	QDir::SortSpec spec = KFileView::sorting();

	if(spec & QDir::Time)
		item->setKey(sortingKey(i->time(KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
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
		if(e->button() == Qt::LeftButton && !SQ_WidgetStack::instance()->visibleWidget()->sing)
			emitExecute(item, e->globalPos(), col);

		emit doubleClicked(item, e->globalPos(), col);
	}
	else
	{
		kapp->invokeBrowser(SQ_WidgetStack::instance()->getURL().path());
	}
}

void SQ_FileDetailView::slotDropped(QDropEvent *, const KURL::List &urls, const KURL &_url)
{
	KURL url = (_url.isEmpty()) ? SQ_WidgetStack::instance()->getURL() : _url;

	SQ_NavigatorDropMenu::instance()->setupFiles(urls, url);
	SQ_NavigatorDropMenu::instance()->exec(QCursor::pos());
}

void SQ_FileDetailView::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept(true);
}

void SQ_FileDetailView::insertCdUpItem(const KURL &base)
{
	KFileItem *fi = new KFileItem(base.upURL(), QString::null, KFileItem::Unknown);

	SQ_FileListViewItem *item = new SQ_FileListViewItem(this, QString::fromLatin1(".."), dirPix, fi);

	item->setSelectable(false);
	item->setKey(sortingKey("..", true, QDir::Name|QDir::DirsFirst));

	fi->setExtraData(this, item);
}

void SQ_FileDetailView::clearView()
{
	KListView::clear();

	insertCdUpItem(SQ_WidgetStack::instance()->getURL());
}

void SQ_FileDetailView::listingCompleted()
{}
