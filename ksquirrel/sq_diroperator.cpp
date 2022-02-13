/***************************************************************************
                          sq_diroperator.cpp  -  description
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

#include "sq_diroperator.h"

#include <krun.h>
#include <kcombiview.h>

#include "sq_fileiconview.h"
#include "sq_filedetailview.h"

#include "ksquirrel.h"
#include "sq_glviewwidget.h"
#include "sq_libraryhandler.h"

#include <qmessagebox.h>

SQ_DirOperator::SQ_DirOperator(const KURL &url, QWidget *parent, const char *name) : KDirOperator(url, parent, name)
{
	sing = KGlobalSettings::singleClick();

	pARunSeparately = new KAction("Run separately", QIconSet(sqLoader->loadIcon("launch", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("launch", KIcon::Desktop, 22)), KShortcut(CTRL+Key_J), this, SLOT(slotRunSeparately()), sqApp->actionCollection(), "Run separately");

	// @todo get rid of kdecore warning
	pADirOperatorMenu = (KActionMenu*)actionCollection()->action("popupMenu");
	pADirOperatorMenu->insert(pARunSeparately, 0);
	pADirOperatorMenu->insert(new KActionSeparator(actionCollection()), 1);

	connect(this, SIGNAL(finishedLoading()), SLOT(slotFinishedLoading()));
}

SQ_DirOperator::~SQ_DirOperator()
{}

KFile::FileView SQ_DirOperator::getViewType()
{
	return View;
}

KFileView* SQ_DirOperator::createView(QWidget *parent, KFile::FileView view)
{
	fileview = 0L;
	View = view;

	bool separateDirs = KFile::isSeparateDirs(view);

	if(separateDirs)
	{
		KCombiView *combi = new KCombiView(parent, "combi view");
		combi->setOnlyDoubleClickSelectsFiles(onlyDoubleClickSelectsFiles());

		KFileView *v = 0L;

		if((view & KFile::Simple) == KFile::Simple)
			v = createView(combi, KFile::Simple);
		else
			v = createView(combi, KFile::Detail);
			
		combi->setRight(v);
		fileview = combi;
	}
	else if(view == KFile::Detail)
	{
		fileview = new SQ_FileDetailView(parent, "detail view");

		connect((SQ_FileDetailView*)fileview, SIGNAL(doubleClicked(QListViewItem*)), SLOT(slotDoubleClicked(QListViewItem*)));
		connect((SQ_FileDetailView*)fileview, SIGNAL(currentChanged(QListViewItem*)), SLOT(slotSelected(QListViewItem*)));
	}
	else if(view == KFile::Simple)
	{		
		SQ_FileIconView *tt = new SQ_FileIconView(parent, "simple view");
		fileview = tt;
	
		fileview->setViewName("Short View");

		connect((SQ_FileIconView*)fileview, SIGNAL(doubleClicked(QIconViewItem*)), SLOT(slotDoubleClicked(QIconViewItem*)));
		connect((SQ_FileIconView*)fileview, SIGNAL(currentChanged(QIconViewItem*)), SLOT(slotSelected(QIconViewItem*)));
	}

	return fileview;
}

void SQ_DirOperator::slotDoubleClicked(QIconViewItem *item)
{
	if(!item) return;

	if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
	{
		QFileInfo fm(f->fileInfo()->url().path());

		if(f->fileInfo()->isFile())
			if(sqLibHandler->supports(fm.extension(false)))
				sqGLView->showIfCan(f->fileInfo()->url().path());
	}
}

void SQ_DirOperator::slotDoubleClicked(QListViewItem *item)
{
	if(!item) return;

	if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
	{
		QFileInfo fm(f->fileInfo()->url().path());

		if(f->fileInfo()->isFile())
			if(!sqLibHandler->supports(fm.extension(false)))
					sqGLView->showIfCan(f->fileInfo()->url().path());
	}
}

void SQ_DirOperator::slotSelected(QIconViewItem *item)
{
	if(!item) return;
	
	if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
	{
		KFileItem *fi = f->fileInfo();
		if(fi->isFile() || fi->isDir())
		{
			QString str2 = " " + fi->timeString() + " ";
			QPixmap px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, KIcon::SizeSmall);
			sqSBcurFileInfo->setText(str2);
			sqSBfileIcon->setPixmap(px);
			sqSBfileName->setText("  " + fi->text() + ((fi->isDir())?"":(" (" + KIO::convertSize(fi->size()) + ")")));
		}

		if(f->fileInfo()->isFile() && (sqViewType == Squirrel::Gqview || sqViewType == Squirrel::Xnview || sqViewType == Squirrel::WinViewer))
		{
			QFileInfo fm(f->fileInfo()->url().path());
			sqGLView->showIfCan(f->fileInfo()->url().path());
		}
	}
}

void SQ_DirOperator::slotSelected(QListViewItem *item)
{
	if(!item) return;

	if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
	{
		KFileItem *fi = f->fileInfo();
		if(fi->isFile() || fi->isDir())
		{
			QString str2 = " " + fi->timeString() + " ";
			QPixmap px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, KIcon::SizeSmall);
			sqSBcurFileInfo->setText(str2);
			sqSBfileIcon->setPixmap(px);
			sqSBfileName->setText("  " + fi->text() + ((fi->isDir())?"":(" (" + KIO::convertSize(fi->size()) + ")")));
		}

		if(f->fileInfo()->isFile() && (sqViewType == Squirrel::Gqview || sqViewType == Squirrel::Xnview || sqViewType == Squirrel::WinViewer))
		{
			QFileInfo fm(f->fileInfo()->url().path());
			sqGLView->showIfCan(f->fileInfo()->url().path());
		}
	}
}

void SQ_DirOperator::setShowHiddenFilesF(bool s)
{
	this->setShowHiddenFiles(s);
}

void SQ_DirOperator::setIconSize(int sz)
{
	((SQ_FileIconView*)fileview)->setIconSize(sz);
}

void SQ_DirOperator::slotRunSeparately()
{
	const KFileItemList *selected = selectedItems();
	KFileItemListIterator it(*selected);

	KFileItem *item;

	while((item = it.current()) != 0)
	{
		++it;
		if(item->isFile() && item)
			item->run();
	}
}

void SQ_DirOperator::slotFinishedLoading()
{
	QString str;
	int dirs = numDirs(), files = numFiles(), total = dirs+files;

	str.sprintf(" Total %d (%d dirs, %d files) ", total, dirs, files);
	sqSBdirInfo->setText(str);

	if(total)
	{
		const KFileItemList *list = view()->items();
		setCurrentItem(list->getFirst()->name());
	}
	else
	{
		sqSBcurFileInfo->setText("");
		sqSBfileIcon->setPixmap(QPixmap(0));
		sqSBfileName->setText("");
	}
}
