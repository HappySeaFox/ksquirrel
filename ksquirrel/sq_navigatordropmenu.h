/***************************************************************************
                          sq_navigatordropmenu.h  -  description
                             -------------------
    begin                : ??? Feb 23 2005
    copyright            : (C) 2005 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_NAVIGATORDROPMENU_H
#define SQ_NAVIGATORDROPMENU_H

#include <qobject.h>
#include <qpoint.h>

#include <kurl.h>

namespace KIO { class Job; }

class SQ_PopupMenu;

/*
 *  SQ_NavigatorDropMenu will help in drag-and-drop
 *  things. It creates popup menu with available actions
 *  (copy, move, ...), which will be executed by SQ_FileIconViewBase,
 *  when user dropped some files:
 *
 *  void SQ_FileIconViewBase::slotDropped(QDropEvent *, const KURL::List &urls, const KURL &_url)
 *  {
 *      KURL url = (_url.isEmpty()) ? SQ_WidgetStack::instance()->url() : _url;
 *
 *      SQ_NavigatorDropMenu::instance()->setupFiles(urls, url);
 *      SQ_NavigatorDropMenu::instance()->exec(QCursor::pos());
 *  }
 *  
 */

class SQ_NavigatorDropMenu : public QObject
{
    Q_OBJECT

    public:
        SQ_NavigatorDropMenu(QObject *parent = 0);
        ~SQ_NavigatorDropMenu();

        static SQ_NavigatorDropMenu* instance() { return m_instance; }

        enum FileAction { Copy = 0, Move, Link };

        /*
         *  Save destination url and urls of dropped files.
         */
        void setupFiles(const KURL::List &l, const KURL &u);

        /*
         *  Show popup menu with available actions.
         */
        void exec(const QPoint &pos, bool = false);

    private slots:
        /*
         *  These slots will use KIO to
         *  copy, move or link files.
         */
        void slotCopy();
        void slotMove();
        void slotLink();
        void slotJobResult(KIO::Job *);

    signals:
        void done(const KURL &, int);

    private:
        SQ_PopupMenu *dropmenu;
        KURL::List list;
        KURL url;
        bool also;

        static SQ_NavigatorDropMenu *m_instance;
};

#endif
