/***************************************************************************
                          sq_categoriesview.h  -  description
                             -------------------
    begin                : ??? June 3 2006
    copyright            : (C) 2006 by Baryshev Dmitry
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

#ifndef SQ_CATEGORIESVIEW_H
#define SQ_CATEGORIESVIEW_H

#include <qvbox.h>

#include <kfiletreeview.h>
#include <kfiletreebranch.h>

class KToolBar;
class KPopupMenu;

class SQ_CategoryBrowserMenu;

/**
  *@author Baryshev Dmitry
  */

/* *************************************************** */

class SQ_CategoriesViewBranch : public KFileTreeBranch
{
    public:
        SQ_CategoriesViewBranch(KFileTreeView*, const KURL &url, const QString &name, const QPixmap &pix);
        ~SQ_CategoriesViewBranch();

    protected:
        virtual KFileTreeViewItem *createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem);
};

/* *************************************************** */

class SQ_CategoriesView : public KFileTreeView
{
    Q_OBJECT

    public: 
	SQ_CategoriesView(QWidget *parent = 0, const char *name = 0);
	~SQ_CategoriesView();

    private slots:
        void slotItemExecuted(QListViewItem *item);
        void slotContextMenu(KListView *, QListViewItem *i, const QPoint &p);

    private:
        KFileTreeBranch *root;
        KPopupMenu *menu;
};

/* *************************************************** */

class SQ_CategoriesBox : public QVBox
{
    Q_OBJECT

    public:
        SQ_CategoriesBox(QWidget *parent = 0, const char *name = 0);
        ~SQ_CategoriesBox();

        /*
         *  Get current popup menu.
         */
        SQ_CategoryBrowserMenu* popupMenu() const;

        /*
         *  Add selected files to some category
         */
        void addToCategory(const QString &);

        static SQ_CategoriesBox* instance() { return sing; };

    private slots:
        void slotNewCategory();
        void slotDefaultCategories();
        void slotDeleteItem();
        void slotItemProperties();
        void slotDropped(QDropEvent *, QListViewItem *, QListViewItem *);

    private:
        SQ_CategoriesView *view;
        KToolBar *toolbar;
        QString lastdir, copypath;
        SQ_CategoryBrowserMenu *menu;

        static SQ_CategoriesBox *sing;
};

inline
SQ_CategoryBrowserMenu* SQ_CategoriesBox::popupMenu() const
{
    return menu;
}

#endif
