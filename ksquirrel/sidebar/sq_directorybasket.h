/***************************************************************************
                          sq_directorybasket.h  -  description
                             -------------------
    begin                : ??? Sep 29 2007
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

#ifndef SQ_DIRECTORYBASKET_H
#define SQ_DIRECTORYBASKET_H

#include <kfiletreeview.h>
#include <kfiletreebranch.h>

#include "sq_treeviewmenu.h"

class QTimer;

namespace KIO { class Job; }

class SQ_DirectoryItem;
class SQ_Dir;

class SQ_DBMenu : public SQ_TreeViewMenu
{
    Q_OBJECT

    public:
        SQ_DBMenu(QWidget *parent = 0, const char *name = 0);
        ~SQ_DBMenu();

        virtual void updateDirActions(bool, bool isroot = false);

        void setItem(SQ_DirectoryItem *);

    private slots:
        void slotChangeIcon();
        void slotDirectoryRename();
        void slotDirectoryDelete();
        void slotDirectoryResult(KIO::Job *job);

    private:
        SQ_DirectoryItem *item;
        int id_icon;
};

inline
void SQ_DBMenu::setItem(SQ_DirectoryItem *i)
{
    item = i;
}

class SQ_DirectoryItem : public KFileTreeViewItem
{
    public:
        SQ_DirectoryItem(KFileTreeViewItem *parentItem, KFileItem *fileItem, KFileTreeBranch *parentBranch);
        SQ_DirectoryItem(KFileTreeView *parent, KFileItem *fileItem, KFileTreeBranch *parentBranch);
        ~SQ_DirectoryItem();

        int index() const;
        void setIndex(int ind);

        KURL url() const;
        void setURL(const KURL &u);

        QString name() const;
        void setName(const QString &n);

        QString icon() const;
        void setIcon(const QString &n);

        bool hasName() const;

    private:
        int m_index;
        QString m_name, m_icon;
        KURL m_url;
};

inline
KURL SQ_DirectoryItem::url() const
{
    return m_url;
}

inline
void SQ_DirectoryItem::setURL(const KURL &u)
{
    m_url = u;
}

inline
int SQ_DirectoryItem::index() const
{
    return m_index;
}

inline
void SQ_DirectoryItem::setIndex(int ind)
{
    m_index = ind;
}

inline
QString SQ_DirectoryItem::name() const
{
    return m_name;
}

inline
void SQ_DirectoryItem::setName(const QString &n)
{
    m_name = n;
}

inline
QString SQ_DirectoryItem::icon() const
{
    return m_icon;
}

inline
void SQ_DirectoryItem::setIcon(const QString &n)
{
    m_icon = n;
}

inline
bool SQ_DirectoryItem::hasName() const
{
    return !m_name.isEmpty();
}

/* ****************************************************************** */

class SQ_DirectoryBasketBranch : public KFileTreeBranch
{
    public:
        SQ_DirectoryBasketBranch(KFileTreeView*, const KURL &url, const QString &name, const QPixmap &pix);
        ~SQ_DirectoryBasketBranch();

    protected:
        virtual KFileTreeViewItem *createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem);
};

class SQ_DirectoryBasket : public KFileTreeView
{
    Q_OBJECT

    public: 
        SQ_DirectoryBasket(QWidget *parent = 0, const char *name = 0);
        ~SQ_DirectoryBasket();

        void add(const KFileItemList &list);

        static SQ_DirectoryBasket* instance() { return m_inst; }

    private:
        void sort();

    private slots:
        void slotDropped(QDropEvent *, QListViewItem *, QListViewItem *);
        void slotItemExecuted(QListViewItem *item);
        void slotContextMenu(KListView *, QListViewItem *item, const QPoint &p);
        void slotNewDirectory();
        void slotSortReal();
        void slotSort();
        void slotReindex();
        void slotItemRenamedMy(QListViewItem *, int, const QString &);

    private:
        KFileTreeBranch *root;
        SQ_Dir          *dir;
        QTimer          *timer;
        SQ_DBMenu       *menu;

        static SQ_DirectoryBasket *m_inst;
};

#endif
