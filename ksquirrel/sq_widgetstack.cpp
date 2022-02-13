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

#include "sq_widgetstack.h"

#include <kiconloader.h>
#include <kaction.h>

#include <qlabel.h>
#include <qbuttongroup.h>

#include "sq_fileiconview.h"
#include "sq_filedetailview.h"

#include "ksquirrel.h"

#include <qvaluevector.h>

SQ_WidgetStack::SQ_WidgetStack(QWidget *parent) : QWidgetStack(parent)
{
	pIconSizeList = new QValueVector<int>;
	pIconSizeList->append(16);
	pIconSizeList->append(22);
	pIconSizeList->append(32);
	pIconSizeList->append(48);
	pIconSizeList->append(64);
	pIconSizeList->append(96);
	pIconSizeList->append(128);
	pIconSizeList->append(192);
	pIconSizeList->append(256);

	sqConfig->setGroup("Fileview");
	iCurrentListIndex = sqConfig->readNumEntry("iCurrentListIndex", 0);
	iCurrentIconIndex = sqConfig->readNumEntry("iCurrentIconIndex", 2);

	pDirOperatorList = 0L;
	pDirOperatorIcon = 0L;
	pDirOperatorDetail = 0L;

	path = 0L;
}

SQ_WidgetStack::~SQ_WidgetStack()
{}

KURL SQ_WidgetStack::getURL() const
{
	SQ_DirOperator* dirop = (SQ_DirOperator*)visibleWidget();

	if(dirop)
		return dirop->url();
	else
		return "";
}

void SQ_WidgetStack::setURL(const QString &newpath, bool cl)
{
	KURL url(newpath);
	if(sqCurrentURL) sqCurrentURL->setEditText(newpath);
	if(sqCurrentURL) sqCurrentURL->addToHistory(newpath);
	if(pDirOperatorList) pDirOperatorList->setURL(url, cl);
	if(pDirOperatorIcon) pDirOperatorIcon->setURL(url, cl);
	if(pDirOperatorDetail) pDirOperatorDetail->setURL(url, cl);
}

void SQ_WidgetStack::setURL(const KURL &newurl, bool cl)
{
	if(sqCurrentURL) sqCurrentURL->setEditText(newurl.path());
	if(sqCurrentURL) sqCurrentURL->addToHistory(newurl.path());
	if(pDirOperatorList) pDirOperatorList->setURL(newurl, cl);
	if(pDirOperatorIcon) pDirOperatorIcon->setURL(newurl, cl);
	if(pDirOperatorDetail) pDirOperatorDetail->setURL(newurl, cl);
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
	if(pDirOperatorList) pDirOperatorList->setURL(newurl, cl);
	if(pDirOperatorIcon) pDirOperatorIcon->setURL(newurl, cl);
	if(pDirOperatorDetail) pDirOperatorDetail->setURL(newurl, cl);
}

void SQ_WidgetStack::slotSetIconBigger()
{
	QWidget *vis = visibleWidget();
	if(vis == pDirOperatorList && iCurrentListIndex != (signed)pIconSizeList->count()-1)
	{
		iCurrentListIndex++;
		int iconsize = (*pIconSizeList)[iCurrentListIndex];
		pDirOperatorList->setIconSize(iconsize);
	}
	else if(vis == pDirOperatorIcon && iCurrentIconIndex != (signed)pIconSizeList->count()-1)
	{
		iCurrentIconIndex++;
		int iconsize = (*pIconSizeList)[iCurrentIconIndex];
		pDirOperatorIcon->setIconSize(iconsize);
	}
}

void SQ_WidgetStack::slotSetIconSmaller()
{
	QWidget *vis = visibleWidget();
	if(vis == pDirOperatorList && (unsigned)iCurrentListIndex != 0)
	{
		iCurrentListIndex--;
		int iconsize = (*pIconSizeList)[iCurrentListIndex];
		pDirOperatorList->setIconSize(iconsize);
	}
	else if(vis == pDirOperatorIcon && (unsigned)iCurrentIconIndex != 0)
	{
		iCurrentIconIndex--;
		int iconsize = (*pIconSizeList)[iCurrentIconIndex];
		pDirOperatorIcon->setIconSize(iconsize);
	}
}

void SQ_WidgetStack::slotPrevious()
{
	KFileView *local;

	local = ((SQ_DirOperator*)visibleWidget())->fileview;

	local->clearSelection();
	local->setCurrentItem(local->prevItem(local->currentFileItem()));
	local->setSelected(local->currentFileItem(), true);
}

void SQ_WidgetStack::slotNext()
{
	KFileView *local;

	local = ((SQ_DirOperator*)visibleWidget())->fileview;

	local->clearSelection();
	local->setCurrentItem(local->nextItem(local->currentFileItem()));
	local->setSelected(local->currentFileItem(), true);
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

	if(pDirOperatorList) pDirOperatorList->actionCollection()->action("reload")->activate();
	if(pDirOperatorIcon) pDirOperatorIcon->actionCollection()->action("reload")->activate();
	if(pDirOperatorDetail) pDirOperatorDetail->actionCollection()->action("reload")->activate();
}

