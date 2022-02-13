/***************************************************************************
                          sq_diroperatorbase.cpp  -  description
                             -------------------
    begin                : Sep 8 2004
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qlabel.h>
#include <qtimer.h>

#include <krun.h>
#include <kapp.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kstdaccel.h>
#include <krun.h>
#include <kservice.h>
#include <kdesktopfile.h>

#include "ksquirrel.h"
#include "sq_treeview.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filedetailview.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_externaltool.h"
#include "sq_libraryhandler.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_archivehandler.h"
#include "sq_converter.h"
#include "sq_resizer.h"

#define SQ_MAX_WORD_LENGTH 50

static const int timer_value = 10;

SQ_DirOperatorBase::SQ_DirOperatorBase(const KURL &url, VV type_, QWidget *parent, const char *name) :
	KDirOperator(url, parent, name)
{
	type = type_;

	// create and insert new actions in context menu
	setupActions();

	if(!SQ_WidgetStack::instance()->count())
		connect(SQ_ExternalTool::instance()->constPopupMenu(), SIGNAL(activated(int)), SLOT(slotActivateExternalTool(int)));

	connect(this, SIGNAL(urlEntered(const KURL&)), this, SLOT(slotUrlEntered(const KURL&)));

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotDelayedDecode()));
}

SQ_DirOperatorBase::~SQ_DirOperatorBase()
{}

KFileView* SQ_DirOperatorBase::createView(QWidget *parent, KFile::FileView)
{
	fileview = NULL;

	switch(type)
	{
		case  SQ_DirOperatorBase::TypeDetail:
			dv = new SQ_FileDetailView(parent, "detail view");

			Q_CHECK_PTR(dv);
			
			fileview = dv;
			reconnectClick(true);

			disconnect(dv, SIGNAL(returnPressed(QListViewItem*)), dv, 0);
			connect(dv, SIGNAL(returnPressed(QListViewItem*)), SLOT(slotReturnPressed(QListViewItem*)));
			connect(dv, SIGNAL(currentChanged(QListViewItem*)), SLOT(slotSelected(QListViewItem*)));
		break;

		case  SQ_DirOperatorBase::TypeList:
		case  SQ_DirOperatorBase::TypeIcon:
		{
			iv = new SQ_FileIconView(parent, (type == SQ_DirOperatorBase::TypeIcon) ? "icon view":"list view");

			Q_CHECK_PTR(iv);

			fileview = iv;
			reconnectClick(true);

			disconnect(iv, SIGNAL(returnPressed(QIconViewItem*)), iv, 0);
			connect(iv, SIGNAL(returnPressed(QIconViewItem*)), SLOT(slotReturnPressed(QIconViewItem*)));
			connect(iv, SIGNAL(currentChanged(QIconViewItem*)), SLOT(slotSelected(QIconViewItem*)));

//			if(type == SQ_DirOperatorBase::TypeIcon)
//				iv->setArrangement(QIconView::LeftToRight);
		}
		break;

		case  SQ_DirOperatorBase::TypeThumbs:
		{
			tv = new SQ_FileThumbView(parent, "thumb view");

			Q_CHECK_PTR(tv);

			fileview = tv;

			reconnectClick(true);

			disconnect(tv, SIGNAL(returnPressed(QIconViewItem*)), tv, 0);
			connect(tv, SIGNAL(returnPressed(QIconViewItem*)), SLOT(slotReturnPressed(QIconViewItem*)));
//			connect(dirLister(), SIGNAL(newItems(const KFileItemList&)), SLOT(slotNewItems(const KFileItemList&)));
			connect(dirLister(), SIGNAL(deleteItem(KFileItem*)), SLOT(slotDeletedItem(KFileItem*)));
			connect(dirLister(), SIGNAL(canceled()), tv, SLOT(stopThumbnailUpdate()));
			connect(tv, SIGNAL(currentChanged(QIconViewItem*)), SLOT(slotSelected(QIconViewItem*)));

//			new KAction(i18n("Reload thumbnails"), "reload", KShortcut(Key_F5), tv, SLOT(startThumbnailUpdate()), actionCollection(), "SQ reload dir");
		}
		break;
	}

	return fileview;
}

void SQ_DirOperatorBase::slotReturnPressed(QIconViewItem *item)
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

void SQ_DirOperatorBase::slotReturnPressed(QListViewItem *item)
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

void SQ_DirOperatorBase::slotExecuted(QIconViewItem *item)
{
	if(!item) return;

	KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item);

	if(!f)
		return;

	KFileItem *fi = f->fileInfo();

	if(!fi)
		return;

	QString fullpath = fi->url().path();
	QFileInfo fm(fullpath);

	// if selected item points to file, not directory
	if(!fi->isDir())
	{
		tobeDecoded = fi->url().path();

		// it is supported image type, let's decode and show it
		if(SQ_LibraryHandler::instance()->supports(tobeDecoded))
			timer->start(timer_value, true);
		else
		{
			SQ_GLView::window()->sbarWidget("SBDecoded")->setText(i18n("Format \"%1\" not supported").arg(fm.extension(false)));

			// if it is supported archive file
			if(SQ_ArchiveHandler::instance()->findProtocolByFile(fi) != -1)
				if(SQ_Config::instance()->readBoolEntry("Fileview", "archives", true))
					emit tryUnpack(fi);
				else;
			// run with default application, if needed
			else if(SQ_Config::instance()->readBoolEntry("Fileview", "run unknown", true))
				pARunSeparately->activate();
		}
	}
}

void SQ_DirOperatorBase::slotExecuted(QListViewItem *item)
{
	if(!item) return;

	KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item);

	if(!f)
		return;

	KFileItem *fi = f->fileInfo();

	if(!fi)
		return;

	QString fullpath = fi->url().path();
	QFileInfo fm(fullpath);

	if(!fi->isDir())
	{
		tobeDecoded = fi->url().path();

		if(SQ_LibraryHandler::instance()->supports(tobeDecoded))
			timer->start(timer_value, true);
		else
		{
			SQ_GLView::window()->sbarWidget("SBDecoded")->setText(i18n("Format \"%1\" not supported").arg(fm.extension(false)));

			if(SQ_ArchiveHandler::instance()->findProtocolByFile(fi) != -1)
				if(SQ_Config::instance()->readBoolEntry("Fileview", "archives", true))
					emit tryUnpack(fi);
				else;
			else if(SQ_Config::instance()->readBoolEntry("Fileview", "run unknown", true))
				pARunSeparately->activate();
		}
	}
}

/*
 *  Invoked, when user selected some external tool in menu.
 */
