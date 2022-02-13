/***************************************************************************
                          sq_widgetstack.cpp  -  description
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
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qtimer.h>
#include <qfileinfo.h>
#include <qapplication.h>

#include <kaction.h>
#include <kcombobox.h>
#include <kstandarddirs.h>
#include <kstdaccel.h>
#include <klocale.h>

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
#include "sq_thumbnailjob.h"
#include "sq_pixmapcache.h"

#define SQ_SECTION_ICONS "Navigator Icons"
#define SQ_SECTION_LIST "Navigator List"

SQ_WidgetStack::SQ_WidgetStack(QWidget *parent) : QWidgetStack(parent), ncount(0)
{
	pDirOperatorList = 0L;
	pDirOperatorIcon = 0L;
	pDirOperatorDetail = 0L;
	pDirOperatorThumb = 0L;
	path = 0L;

	ac = new KActionCollection(this);

	pABack = KStdAction::back(this, SLOT(slotBack()), ac, "SQ back wrapper");
	pAForw = KStdAction::forward(this, SLOT(slotForward()), ac, "SQ forward wrapper");
	pAUp = KStdAction::up(this, SLOT(slotUp()), ac, "SQ up wrapper");
	pARefresh = KStdAction::redisplay(this, SLOT(slotReload()), ac, "SQ reload wrapper");
	pARefresh->setText(i18n("Reload"));
	pARefresh->setShortcut(KStdAccel::shortcut(KStdAccel::Reload));
	pAHome = KStdAction::home(this, SLOT(slotHome()), ac, "SQ h0me wrapper");
	pAHome->setText(i18n("Home"));
	pAMkDir = new KAction(i18n("New directory ..."), "folder_new", 0, this, SLOT(slotMkDir()), ac, "SQ mkdir wrapper");
	pAProp = new KAction(i18n("Properties ..."), QString::null, 0, this, SLOT(slotProperties()), ac, "SQ prop wrapper");
	pADelete = new KAction(i18n("Delete"), "editdelete", Qt::Key_Delete, this, SLOT(slotDelete()), ac, "SQ delete wrapper");
	pAHidden = new KToggleAction(i18n("Show hidden files"), "folder_grey", 0, 0, 0, ac, "SQ hidden files wrapper");
	connect(pAHidden, SIGNAL(toggled(bool)), this, SLOT(slotShowHidden(bool)));

	timerShowProgress = new QTimer(this);
	connect(timerShowProgress, SIGNAL(timeout()), this, SLOT(slotDelayedShowProgress()));
}

SQ_WidgetStack::~SQ_WidgetStack()
{}

KURL SQ_WidgetStack::getURL() const
{
	SQ_DirOperator *dirop = visibleWidget();

	return (dirop)?dirop->url():KURL();
}

void SQ_WidgetStack::setURL(const QString &newpath, bool cl, bool parseTree)
{
	KURL url = newpath;

	setURL(url, cl, parseTree);
}

void SQ_WidgetStack::setURL(const KURL &newurl, bool cl, bool parseTree)
{
	KURL url = newurl;
	url.adjustPath(1);

	if(sqBookmarks) sqBookmarks->setURL(url);

	if(sqCurrentURL)
	{
		sqCurrentURL->addToHistory(url.prettyURL());
		sqCurrentURL->setEditURL(url);
	}

	if(sqGLWidget)
	{
		sqQuickOperator->blockSignals(true);
		sqQuickOperator->setURL(url, cl);
		sqQuickOperator->blockSignals(false);
	}

	if(sqTree && parseTree)
	{
		int sync_type = sqConfig->readNumEntry("Fileview", "sync type", 0);

		if(sync_type == 2 || sync_type == 0)
			sqTree->emitNewURL(url);
	}
}

void SQ_WidgetStack::setURLfromtree(const KURL &newurl)
{
	bool cl = true;

	setURL(newurl, cl, false);

	visibleWidget()->setURL(newurl, cl);
}

bool SQ_WidgetStack::moveTo(Direction direction, KFileItem *it)
{
	KFileView *local;
	KFileItem *item;

	local = visibleWidget()->fileview;
	if(it)
		item = it;
	else
		item = (direction == SQ_WidgetStack::Next)?
				(local->nextItem(local->currentFileItem())):
				(local->prevItem(local->currentFileItem()));

	if(!item)
		return false;

	while(1)
	{
		if(item->isFile())
			if(sqLibHandler->supports(item->url().path()))
				break;

		item = (direction == SQ_WidgetStack::Next)?
				(local->nextItem(item)):
				(local->prevItem(item));

		if(!item)
			return false;
	}

	selectFile(item);
	return true;
}

void SQ_WidgetStack::slotShowHidden(bool)
{
	if(pDirOperatorList) pDirOperatorList->actionCollection()->action("show hidden")->activate();
	if(pDirOperatorIcon) pDirOperatorIcon->actionCollection()->action("show hidden")->activate();
	if(pDirOperatorDetail) pDirOperatorDetail->actionCollection()->action("show hidden")->activate();
	if(pDirOperatorThumb) pDirOperatorThumb->actionCollection()->action("show hidden")->activate();
}

void SQ_WidgetStack::setNameFilter(const QString &f)
{
	if(pDirOperatorList) pDirOperatorList->setNameFilter(f);
	if(pDirOperatorIcon) pDirOperatorIcon->setNameFilter(f);
	if(pDirOperatorDetail) pDirOperatorDetail->setNameFilter(f);
	if(pDirOperatorThumb) pDirOperatorThumb->setNameFilter(f);
	if(sqGLWidget) sqQuickOperator->setNameFilter(f);

	if(pDirOperatorList) pDirOperatorList->actionCollection()->action("reload")->activate();
	if(pDirOperatorIcon) pDirOperatorIcon->actionCollection()->action("reload")->activate();
	if(pDirOperatorDetail) pDirOperatorDetail->actionCollection()->action("reload")->activate();
	if(pDirOperatorThumb) pDirOperatorThumb->actionCollection()->action("reload")->activate();
	if(sqGLWidget) sqQuickOperator->actionCollection()->action("reload")->activate();
}

const QString SQ_WidgetStack::getNameFilter() const
{
	return visibleWidget()->nameFilter();
}

void SQ_WidgetStack::raiseWidget(int id)
{
	KFileView *local = 0L;
	KFileItem *item = 0L;
	QString	filter = "*";
	SQ_DirOperator *local_operator = visibleWidget();
	SQ_DirOperator *shown = 0L;

	if(ncount)
	{
		local = local_operator->fileview;
		item = local->currentFileItem();
		filter = local_operator->nameFilter();
	}

	// load views only on call.
	if(id == 0 && pDirOperatorList == 0L)
	{
		KURL _url;
		if(path)
			_url.setPath(*path);
		else
			_url = getURL();

		pDirOperatorList = new SQ_DirOperator(_url,  SQ_DirOperator::TypeList, this);

		Q_CHECK_PTR(pDirOperatorList);

		setupDirOperator(pDirOperatorList, filter);
		
		pDirOperatorList->readConfig(KGlobal::config(), SQ_SECTION_LIST);
		pDirOperatorList->setViewConfig(KGlobal::config(), SQ_SECTION_LIST);
		pDirOperatorList->setMode(KFile::Files);
		pDirOperatorList->iv->setArrangement(QIconView::TopToBottom);
//		pDirOperatorList->iv->setSelectionMode(KFile::Single);
		pDirOperatorList->iv->setIconSize(KIcon::SizeSmall);
		shown = pDirOperatorList;

		connect(pDirOperatorList, SIGNAL(tryUnpack(KFileItem *)), SLOT(tryUnpack(KFileItem *)));

		addWidget(pDirOperatorList, 0);
		ncount++;
	}
	if(id == 1 && pDirOperatorIcon == 0L)
	{
		KURL _url;
		if(path)
			_url.setPath(*path);
		else
			_url = getURL();

		pDirOperatorIcon = new SQ_DirOperator(_url,  SQ_DirOperator::TypeIcon, this);

		Q_CHECK_PTR(pDirOperatorIcon);

		setupDirOperator(pDirOperatorIcon, filter);

		pDirOperatorIcon->readConfig(KGlobal::config(), SQ_SECTION_ICONS);
		pDirOperatorIcon->setViewConfig(KGlobal::config(), SQ_SECTION_ICONS);
		pDirOperatorIcon->setMode(KFile::Files);
		pDirOperatorIcon->iv->setArrangement(QIconView::LeftToRight);
//		pDirOperatorIcon->iv->setSelectionMode(KFile::Single);
		pDirOperatorIcon->iv->setIconSize(KIcon::SizeMedium);
		shown = pDirOperatorIcon;

		connect(pDirOperatorIcon, SIGNAL(tryUnpack(KFileItem *)), SLOT(tryUnpack(KFileItem *)));

		addWidget(pDirOperatorIcon, 1);
		ncount++;
	}
	else if(id == 2 &&  pDirOperatorDetail == 0L)
	{
		KURL _url;
		if(path)
			_url.setPath(*path);
		else
			_url = getURL();

		pDirOperatorDetail = new SQ_DirOperator(_url,  SQ_DirOperator::TypeDetail, this);

		Q_CHECK_PTR(pDirOperatorDetail);

		setupDirOperator(pDirOperatorDetail, filter);
		
		pDirOperatorDetail->readConfig(KGlobal::config(), SQ_SECTION_LIST);
		pDirOperatorDetail->setMode(KFile::Files);
//		pDirOperatorDetail->dv->setSelectionMode(KFile::Single);
		shown = pDirOperatorDetail;

		connect(pDirOperatorDetail, SIGNAL(tryUnpack(KFileItem *)), SLOT(tryUnpack(KFileItem *)));

		addWidget(pDirOperatorDetail, 2);
		ncount++;
	}
	else if(id == 3 &&  pDirOperatorThumb == 0L)
	{
		KURL _url;
		if(path)
			_url = *path;
		else
			_url = getURL();

		pDirOperatorThumb = new SQ_DirOperator(_url,  SQ_DirOperator::TypeThumbs, this);

		Q_CHECK_PTR(pDirOperatorThumb);

		setupDirOperator(pDirOperatorThumb, filter);

		pDirOperatorThumb->readConfig(KGlobal::config(), SQ_SECTION_ICONS);
		pDirOperatorThumb->setMode(KFile::Files);
		pDirOperatorThumb->tv->setArrangement(QIconView::LeftToRight);
//		pDirOperatorThumb->tv->setSelectionMode(KFile::Single);

		int pixelSize = (sqThumbSize->isExtended())?
				sqThumbSize->extendedPixelSize():sqThumbSize->pixelSize();
		pDirOperatorThumb->tv->setGridX(pixelSize + sqConfig->readNumEntry("Thumbnails", "margin", 2) + 2);
		shown = pDirOperatorThumb;

		connect(sqApp, SIGNAL(thumbSizeChanged(const QString&)), pDirOperatorThumb, SLOT(slotSetThumbSize(const QString&)));
		connect(pDirOperatorThumb, SIGNAL(tryUnpack(KFileItem *)), SLOT(tryUnpack(KFileItem *)));

		addWidget(pDirOperatorThumb, 3);
		ncount++;

		pARecreate = new KAction(i18n("Recreate selected thumbnails"), "reload", KShortcut(Key_F5), this, SLOT(slotRecreateThumbnail()), pDirOperatorThumb->actionCollection(), "SQ recreate thumbnail");
//		sqApp->insertRecreateAction();
	}

	if(shown == 0L && ncount > 1)
		widget(id)->setURL(visibleWidget()->url(), true);

	if(shown)
	{
		if(pAHidden->isChecked())
			shown->actionCollection()->action("show hidden")->activate();

		shown->show();
	}

	QWidgetStack::raiseWidget(id);

	if(ncount > 1)
	{
		local_operator = visibleWidget();
		local_operator->setCurrentItem(item);
	}

	sqApp->enableThumbsMenu(id == 3);
}

const KFileItemList* SQ_WidgetStack::selectedItems() const
{
	KFileView *local;

	local = visibleWidget()->fileview;

	return local->selectedItems();	
}

const KFileItemList* SQ_WidgetStack::items() const
{
	KFileView *local;

	local = visibleWidget()->fileview;

	return local->items();
}

void SQ_WidgetStack::raiseFirst(int id)
{
	path = new QString;

	if(sqHighLevel->path.isEmpty())
		switch(sqConfig->readNumEntry("Fileview", "set path", 1))
		{
			case 2: *path = sqConfig->readEntry("Fileview", "custom directory", "/"); break;
			case 1: *path = ""; break;
			case 0: *path = sqConfig->readEntry("Fileview", "last visited", "/"); break;
			default: *path = "/";
		}
	else
	{
		QFileInfo fm(sqHighLevel->path);
		*path = (fm.isDir()) ? sqHighLevel->path : fm.dirPath(true);
	}

	raiseWidget(id);

	if(sqCurrentURL)
		sqCurrentURL->setEditURL(getURL());

        delete path;
        path = 0L;
}

void SQ_WidgetStack::emitNextSelected()
{
	if(!moveTo(SQ_WidgetStack::Next)) return;

	KFileView *local = visibleWidget()->fileview;

	sqGLWidget->slotStartDecoding(local->currentFileItem()->url());
}

void SQ_WidgetStack::emitPreviousSelected()
{
	if(!moveTo(SQ_WidgetStack::Previous)) return;

	KFileView *local = visibleWidget()->fileview;

	sqGLWidget->slotStartDecoding(local->currentFileItem()->url());
}

int SQ_WidgetStack::count() const
{
	return ncount;
}

void SQ_WidgetStack::tryUnpack(KFileItem *item)
{
	if(sqArchive->findProtocolByFile(item) != -1)
	{
		sqArchive->setFile(item);

		if(sqArchive->unpack())
		{
			QTimer::singleShot(0, this, SLOT(slotDelayedSetExtractURL()));
		}
	}
}

void SQ_WidgetStack::slotDelayedSetExtractURL()
{
	SQ_DirOperator *local = visibleWidget();

	KURL _url;
	_url.setPath(sqArchive->getExtractedPath());

	local->setURL(_url, true);
}

void SQ_WidgetStack::configureClickPolicy()
{
	if(pDirOperatorList) pDirOperatorList->reconnectClick();
	if(pDirOperatorIcon) pDirOperatorIcon->reconnectClick();
	if(pDirOperatorDetail) pDirOperatorDetail->reconnectClick();
	if(pDirOperatorThumb) pDirOperatorThumb->reconnectClick();

	sqQuickOperator->reconnectClick();
}

void SQ_WidgetStack::selectFile(KFileItem *item, SQ_DirOperatorBase *workAround)
{
	SQ_DirOperatorBase *local = (SQ_DirOperatorBase*)visibleWidget();

	if(local != workAround) local->setCurrentItem(item);
	if(sqQuickOperator != workAround) sqQuickOperator->setCurrentItem(item);
}

void SQ_WidgetStack::slotFirstFile()
{
	SQ_DirOperator *local_operator = visibleWidget();
	KFileView *local_view = local_operator->fileview;
	KFileItemList *list = (KFileItemList*)local_view->items();
	KFileItem *item = list->first();

	if(!item)
		return;

	if(!moveTo(SQ_WidgetStack::Next, item))
		return;

	item = local_view->currentFileItem();
	local_operator->setCurrentItem(item);
	sqGLWidget->slotStartDecoding(item->url());
}

void SQ_WidgetStack::slotLastFile()
{
	SQ_DirOperator *local_operator = visibleWidget();
	KFileView *local_view = local_operator->fileview;
	KFileItemList *list = (KFileItemList*)local_view->items();
	KFileItem *item = list->last();

	if(!item)
		return;

	if(!moveTo(SQ_WidgetStack::Previous, item))
		return;

	item = local_view->currentFileItem();
	local_operator->setCurrentItem(item);
	sqGLWidget->slotStartDecoding(item->url());
}

void SQ_WidgetStack::updateGrid(bool arrange)
{
	if(!pDirOperatorThumb)
		return;

	int pixelSize = (sqThumbSize->isExtended())?
			sqThumbSize->extendedPixelSize():sqThumbSize->pixelSize();

	int newgrid = pixelSize + sqConfig->readNumEntry("Thumbnails", "margin", 2) + 2;

	if(pDirOperatorThumb->tv->gridX() != newgrid)
	{
		pDirOperatorThumb->tv->setGridX(newgrid);

		if(arrange)
			pDirOperatorThumb->tv->arrangeItemsInGrid();
	}
}

void SQ_WidgetStack::thumbnailsUpdateEnded()
{
	static QPixmap play_pixmap = QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumb_resume.png"));

	timerShowProgress->stop();
	pDirOperatorThumb->tv->progressBox->hide();
	pDirOperatorThumb->tv->progress->flush();
	pDirOperatorThumb->tv->buttonStop->setPixmap(play_pixmap);
}

void SQ_WidgetStack::thumbnailUpdateStart(int count)
{
	static QPixmap stop_pixmap = QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumb_stop.png"));

	pDirOperatorThumb->tv->progress->setTotalSteps(count);
	pDirOperatorThumb->tv->buttonStop->setPixmap(stop_pixmap);

	timerShowProgress->start(1000, true);
}

void SQ_WidgetStack::slotDelayedShowProgress()
{
	pDirOperatorThumb->tv->progressBox->show();
}

void SQ_WidgetStack::thumbnailProcess()
{
	pDirOperatorThumb->tv->progress->advance(1);
}

void SQ_WidgetStack::slotUp()
{
	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->actionCollection()->action("up")->activate();
}

void SQ_WidgetStack::slotBack()
{
	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->actionCollection()->action("back")->activate();
}

void SQ_WidgetStack::slotForward()
{
	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->actionCollection()->action("forward")->activate();
}

void SQ_WidgetStack::slotReload()
{
	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->actionCollection()->action("reload")->activate();
}

void SQ_WidgetStack::slotHome()
{
	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->actionCollection()->action("home")->activate();
}

void SQ_WidgetStack::slotMkDir()
{
	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->actionCollection()->action("mkdir")->activate();
}
void SQ_WidgetStack::slotProperties()
{
	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->actionCollection()->action("properties")->activate();
}

void SQ_WidgetStack::slotDelete()
{
	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->actionCollection()->action("delete")->activate();
}

void SQ_WidgetStack::setURLForCurrent(const QString &path)
{
	KURL url = path;

	setURLForCurrent(url);
}

void SQ_WidgetStack::setURLForCurrent(const KURL &url)
{
	setURL(url, true);

	SQ_DirOperator *local_operator = visibleWidget();
	local_operator->setURL(url, true);
}

void SQ_WidgetStack::updateView()
{
	if(pDirOperatorList) pDirOperatorList->rereadDir();
	if(pDirOperatorIcon) pDirOperatorIcon->rereadDir();
	if(pDirOperatorDetail) pDirOperatorDetail->rereadDir();
	if(pDirOperatorThumb) pDirOperatorThumb->rereadDir();
}

void SQ_WidgetStack::slotRunSeparately()
{
	SQ_DirOperator *local_operator = visibleWidget();

	const KFileItemList *selected = local_operator->selectedItems();
	KFileItemListIterator it(*selected);

	KFileItem *item;

	while((item = it.current()) != 0)
	{
		if(item)
			item->run();

		++it;
	}
}

SQ_DirOperator* SQ_WidgetStack::visibleWidget() const
{
	SQ_DirOperator *local = (SQ_DirOperator*)QWidgetStack::visibleWidget();

	return local;
}

SQ_DirOperator* SQ_WidgetStack::widget(int id) const
{
	SQ_DirOperator *local = (SQ_DirOperator*)QWidgetStack::widget(id);

	return local;
}

void SQ_WidgetStack::setupDirOperator(SQ_DirOperator *op, const QString &filter)
{
	if(ncount)
	{
		KDirLister *d = new KDirLister;
		d->setNameFilter(filter);
		op->setDirLister(d);
	}
}


void SQ_WidgetStack::slotRecreateThumbnail()
{
	if(visibleWidget() != pDirOperatorThumb || !pDirOperatorThumb)
		return;

	if(pDirOperatorThumb->tv->updateRunning())
		return;

	pDirOperatorThumb->tv->progressBox->show();

	QTimer::singleShot(50, this, SLOT(slotDelayedRecreateThumbnail()));

	qApp->processEvents();
}

void SQ_WidgetStack::slotDelayedRecreateThumbnail()
{
	SQ_Thumbnail thumb;

	KFileItemList *list = (KFileItemList *)selectedItems();

	if(!list)
		return;

	KFileItem *item;

	thumbnailUpdateStart(list->count());

	qApp->processEvents();

	while((item = list->take()) != 0)
	{
		if(!item)
			continue;

		QString path = item->url().path();

		printf("SQ_ThumbnailLoadJob: recreating %s\n", item->name().ascii());

		if(!SQ_ThumbnailLoadJob::loadThumbnail(path, thumb, true))
			continue;

		sqCache->removeEntryFull(path);

		sqCache->insert(path, thumb);

		thumbnailProcess();

		pDirOperatorThumb->tv->setThumbnailPixmap(item, thumb);
	}

	thumbnailsUpdateEnded();
}
