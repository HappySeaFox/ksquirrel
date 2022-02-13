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
	QTimer::singleShot(0, this, SLOT(slotDelayedFinishedLoading()));
}

void SQ_DirOperator::slotUpdateInformation(int files, int dirs)
{
	int total = dirs + files;

	QString str = QString(i18n(" Total %1 (%2, %3) "))
			.arg(total)
			.arg(i18n("1 directory", "%n dirs", dirs))
			.arg(i18n("1 file", "%n files", files));
	sqSBdirInfo->setText(str);
}

void SQ_DirOperator::slotDelayedFinishedLoading()
{
	QString str;
	int dirs = numDirs(), files = numFiles(), total = dirs+files;

	str = QString(i18n(" Total %1 (%2, %3) "))
			.arg(total)
			.arg(i18n("1 directory", "%n dirs", dirs))
			.arg(i18n("1 file", "%n files", files));

	sqSBdirInfo->setText(str);

	if(!total)
	{
		sqSBfileIcon->clear();
		sqSBfileName->clear();
	}

	if(sqWStack->count() == 1)
	{
		KFileItemList *list = (KFileItemList *)fileview->items();
		KFileItem *first = list->first();

		if(sqConfig->readBoolEntry("Fileview", "tofirst", true) && first)
			sqWStack->moveTo(SQ_WidgetStack::Next, first);
		else
			setCurrentItem(first);

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
		} // switch
	} // if

	if(type == SQ_DirOperator::TypeThumbs)
	{
		if(tv->isVisible())
			tv->startThumbnailUpdate();
		else
			tv->waitForShowEvent();
	}
}

void SQ_DirOperator::slotDeletedItem(KFileItem *item)
{
	if(!item)
		return;

	sqCache->remove(item->url().path());
}

void SQ_DirOperator::slotSetThumbSize(const QString &size)
{
	sqThumbSize->setPixelSize(size);

	int pixelSize = (sqThumbSize->isExtended())?
			sqThumbSize->extendedPixelSize():sqThumbSize->pixelSize();

	int newgrid = pixelSize + sqConfig->readNumEntry("Thumbnails", "margin", 2) + 2;

	tv->setGridX(newgrid);

	rereadDir();
}

void SQ_DirOperator::slotSelected(QIconViewItem *item)
{
        if(!item) return;

        QString str;
        QPixmap px;
        KFileItem *fi;

        if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
        {
                fi = f->fileInfo();
                px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, KIcon::SizeSmall);
                sqSBfileIcon->setPixmap(px);
                str = QString("  %1 %2").arg(fi->text()).arg((fi->isDir())?"":QString(" (" + KIO::convertSize(fi->size()) + ")"));
                sqSBfileName->setText(KStringHandler::csqueeze(str, SQ_MAX_WORD_LENGTH));
                sqWStack->selectFile(fi, this);
        }
}

void SQ_DirOperator::slotSelected(QListViewItem *item)
{
        if(!item) return;

        QString str;
        QPixmap px;
        KFileItem *fi;

        if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
        {
                fi = f->fileInfo();
                px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, KIcon::SizeSmall);
                sqSBfileIcon->setPixmap(px);
                str = QString("  %1 %2").arg(fi->text()).arg((fi->isDir())?"":QString(" (" + KIO::convertSize(fi->size()) + ")"));
                sqSBfileName->setText(KStringHandler::csqueeze(str, SQ_MAX_WORD_LENGTH));
                sqWStack->selectFile(fi, this);
        }
}

void SQ_DirOperator::slotUrlEntered(const KURL &url)
{
	sqWStack->setURL(url, true);
}

void SQ_DirOperator::setDirLister(KDirLister *lister)
{
	KDirOperator::setDirLister(lister);
}
