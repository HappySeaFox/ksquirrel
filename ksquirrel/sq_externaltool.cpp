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

#include "sq_externaltool.h"
#include "ksquirrel.h"
#include "sq_config.h"

SQ_ExternalTool::SQ_ExternalTool() : QValueVector<SQ_EXT_TOOL>()
{
	menu = 0L;
}

SQ_ExternalTool::~SQ_ExternalTool()
{}

void SQ_ExternalTool::addTool(const QString &name, const QString &command)
{
	 SQ_EXT_TOOL tool = {name, command};

	 append(tool);
}

void SQ_ExternalTool::addTool(const SQ_EXT_TOOL &tool)
{
	append(tool);
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
	if(menu)
	{
		menu->clear();
		delete menu;
	}

	int id;

	menu = new QPopupMenu;

	for(unsigned int i = 0;i < count();i++)
	{
		id = menu->insertItem(getToolName(i));
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
	int ncount = count(), cur = 1;
	QString num;

	for(int i = 0;i < ncount;i++,cur++)
	{
		sqConfig->setGroup("External tool name");
		num.sprintf("%d", cur);
		sqConfig->writeEntry(num, getToolName(i));
		sqConfig->setGroup("External tool program");
		sqConfig->writeEntry(num, getToolCommand(i));
	}
}
