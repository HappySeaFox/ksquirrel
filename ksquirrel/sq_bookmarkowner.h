/***************************************************************************
                          sq_bookmarkowner.h  -  description
                             -------------------
    begin                : ??? ??? 27 2004
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

#ifndef SQ_BOOKMARKOWNER_H
#define SQ_BOOKMARKOWNER_H

#include <kbookmarkmanager.h>
#include <kurl.h>

#include <qobject.h>
#include <qwidget.h>

/*
 *  Class for managing bookmarks. It used in main class (KSquirrel::initBookmarks())
 *  and in SQ_WidgetStack to store and manage bookmarks.
 */

class SQ_BookmarkOwner : public QObject, public KBookmarkOwner
{
    Q_OBJECT

    public: 
        SQ_BookmarkOwner(QWidget *parent = 0);
        ~SQ_BookmarkOwner();

        /*
         *  User selected some bookmark. It will emit 'openURL' signal,
         *  and SQ_WidgetStack will catch it.
         */
        virtual void openBookmarkURL(const QString &);

        /*
         *  Current url. If user selected "Add bookmark", this
         *  url will be added to bookmarks. See also SQ_BookmarkOwner::setURL()
         */
        virtual QString currentURL() const;
    
        static SQ_BookmarkOwner* instance();

        /*
         *  Set current url
         */
    public slots:
        void setURL(const KURL &);

        /*
         *  Signal to open some url. SQ_WidgetStack will catch it
         *  and change current directory.
         */
    signals:
        void openURL(const KURL &);

    private:
        KURL URL;

        // singleton
        static SQ_BookmarkOwner *sing;
};

#endif
