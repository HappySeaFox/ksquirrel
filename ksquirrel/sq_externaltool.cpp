/***************************************************************************
                          sq_externaltool.cpp  -  description
                             -------------------
    begin                : ??? ??? 12 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@uandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qpopupmenu.h>

#include <kiconloader.h>

#include "ksquirrel.h"
#include "sq_externaltool.h"
#include "sq_config.h"

SQ_ExternalTool::SQ_ExternalTool() : QValueVector<SQ_EXT_TOOL>()
{
	menu = new QPopupMenu;
}

SQ_ExternalTool::~SQ_ExternalTool()
{}

void SQ_ExternalTool::addTool(const QString &pixmap, const QString &name, const QString &command)
{
	SQ_EXT_TOOL tool = {pixmap, name, command};

	append(tool);
}

void SQ_ExternalTool::addTool(const SQ_EXT_TOOL &tool)
{
	append(tool);
}

QString SQ_ExternalTool::getToolPixmap(const int i)
{
	return (*this)[i].pixmap;
}

QString SQ_ExternalTool::getToolName(const int i)
{
	return (*this)[i].name;
}

QString SQ_ExternalTool::getToolCommand(const int i)
{
	return (*this)[i].command;
}

QPopupMenu* SQ_ExternalTool::getNewPopupMenu()
{
	int id;

	menu->clear();

	for(unsigned int i = 0;i < count();i++)
	{
		id = menu->insertItem(sqLoader->loadIcon(getToolPixmap(i), KIcon::Desktop, 16), getToolName(i));
		menu->setItemParameter(id, i);
	}

	return menu;
}

QPopupMenu* SQ_ExternalTool::getConstPopupMenu() const
{
	return menu;
}

void SQ_ExternalTool::writeEntries()
{
	int ncount = count(), cur = 1, i;
	QString num;

	for(i = 0;i < ncount;i++,cur++)
	{
		num.sprintf("%d", cur);
		sqConfig->setGroup("External tool pixmap");
		sqConfig->writeEntry(num, getToolPixmap(i));
		sqConfig->setGroup("External tool name");
		sqConfig->writeEntry(num, getToolName(i));
		sqConfig->setGroup("External tool program");
		sqConfig->writeEntry(num, getToolCommand(i));
	}
}