const QString SQ_WidgetStack::getNameFilter() const
{
	return ((SQ_DirOperator*)visibleWidget())->nameFilter();
}

void SQ_WidgetStack::raiseWidget(int id)
{
	// load views only on call.
	if(id == 0 && pDirOperatorList == 0L)
	{
		pDirOperatorList = new SQ_DirOperator(KURL(((path)?*path:"/")));
		pDirOperatorList->readConfig(KGlobal::config(), "squirrel image viewer file browser");
		pDirOperatorList->setViewConfig(KGlobal::config(), "squirrel image viewer file browser");
		pDirOperatorList->setMode(KFile::Files);
		pDirOperatorList->setView(KFile::Simple);
		pDirOperatorList->view()->actionCollection()->action(2)->activate();
		if(!path) pDirOperatorList->setURL(getURL(), true);
		int iconsize = (*pIconSizeList)[iCurrentListIndex];
		pDirOperatorList->setIconSize(iconsize);

		connect(pDirOperatorList, SIGNAL(urlEntered(const KURL&)), SLOT(setURL(const KURL&)));
		addWidget(pDirOperatorList, 0);
	}
	if(id == 1 && pDirOperatorIcon == 0L)
	{
		pDirOperatorIcon = new SQ_DirOperator(KURL("/"));
		pDirOperatorIcon->readConfig(KGlobal::config(), "squirrel image viewer file browser");
		pDirOperatorIcon->setViewConfig(KGlobal::config(), "squirrel image viewer file browser");
		pDirOperatorIcon->setMode(KFile::Files);
		pDirOperatorIcon->setView(KFile::Simple);
		pDirOperatorIcon->view()->actionCollection()->action(1)->activate();
		if(!path) pDirOperatorIcon->setURL(getURL(), true);
		int iconsize = (*pIconSizeList)[iCurrentIconIndex];
		pDirOperatorIcon->setIconSize(iconsize);

		connect(pDirOperatorIcon, SIGNAL(urlEntered(const KURL&)), SLOT(setURL(const KURL&)));
		addWidget(pDirOperatorIcon, 1);
	}
	else if(id == 2 &&  pDirOperatorDetail == 0L)
	{
		pDirOperatorDetail = new SQ_DirOperator(KURL("/"));
		pDirOperatorDetail->readConfig(KGlobal::config(), "squirrel image viewer file browser");
		pDirOperatorDetail->setViewConfig(KGlobal::config(), "squirrel image viewer file browser");
		pDirOperatorDetail->setMode(KFile::Files);
		pDirOperatorDetail->setView(KFile::Detail);
		if(!path) pDirOperatorDetail->setURL(getURL(), true);

		connect(pDirOperatorDetail, SIGNAL(urlEntered(const KURL&)), SLOT(setURL(const KURL&)));
		addWidget(pDirOperatorDetail, 2);
	}

	// and raise widget
	QWidgetStack::raiseWidget(id);
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

	sqConfig->setGroup("Fileview");
	switch(sqConfig->readNumEntry("set path", 1))
	{
		case 2: *path = sqConfig->readEntry("custom directory", "/"); break;
		case 1: *path = ""; break;
		case 0: *path = sqConfig->readEntry("last visited", "/"); break;
		default: *path = "/";
	}

	raiseWidget(id);

	QString url = getURL().path();
	if(sqCurrentURL)
	{
		sqCurrentURL->setEditText(url);
		sqCurrentURL->addToHistory(url);
	}

        delete path;
        path = 0L;

	SQ_DirOperator *dirop = (SQ_DirOperator*)visibleWidget();
	
	pABack = dirop->actionCollection()->action("back");
	pAForw = dirop->actionCollection()->action("forward");
	pAUp = dirop->actionCollection()->action("up");
	pARefresh = dirop->actionCollection()->action("reload");
	pAHome = dirop->actionCollection()->action("home");
	pANewDir = dirop->actionCollection()->action("mkdir");
	pAProp = dirop->actionCollection()->action("properties");
	pADelete = dirop->actionCollection()->action("delete");
	pAIconSmaller = new KAction("Smaller icons", sqLoader->loadIcon("viewmag-", KIcon::Desktop, 22), 0, this, SLOT(slotSetIconSmaller()), sqApp->actionCollection(), "Set smaller icons");
	pAIconBigger = new KAction("Bigger icons", sqLoader->loadIcon("viewmag+", KIcon::Desktop, 22), 0, this, SLOT(slotSetIconBigger()), sqApp->actionCollection(), "Set bigger icons");
}
