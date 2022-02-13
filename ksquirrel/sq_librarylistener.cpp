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
	connect(this, SIGNAL(showInfo(const QStringList &)), SLOT(slotShowInfo(const QStringList &)));
}

SQ_LibraryListener::~SQ_LibraryListener()
{}

void SQ_LibraryListener::slotStarted(const KURL &_url)
{
	url = _url;
}

void SQ_LibraryListener::slotCompleted()
{

}

void SQ_LibraryListener::slotNewItems(const KFileItemList &items)
{
	KFileItemListIterator	it(items);
	KFileItem 			*item;
	QStringList			list;
	QString				stritems = "";
	
	while((item = it.current()) != 0)
	{
		++it;
		if(item->isFile() && item)
			stritems = stritems + url.path() + "/" + item->name() + "\n";
	}

	list = QStringList::split("\n", stritems);
	sqLibHandler->add(&list);

 	sqConfig->setGroup("Libraries");

	if(sqConfig->readBoolEntry("show dialog", true))
		emit showInfo(list);

	setAutoUpdate(sqConfig->readBoolEntry("monitor", true));
}

void SQ_LibraryListener::slotDeleteItem(KFileItem *item)
{

}

void SQ_LibraryListener::slotShowInfo(const QStringList &linfo)
{
	SQ_LibrariesChanged cd(sqApp);
	cd.setLibsInfo(linfo, true);
	cd.exec();
}
