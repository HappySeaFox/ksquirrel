/***************************************************************************
                          sq_librarylistener.cpp  -  description
                             -------------------
    begin                : Fri Mar 26 2004
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

#include <qstring.h>
#include <qfile.h>

#include "ksquirrel.h"
#include "sq_librarylistener.h"
#include "sq_libraryhandler.h"
#include "sq_librarieschanged.h"
#include "sq_config.h"

SQ_LibraryListener * SQ_LibraryListener::listener = 0L;

SQ_LibraryListener::SQ_LibraryListener(bool delayed) : KDirLister(delayed)
{
	listener = this;
	operation = true;

	setAutoUpdate(true);
	setDirOnlyMode(false);
	setShowingDotFiles(false);

	connect(this, SIGNAL(completed()), this, SLOT(slotCompleted()));
	connect(this, SIGNAL(newItems(const KFileItemList &)), this, SLOT(slotNewItems(const KFileItemList &)));
	connect(this, SIGNAL(deleteItem(KFileItem *)), this, SLOT(slotDeleteItem(KFileItem *)));
	connect(this, SIGNAL(showInfo(const QStringList &,bool)), this, SLOT(slotShowInfo(const QStringList &,bool)));
}

SQ_LibraryListener::~SQ_LibraryListener()
{}

void SQ_LibraryListener::slotCompleted()
{
	if(operation)
	{
		if(list.count())
			SQ_LibraryHandler::instance()->add(&list);

		emit finishedInit();
	}
	else
		if(list.count())
			SQ_LibraryHandler::instance()->remove(&list);

	setAutoUpdate(SQ_Config::instance()->readBoolEntry("Libraries", "monitor", true));

	if(SQ_Config::instance()->readBoolEntry("Libraries", "monitor", true))
		if(SQ_Config::instance()->readBoolEntry("Libraries", "show dialog", true))
			emit showInfo(list, operation);
		else
			list.clear();
}

void SQ_LibraryListener::slotNewItems(const KFileItemList &items)
{
	KFileItemListIterator	it(items);

	KFileItem 		*item;
	QString		stritems;

	while((item = it.current()) != 0)
	{
		++it;
		if(item->isFile() && item)
			stritems = stritems + url().path() + "/" + item->name() + "\n";
	}

	list = list + QStringList::split("\n", stritems);

	operation = true;
}

void SQ_LibraryListener::slotDeleteItem(KFileItem *item)
{
	QString stritems("");

	if(item->isFile() && item)
	{
		stritems = stritems + url().path() + "/" + item->name() + "\n";
		list = list + QStringList::split("\n", stritems);
	}

	operation = false;
}

void SQ_LibraryListener::slotShowInfo(const QStringList &linfo, bool added)
{
	SQ_LibrariesChanged cd(KSquirrel::app());
	cd.setLibsInfo(linfo, added);
	cd.exec();

	list.clear();
}

void SQ_LibraryListener::slotOpenURL(const KURL &url, bool b1, bool b2)
{
	if(!QFile::exists(url.path()))
	{
	    SQ_LibraryHandler::instance()->clear();
	    emit finishedInit();
	    return;
	}

	openURL(url, b1, b2);
}

SQ_LibraryListener* SQ_LibraryListener::instance()
{
	return listener;
}
