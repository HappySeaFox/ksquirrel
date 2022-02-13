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

SQ_WidgetStack::SQ_WidgetStack(QWidget *parent) : QWidgetStack(parent)
{
	pIconSizeList = new QValueList<int>;
	pIconSizeList->append(16);
	pIconSizeList->append(22);
	pIconSizeList->append(32);
	pIconSizeList->append(48);
	pIconSizeList->append(64);
	pIconSizeList->append(96);
	pIconSizeList->append(128);
	pIconSizeList->append(192);
	pIconSizeList->append(256);

	sqConfig->setGroup("Interface");
	iCurrentListIndex = sqConfig->readNumEntry("iCurrentListIndex", 0);
	iCurrentIconIndex = sqConfig->readNumEntry("iCurrentIconIndex", 2);

	pDirOperatorList = 0L;
	pDirOperatorIcon = 0L;
	pDirOperatorDetail = 0L;

	// pDirOperatorIcon & pDirOperatorDetail will be loaded later, if I'll press "Icon View" or "Detailed view" button on toolbar.
	// we needn't load them at the beginning, by default only pDirOperatorList exists.
	pDirOperatorList = new SQ_DirOperator(KURL("/"));
	pDirOperatorList->readConfig(sqConfig, "file browser");
	pDirOperatorList->setViewConfig(sqConfig, "file browser");
	pDirOperatorList->setMode(KFile::Files);
	pDirOperatorList->setView(KFile::Simple);
	pDirOperatorList->view()->actionCollection()->action(2)->activate();

	int iconsize = *(pIconSizeList->at(iCurrentListIndex));
	pDirOperatorList->setIconSize(iconsize);

	pABack = pDirOperatorList->actionCollection()->action("back");
	pAForw = pDirOperatorList->actionCollection()->action("forward");
	pAUp = pDirOperatorList->actionCollection()->action("up");
	pARefresh = pDirOperatorList->actionCollection()->action("reload");
	pAHome = pDirOperatorList->actionCollection()->action("home");
	pANewDir = pDirOperatorList->actionCollection()->action("mkdir");
	pAProp = pDirOperatorList->actionCollection()->action("properties");
	pADelete = pDirOperatorList->actionCollection()->action("delete");
	pAIconSmaller = new KAction("Smaller icons", sqLoader->loadIcon("viewmag-", KIcon::Desktop, 22), KShortcut(CTRL+Key_Minus), this, SLOT(slotSetIconSmaller()), sqApp->actionCollection(), "Set smaller icons");
	pAIconBigger = new KAction("Bigger icons", sqLoader->loadIcon("viewmag+", KIcon::Desktop, 22), KShortcut(CTRL+Key_Plus), this, SLOT(slotSetIconBigger()), sqApp->actionCollection(), "Set bigger icons");

	connect(pDirOperatorList, SIGNAL(urlEntered(const KURL&)), SLOT(setURL(const KURL&)));

	addWidget(pDirOperatorList, 0);
}

SQ_WidgetStack::~SQ_WidgetStack()
{}

KURL SQ_WidgetStack::getURL() const
{
	return  pDirOperatorList->url();
}

void SQ_WidgetStack::setURL(const QString &newpath, bool cl)
{
	KURL url(newpath);
	pDirOperatorList->setURL(url, cl);
	if(pDirOperatorIcon) pDirOperatorIcon->setURL(url, cl);
	if(pDirOperatorDetail) pDirOperatorDetail->setURL(url, cl);
}

void SQ_WidgetStack::setURL(const KURL &newurl, bool cl)
{
	if(sqCurrentURL) sqCurrentURL->setEditText(newurl.path());
	if(sqCurrentURL) sqCurrentURL->addToHistory(newurl.path());
	pDirOperatorList->setURL(newurl, cl);
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
	pDirOperatorList->setURL(newurl, cl);
	if(pDirOperatorIcon) pDirOperatorIcon->setURL(newurl, cl);
	if(pDirOperatorDetail) pDirOperatorDetail->setURL(newurl, cl);
}

