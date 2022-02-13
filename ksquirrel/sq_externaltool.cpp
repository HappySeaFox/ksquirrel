/***************************************************************************
                          sq_externaltool.cpp  -  description
                             -------------------
    begin                : ??? ??? 12 2004
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

#include <qfile.h>

#include <kpopupmenu.h>
#include <kstringhandler.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include "ksquirrel.h"
#include "sq_iconloader.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_externaltool.h"
#include "sq_config.h"
#include "sq_dir.h"

SQ_ExternalTool * SQ_ExternalTool::ext = NULL;

SQ_ExternalTool::SQ_ExternalTool() : QObject(), QPtrList<KDesktopFile>()
{
	ext = this;
	menu = new KPopupMenu(NULL, "External tools");

	dir = new SQ_Dir(SQ_Dir::Desktops);

	connect(menu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowMenu()));

	QString str, tmp;

	for(int i = 1;;i++)
	{
		str.sprintf("%d", i);
		tmp = SQ_Config::instance()->readEntry("External tools", str, QString::null);

		if(tmp.isEmpty())
			break;

		QString path = dir->absPath(tmp + ".desktop");

		if(!QFile::exists(path))
			continue;

		KDesktopFile *d = new KDesktopFile(path);
		append(d);
	}
}

SQ_ExternalTool::~SQ_ExternalTool()
{}

void SQ_ExternalTool::addTool(const QString &pixmap, const QString &name, const QString &command)
{
	// construct a path to new .desktop file
	QString abs = dir->absPath(name + ".desktop");

	QFile::remove(abs);

	// create and init new KDesktopFile
	KDesktopFile *d = new KDesktopFile(abs);
	d->writeEntry("ServiceTypes", "*");
	d->writeEntry("Exec", command);
	d->writeEntry("Icon", pixmap);
	d->writeEntry("Name", name);

	// save it to disk now!
	d->sync();

	// add new entry
	append(d);
}

QString SQ_ExternalTool::toolPixmap(const int i)
{
	return at(i)->readIcon();
}

QString SQ_ExternalTool::toolName(const int i)
{
	return at(i)->readName();
}

QString SQ_ExternalTool::toolCommand(const int i)
{
	return at(i)->readEntry("Exec");
}

/*
 *  Recreate current popop menu.
 */
KPopupMenu* SQ_ExternalTool::newPopupMenu()
{
	int id;

	// clear menu
	menu->clear();

	title = menu->insertTitle(i18n("No file selected"));

	// construct new menu
	for(unsigned int i = 0;i < count();i++)
	{
		id = menu->insertItem(SQ_IconLoader::instance()->loadIcon(toolPixmap(i), KIcon::Desktop, 16), toolName(i));
		menu->setItemParameter(id, i);
	}

	return menu;
}

/*
 *  Get current popup menu.
 */
KPopupMenu* SQ_ExternalTool::constPopupMenu() const
{
	return menu;
}

/*
 *  Write current state to config file
 */
void SQ_ExternalTool::writeEntries()
{
	int ncount = count(), cur = 1, i;

	// no tools ?
	if(!ncount)
		return;

	QString num;

	// delete old group with old items
	SQ_Config::instance()->deleteGroup("External tools");
	SQ_Config::instance()->setGroup("External tools");

	// write items in config file
	for(i = 0;i < ncount;i++,cur++)
	{
		num.sprintf("%d", cur);
		SQ_Config::instance()->writeEntry(num, toolName(i));
	}
}

/*
 *  Invoked, when user executed popup menu with external tools.
 *  This slot will do some useful stuff.
 */
void SQ_ExternalTool::slotAboutToShowMenu()
{
	// get selected items in filemanager
	KFileItemList *items = (KFileItemList *)SQ_WidgetStack::instance()->visibleWidget()->selectedItems();

	if(!items || !items->count())
	{
		menu->changeTitle(title, i18n("No file selected"));
		return;
	}

	KFileItem *item = items->first();

	if(!item)
	{
		menu->changeTitle(title, i18n("No file selected"));
		return;
	}

	// make title shorter
	QString file = KStringHandler::rsqueeze(item->name(), 30);

	// finally, change title
	if(items)
	{
		QString final = (items->count() == 1 || items->count() == 0) ? file : (file + QString::fromLatin1(" (+%1)").arg(items->count()-1));
		menu->changeTitle(title, final);
	}
	else
		menu->changeTitle(title, file);
}

SQ_ExternalTool* SQ_ExternalTool::instance()
{
	return ext;
}
