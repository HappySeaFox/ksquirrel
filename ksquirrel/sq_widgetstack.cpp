/***************************************************************************
                          sq_widgetstack.cpp  -  description
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
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qvaluevector.h>
#include <qtimer.h>

#include <kaction.h>
#include <kcombobox.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kstdaccel.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_externaltool.h"
#include "sq_treeview.h"
#include "sq_hloptions.h"
#include "sq_archivehandler.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_quickbrowser.h"
#include "sq_libraryhandler.h"
#include "sq_diroperator.h"
#include "sq_bookmarkowner.h"
#include "sq_thumbnailsize.h"
#include "sq_progress.h"

#define SQ_SECTION_NAME ("squirrel image viewer file browser")

SQ_WidgetStack::SQ_WidgetStack(QWidget *parent) : QWidgetStack(parent), ncount(0)
{
	pDirOperatorList = 0L;
	pDirOperatorIcon = 0L;
	pDirOperatorDetail = 0L;
	pDirOperatorThumb = 0L;
	path = 0L;

	ar = new SQ_ArchiveHandler;

	KDirOperator *w = new KDirOperator(KURL("/"), 0);
	w->hide();
	KAction *abstract;

	pABack = KStdAction::back(this, SLOT(slotBack()), sqApp->actionCollection(), "SQ back wrapper");
	pAForw = KStdAction::forward(this, SLOT(slotForward()), sqApp->actionCollection(), "SQ forward wrapper");
	pAUp = KStdAction::up(this, SLOT(slotUp()), sqApp->actionCollection(), "SQ up wrapper");
	pARefresh = KStdAction::redisplay(this, SLOT(slotReload()), sqApp->actionCollection(), "SQ reload wrapper");
	pARefresh->setText(i18n("Reload"));
	pARefresh->setShortcut(KStdAccel::shortcut(KStdAccel::Reload));
	pAHome = KStdAction::home(this, SLOT(slotHome()), sqApp->actionCollection(), "SQ h0me wrapper");
	pAHome->setText(i18n("Home"));
	abstract = w->actionCollection()->action("mkdir");
	pAMkDir = new KAction(abstract->text(), abstract->icon(), abstract->shortcut(), this, SLOT(slotMkDir()), sqApp->actionCollection(), "SQ mkdir wrapper");
	abstract = w->actionCollection()->action("properties");
	pAProp = new KAction(abstract->text(), abstract->icon(), abstract->shortcut(), this, SLOT(slotProperties()), sqApp->actionCollection(), "SQ prop wrapper");
	abstract = w->actionCollection()->action("delete");
	pADelete = new KAction(i18n("Delete"), "editdelete", Qt::Key_Delete, this, SLOT(slotDelete()), sqApp->actionCollection(), "SQ delete wrapper");
	delete w;
}

SQ_WidgetStack::~SQ_WidgetStack()
{}

KURL SQ_WidgetStack::getURL() const
{
	SQ_DirOperator* dirop = (SQ_DirOperator*)visibleWidget();

	return (dirop)?dirop->url():"";
}

void SQ_WidgetStack::setURL(const QString &newpath, bool cl)
{
	setURL(KURL(newpath), cl);
}

void SQ_WidgetStack::setURL(const KURL &newurl, bool cl)
{
	if(sqBookmarks) sqBookmarks->setURL(newurl);

	if(sqCurrentURL)
	{
		sqCurrentURL->setEditText(newurl.path());
		sqCurrentURL->addToHistory(newurl.path());
	}

	if(!sqTree) return;

	int sync_type = sqConfig->readNumEntry("Fileview", "sync type", 0);

	if(sync_type == 2 || sync_type == 0)
		sqTree->emitNewURL(newurl);

	if(sqGLView) sqQuickOperator->setURL(newurl, cl);
}

void SQ_WidgetStack::setURL(const QString &newpath)
{
	setURL(newpath, true);
}

void SQ_WidgetStack::setURL(const KURL &newurl)
{
	setURL(newurl, true);
}

void SQ_WidgetStack::setURLfromtree(const KURL &newurl)
{
	bool cl = true;
	((SQ_DirOperator*)visibleWidget())->setURL(newurl, cl);
	sqQuickOperator->setURL(newurl, cl);
}

bool SQ_WidgetStack::slotPrevious(KFileItem *it)
{
	KFileView *local;
	KFileItem *item;

	local = ((SQ_DirOperator*)visibleWidget())->fileview;
	if(it)
		item = it;
	else
		item = local->prevItem(local->currentFileItem());

	if(!item)
		return false;

	while(1)
	{
		if(item->isFile())
		{
			QFileInfo fm(item->url().path());

			if(sqLibHandler->supports(fm.extension(false)))
				break;
		}

		item = local->prevItem(item);
		if(!item)
			return false;
	}

	selectFile(item);
	return true;
}

bool SQ_WidgetStack::slotNext(KFileItem *it)
{
	KFileView *local;
	KFileItem *item;

	local = ((SQ_DirOperator*)visibleWidget())->fileview;
	if(it)
		item = it;
	else
		item = local->nextItem(local->currentFileItem());

	if(!item)
		return false;

	while(1)
	{
		if(item->isFile())
		{
			QFileInfo fm(item->url().path());

			if(sqLibHandler->supports(fm.extension(false)))
				break;
		}

		item = local->nextItem(item);
		if(!item)
			return false;
	}

	selectFile(item);
	return true;
}

void SQ_WidgetStack::slotShowHidden(bool)
{
	((SQ_DirOperator*)visibleWidget())->actionCollection()->action("show hidden")->activate();
}

void SQ_WidgetStack::setNameFilter(const QString &f)
{
	if(pDirOperatorList) pDirOperatorList->setNameFilter(f);
	if(pDirOperatorIcon) pDirOperatorIcon->setNameFilter(f);
	if(pDirOperatorDetail) pDirOperatorDetail->setNameFilter(f);
	if(pDirOperatorThumb) pDirOperatorThumb->setNameFilter(f);
	if(sqGLView) sqQuickOperator->setNameFilter(f);

	if(pDirOperatorList) pDirOperatorList->actionCollection()->action("reload")->activate();
	if(pDirOperatorIcon) pDirOperatorIcon->actionCollection()->action("reload")->activate();
	if(pDirOperatorDetail) pDirOperatorDetail->actionCollection()->action("reload")->activate();
	if(pDirOperatorThumb) pDirOperatorThumb->actionCollection()->action("reload")->activate();
	if(sqGLView) sqQuickOperator->actionCollection()->action("reload")->activate();
}

const QString SQ_WidgetStack::getNameFilter() const
{
	return ((SQ_DirOperator*)visibleWidget())->nameFilter();
}

void SQ_WidgetStack::raiseWidget(int id)
{
	KFileView *local = 0L;
	KFileItem *item = 0L;
	QString	filter = "*";
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	bool created = false;

	if(ncount)
	{
		local = local_operator->fileview;
		item = local->currentFileItem();
		filter = local_operator->nameFilter();
	}

	// load views only on call.
	if(id == 0 && pDirOperatorList == 0L)
	{
		pDirOperatorList = new SQ_DirOperator(KURL(((path)?*path:"/")),  SQ_DirOperator::TypeList);
//		pDirOperatorList->readConfig(KGlobal::config(), SQ_SECTION_NAME);
		pDirOperatorList->setViewConfig(KGlobal::config(), SQ_SECTION_NAME);
		pDirOperatorList->setMode(KFile::Files);
//		pDirOperatorList->setView(KFile::Simple);
//		pDirOperatorList->view()->actionCollection()->action(2)->activate();
		if(!path) pDirOperatorList->setURL(getURL(), true);
		pDirOperatorList->setIconSize(16);

              connect(pDirOperatorList, SIGNAL(tryUnpack(const QString&)), SLOT(tryUnpack(const QString&)));

		if(ncount)
			pDirOperatorList->setNameFilter(filter);

		addWidget(pDirOperatorList, 0);
		created = true;
		ncount++;
	}
	if(id == 1 && pDirOperatorIcon == 0L)
	{
		pDirOperatorIcon = new SQ_DirOperator(KURL(((path)?*path:"/")),  SQ_DirOperator::TypeIcon);
//		pDirOperatorIcon->readConfig(KGlobal::config(), SQ_SECTION_NAME);
		pDirOperatorIcon->setViewConfig(KGlobal::config(), SQ_SECTION_NAME);
		pDirOperatorIcon->setMode(KFile::Files);
//		pDirOperatorIcon->setView(KFile::Simple);
		if(!path) pDirOperatorIcon->setURL(getURL(), true);
		pDirOperatorIcon->setIconSize(32);

              connect(pDirOperatorIcon, SIGNAL(tryUnpack(const QString&)), SLOT(tryUnpack(const QString&)));

		if(ncount)
			pDirOperatorIcon->setNameFilter(filter);

		addWidget(pDirOperatorIcon, 1);
		created = true;
		ncount++;
	}
	else if(id == 2 &&  pDirOperatorDetail == 0L)
	{
		pDirOperatorDetail = new SQ_DirOperator(KURL(((path)?*path:"/")),  SQ_DirOperator::TypeDetail);
//		pDirOperatorDetail->readConfig(KGlobal::config(), SQ_SECTION_NAME);
		pDirOperatorDetail->setViewConfig(KGlobal::config(), SQ_SECTION_NAME);
		pDirOperatorDetail->setMode(KFile::Files);
//		pDirOperatorDetail->setView(KFile::Detail);
		if(!path) pDirOperatorDetail->setURL(getURL(), true);

              connect(pDirOperatorDetail, SIGNAL(tryUnpack(const QString&)), SLOT(tryUnpack(const QString&)));

		if(ncount)
			pDirOperatorDetail->setNameFilter(filter);

		addWidget(pDirOperatorDetail, 2);
		created = true;
		ncount++;
	}
	else if(id == 3 &&  pDirOperatorThumb == 0L)
	{
		pDirOperatorThumb = new SQ_DirOperator(KURL(((path)?*path:"/")),  SQ_DirOperator::TypeThumbs);
//		pDirOperatorThumb->readConfig(KGlobal::config(), SQ_SECTION_NAME);
//		pDirOperatorThumb->setViewConfig(KGlobal::config(), SQ_SECTION_NAME);
		pDirOperatorThumb->setMode(KFile::Files);
//		pDirOperatorThumb->setView(KFile::Simple);
		pDirOperatorThumb->setIconSize(sqThumbSize->pixelSize());
		pDirOperatorThumb->tv->setGridX(sqThumbSize->pixelSize() + sqConfig->readNumEntry("Thumbnails", "margin", 2));
		pDirOperatorThumb->tv->setItemTextPos(QIconView::Bottom);
		if(!path) pDirOperatorThumb->setURL(getURL(), true);

		connect(sqApp, SIGNAL(thumbSizeChanged(const QString&)), pDirOperatorThumb, SLOT(slotSetThumbSize(const QString&)));
              connect(pDirOperatorThumb, SIGNAL(tryUnpack(const QString&)), SLOT(tryUnpack(const QString&)));

		if(ncount)
			pDirOperatorThumb->setNameFilter(filter);

		addWidget(pDirOperatorThumb, 3);
		created = true;
		ncount++;
	}

	if(!created && ncount > 1)
		((SQ_DirOperator*)widget(id))->setURL(((SQ_DirOperator*)visibleWidget())->url(), true);

	QWidgetStack::raiseWidget(id);

	if(ncount > 1)
	{
		local_operator = (SQ_DirOperator*)visibleWidget();
		local_operator->setCurrentItem(item);
	}
}

const KFileItemList* SQ_WidgetStack::selectedItems() const
{
	KFileView *local;

	local = ((SQ_DirOperator*)visibleWidget())->fileview;

	return local->selectedItems();	
}

void SQ_WidgetStack::raiseFirst(int id)
{
	path = new QString;

	if(sqHighLevel->HL_url.isEmpty())
		switch(sqConfig->readNumEntry("Fileview", "set path", 1))
		{
			case 2: *path = sqConfig->readEntry("Fileview", "custom directory", "/"); break;
			case 1: *path = ""; break;
			case 0: *path = sqConfig->readEntry("Fileview", "last visited", "/"); break;
			default: *path = "/";
		}
	else
		*path = (!sqHighLevel->HL_url.fileName(false).isEmpty()) ? (sqHighLevel->HL_url.directory()) : (sqHighLevel->HL_url.path());

	raiseWidget(id);

	QString url = getURL().path();
	if(sqCurrentURL)
	{
		sqCurrentURL->setEditText(url);
		sqCurrentURL->addToHistory(url);
	}

        delete path;
        path = 0L;
}

void SQ_WidgetStack::emitNextSelected()
{
	KFileView *local = ((SQ_DirOperator*)visibleWidget())->fileview;

	if(!slotNext()) return;

	sqGLWidget->emitShowImage(local->currentFileItem()->url());
}

void SQ_WidgetStack::emitPreviousSelected()
{
	KFileView *local = ((SQ_DirOperator*)visibleWidget())->fileview;

	if(!slotPrevious()) return;

	sqGLWidget->emitShowImage(local->currentFileItem()->url());
}

int SQ_WidgetStack::count() const
{
	return ncount;
}

void SQ_WidgetStack::emitSelected(const QString &file)
{
	SQ_DirOperator *local = (SQ_DirOperator*)visibleWidget();

	local->emitSelected(file);
}

void SQ_WidgetStack::tryUnpack(const QString &fullpath)
{
	if(ar->supported(fullpath))
	{
		ar->setPath(fullpath);

		if(ar->unpack())
		{
			QTimer::singleShot(0, this, SLOT(slotDelayedSetExtractURL()));
		}
	}
}

void SQ_WidgetStack::slotDelayedSetExtractURL()
{
	SQ_DirOperator *local = (SQ_DirOperator*)visibleWidget();
	local->setURL(KURL(ar->getExtractedPath()), true);
}

void SQ_WidgetStack::cleanUnpacked()
{
	if(!ar->needClean())
		return;

	ar->clean();
}

void SQ_WidgetStack::configureClickPolicy()
{
	if(pDirOperatorList) pDirOperatorList->reconnectClick();
	if(pDirOperatorIcon) pDirOperatorIcon->reconnectClick();
	if(pDirOperatorDetail) pDirOperatorDetail->reconnectClick();
	if(pDirOperatorThumb) pDirOperatorThumb->reconnectClick();

	sqQuickOperator->reconnectClick();
}

void SQ_WidgetStack::selectFile(KFileItem *item, SQ_DirOperator *workAround)
{
	SQ_DirOperator *local = (SQ_DirOperator*)visibleWidget();

	if(local != workAround) local->setCurrentItem(item);
	if(sqQuickOperator != workAround) sqQuickOperator->setCurrentItem(item);
}

void SQ_WidgetStack::slotFirstFile()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	KFileView *local_view = local_operator->fileview;
	KFileItemList *list = (KFileItemList*)local_view->items();
	KFileItem *item = list->first();

	if(!item)
		return;

	if(!slotNext(item))
		return;

	item = local_view->currentFileItem();
	local_operator->setCurrentItem(item);
	sqGLWidget->emitShowImage(item->url());
}

void SQ_WidgetStack::slotLastFile()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	KFileView *local_view = local_operator->fileview;
	KFileItemList *list = (KFileItemList*)local_view->items();
	KFileItem *item = list->last();

	if(!item)
		return;

	if(!slotPrevious(item))
		return;

	item = local_view->currentFileItem();
	local_operator->setCurrentItem(item);
	sqGLWidget->emitShowImage(item->url());
}

void SQ_WidgetStack::updateGrid()
{
	if(!pDirOperatorThumb)
		return;

	pDirOperatorThumb->tv->setGridX(sqThumbSize->pixelSize() + sqConfig->readNumEntry("Thumbnails", "margin", 2));
	pDirOperatorThumb->actionCollection()->action("reload")->activate();
	pDirOperatorThumb->tv->arrangeItemsInGrid();
}

void SQ_WidgetStack::thumbnailsUpdateEnded()
{
	static QPixmap play_pixmap = QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumb_resume.png"));

	pDirOperatorThumb->tv->progressBox->hide();
	pDirOperatorThumb->tv->sqProgress->flush();
	pDirOperatorThumb->tv->buttonStop->setPixmap(play_pixmap);
}

void SQ_WidgetStack::thumbnailUpdateStart(int count)
{
	static QPixmap stop_pixmap = QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumb_stop.png"));

	pDirOperatorThumb->tv->progressBox->show();
	pDirOperatorThumb->tv->sqProgress->setTotalSteps(count);
	pDirOperatorThumb->tv->buttonStop->setPixmap(stop_pixmap);
}

void SQ_WidgetStack::thumbnailProcess()
{
	pDirOperatorThumb->tv->sqProgress->advance(1);
}

void SQ_WidgetStack::slotUp()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->actionCollection()->action("up")->activate();
}

void SQ_WidgetStack::slotBack()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->actionCollection()->action("back")->activate();
}

void SQ_WidgetStack::slotForward()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->actionCollection()->action("forward")->activate();
}

void SQ_WidgetStack::slotReload()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->actionCollection()->action("reload")->activate();
}

void SQ_WidgetStack::slotHome()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->actionCollection()->action("home")->activate();
}

void SQ_WidgetStack::slotMkDir()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->actionCollection()->action("up")->activate();
}
void SQ_WidgetStack::slotProperties()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->actionCollection()->action("properties")->activate();
}

void SQ_WidgetStack::slotDelete()
{
	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->actionCollection()->action("delete")->activate();
}

void SQ_WidgetStack::setURLForCurrent(const QString &path)
{
	setURLForCurrent(KURL(path));
}

void SQ_WidgetStack::setURLForCurrent(const KURL &url)
{
	setURL(url, true);

	SQ_DirOperator *local_operator = (SQ_DirOperator*)visibleWidget();
	local_operator->setURL(url, true);
}
