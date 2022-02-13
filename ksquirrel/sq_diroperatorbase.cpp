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
#include <qmessagebox.h>
#include <qlabel.h>
#include <qtimer.h>

#include <krun.h>
#include <kstringhandler.h>
#include <kprocess.h>
#include <kapp.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <klocale.h>
#include <kstandarddirs.h>

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

#define SQ_MAX_WORD_LENGTH 50

const int timer_value = 10;

SQ_DirOperatorBase::SQ_DirOperatorBase(const KURL &url, VV type_, QWidget *parent, const char *name) :
	KDirOperator(url, parent, name)
{
	type = type_;

	KActionSeparator *pASep = new KActionSeparator(actionCollection());

	setupMenu(KDirOperator::SortActions | KDirOperator::NavActions | KDirOperator::FileActions);

	pARunSeparately = new KAction(i18n("Run separately"), "launch", KShortcut(CTRL+Key_J), sqWStack, SLOT(slotRunSeparately()), (KActionCollection*)0, "Run separately");

	pADirOperatorMenu = (KActionMenu*)actionCollection()->action("popupMenu");
	pADirOperatorMenu->insert(pARunSeparately, 0);
	pADirOperatorMenu->insert(pASep, 1);
	pADirOperatorMenu->popupMenu()->insertItem(i18n("View"), sqApp->menuViews());
	pADirOperatorMenu->insert(pASep);
	pADirOperatorMenu->popupMenu()->insertItem(i18n("External Tools"), sqExternalTool->getConstPopupMenu());
	pADirOperatorMenu->insert(pASep);
	pADirOperatorMenu->popupMenu()->insertItem(i18n("Fi&lter"), sqApp->menuFilters());

	if(!sqWStack->count())
		connect(sqExternalTool->getConstPopupMenu(), SIGNAL(activated(int)), SLOT(slotActivateExternalTool(int)));

	connect(this, SIGNAL(urlEntered(const KURL&)), this, SLOT(slotUrlEntered(const KURL&)));

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotDelayedDecode()));
}

SQ_DirOperatorBase::~SQ_DirOperatorBase()
{}

KFileView* SQ_DirOperatorBase::createView(QWidget *parent, KFile::FileView)
{
	fileview = 0L;

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
			iv = new SQ_FileIconView(parent, "icon/list view");

			Q_CHECK_PTR(iv);

			fileview = iv;
			reconnectClick(true);

			disconnect(iv, SIGNAL(returnPressed(QIconViewItem*)), iv, 0);
			connect(iv, SIGNAL(returnPressed(QIconViewItem*)), SLOT(slotReturnPressed(QIconViewItem*)));
			connect(iv, SIGNAL(currentChanged(QIconViewItem*)), SLOT(slotSelected(QIconViewItem*)));

			if(type == SQ_DirOperatorBase::TypeIcon)
				iv->setArrangement(QIconView::LeftToRight);
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

	if(!fi->isDir())
	{
		tobeDecoded = fi->url().path();

		if(sqLibHandler->supports(tobeDecoded))
			timer->start(timer_value, true);
		else
		{
			sqSBDecoded->setText(i18n("Format \"")+ fm.extension(false)+i18n("\" not supported"));

			if(sqArchive->findProtocolByFile(fi) != -1)
				if(sqConfig->readBoolEntry("Fileview", "archives", true))
					emit tryUnpack(fi);
				else;
			else if(sqConfig->readBoolEntry("Fileview", "run unknown", true))
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

		if(sqLibHandler->supports(tobeDecoded))
			timer->start(timer_value, true);
		else
		{
			sqSBDecoded->setText(i18n("Format \"")+ fm.extension(false)+i18n("\" not supported"));

			if(sqArchive->findProtocolByFile(fi) != -1)
				if(sqConfig->readBoolEntry("Fileview", "archives", true))
					emit tryUnpack(fi);
				else;
			else if(sqConfig->readBoolEntry("Fileview", "run unknown", true))
				pARunSeparately->activate();
		}
	}
}

void SQ_DirOperatorBase::slotActivateExternalTool(int id)
{
	KFileItem *item = ((SQ_DirOperatorBase*)sqWStack->visibleWidget())->view()->currentFileItem();

	if(!item) return;

	int index = sqExternalTool->getConstPopupMenu()->itemParameter(id);
	QString command = sqExternalTool->getToolCommand(index), final, file;
	KShellProcess	run;

	file = KShellProcess::quote(item->url().path());
	final = command.replace(QRegExp("%s"), file);

	run << final;
	run.start(KProcess::DontCare);
}

void SQ_DirOperatorBase::reconnectClick(bool firstconnect)
{
	bool old_sing = sing;

	switch(sqConfig->readNumEntry("Fileview", "click policy", 0))
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

void SQ_DirOperatorBase::setCurrentItem(KFileItem *item)
{
	fileview->clearSelection();
	fileview->setCurrentItem(item);
	fileview->setSelected(item, true);

	fileview->ensureItemVisible(item);
}

void SQ_DirOperatorBase::slotUrlEntered(const KURL &url)
{
	sqWStack->setURLForCurrent(url);
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
                sqWStack->selectFile(fi, this);
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
                sqWStack->selectFile(fi, this);
        }
}

void SQ_DirOperatorBase::slotDelayedDecode()
{
	sqGLWidget->slotStartDecoding(tobeDecoded, true);
}