void SQ_DirOperatorBase::slotActivateExternalTool(int id)
{
	// get currently selected items
	KFileItemList *items = (KFileItemList *)SQ_WidgetStack::instance()->selectedItems();
	KURL::List list;

	if(!items) return;
	if(items->isEmpty()) return;

	int index = SQ_ExternalTool::instance()->constPopupMenu()->itemParameter(id);

	KFileItem *f = items->first();

	while(f != NULL)
	{
		list.append(f->url().path());
		f = items->next();
	}

	// get appropriate desktop file
	KDesktopFile *kd = SQ_ExternalTool::instance()->at(index);

	// create KService instance
	KService ks(kd);

	// create argument for KService. If multiple files
	// allowed, return entire list, and first item otherwise.
	QStringList args = (ks.allowMultipleFiles()) ?
								KRun::processDesktopExec(ks, list, true) :
								KRun::processDesktopExec(ks, list.first(), true);

	// finally, open items with selected application
	KRun::runCommand(args.join(" "), ks.name(), ks.icon());
}

/*
 *  SQ_DirOperatorBase can activate items by single or double click.
 *  This method will connect appropriate signals.
 */
void SQ_DirOperatorBase::reconnectClick(bool firstconnect)
{
	bool old_sing = sing;

	// determine clicking policy
	switch(SQ_Config::instance()->readNumEntry("Fileview", "click policy", 0))
	{
		case 0: sing = KGlobalSettings::singleClick(); break;
		case 1: sing = true; break;
		case 2: sing = false; break;

		default:
			sing = true;
	}

	if(!firstconnect)
		if(sing == old_sing)
			return;

	// finally, connect signals
	switch(type)
	{
		case SQ_DirOperatorBase::TypeIcon:
		case SQ_DirOperatorBase::TypeList:
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

		case SQ_DirOperatorBase::TypeThumbs:
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

		case SQ_DirOperatorBase::TypeDetail:
			if(sing)
			{
				disconnect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, 0);
				disconnect(dv, SIGNAL(clicked(QListViewItem*)), this, 0);
				disconnect(dv, SIGNAL(doubleClicked(QListViewItem*)), this, 0);
				connect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)));
				connect(dv, SIGNAL(clicked(QListViewItem*)), SLOT(slotExecuted(QListViewItem*)));
			}
			else
			{
				disconnect(dv, SIGNAL(doubleClicked(QListViewItem*)), this, 0);
				disconnect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, 0);
				disconnect(dv, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));
				connect(dv, SIGNAL(doubleClicked(QListViewItem*)), SLOT(slotExecuted(QListViewItem*)));
			}
		break;
	}
}

/*
 *  Deselect all items, set current item, select this item,
 *  and ensure it visible.
 */
void SQ_DirOperatorBase::setCurrentItem(KFileItem *item)
{
	// clear selection
	fileview->clearSelection();

	// set current item and select it
	fileview->setCurrentItem(item);
	fileview->setSelected(item, true);

	// make this item visible
	fileview->ensureItemVisible(item);
}

void SQ_DirOperatorBase::slotUrlEntered(const KURL &url)
{
	SQ_WidgetStack::instance()->setURLForCurrent(url);
}

bool SQ_DirOperatorBase::isThumbView() const
{
	return (type == SQ_DirOperatorBase::TypeThumbs);
}

