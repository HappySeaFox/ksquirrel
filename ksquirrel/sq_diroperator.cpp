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
#include <qprocess.h>
#include <qevent.h>
#include <qlabel.h>

#include <krun.h>
#include <kcombiview.h>
#include <kstringhandler.h>
#include <kprocess.h>
#include <kapp.h>
#include <kpopupmenu.h>

#include "ksquirrel.h"
#include "sq_treeview.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_filedetailview.h"
#include "sq_glviewwidget.h"
#include "sq_glviewgeneral.h"
#include "sq_glviewspec.h"
#include "sq_externaltool.h"
#include "sq_libraryhandler.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_dirlister.h"
#include "sq_bookmarkowner.h"

#define SQ_MAX_WORD_LENGTH 50

SQ_DirOperator::SQ_DirOperator(const KURL &url, QWidget *parent, const char *name) : KDirOperator(url, parent, name)
{
	setDirLister(new SQ_DirLister);

	static KActionSeparator *pASep = new KActionSeparator(actionCollection());

	if(sqConfig->readBoolEntry("Fileview", "click policy system", true))
		sing = KGlobalSettings::singleClick();
	else
		sing = (bool)1 - (bool)sqConfig->readNumEntry("Fileview", "click policy custom", 0);

	setupMenu(KDirOperator::SortActions | KDirOperator::NavActions | KDirOperator::FileActions);

	if(!sqWStack->count())
	{
		pARunSeparately = new KAction("Run separately", QIconSet(sqLoader->loadIcon("launch", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("launch", KIcon::Desktop, 22)), KShortcut(CTRL+Key_J), this, SLOT(slotRunSeparately()), sqApp->actionCollection(), "SQ Run files separately");
		pAShowEMenu = new KAction("Show 'External tools' menu", 0, KShortcut(CTRL+Key_E), this, SLOT(slotShowExternalToolsMenu()), sqApp->actionCollection(), "SQ SETM");
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

	if(!sqWStack->count())
	{
		toolsId = pADirOperatorMenu->popupMenu()->insertItem("External Tools", sqExternalTool->getNewPopupMenu());
		connect(sqExternalTool->getConstPopupMenu(), SIGNAL(activated(int)), SLOT(slotActivateExternalTool(int)));
	}
	else
		pADirOperatorMenu->popupMenu()->insertItem("External Tools", sqExternalTool->getConstPopupMenu());
	
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

		if(sing)
		{
			connect((SQ_FileDetailView*)fileview, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), (SQ_FileDetailView*)fileview, SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
			connect((SQ_FileDetailView*)fileview, SIGNAL(clicked(QListViewItem*)), SLOT(slotDoubleClicked(QListViewItem*)));
		}
		else
			connect((SQ_FileDetailView*)fileview, SIGNAL(doubleClicked(QListViewItem*)), SLOT(slotDoubleClicked(QListViewItem*)));

//		disconnect((SQ_FileDetailView*)fileview, SIGNAL(returnPressed(QListViewItem*)), (SQ_FileDetailView*)fileview, SIGNAL(clicked(QListViewItem*)));
		connect((SQ_FileDetailView*)fileview, SIGNAL(returnPressed(QListViewItem*)), SLOT(slotDoubleClicked(QListViewItem*)));
		connect((SQ_FileDetailView*)fileview, SIGNAL(currentChanged(QListViewItem*)), SLOT(slotSelected(QListViewItem*)));
	}
	else if(view == KFile::Simple)
	{		
		SQ_FileIconView *tt = new SQ_FileIconView(parent, "simple view");
		fileview = tt;
	
		fileview->setViewName("Short View");

		if(sing)
		{
			connect((SQ_FileIconView*)fileview, SIGNAL(clicked(QIconViewItem*, const QPoint&)), (SQ_FileIconView*)fileview, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
			connect((SQ_FileIconView*)fileview, SIGNAL(clicked(QIconViewItem*)), SLOT(slotDoubleClicked(QIconViewItem*)));
		}
		else
			connect((SQ_FileIconView*)fileview, SIGNAL(doubleClicked(QIconViewItem*)), SLOT(slotDoubleClicked(QIconViewItem*)));

//		disconnect((SQ_FileIconView*)fileview, SIGNAL(returnPressed(QIconViewItem*)), (SQ_FileIconView*)fileview, SIGNAL(clicked(QIconViewItem*)));
		connect((SQ_FileIconView*)fileview, SIGNAL(returnPressed(QIconViewItem*)), SLOT(slotDoubleClicked(QIconViewItem*)));
		connect((SQ_FileIconView*)fileview, SIGNAL(currentChanged(QIconViewItem*)), SLOT(slotSelected(QIconViewItem*)));
	}

	return fileview;
}

void SQ_DirOperator::slotDoubleClicked(QIconViewItem *item)
{
	if(!item) return;

	if(KFileIconViewItem* f = (KFileIconViewItem*)item)
	{
		QFileInfo fm(f->fileInfo()->url().path());

		if(f->fileInfo()->isFile())
		{
			if(!sqLibHandler->supports(fm.extension(false)))
			{
				sqSBDecoded->setText("Format \""+ fm.extension(false)+"\" not supported");

				if(sqConfig->readBoolEntry("Fileview", "run unknown", true))
					pARunSeparately->activate();
			}
			else
				sqGLView->getGL()->emitShowImage(f->fileInfo()->url().path());
		}
		else
			emit dirActivated((const KFileItem*)f->fileInfo());
	}
}

void SQ_DirOperator::slotDoubleClicked(QListViewItem *item)
{
	if(!item) return;

	if(KFileListViewItem* f = (KFileListViewItem*)item)
	{
		QFileInfo fm(f->fileInfo()->url().path());

		if(f->fileInfo()->isFile())
		{
			if(!sqLibHandler->supports(fm.extension(false)))
			{
				sqSBDecoded->setText("Format \""+ fm.extension(false)+"\" not supported");

				if(sqConfig->readBoolEntry("Fileview", "run unknown", true))
					pARunSeparately->activate();
			}
			else
				sqGLView->getGL()->emitShowImage(f->fileInfo()->url().path());
		}
		else
			emit dirActivated((const KFileItem*)f->fileInfo());
	}
}

void SQ_DirOperator::slotSelected(QIconViewItem *item)
{
	if(!item) return;

	static QString str;
	static QPixmap px;
	static KFileItem *fi;

	if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
	{
		fi = f->fileInfo();
		if(fi->isFile() || fi->isDir())
		{
			str = " " + fi->timeString() + " ";
			px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, KIcon::SizeSmall);
			sqSBcurFileInfo->setText(str);
			sqSBfileIcon->setPixmap(px);
			str = "  " + fi->text() + ((fi->isDir())?"":(" (" + KIO::convertSize(fi->size()) + ")"));
			sqSBfileName->setText(KStringHandler::csqueeze(str, SQ_MAX_WORD_LENGTH));
		}
	}
}

void SQ_DirOperator::slotSelected(QListViewItem *item)
{
	if(!item) return;

	static QString str;
	static QPixmap px;
	static KFileItem *fi;

	if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
	{
		fi = f->fileInfo();
		if(fi->isFile() || fi->isDir())
		{
			str = " " + fi->timeString() + " ";
			px = KMimeType::pixmapForURL(fi->url(), 0, KIcon::Desktop, KIcon::SizeSmall);
			sqSBcurFileInfo->setText(str);
			sqSBfileIcon->setPixmap(px);
			str = "  " + fi->text() + ((fi->isDir())?"":(" (" + KIO::convertSize(fi->size()) + ")"));
			sqSBfileName->setText(KStringHandler::csqueeze(str, SQ_MAX_WORD_LENGTH));
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

	view()->clearSelection();

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

	sqBookmarks->setURL(url());
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

	sqGLView->getGL()->emitShowImage(file);
}

void SQ_DirOperator::emitNextSelected()
{
	KFileItem *item;
	KFileView *local = view();
	QString name;

	item = local->nextItem(local->currentFileItem());
	if(!item) return;

	while(1)
	{
		if(item->isFile())
		{
			name = item->url().path();
			QFileInfo fm(name);

			if(sqLibHandler->supports(fm.extension(false)))
				break;
		}

		item = local->nextItem(item);
		if(!item) return;
	}

	local->clearSelection();
	local->setCurrentItem(item);
	local->setSelected(local->currentFileItem(), true);

	sqGLView->getGL()->emitShowImage(item->url());
}

void SQ_DirOperator::emitPreviousSelected()
{
	KFileItem *item;
	KFileView *local = view();
	QString name;

	item = local->prevItem(local->currentFileItem());
	if(!item) return;

	while(1)
	{
		if(item->isFile())
		{
			name = item->url().path();
			QFileInfo fm(name);

			if(sqLibHandler->supports(fm.extension(false)))
				break;
		}

		item = local->prevItem(item);
		if(!item) return;
	}

	local->clearSelection();
	local->setCurrentItem(item);
	local->setSelected(local->currentFileItem(), true);

	sqGLView->getGL()->emitShowImage(item->url());
}

void SQ_DirOperator::slotActivateExternalTool(int id)
{
	// we have only one 'External tools' menu, duplicated from first created diroperator to
	// all other, so we need to know wich diroperator is currently active (visible).
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

//@warning: must be called from sqWStack
void SQ_DirOperator::reInitToolsMenu()
{
	disconnect(sqExternalTool->getConstPopupMenu(), SIGNAL(activated(int)), this, SLOT(slotActivateExternalTool(int)));

	pADirOperatorMenu->popupMenu()->removeItem(toolsId);
	toolsId = pADirOperatorMenu->popupMenu()->insertItem("External Tools", sqExternalTool->getConstPopupMenu());

	connect(sqExternalTool->getConstPopupMenu(), SIGNAL(activated(int)), SLOT(slotActivateExternalTool(int)));
}

void SQ_DirOperator::slotShowExternalToolsMenu()
{
	sqExternalTool->getConstPopupMenu()->exec(QCursor::pos());
}
