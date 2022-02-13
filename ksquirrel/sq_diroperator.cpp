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
#include <qmessagebox.h>
#include <qlabel.h>
#include <qtimer.h>

#include <krun.h>
#include <kcombiview.h>
#include <kstringhandler.h>
#include <kprocess.h>
#include <kapp.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <klocale.h>

#include "ksquirrel.h"
#include "sq_treeview.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filedetailview.h"
#include "sq_glwidget.h"
#include "sq_glview.h"
#include "sq_externaltool.h"
#include "sq_libraryhandler.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_dirlister.h"
#include "sq_pixmapcache.h"
#include "sq_thumbnailsize.h"
#include "sq_pixmapcache.h"
#include "sq_quickbrowser.h"
#include "sq_archivehandler.h"

#define SQ_MAX_WORD_LENGTH 50

SQ_DirOperator::SQ_DirOperator(const KURL &url, VV type_, QWidget *parent, const char *name) :
	KDirOperator(url, parent, name)
{
	type = type_;

	static KActionSeparator *pASep = new KActionSeparator(actionCollection());

	setupMenu(KDirOperator::SortActions | KDirOperator::NavActions | KDirOperator::FileActions);

	if(!sqWStack->count())
	{
		pARunSeparately = new KAction(i18n("Run separately"), QIconSet(sqLoader->loadIcon("launch", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("launch", KIcon::Desktop, 22)), KShortcut(CTRL+Key_J), this, SLOT(slotRunSeparately()), sqApp->actionCollection(), "SQ Run files separately");
		pAShowEMenu = new KAction(i18n("Show 'External tools' menu"), 0, KShortcut(CTRL+Key_E), this, SLOT(slotShowExternalToolsMenu()), sqApp->actionCollection(), "SQ SETM");
	}
	else
	{
		pARunSeparately = sqApp->actionCollection()->action("SQ Run files separately");
		pAShowEMenu = sqApp->actionCollection()->action("SQ SETM");
	}

	// @todo get rid of kdecore warning
	pADirOperatorMenu = (KActionMenu*)actionCollection()->action("popupMenu");
	pADirOperatorMenu->insert(pARunSeparately, 0);
	pADirOperatorMenu->insert(pASep, 1);
	pADirOperatorMenu->insert(pASep);
	pADirOperatorMenu->popupMenu()->insertItem(i18n("External Tools"), sqExternalTool->getConstPopupMenu());

	if(!sqWStack->count())
		connect(sqExternalTool->getConstPopupMenu(), SIGNAL(activated(int)), SLOT(slotActivateExternalTool(int)));

	setDirLister(new SQ_DirLister);

	connect(this, SIGNAL(finishedLoading()), SLOT(slotFinishedLoading()));
	connect(this, SIGNAL(updateInformation(int,int)), SLOT(slotUpdateInformation(int,int)));
	connect(this, SIGNAL(urlEntered(const KURL&)), SLOT(slotUrlEntered(const KURL&)));
}

SQ_DirOperator::~SQ_DirOperator()
{}

KFileView* SQ_DirOperator::createView(QWidget *parent, KFile::FileView view)
{
	fileview = 0L;

	switch(type)
	{
		case  SQ_DirOperator::TypeDetail:
			dv = new SQ_FileDetailView(parent, "detail view");
			fileview = dv;
			reconnectClick();

			disconnect(dv, SIGNAL(returnPressed(QListViewItem*)), dv, 0);
			connect(dv, SIGNAL(returnPressed(QListViewItem*)), SLOT(slotReturnPressed(QListViewItem*)));
			connect(dv, SIGNAL(currentChanged(QListViewItem*)), SLOT(slotSelected(QListViewItem*)));
		break;
	
		case  SQ_DirOperator::TypeList:
		case  SQ_DirOperator::TypeIcon:
		{
			iv = new SQ_FileIconView(parent, "icon/list view");
			fileview = iv;
			reconnectClick();

			disconnect(iv, SIGNAL(returnPressed(QIconViewItem*)), iv, 0);
			connect(iv, SIGNAL(returnPressed(QIconViewItem*)), SLOT(slotReturnPressed(QIconViewItem*)));
			connect(iv, SIGNAL(currentChanged(QIconViewItem*)), SLOT(slotSelected(QIconViewItem*)));

			if(type == SQ_DirOperator::TypeIcon)
				iv->setArrangement(QIconView::LeftToRight);
		}
		break;
	
		case  SQ_DirOperator::TypeThumbs:
		{
			tv = new SQ_FileThumbView(parent, "thumb view");
			fileview = tv;

			reconnectClick();

			disconnect(tv, SIGNAL(returnPressed(QIconViewItem*)), tv, 0);
			connect(tv, SIGNAL(returnPressed(QIconViewItem*)), SLOT(slotReturnPressed(QIconViewItem*)));
			connect(tv, SIGNAL(currentChanged(QIconViewItem*)), SLOT(slotSelected(QIconViewItem*)));
			connect(dirLister(), SIGNAL(newItems(const KFileItemList&)), SLOT(slotNewItems(const KFileItemList&)));
			connect(dirLister(), SIGNAL(deleteItem(KFileItem*)), SLOT(slotDeletedItem(KFileItem*)));
			connect(dirLister(), SIGNAL(canceled()), tv, SLOT(stopThumbnailUpdate()));

			KAction *r = this->actionCollection()->action("reload");
			KShortcut t = r->shortcut();
			r->setShortcut(KShortcut());
			new KAction(QString::null, QIconSet(), t, tv, SLOT(startThumbnailUpdate()), actionCollection(), "SQ reload dir");
		}
		break;
	}

	return fileview;
}

void SQ_DirOperator::slotReturnPressed(QIconViewItem *item)
{
	if(!item) return;

	KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item);

	if(!f)
		return;

	KFileItem *fi = f->fileInfo();

	if(!fi)
		return;

	if(fi->isDir())
		setURL(fi->url(), true);
	else
		slotExecuted(item);
}

