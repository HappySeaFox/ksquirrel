/***************************************************************************
                          sq_externaltool.h  -  description
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

#ifndef SQ_EXTERNALTOOL_H
#define SQ_EXTERNALTOOL_H

#include <qstring.h>
#include <qobject.h>
#include <qvaluevector.h>

class KPopupMenu;

struct Tool
{
    Tool();
    Tool(const QString &, const QString &, const QString &);

    QString name, command;
    QString icon;
};

/*
 *  Class which manages external tools. It store all available external tools
 *  in memory, and create popup menu where external tools been inserted.
 */

class SQ_ExternalTool : public QObject, public QValueVector<Tool>
{
    Q_OBJECT

    public: 
        SQ_ExternalTool(QObject *parent);
        ~SQ_ExternalTool();

        /*
         *  Get pixmap, name or command of external tool.
         */
        QString toolPixmap(const int i);
        QString toolName(const int i);
        QString toolCommand(const int i);

        /*
         *  Recreate current popop menu.
         */
        KPopupMenu* newPopupMenu();

        /*
         *  Get current popup menu.
         */
        KPopupMenu* constPopupMenu() const;

        /*
         *  Write current state to config file
         */
        void writeEntries();

        static SQ_ExternalTool* instance() { return m_instance; }

    private slots:
        /*
         *  Invoked, when user executed popup menu with external tools.
         *  This slot will do some useful stuff.
         */
        void slotAboutToShowMenu();

    private:
        /*
         *  Popup menu with all tools.
         */
        KPopupMenu *menu;

        /*
         *  Id of menu's title.
         */
        int title;

        static SQ_ExternalTool *m_instance;
};

#endif
