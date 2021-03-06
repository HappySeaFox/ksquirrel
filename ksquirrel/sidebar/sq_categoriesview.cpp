/***************************************************************************
                          sq_categoriesview.cpp  -  description
                             -------------------
    begin                : ??? June 3 2006
    copyright            : (C) 2006 by Baryshev Dmitry
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qfile.h>
#include <qheader.h>

#include <kio/job.h>
#include <ktoolbar.h>
#include <kicontheme.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kfileitem.h>
#include <kurldrag.h>
#include <kinputdialog.h>
#include <kpropertiesdialog.h>
#include <kfiletreeviewitem.h>
#include <kmessagebox.h>

#include "ksquirrel.h"
#include "sq_dir.h"
#include "sq_iconloader.h"
#include "sq_categoriesview.h"
#include "sq_libraryhandler.h"
#include "sq_externaltool.h"
#include "sq_widgetstack.h"
#include "sq_categorybrowsermenu.h"
#include "sq_storagefile.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_treeviewmenu.h"

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

    m_dir = new SQ_Dir(SQ_Dir::Categories);

    // create custom branch
    root = new SQ_CategoriesViewBranch(this, m_dir->root(), i18n("Categories"),
                SQ_IconLoader::instance()->loadIcon("bookmark", KIcon::Desktop, KIcon::SizeSmall));

    addBranch(root);

    header()->hide();
    addColumn(i18n("File"));
    setDirOnlyMode(root, false);
    setRootIsDecorated(true);
    setCurrentItem(root->root());
    root->setChildRecurse(true);
    root->setOpen(true);

    menu = new SQ_TreeViewMenu(this);

    connect(this, SIGNAL(spacePressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(executed(QListViewItem*)), this, SLOT(slotItemExecuted(QListViewItem*)));
    connect(this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)), this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&)));
}

SQ_CategoriesView::~SQ_CategoriesView()
{
    delete m_dir;
}

void SQ_CategoriesView::slotContextMenu(KListView *, QListViewItem *item, const QPoint &p)
{
    if(item)
    {
        KFileTreeViewItem *kfi = static_cast<KFileTreeViewItem*>(item);
        menu->updateDirActions(kfi->isDir(), (item == root->root()));
        menu->setURL(kfi->url());
        menu->exec(p);
    }
}

void SQ_CategoriesView::slotItemExecuted(QListViewItem *item)
{
    if(!item) return;

    if(item == root->root())
    {
        root->setOpen(true);
        return;
    }

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

    menu = new SQ_CategoryBrowserMenu(view->dir()->root(), 0, "Categories menu");

    toolbar->setIconSize(KIcon::SizeSmall);

    toolbar->insertButton("folder_new", 0, SIGNAL(clicked()), this, SLOT(slotNewCategory()), true, i18n("New category"));
    toolbar->insertButton("edittrash", 0, SIGNAL(clicked()), this, SLOT(slotDeleteItem()), true, i18n("Delete"));
    toolbar->insertButton("info", 0, SIGNAL(clicked()), this, SLOT(slotItemProperties()), true, i18n("Properties"));
    toolbar->insertButton("bookmark_add", 0, SIGNAL(clicked()), this, SLOT(slotDefaultCategories()), true, i18n("Create default categories"));

    view->popupMenu()->reconnect(SQ_TreeViewMenu::New, this, SLOT(slotNewCategory()));
    view->popupMenu()->reconnect(SQ_TreeViewMenu::Delete, this, SLOT(slotDeleteItem()));
    view->popupMenu()->reconnect(SQ_TreeViewMenu::Properties, this, SLOT(slotItemProperties()));
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
        i18n("This will create default categories: Concerts, Pets, Home, Friends, Free time, Travelling and Nature. Continue?"),
        i18n("Create default categories")) == KMessageBox::Yes)
    {
        QStringList list;

        list << "Concerts" << "Pets" << "Home" << "Friends" << "Free time" << "Traveling" << "Nature";

        for(QStringList::iterator it = list.begin();it != list.end();++it)
            view->dir()->mkdir(*it);
    }
}

void SQ_CategoriesBox::slotNewCategory()
{
    bool ok;

    KFileTreeViewItem *cur = view->currentKFileTreeViewItem();

    if(!cur) return;

    if(!cur->isDir())
        cur = static_cast<KFileTreeViewItem *>(cur->parent());

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

    if(list.first().path().startsWith(view->dir()->root()))
        KIO::move(list, cur->url());
    else
    {
        KURL::List::iterator itEnd = list.end();
        QString mimeDet;

        for(KURL::List::iterator it = list.begin(); it != itEnd;++it)
        {
            mimeDet = KMimeType::findByURL(*it)->name();

            if(mimeDet != "inode/directory")
                SQ_StorageFile::writeStorageFile(path + QDir::separator() + (*it).fileName(), (*it));
        }
    }
}

void SQ_CategoriesBox::slotDeleteItem()
{
    KFileTreeViewItem *cur = view->currentKFileTreeViewItem();

    if(!cur) return;

    KURL root;
    root.setPath(view->dir()->root());

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
