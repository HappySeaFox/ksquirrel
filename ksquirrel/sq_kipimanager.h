/***************************************************************************
                          sq_kipimanager.h  -  description
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


#ifndef SQ_KIPIMANAGER_H
#define SQ_KIPIMANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef SQ_HAVE_KIPI

#include <qmap.h>
#include <qvaluevector.h>

#include <kaction.h>

#include <libkipi/plugin.h>

namespace KIPI { class PluginLoader; }

class QWidget;

class KPopupMenu;

/*
 *  This subclass of KActionMenu should know which KActions
 *  are currently plugged in it.
 */
class SQ_ActionMenu : public KActionMenu
{
    public:
        SQ_ActionMenu(const QString &text, QObject *parent = 0, const char *name = 0);
        SQ_ActionMenu(const QString &text, const QString &icon, QObject *parent = 0, const char *name = 0);

        /*
         *  remove() all KActions currently plugged.
         */
        ~SQ_ActionMenu();

        /*
         *  Reimplement insert() to let us remember "ka"
         */
        void insert(KAction *ka, int index = -1);

        int count() const;

    private:
        // Currently plugged KActions
        QValueVector<KAction *> plugged;
};

inline
int SQ_ActionMenu::count() const
{
    return plugged.count();
}

/*
 *  KIPI plugins manager. It loads KIPI plugins and creates popup
 *  menu with all available actions.
 */
class SQ_KIPIManager : public QObject
{
    Q_OBJECT

    public:
	SQ_KIPIManager(QWidget *_parent, const char *name = 0);
	~SQ_KIPIManager();

        KPopupMenu* popup() const;

        void loadPlugins();

    private:
        void clearMap();

    private slots:
        void slotReplug();
        void slotAboutToShow();

    private:
        typedef QMap<KIPI::Category, SQ_ActionMenu*> CategoryMap;

        KIPI::PluginLoader *mPluginLoader;
        CategoryMap cmenus;
        KAction *noPlugin;
        QWidget *parent;
        KPopupMenu *p;
        bool loaded;
};

inline
KPopupMenu* SQ_KIPIManager::popup() const
{
     return p;
}

#endif

#endif