void SQ_WidgetStack::slotSetIconBigger()
{
	QWidget *vis = visibleWidget();
	if(vis == pDirOperatorList && iCurrentListIndex != (signed)pIconSizeList->count()-1)
	{
		iCurrentListIndex++;
		int iconsize = *(pIconSizeList->at(iCurrentListIndex));
		pDirOperatorList->setIconSize(iconsize);
	}
	else if(vis == pDirOperatorIcon && iCurrentIconIndex != (signed)pIconSizeList->count()-1)
	{
		iCurrentIconIndex++;
		int iconsize = *(pIconSizeList->at(iCurrentIconIndex));
		pDirOperatorIcon->setIconSize(iconsize);
	}
}

void SQ_WidgetStack::slotSetIconSmaller()
{
	QWidget *vis = visibleWidget();
	if(vis == pDirOperatorList && (unsigned)iCurrentListIndex != 0)
	{
		iCurrentListIndex--;
		int iconsize = *(pIconSizeList->at(iCurrentListIndex));
		pDirOperatorList->setIconSize(iconsize);
	}
	else if(vis == pDirOperatorIcon && (unsigned)iCurrentIconIndex != 0)
	{
		iCurrentIconIndex--;
		int iconsize = *(pIconSizeList->at(iCurrentIconIndex));
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
	pDirOperatorList->actionCollection()->action("show hidden")->activate();
}

void SQ_WidgetStack::setNameFilter(const QString &f)
{
	pDirOperatorList->setNameFilter(f);
	if(pDirOperatorIcon) pDirOperatorIcon->setNameFilter(f);
	if(pDirOperatorDetail) pDirOperatorDetail->setNameFilter(f);

	pDirOperatorList->updateDir();
	if(pDirOperatorIcon) pDirOperatorIcon->actionCollection()->action("reload")->activate();
	if(pDirOperatorDetail) pDirOperatorDetail->actionCollection()->action("reload")->activate();
}

void SQ_WidgetStack::raiseWidget(int id)
{
	// load views only on call.
	if(id == 1 && pDirOperatorIcon == 0L)
	{
		pDirOperatorIcon = new SQ_DirOperator(KURL("/"));
		pDirOperatorIcon->readConfig(sqConfig, "file browser");
		pDirOperatorIcon->setViewConfig(sqConfig, "file browser");
		pDirOperatorIcon->setMode(KFile::Files);
		pDirOperatorIcon->setView(KFile::Simple);
		pDirOperatorIcon->view()->actionCollection()->action(1)->activate();
		pDirOperatorIcon->setNameFilter(pDirOperatorList->nameFilter());
		pDirOperatorIcon->setURL(getURL(), true);
		int iconsize = *(pIconSizeList->at(iCurrentIconIndex));
		pDirOperatorIcon->setIconSize(iconsize);

		connect(pDirOperatorIcon, SIGNAL(urlEntered(const KURL&)), this, SLOT(setURL(const KURL&)));
		addWidget(pDirOperatorIcon, 1);
	}
	else if(id == 2 &&  pDirOperatorDetail == 0L)
	{
		pDirOperatorDetail = new SQ_DirOperator(KURL("/"));
		pDirOperatorDetail->readConfig(sqConfig, "file browser");
		pDirOperatorDetail->setViewConfig(sqConfig, "file browser");
		pDirOperatorDetail->setMode(KFile::Files);
		pDirOperatorDetail->setView(KFile::Detail);
		pDirOperatorDetail->setNameFilter(pDirOperatorList->nameFilter());
		pDirOperatorDetail->setURL(getURL(), true);

		connect(pDirOperatorDetail, SIGNAL(urlEntered(const KURL&)), this, SLOT(setURL(const KURL&)));
		addWidget(pDirOperatorDetail, 2);
	}

	// and raise widget
	QWidgetStack::raiseWidget(id);
}