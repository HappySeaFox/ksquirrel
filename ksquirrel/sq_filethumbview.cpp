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
#include <qfileinfo.h>
#include <qpainter.h>
#include <qhbox.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>

#include <kurldrag.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_dir.h"
#include "sq_filethumbview.h"
#include "sq_libraryhandler.h"
#include "sq_thumbnailjob.h"
#include "sq_widgetstack.h"
#include "sq_pixmapcache.h"
#include "sq_progress.h"

SQ_FileThumbView::SQ_FileThumbView(QWidget *parent, const char *name) : KFileIconView(parent, name)
{
	disconnect(this, SIGNAL(clicked(QIconViewItem*, const QPoint&)), this, 0);
	setAcceptDrops(true);
	setMode(KIconView::Select);

	progressBox = new QHBox(this);
	QToolBar *progressBoxBar = new QToolBar(QString::null, 0L, progressBox);
	buttonStop = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumb_resume.png")), QString::null, QString::null, this, SLOT(slotThumbnailUpdateToggle()), progressBoxBar);
	sqProgress = new SQ_Progress(progressBox);
	progressBox->setPalette(QPalette(QColor(255,255,255), QColor(255,255,255)));
	progressBox->setFrameShape(QFrame::Box);
	progressBox->setSpacing(3);
	progressBox->setStretchFactor(sqProgress, 1);
	progressBox->setStretchFactor(progressBoxBar, 0);
	progressBox->setGeometry(5, 5, 235, 24);

	thumbJob = new SQ_ThumbnailLoadJob(new KFileItemList(), SQ_ThumbnailSize());
	delete (SQ_ThumbnailLoadJob*)thumbJob;

	sqCache->setCacheLimit(sqConfig->readNumEntry("Thumbnails", "cache", 1024*10));
}

SQ_FileThumbView::~SQ_FileThumbView()
{}

void SQ_FileThumbView::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept(true);
}

void SQ_FileThumbView::dropEvent(QDropEvent *e)
{
	e->accept(true);
}

QDragObject* SQ_FileThumbView::dragObject()
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

void SQ_FileThumbView::slotSelected(QIconViewItem *item, const QPoint &point)
{
	emit doubleClicked(item, point);
}

KFileIconViewItem* SQ_FileThumbView::viewItem(const KFileItem *item)
{
	return (item)?((KFileIconViewItem*)item->extraData(this)):(0L);
}

void SQ_FileThumbView::insertItem(KFileItem *i)
{
//	KFileView::insertItem(i);

	QFileInfo fm(i->name().local8Bit());

	bool disable_mime = sqConfig->readBoolEntry("Thumbnails", "disable_mime", true);
	const QPixmap mimeall = sqLoader->loadIcon("unknown", KIcon::Desktop, sqThumbSize->pixelSize());

	KFileIconViewItem *item;

	if(i->isDir())
		item = new KFileIconViewItem((QIconView*)this, i->text(), i->pixmap(sqThumbSize->pixelSize()), i);
	else if(disable_mime)
		item = new KFileIconViewItem((QIconView*)this, i->text(), mimeall, i);
	else
		item = new KFileIconViewItem((QIconView*)this, i->text(), i->pixmap(sqThumbSize->pixelSize()), i);

	initItem(item, i);

	i->setExtraData(this, item);
}

void SQ_FileThumbView::initItem(KFileIconViewItem *item, const KFileItem *i)
{
	QDir::SortSpec spec = KFileView::sorting();

	if(spec & QDir::Time)
		item->setKey(sortingKey((unsigned long)i->time(KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
	else if(spec & QDir::Size)
		item->setKey(sortingKey(i->size(), i->isDir(), spec));
	else
		item->setKey(sortingKey(i->text(), i->isDir(), spec));
}

void SQ_FileThumbView::setThumbnailPixmap(const KFileItem* fileItem, const QPixmap& thumbnail)
{
	KFileIconViewItem *iconItem = viewItem(fileItem);

	if(!iconItem) return;

	QPixmap *p = iconItem->pixmap();

	if(p->convertToImage() == thumbnail.convertToImage())
		return;

	iconItem->setPixmap(thumbnail);
}

void SQ_FileThumbView::startThumbnailUpdate()
{
	stopThumbnailUpdate();
	doStartThumbnailUpdate(items());
}

void SQ_FileThumbView::doStartThumbnailUpdate(const KFileItemList* list)
{
	thumbJob = new SQ_ThumbnailLoadJob(list, *sqThumbSize);

	connect(thumbJob, SIGNAL(thumbnailLoaded(const KFileItem*,const QPixmap&)),
		this, SLOT(setThumbnailPixmap(const KFileItem*,const QPixmap&)) );

	connect(thumbJob, SIGNAL(result(KIO::Job*)), sqWStack, SLOT(thumbnailsUpdateEnded()));

	sqWStack->thumbnailUpdateStart(list->count());
	thumbJob->start();
}

void SQ_FileThumbView::stopThumbnailUpdate()
{
	if(!thumbJob.isNull())
	{
		sqWStack->thumbnailsUpdateEnded();
		thumbJob->kill();
		delete (SQ_ThumbnailLoadJob*)thumbJob;
	}
}

void SQ_FileThumbView::slotThumbnailUpdateToggle()
{
	if(!thumbJob.isNull())
		stopThumbnailUpdate();
	else
		startThumbnailUpdate();
}

void SQ_FileThumbView::appendItems(const KFileItemList &items)
{
	if(thumbJob.isNull())
		doStartThumbnailUpdate(&items);
	else
		thumbJob->appendItems(items);
}
