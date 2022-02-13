/***************************************************************************
                          sq_diroperator.cpp  -  description
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
#include <qtimer.h>
#include <qlabel.h>
#include <qapplication.h>

#include <kstringhandler.h>
#include <klocale.h>
#include <kprogress.h>

#include "ksquirrel.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_pixmapcache.h"
#include "sq_thumbnailsize.h"

#define SQ_MAX_WORD_LENGTH 50

SQ_DirOperator::SQ_DirOperator(const KURL &url, VV type_, QWidget *parent, const char *name) :
	SQ_DirOperatorBase(url, type_, parent, name)
{
	connect(this, SIGNAL(finishedLoading()), this, SLOT(slotFinishedLoading()));
	connect(this, SIGNAL(updateInformation(int,int)), SLOT(slotUpdateInformation(int,int)));
}

SQ_DirOperator::~SQ_DirOperator()
{}

void SQ_DirOperator::slotFinishedLoading()
{
	int dirs = numDirs(), files = numFiles(), total = dirs+files;

	slotUpdateInformation(files, dirs);

	if(!total)
	{
		KSquirrel::app()->sbarWidget("fileIcon")->clear();
		KSquirrel::app()->sbarWidget("fileName")->clear();
		return;
	}

	QTimer::singleShot(10, this, SLOT(slotDelayedFinishedLoading()));

	if(type == SQ_DirOperator::TypeThumbs)
	{
		if(tv->isVisible())
			tv->startThumbnailUpdate();
		else
			tv->waitForShowEvent();
	}

	qApp->processEvents();
}

void SQ_DirOperator::slotUpdateInformation(int files, int dirs)
{
	int total = dirs + files;

	QString str = QString(i18n(" Total %1 (%2, %3) "))
			.arg(total)
			.arg(i18n("1 directory", "%n dirs", dirs))
			.arg(i18n("1 file", "%n files", files));
	KSquirrel::app()->sbarWidget("dirInfo")->setText(str);
}

void SQ_DirOperator::slotDelayedFinishedLoading()
{
	if(SQ_WidgetStack::instance()->count() && SQ_WidgetStack::instance()->visibleWidget() == this)
	{
		KFileItemList list = dirLister()->items();
		KFileItem *first = list.first();

		if(!first) return;

		if(SQ_Config::instance()->readBoolEntry("Fileview", "tofirst", true))
			SQ_WidgetStack::instance()->moveTo(SQ_WidgetStack::Next, first);
		else
			setCurrentItem(first);
/*
		switch(type)
		{
			case SQ_DirOperator::TypeDetail:
				slotSelected(dv->currentItem());
			break;

			case  SQ_DirOperatorBase::TypeList:
			case  SQ_DirOperatorBase::TypeIcon:
				slotSelected(iv->currentItem());
			break;

			case SQ_DirOperator::TypeThumbs:
				slotSelected(tv->currentItem());
			break;
		}*/
	}
}

void SQ_DirOperator::slotDeletedItem(KFileItem *item)
{
	if(!item)
		return;

	SQ_PixmapCache::instance()->remove(item->url().path());
}

void SQ_DirOperator::slotSetThumbSize(const QString &size)
{
	SQ_ThumbnailSize::instance()->setPixelSize(size);

	int pixelSize = (SQ_ThumbnailSize::instance()->extended())?
			SQ_ThumbnailSize::instance()->extendedPixelSize():SQ_ThumbnailSize::instance()->pixelSize();

	int newgrid = pixelSize + SQ_Config::instance()->readNumEntry("Thumbnails", "margin", 2) + 2;

	tv->setGridX(newgrid);

	rereadDir();
}

void SQ_DirOperator::slotSelected(QIconViewItem *item)
{
	if(!item) return;

	QString str;
	QPixmap px;
	
	KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item);

	if(f)
	{
		KFileItem *fi = f->fileInfo();
		statusFile(fi);
	}
}

void SQ_DirOperator::slotSelected(QListViewItem *item)
{
	if(!item) return;

	KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item);

	if(f)
	{
		KFileItem *fi = f->fileInfo();
		statusFile(fi);
	}
}

void SQ_DirOperator::statusFile(KFileItem *fi)
{
	QString str;
	QPixmap px;

	px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, KIcon::SizeSmall);
	KSquirrel::app()->sbarWidget("fileIcon")->setPixmap(px);
	str = QString("  %1 %2").arg(fi->text()).arg((fi->isDir())?"":QString(" (" + KIO::convertSize(fi->size()) + ")"));
	KSquirrel::app()->sbarWidget("fileName")->setText(KStringHandler::csqueeze(str, SQ_MAX_WORD_LENGTH));
	SQ_WidgetStack::instance()->selectFile(fi, this);
}

void SQ_DirOperator::setDirLister(KDirLister *lister)
{
	KDirOperator::setDirLister(lister);
}
