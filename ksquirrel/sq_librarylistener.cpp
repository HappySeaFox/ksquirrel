/***************************************************************************
                          sq_librarylistener.cpp  -  description
                             -------------------
    begin                : Fri Mar 26 2004
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

#include "sq_librarylistener.h"
#include "ksquirrel.h"
#include "sq_libraryhandler.h"
#include "sq_librarieschanged.h"

#include <qstring.h>

SQ_LibraryListener::SQ_LibraryListener(bool delayed) : KDirLister(delayed)
{
       setAutoUpdate(true);
       setDirOnlyMode(false);
       setShowingDotFiles(false);

       connect(this, SIGNAL(started(const KURL &)), SLOT(slotStarted(const KURL &)));
       connect(this, SIGNAL(completed()), SLOT(slotCompleted()));
       connect(this, SIGNAL(newItems(const KFileItemList &)), SLOT(slotNewItems(const KFileItemList &)));
       connect(this, SIGNAL(deleteItem(KFileItem *)), SLOT(slotDeleteItem(KFileItem *)));
	connect(this, SIGNAL(showInfo(const QStringList &,bool)), SLOT(slotShowInfo(const QStringList &,bool)));
}

SQ_LibraryListener::~SQ_LibraryListener()
{}

void SQ_LibraryListener::slotStarted(const KURL &_url)
{
	url = _url;
}

void SQ_LibraryListener::slotCompleted()
{
        if(operation)
		sqLibHandler->add(&list);
	else
		sqLibHandler->remove(&list);

	setAutoUpdate(sqConfig->readBoolEntry("monitor", true));

 	sqConfig->setGroup("Libraries");
	if(sqConfig->readBoolEntry("show dialog", true))
		emit showInfo(list, operation);
	else
		list.clear();
}

void SQ_LibraryListener::slotNewItems(const KFileItemList &items)
{
	KFileItemListIterator	it(items);

	KFileItem 				*item;
	QString				stritems = "";
	
	while((item = it.current()) != 0)
	{
		++it;
		if(item->isFile() && item)
			stritems = stritems + url.path() + "/" + item->name() + "\n";
	}

	list = list + QStringList::split("\n", stritems);

	operation = true;
}

void SQ_LibraryListener::slotDeleteItem(KFileItem *item)
{
	QString				stritems = "";

	if(item->isFile() && item)
	{
		stritems = stritems + url.path() + "/" + item->name() + "\n";
		list = list + QStringList::split("\n", stritems);
	}

	operation = false;
}

void SQ_LibraryListener::slotShowInfo(const QStringList &linfo, bool added)
{
	SQ_LibrariesChanged cd(sqApp);
	cd.setLibsInfo(linfo, added);
	cd.exec();

	list.clear();
}
