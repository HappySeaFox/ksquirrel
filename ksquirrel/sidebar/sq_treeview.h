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

class SQ_TreeViewItem;
class SQ_ThreadDirLister;
class SQ_TreeViewMenu;

/*
 *  We should subclass KFileTreeBranch to let us create
 *  our own items. See SQ_TreeViewitem.
 */
class SQ_FileTreeViewBranch : public KFileTreeBranch
{
    public:
        SQ_FileTreeViewBranch(KFileTreeView*, const KURL &url, const QString &name, const QPixmap &pix);
        ~SQ_FileTreeViewBranch();

    protected:
        virtual KFileTreeViewItem *createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem);
};

/*
 *  SQ_TreeView represents a file tree.
 */

class SQ_TreeView : public KFileTreeView
{
    Q_OBJECT

    public:
        SQ_TreeView(QWidget *parent = 0, const char *name = 0);
        ~SQ_TreeView();

        enum Recursion { No = 0, Files, Dirs, FilesDirs };

        /*
         *  Recursion settings. If recursion > 0, treeview will
         *  show a number of files/directories in the given directory.
         *  It will look like that:
         *
         *  + mypictures [8]
         *  | + wallpapers [231]
         *  | + nature [12]
         *  | + pets [43]
         *    | + cats [22]
         *    | + dogs [32]
         *
         *  This operation is threaded.
         */
        void setupRecursion();

        /*
         *  Load new url, if tree is visible. Save url and do nothing
         *  otherwise.
         */
        void emitNewURL(const KURL &url);

        virtual void clearSelection();
        virtual void setSelected(QListViewItem *item, bool selected);
        virtual void setCurrentItem(QListViewItem *item);
        virtual void setOpen(QListViewItem *item, bool open);

        static SQ_TreeView* instance() { return m_instance; }

    protected:
        virtual void customEvent(QCustomEvent *e);
        virtual void startAnimation(KFileTreeViewItem* item, const char*, uint);
        virtual void stopAnimation(KFileTreeViewItem* item);
        virtual void viewportResizeEvent(QResizeEvent *);
        virtual void contentsMousePressEvent(QMouseEvent *e);
        virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);

        /*
         *  On show event load saved url, if any. See emitNewURL().
         */
        virtual void showEvent(QShowEvent *);

    private:
        void toggle(SQ_TreeViewItem *, bool, bool = false);
        void setRecursion(int);

        /*
         *  Set given item visible, current, and populate it.
         */
        void populateItem(KFileTreeViewItem *);

        /*
         *  Search first available url in variable 'paths'. Open found item.
         *  If item was found return true.
         */
        bool doSearch();

    public slots:
        void slotClearChecked();

        /*
         *  Item executed. Let's pass its url to SQ_WidgetStack (if needed).
         */
        void slotItemExecuted(QListViewItem*);

        /*
         *  New item is opened. Try to continue loading url.
         */
        void slotOpened(KFileTreeViewItem *);

    private slots:
        /*
         *  Load url.
         */
        void slotNewURL(const KURL &url);

        void slotNewTreeViewItems(KFileTreeBranch*, const KFileTreeViewItemList &);
        void slotDelayedScan();
        void slotAnimation();

        void slotDropped(QDropEvent *, QListViewItem *, QListViewItem *);
        void slotContextMenu(KListView *, QListViewItem *, const QPoint &);

    signals:
        void newURL(const KURL &url);

        /*
         *  Since 0.7.0 our file manager supports multiple directories.
         *  These signals tell SQ_DirOperator to add or remove some
         *  urls.
         */
        void urlAdded(const KURL &);
        void urlRemoved(const KURL &);

    private:
        SQ_FileTreeViewBranch *root;
        QStringList paths;
        KURL pendingURL;
        SQ_ThreadDirLister    *lister;
        KFileTreeViewItemList m_mapFolders;
        QTimer *m_animTimer, *scanTimer;
        bool m_ignoreClick;
        int m_recurs;
        SQ_TreeViewMenu *menu;

        static SQ_TreeView *m_instance;
};

#endif