void SQ_DirOperator::slotReturnPressed(QListViewItem *item)
{
	if(!item) return;

	KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item);

	if(!f)
		return;

	KFileItem *fi = f->fileInfo();

	if(!fi)
		return;

	if(fi->isDir())
		setURL(fi->url(), true);
	else
		slotExecuted(item);
}

void SQ_DirOperator::slotExecuted(QIconViewItem *item)
{
	if(!item) return;

	KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item);

	if(!f)
		return;

	KFileItem *fi = f->fileInfo();

	if(!fi)
		return;

	QString fullpath = f->fileInfo()->url().path();
	QFileInfo fm(fullpath);

	if(!fi->isDir())
	{
		if(!sqLibHandler->supports(fm.extension(false)))
		{
			sqSBDecoded->setText(i18n("Format \"")+ fm.extension(false)+i18n("\" not supported"));

			if(SQ_ArchiveHandler::supported(fullpath))
				if(sqConfig->readBoolEntry("Fileview", "archives", true))
					emit tryUnpack(fullpath);
				else;
			else if(sqConfig->readBoolEntry("Fileview", "run unknown", true))
				pARunSeparately->activate();
		}
		else
			sqGLWidget->emitShowImage(fullpath);
	}
}

void SQ_DirOperator::slotExecuted(QListViewItem *item)
{
	if(!item) return;

	KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item);

	if(!f)
		return;

	KFileItem *fi = f->fileInfo();

	if(!fi)
		return;

	QString fullpath = f->fileInfo()->url().path();
	QFileInfo fm(fullpath);

	if(!fi->isDir())
	{
		if(!sqLibHandler->supports(fm.extension(false)))
		{
			sqSBDecoded->setText(i18n("Format \"")+ fm.extension(false)+i18n("\" not supported"));

			if(SQ_ArchiveHandler::supported(fullpath))
				if(sqConfig->readBoolEntry("Fileview", "archives", true))
					emit tryUnpack(fullpath);
				else;
			else if(sqConfig->readBoolEntry("Fileview", "run unknown", true))
				pARunSeparately->activate();
		}
		else
			sqGLWidget->emitShowImage(fullpath);
	}
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
		if(item)
			if(item->isFile())
				item->run();

		++it;
	}
}

void SQ_DirOperator::slotFinishedLoading()
{
	QTimer::singleShot(0, this, SLOT(slotDelayedFinishedLoading()));
}

void SQ_DirOperator::slotUpdateInformation(int files, int dirs)
{
	int total = dirs + files;

	QString str = QString(i18n(" Total %1 (%2 dirs, %3 files) ")).arg(total).arg(dirs).arg(files);
	sqSBdirInfo->setText(str);
}

void SQ_DirOperator::slotDelayedFinishedLoading()
{
	QString str;
	int dirs = numDirs(), files = numFiles(), total = dirs+files;

	str = QString(i18n(" Total %1 (%2 dirs, %3 files) ")).arg(total).arg(dirs).arg(files);
	sqSBdirInfo->setText(str);

	if(total)
	{
//		const KFileItemList *list = fileview->items();
//		setCurrentItem(list->getFirst());
	}
	else
	{
		sqSBfileIcon->clear();
		sqSBfileName->clear();
	}

	if(sqConfig->readBoolEntry("Fileview", "tofirst", true) && sqWStack->count() == 1)
	{
		const KFileItemList *list = fileview->items();
		sqWStack->slotNext(list->getFirst());
	}

//	if(type == SQ_DirOperator::TypeThumbs)
//		tv->startThumbnailUpdate();
}

