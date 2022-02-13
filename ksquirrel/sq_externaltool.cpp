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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qfile.h>

#include <kstringhandler.h>
#include <klocale.h>
#include <kicontheme.h>
#include <kstandarddirs.h>

#include "ksquirrel.h"
#include "sq_iconloader.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_externaltool.h"
#include "sq_popupmenu.h"
#include "sq_config.h"

SQ_ExternalTool * SQ_ExternalTool::m_instance = 0;

Tool::Tool()
{}

Tool::Tool(const QString &pix, const QString &nam, const QString &com)
{
    icon = pix;
    name = nam;
    command = com;
}

SQ_ExternalTool::SQ_ExternalTool(QObject *parent) : QObject(parent), QValueVector<Tool>()
{
    m_instance = this;
    menu = new SQ_PopupMenu(0, "External tools");

    connect(menu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowMenu()));

    QString str, tmp;

    SQ_Config::instance()->setGroup("External tools");

    QStringList names = SQ_Config::instance()->readListEntry("names");
    QStringList commands = SQ_Config::instance()->readListEntry("commands");
    QStringList icons = SQ_Config::instance()->readListEntry("icons");

    for(QStringList::iterator it_n = names.begin(),it_c = commands.begin(),it_i = icons.begin();
            it_n != names.end() || it_c != commands.end() || it_i != icons.end();
            ++it_n, ++it_c, ++it_i)
    {
        append(Tool(*it_i, *it_n, *it_c));
    }
}

SQ_ExternalTool::~SQ_ExternalTool()
{
    delete menu;
}

QString SQ_ExternalTool::toolPixmap(const int i)
{
    return (*this)[i].icon;
}

QString SQ_ExternalTool::toolName(const int i)
{
    return (*this)[i].name;
}

QString SQ_ExternalTool::toolCommand(const int i)
{
    return (*this)[i].command;
}

/*
 *  Recreate current popop menu.
 */
SQ_PopupMenu* SQ_ExternalTool::newPopupMenu()
{
    int id;

    // clear menu
    menu->clear();

    menu->insertTitle(i18n("No file selected"));

    // construct new menu
    for(unsigned int i = 0;i < count();i++)
    {
        id = menu->insertItem(SQ_IconLoader::instance()->loadIcon(toolPixmap(i), KIcon::Desktop, KIcon::SizeSmall), toolName(i));
        menu->setItemParameter(id, i);
    }

    return menu;
}

/*
 *  Get current popup menu.
 */
SQ_PopupMenu* SQ_ExternalTool::constPopupMenu() const
{
    return menu;
}

/*
 *  Write tools to config file
 */
void SQ_ExternalTool::writeEntries()
{
    // no tools ?
    if(!count()) return;

    QString num;

    // delete old group with old items
    SQ_Config::instance()->deleteGroup("External tools");
    SQ_Config::instance()->setGroup("External tools");
    QStringList names, icons, commands;

    // write items in config file
    for(QValueVector<Tool>::iterator it = begin();it != end();++it)
    {
        names.append((*it).name);
        icons.append((*it).icon);
        commands.append((*it).command);
    }

    SQ_Config::instance()->writeEntry("names", names);
    SQ_Config::instance()->writeEntry("commands", commands);
    SQ_Config::instance()->writeEntry("icons", icons);
}

/*
 *  Invoked, when user executed popup menu with external tools.
 *  This slot will do some useful stuff.
 */

void SQ_ExternalTool::aboutToShowMenu(SQ_PopupMenu *m)
{
    // get selected items in filemanager
    KFileItemList *items = const_cast<KFileItemList *>(SQ_WidgetStack::instance()->selectedItems());

    if(!items || !items->count())
    {
        m->changeTitle(i18n("No file selected"));
        return;
    }

    KFileItem *item = items->first();

    if(!item)
    {
        m->changeTitle(i18n("No file selected"));
        return;
    }

    // make title shorter
    QString file = KStringHandler::rsqueeze(item->name(), 30);

    // finally, change title
    if(items)
    {
        QString final = (items->count() == 1 || items->count() == 0) ? file : (file + QString::fromLatin1(" (+%1)").arg(items->count()-1));
        m->changeTitle(final);
    }
    else
        m->changeTitle(file);
}

void SQ_ExternalTool::slotAboutToShowMenu()
{
    SQ_ExternalTool::aboutToShowMenu(menu);
}

#include "sq_externaltool.moc"
