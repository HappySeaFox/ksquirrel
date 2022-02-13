/***************************************************************************
                          sq_kipimanager.cpp  -  description
                             -------------------
    begin                :  Feb 5 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#include "sq_kipimanager.h"

#ifdef SQ_HAVE_KIPI

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qwidget.h>

#include <kactioncollection.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kdebug.h>

#include <libkipi/pluginloader.h>
#include <libkipi/interface.h>

#include "sq_kipiinterface.h"

SQ_ActionMenu::SQ_ActionMenu(const QString &text, QObject *parent, const char *name)
    : KActionMenu(text, parent, name)
{}

SQ_ActionMenu::SQ_ActionMenu(const QString &text, const QString &icon, QObject *parent, const char *name)
    : KActionMenu(text, icon, parent, name)
{}

SQ_ActionMenu::~SQ_ActionMenu()
{
    QValueVector<KAction *>::iterator itEnd = plugged.end();

    for(QValueVector<KAction *>::iterator it = plugged.begin();it != itEnd;++it)
        remove(*it);
}

void SQ_ActionMenu::insert(KAction *ka, int index)
{
    KActionMenu::insert(ka, index);

    plugged.append(ka);
}

/***********************************************************************/                                        

SQ_KIPIManager::SQ_KIPIManager(QWidget *_parent, const char *name) : QObject(_parent, name), parent(_parent)
{
    kdDebug() << "+SQ_KIPIManager" << endl;

    // Create a dummy "no plugin" action
    noPlugin = new KAction(i18n("No Plugins"), 0, 0, 0, (KActionCollection *)0/*actionCollection()*/, "no_plugin");
    noPlugin->setShortcutConfigurable(false);
    noPlugin->setEnabled(false);

    loaded = false;
    p = new KPopupMenu;

    connect(p, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShow()));

    SQ_KIPIInterface *interface = new SQ_KIPIInterface(_parent);
    mPluginLoader = new KIPI::PluginLoader(QStringList(), interface);
    connect(mPluginLoader, SIGNAL(replug()), this, SLOT(slotReplug()));
}

SQ_KIPIManager::~SQ_KIPIManager()
{
    kdDebug() << "-SQ_KIPIManager" << endl;

    delete mPluginLoader;

    //unplug all & delete popup menu
    clearMap();
    delete p;

    delete noPlugin;
}

void SQ_KIPIManager::loadPlugins()
{
    kdDebug() << "Loading KIPI plugins..." << endl;

    loaded = true;
    mPluginLoader->loadPlugins();

    kdDebug() << "Done" << endl;
}

void SQ_KIPIManager::clearMap()
{
    // delete menus
    for(CategoryMap::iterator it = cmenus.begin();it != cmenus.end();++it)
        delete it.data(); // destructor will do all things for us

    cmenus.clear();
}

void SQ_KIPIManager::slotReplug()
{
    // Fill the mActions
    KIPI::PluginLoader::PluginList pluginList = mPluginLoader->pluginList();
    KIPI::PluginLoader::PluginList::ConstIterator it(pluginList.begin());
    KIPI::PluginLoader::PluginList::ConstIterator itEnd(pluginList.end());
    KIPI::Plugin *plugin;
    CategoryMap::iterator fnd;

    clearMap();

    SQ_ActionMenu *menuImages  = new SQ_ActionMenu(i18n("Image actions"));
    SQ_ActionMenu *menuEffects = new SQ_ActionMenu(i18n("Effects"));
    SQ_ActionMenu *menuTools   = new SQ_ActionMenu(i18n("Tools"));
    SQ_ActionMenu *menuImport  = new SQ_ActionMenu(i18n("Import"));
    SQ_ActionMenu *menuExport  = new SQ_ActionMenu(i18n("Export"));
    SQ_ActionMenu *menuBatch   = new SQ_ActionMenu(i18n("Batch processing"));
    SQ_ActionMenu *menuColl    = new SQ_ActionMenu(i18n("Collections"));

    cmenus[KIPI::IMAGESPLUGIN]      = menuImages;
    cmenus[KIPI::EFFECTSPLUGIN]     = menuEffects;
    cmenus[KIPI::TOOLSPLUGIN]       = menuTools;
    cmenus[KIPI::IMPORTPLUGIN]      = menuImport;
    cmenus[KIPI::EXPORTPLUGIN]      = menuExport;
    cmenus[KIPI::BATCHPLUGIN]       = menuBatch;
    cmenus[KIPI::COLLECTIONSPLUGIN] = menuColl;

    for( ;it != itEnd;++it)
    {   
        if(!(*it)->shouldLoad())
            continue;

	plugin = (*it)->plugin();

        if (!plugin) continue;

        plugin->setup(parent);

        KActionPtrList actions = plugin->actions();
        KActionPtrList::ConstIterator actionIt = actions.begin(), end = actions.end();
        KIPI::Category category;

        for ( ;actionIt != end;++actionIt)
        {
            category = plugin->category(*actionIt);

            fnd = cmenus.find(category);

            if(fnd == cmenus.end())
                continue;

            fnd.data()->insert(*actionIt);
        }

        plugin->actionCollection()->readShortcutSettings();
    }

    // plug
    for(CategoryMap::iterator it = cmenus.begin();it != cmenus.end();++it)
    {
        if(!it.data()->count())
            it.data()->insert(noPlugin);

        it.data()->plug(p);
    }
}

void SQ_KIPIManager::slotAboutToShow()
{
    // load KIPI plugins on demand
    if(!loaded) loadPlugins();
}

#include "sq_kipimanager.moc"

#endif
