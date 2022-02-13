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

#ifndef SQ_SMALL

#include "ksquirrel.h"
#include "sq_librarieschanged.h"
#include "sq_config.h"

#endif

#include "sq_librarylistener.h"
#include "sq_libraryhandler.h"

SQ_LibraryListener * SQ_LibraryListener::listener = NULL;

SQ_LibraryListener::SQ_LibraryListener(bool delayed) : KDirLister(delayed)
{
	listener = this;
	operation = true;

	// some setup ...
	setAutoUpdate(true);
	setDirOnlyMode(false);
	setShowingDotFiles(false);

	// connect signals
	connect(this, SIGNAL(completed()), this, SLOT(slotCompleted()));
	connect(this, SIGNAL(newItems(const KFileItemList &)), this, SLOT(slotNewItems(const KFileItemList &)));
	connect(this, SIGNAL(deleteItem(KFileItem *)), this, SLOT(slotDeleteItem(KFileItem *)));

#ifndef SQ_SMALL

	connect(this, SIGNAL(showInfo(const QStringList &,bool)), this, SLOT(slotShowInfo(const QStringList &,bool)));

#endif

}

SQ_LibraryListener::~SQ_LibraryListener()
{}

/*
 *  All operations completed.
 */
void SQ_LibraryListener::slotCompleted()
{
	// operation == true means, that new items are added
	if(operation)
	{
		if(list.count())
			SQ_LibraryHandler::instance()->add(&list);

		emit finishedInit();
	}
	// some items deleted
	else
		if(list.count())
			SQ_LibraryHandler::instance()->remove(&list);

#ifndef SQ_SMALL

	setAutoUpdate(SQ_Config::instance()->readBoolEntry("Libraries", "monitor", true));

	if(SQ_Config::instance()->readBoolEntry("Libraries", "monitor", true))
		if(SQ_Config::instance()->readBoolEntry("Libraries", "show dialog", true))
			emit showInfo(list, operation);

#endif

	list.clear();
}

/*
 *  New libraries found!
 */
void SQ_LibraryListener::slotNewItems(const KFileItemList &items)
{
	KFileItemListIterator	it(items);

	KFileItem 		*item;

	// append all items to list
	while((item = it.current()) != 0)
	{
		++it;

		if(item && item->isFile())
			list.append(url().path() + "/" + item->name());
	}

	operation = true;
}

/*
 *  An item deleted.
 */
void SQ_LibraryListener::slotDeleteItem(KFileItem *item)
{
	QString stritems;

	// append new items to list
	if(item && item->isFile())
		list.append(url().path() + "/" + item->name());

	operation = false;
}

#ifndef SQ_SMALL

void SQ_LibraryListener::slotShowInfo(const QStringList &linfo, bool added)
{
	SQ_LibrariesChanged cd(KSquirrel::app());
	cd.setLibsInfo(linfo, added);
	cd.exec();

	list.clear();
}

#else

void SQ_LibraryListener::slotShowInfo(const QStringList &, bool)
{}

#endif

/*
 *  Start monitor given directory.
 */
void SQ_LibraryListener::slotOpenURL(const KURL &url, bool b1, bool b2)
{
	// directory doesn't exist!
	if(!QFile::exists(url.path()))
	{
		// clear cached information
		SQ_LibraryHandler::instance()->clear();

		// all done
		emit finishedInit();

		return;
	}

	KDirLister::openURL(url, b1, b2);
}

SQ_LibraryListener* SQ_LibraryListener::instance()
{
	return listener;
}