void SQ_DirOperator::emitSelected(const QString &file)
{
	KURL url = file;
	KFileView *local = view();

	if(this->url() != url.directory())
		setURL(url.directory(), true);

	local->clearSelection();
	local->setCurrentItem(url.fileName());
	local->setSelected(local->currentFileItem(), true);

	sqGLWidget->emitShowImage(file);
}

void SQ_DirOperator::slotActivateExternalTool(int id)
{
	KFileItem *item = ((SQ_DirOperator*)sqWStack->visibleWidget())->view()->currentFileItem();

	if(!item) return;

	int index = sqExternalTool->getConstPopupMenu()->itemParameter(id);
	QString command = sqExternalTool->getToolCommand(index), final, file;
	KShellProcess	run;

	file = run.quote(item->url().path());
	final = command.replace(QRegExp("%s"), file);

	run << final;
	run.start(KProcess::DontCare);
}

void SQ_DirOperator::slotShowExternalToolsMenu()
{
	sqExternalTool->getConstPopupMenu()->exec(QCursor::pos());
}

void SQ_DirOperator::reconnectClick()
{
	switch(sqConfig->readNumEntry("Fileview", "click policy", 0))
	{
		case 0:
			sing = KGlobalSettings::singleClick();
		break;

		case 1:
			sing = true;
		break;

		case 2:
			sing = false;
		break;
	}

	switch(type)
	{
		case SQ_DirOperator::TypeIcon:
		case SQ_DirOperator::TypeList:
			if(sing)
			{
				disconnect(iv, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
				disconnect(iv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), iv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
				disconnect(iv, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
				connect(iv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), iv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
				connect(iv, SIGNAL(clicked(QIconViewItem*)), SLOT(slotExecuted(QIconViewItem*)));
			}
			else
			{
				disconnect(iv, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
				disconnect(iv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), iv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
				disconnect(iv, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
				connect(iv, SIGNAL(doubleClicked(QIconViewItem*)), SLOT(slotExecuted(QIconViewItem*)));
			}
		break;

		case SQ_DirOperator::TypeThumbs:
			if(sing)
			{
				disconnect(tv, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
				disconnect(tv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), tv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
				disconnect(tv, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
				connect(tv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), tv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
				connect(tv, SIGNAL(clicked(QIconViewItem*)), SLOT(slotExecuted(QIconViewItem*)));
			}
			else
			{
				disconnect(tv, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
				disconnect(tv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), tv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
				disconnect(tv, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
				connect(tv, SIGNAL(doubleClicked(QIconViewItem*)), SLOT(slotExecuted(QIconViewItem*)));
			}
		break;

		case SQ_DirOperator::TypeDetail:
			if(sing)
			{
				disconnect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
				disconnect(dv, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));
				disconnect(dv, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));
				connect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
				connect(dv, SIGNAL(clicked(QListViewItem*)), SLOT(slotExecuted(QListViewItem*)));
			}
			else
			{
				disconnect(dv, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));
				disconnect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
				disconnect(dv, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));
				connect(dv, SIGNAL(doubleClicked(QListViewItem*)), SLOT(slotExecuted(QListViewItem*)));
			}
		break;
	}
}

void SQ_DirOperator::setCurrentItem(KFileItem *item)
{
	fileview->clearSelection();
	fileview->setCurrentItem(item);
	fileview->setSelected(item, true);

	if(isVisible())
		fileview->ensureItemVisible(item);
}

void SQ_DirOperator::slotUrlEntered(const KURL &url)
{
	if(sqGLView)
		if(this == sqQuickOperator)
			sqWStack->setURLForCurrent(url);
		else
			sqWStack->setURL(url);
	else
		sqWStack->setURL(url);
}

void SQ_DirOperator::slotNewItems(const KFileItemList &items)
{
	if(type == SQ_DirOperator::TypeThumbs)
		tv->appendItems(items);
}

void SQ_DirOperator::slotDeletedItem(KFileItem *item)
{
	if(!item)
		return;

	sqCache->remove(item->url().path());
}

bool SQ_DirOperator::isThumbView() const
{
	return (type == SQ_DirOperator::TypeThumbs);
}

void SQ_DirOperator::slotSetThumbSize(const QString &size)
{
	sqThumbSize->setPixelSize(size);
	tv->setIconSize(sqThumbSize->pixelSize());
	tv->setGridX(sqThumbSize->pixelSize() + sqConfig->readNumEntry("Thumbnails", "margin", 2));
//	rereadDir();
	tv->startThumbnailUpdate();
}
