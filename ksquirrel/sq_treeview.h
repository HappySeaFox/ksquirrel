/***************************************************************************
                          sq_treeview.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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

#ifndef SQ_TREEVIEW_H
#define SQ_TREEVIEW_H

#include <qstringlist.h>

#include <kfiletreeview.h>
#include <kurl.h>

/*
 *  SQ_TreeView represents a file tree.
 *
 *  TODO: calculate subpaths.
 */

class SQ_TreeView : public KFileTreeView
{
    Q_OBJECT

    public:
        SQ_TreeView(QWidget *parent = 0, const char *name = 0);
        ~SQ_TreeView();

        /*
         *  Load new url, if tree is visible. Save url and do nothing
         *  otherwise.
         */
        void emitNewURL(const KURL &url);

        /*
         *  Is tree visisble ?
         */
        bool configVisible() const;

        static SQ_TreeView* instance();

    protected:

        /*
         *  On show event load saved url, if any. See emitNewURL().
         */
        void showEvent(QShowEvent *);

    private:

        /*
         *  Set given item visible, current, and populate it.
         */
        void populateItem(KFileTreeViewItem *);

        /*
         *  Search first available url in variable 'paths'. Open found item.
         *  If item was found return true.
         */
        bool doSearch();

        /*
         *  Close all last opened items.
         *
         *  TODO: create option "[X] Collapse opened"
         */
        void collapseOpened();

    public slots:

        /*
         *  Item executed. Let's pass its url to SQ_WidgetStack (if needed).
         */
        void slotItemExecuted(QListViewItem*);

        /*
         *  New item is opened. Try to continue loading url.
         */
        void slotOpened(KFileTreeViewItem *);

        /*
         *  Set tree shown/hidden.
         */
        void setShown(bool shown);

    private slots:

        /*
         *  Load url.
         */
        void slotNewURL(const KURL &url);

    signals:
        void newURL(const KURL &url);

    private:
        KFileTreeBranch *root, *home;
        QStringList paths;
        KFileTreeViewItemList *itemsToClose;
        bool vis;
        KURL pendingURL;
        static SQ_TreeView *tree;
};

inline
bool SQ_TreeView::configVisible() const
{
    return vis;
}

#endif