void SQ_DirOperatorBase::slotSelected(QIconViewItem *item)
{
	if(!item) return;

	QString str;
	KFileItem *fi;

	if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
	{
		fi = f->fileInfo();

		// let SQ_WidgetStack to select this file in all other views
		SQ_WidgetStack::instance()->selectFile(fi, this);
	}
}

void SQ_DirOperatorBase::slotSelected(QListViewItem *item)
{
	if(!item) return;

	QString str;
	KFileItem *fi;

	if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
	{
		fi = f->fileInfo();

		// let SQ_WidgetStack to select this file in all other views
		SQ_WidgetStack::instance()->selectFile(fi, this);
	}
}

void SQ_DirOperatorBase::slotDelayedDecode()
{
	SQ_GLWidget::window()->slotStartDecoding(tobeDecoded, true);
}

// Insert new actions in context menu.
void SQ_DirOperatorBase::setupActions()
{
	// create new separator
	KActionSeparator *pASep = new KActionSeparator(actionCollection());

	actionCollection()->action("mkdir")->setShortcut(KShortcut(CTRL+Qt::Key_N));

	// remove "View" submenu, since we will insert our's one.
	setupMenu(KDirOperator::SortActions | KDirOperator::NavActions | KDirOperator::FileActions);

	pARunSeparately = new KAction(i18n("Run separately"), "launch", KShortcut(CTRL+Key_J), SQ_WidgetStack::instance(), SLOT(slotRunSeparately()), (KActionCollection*)0, "Run separately");

	pADirOperatorMenu = (KActionMenu*)actionCollection()->action("popupMenu");

	pAFileActions = new KActionMenu(i18n("File actions"), "edit");

	// create file actions - copy, move, paste ...
	pAFileActions->insert(new KAction(i18n("Copy"), "editcopy", KStdAccel::copy(), SQ_WidgetStack::instance(), SLOT(slotFileCopy()), actionCollection(), "SQ Menu File Copy"));
	pAFileActions->insert(new KAction(i18n("Cut"), "editcut", KStdAccel::cut(), SQ_WidgetStack::instance(), SLOT(slotFileCut()), actionCollection(), "SQ Menu File Cut"));
	pAFileActions->insert(new KAction(i18n("Paste"), "editpaste", KStdAccel::paste(), SQ_WidgetStack::instance(), SLOT(slotFilePaste()), actionCollection(), "SQ Menu File Paste"));
	pAFileActions->insert(pASep);
	pAFileActions->insert(new KAction(i18n("Copy to..."), "editcopy", 0, SQ_WidgetStack::instance(), SLOT(slotFileCopyTo()), actionCollection(), "SQ Menu File Copy To"));
	pAFileActions->insert(new KAction(i18n("Move to..."), "editcut", 0, SQ_WidgetStack::instance(), SLOT(slotFileMoveTo()), actionCollection(), "SQ Menu File Move To	"));
	pAFileActions->insert(new KAction(i18n("Link to..."), "www", 0, SQ_WidgetStack::instance(), SLOT(slotFileLinkTo()), actionCollection(), "SQ Menu File Link To "));
	pAFileActions->insert(pASep);
	pAFileActions->insert(pARunSeparately);

	// insert edit actions - Convert, Rotate, Resize ...
	pAImageActions = new KActionMenu(i18n("Image actions"), "images");
	pAImageActions->insert(KSquirrel::app()->pAImageToolbar);
	pAImageActions->insert(pASep);
	pAImageActions->insert(KSquirrel::app()->pAImageConvert);
	pAImageActions->insert(KSquirrel::app()->pAImageResize);
	pAImageActions->insert(KSquirrel::app()->pAImageRotate);
	pAImageActions->insert(KSquirrel::app()->pAImageBCG);
	pAImageActions->insert(KSquirrel::app()->pAImageFilter);
	pAImageActions->insert(KSquirrel::app()->pAPrintImages);

	if(type == SQ_DirOperatorBase::TypeThumbs)
	{
		KAction *pARecreate = new KAction(i18n("Recreate selected thumbnails"), "reload", KShortcut(Key_F5), SQ_WidgetStack::instance(), SLOT(slotRecreateThumbnail()), actionCollection(), "SQ recreate thumbnail");
		pAImageActions->insert(pARecreate);
	}

	pADirOperatorMenu->insert(pAFileActions);
	pADirOperatorMenu->insert(pAImageActions);
	pADirOperatorMenu->insert(pASep);
	pADirOperatorMenu->popupMenu()->insertItem(i18n("View"), KSquirrel::app()->menuViews());
	pADirOperatorMenu->insert(pASep);
	pADirOperatorMenu->popupMenu()->insertItem(i18n("External Tools"), SQ_ExternalTool::instance()->constPopupMenu());
	pADirOperatorMenu->insert(pASep);
	pADirOperatorMenu->popupMenu()->insertItem(i18n("Fi&lter"), KSquirrel::app()->menuFilters());
}
