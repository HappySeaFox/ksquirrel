/***************************************************************************
                          sq_categoriesview.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qfileinfo.h>
#include <qfile.h>
#include <qheader.h>
#include <qdir.h>

#include <kio/job.h>
#include <ktoolbar.h>
#include <kicontheme.h>
#include <klocale.h>
#include <kfileitem.h>
#include <kurldrag.h>
#include <kinputdialog.h>
#include <kpropertiesdialog.h>
#include <kfiletreeviewitem.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>

#include "ksquirrel.h"
#include "sq_dir.h"
#include "sq_glwidget.h"
#include "sq_iconloader.h"
#include "sq_categoriesview.h"
#include "sq_libraryhandler.h"
#include "sq_externaltool.h"
#include "sq_widgetstack.h"
#include "sq_categorybrowsermenu.h"
#include "sq_storagefile.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"

SQ_CategoriesBox * SQ_CategoriesBox::sing = 0;

/* *************************************************************************************** */

SQ_CategoriesViewBranch::SQ_CategoriesViewBranch(KFileTreeView *parent, const KURL &url, const QString &name, const QPixmap &pix)
    : KFileTreeBranch(parent, url, name, pix)
{}

SQ_CategoriesViewBranch::~SQ_CategoriesViewBranch()
{}

KFileTreeViewItem* SQ_CategoriesViewBranch::createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem)
{
    KFileTreeViewItem *i = KFileTreeBranch::createTreeViewItem(parent, fileItem);

    /*
     *  In storage there are files with MD5 sum appended to their names.
     *  We should cut off MD5.
     */
    if(i)
    {
        QString n = i->fileItem()->name();
        int ind = n.findRev('.');

        // OOPS
        if(ind != -1)
            n.truncate(ind);

        i->setText(0, n);
    }

    return i;
}

/* *************************************************************************************** */

SQ_CategoriesView::SQ_CategoriesView(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
    setAcceptDrops(true);

    SQ_Dir dir(SQ_Dir::Categories);

    // create custom branch
    root = new SQ_CategoriesViewBranch(this, dir.root(), i18n("Categories"),
                SQ_IconLoader::instance()->loadIcon("bookmark", KIcon::Desktop, KIcon::SizeSmall));

    addBranch(root);

    header()->hide();
    addColumn(i18n("File"));
    setDirOnlyMode(root, false);
    setRootIsDecorated(true);
    setCurrentItem(root->root());
    root->setChildRecurse(true);
    root->setOpen(true);

    connect(this, SIGNAL(spacePressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(executed(QListViewItem*)), this, SLOT(slotItemExecuted(QListViewItem*)));
    connect(this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)), this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&)));

    menu = new KPopupMenu;

    menu->insertItem(SQ_IconLoader::instance()->loadIcon("folder_new", KIcon::Desktop, KIcon::SizeSmall), i18n("New category"), SQ_CategoriesBox::instance(), SLOT(slotNewCategory()));
    menu->insertSeparator();
    menu->insertItem(i18n("Delete"), SQ_CategoriesBox::instance(), SLOT(slotDeleteItem()));
    menu->insertItem(SQ_IconLoader::instance()->loadIcon("info", KIcon::Desktop, KIcon::SizeSmall), i18n("Properties"), SQ_CategoriesBox::instance(), SLOT(slotItemProperties()));
}

SQ_CategoriesView::~SQ_CategoriesView()
{
    delete menu;
}

void SQ_CategoriesView::slotContextMenu(KListView *, QListViewItem *, const QPoint &p)
{
    menu->exec(p);
}

void SQ_CategoriesView::slotItemExecuted(QListViewItem *item)
{
    if(!item) return;

    KFileTreeViewItem *cur = static_cast<KFileTreeViewItem *>(item);

    // file item
    if(cur && !cur->isDir())
    {
        KURL inpath = SQ_StorageFile::readStorageFile(cur->path());

        KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, inpath);
        SQ_WidgetStack::instance()->diroperator()->execute(&fi);
    }
}

/* ************************************************************** */

SQ_CategoriesBox::SQ_CategoriesBox(QWidget *parent, const char *name) : QVBox(parent, name)
{
    sing = this;

    lastdir = i18n("New Category");

    view = new SQ_CategoriesView(this);
    toolbar = new KToolBar(this);

    connect(view, SIGNAL(dropped(QDropEvent*, QListViewItem*, QListViewItem*)), this, SLOT(slotDropped(QDropEvent*, QListViewItem*, QListViewItem*)));

    SQ_Dir dir(SQ_Dir::Categories);

    menu = new SQ_CategoryBrowserMenu(dir.root(), 0, "Categories menu");

    toolbar->setIconSize(KIcon::SizeSmall);

    toolbar->insertButton("folder_new", 0, SIGNAL(clicked()), this, SLOT(slotNewCategory()), true, i18n("New category"));
    toolbar->insertButton("edittrash", 0, SIGNAL(clicked()), this, SLOT(slotDeleteItem()), true, i18n("Delete"));
    toolbar->insertButton("info", 0, SIGNAL(clicked()), this, SLOT(slotItemProperties()), true, i18n("Properties"));
    toolbar->insertButton("bookmark_add", 0, SIGNAL(clicked()), this, SLOT(slotDefaultCategories()), true, i18n("Create default categories"));
}

SQ_CategoriesBox::~SQ_CategoriesBox()
{}

void SQ_CategoriesBox::addToCategory(const QString &path)
{
    KFileItemList *selected = const_cast<KFileItemList *>(SQ_WidgetStack::instance()->selectedItems());

    if(!selected) return;

    KFileItem *item;

    item = selected->first();

    while(item)
    {
        if(item->isFile())
            SQ_StorageFile::writeStorageFile(path + QDir::separator() + item->name(), item->url().path());

        item = selected->next();
    }
}

void SQ_CategoriesBox::slotDefaultCategories()
{
    if(KMessageBox::questionYesNo(KSquirrel::app(),
        i18n("It will create default categories: Concerts, Pets, Home, Friends, Free time, Traveling, Nature. Continue ?"),
        i18n("Create default categories")) == KMessageBox::Yes)
    {
        QStringList list;
        SQ_Dir dir(SQ_Dir::Categories);

        list << "Concerts" << "Pets" << "Home" << "Friends" << "Free time" << "Traveling" << "Nature";

        for(QStringList::iterator it = list.begin();it != list.end();++it)
            dir.mkdir(*it);
    }
}

void SQ_CategoriesBox::slotNewCategory()
{
    bool ok;

    KFileTreeViewItem *cur = view->currentKFileTreeViewItem();

    if(!cur) return;

    QString tmp = KInputDialog::getText(i18n("New Category"), i18n("Create new category:"),
                                         lastdir, &ok, this);
    if(ok)
    {
        lastdir = tmp;
        KIO::mkdir(cur->path() + QDir::separator() + lastdir);
    }
}

void SQ_CategoriesBox::slotDropped(QDropEvent *e, QListViewItem *parent, QListViewItem *item)
{
    if(!item) item = parent;

    KFileTreeViewItem *cur = static_cast<KFileTreeViewItem *>(item);

    if(!cur) return;

    KURL::List list;
    KURLDrag::decode(e, list);
    QString path = cur->path();
    QFileInfo fi;
    SQ_Dir dir(SQ_Dir::Categories);

    if(list.first().path().startsWith(dir.root()))
        KIO::move(list, cur->url());
    else
        for(KURL::List::iterator it = list.begin(); it != list.end();++it)
        {
            fi.setFile((*it).path());

            if(fi.isFile())
                SQ_StorageFile::writeStorageFile(path + QDir::separator() + (*it).fileName(), (*it).path());
        }
}

void SQ_CategoriesBox::slotDeleteItem()
{
    KFileTreeViewItem *cur = view->currentKFileTreeViewItem();

    if(!cur) return;

    SQ_Dir dir(SQ_Dir::Categories);

    KURL root;
    root.setPath(dir.root());

    if(cur->url().equals(root, true))
        return;

    QListViewItem *next = cur->itemBelow();
    if(!next) next = cur->itemAbove();

    if(next)
    {
        view->setCurrentItem(next);
        view->setSelected(next, true);
    }

    QString tmp = cur->path();

    // remove this item manually
    delete cur;
    cur = 0;

    // physically remove file from storage
    KIO::del(tmp, false, false);
}

void SQ_CategoriesBox::slotItemProperties()
{
    KFileTreeViewItem *cur = view->currentKFileTreeViewItem();

    if(!cur) return;

    // directory - just show its properties
    if(cur->isDir())
        (void)new KPropertiesDialog(cur->url(), KSquirrel::app());
    // link to real file
    else
    {
        KURL inpath = SQ_StorageFile::readStorageFile(cur->path());

        if(!inpath.isEmpty())
            (void)new KPropertiesDialog(inpath, KSquirrel::app());
    }
}

#include "sq_categoriesview.moc"
