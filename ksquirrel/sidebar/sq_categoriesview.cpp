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

#include <qfileinfo.h>
#include <qheader.h>

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
#include <kmdcodec.h>
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_dir.h"
#include "sq_glwidget.h"
#include "sq_iconloader.h"
#include "sq_categoriesview.h"
#include "sq_libraryhandler.h"
#include "sq_externaltool.h"
#include "sq_widgetstack.h"
#include "sq_categorybrowsermenu.h"

SQ_CategoriesBox * SQ_CategoriesBox::sing = NULL;

/* *************************************************************************************** */

SQ_CategoriesViewBranch::SQ_CategoriesViewBranch(KFileTreeView *parent, const KURL &url, const QString &name, const QPixmap &pix)
    : KFileTreeBranch(parent, url, name, pix)
{}

SQ_CategoriesViewBranch::~SQ_CategoriesViewBranch()
{}

KFileTreeViewItem* SQ_CategoriesViewBranch::createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem)
{
    KFileTreeViewItem *i = KFileTreeBranch::createTreeViewItem(parent, fileItem);

    if(i)
    {
        QString n = i->fileItem()->name();
        int ind = n.findRev('.');

        // OOPS???
        if(ind != -1)
        {
            n.truncate(ind+1);
            i->setText(0, n);
        }
    }

    return i;
}

/* *************************************************************************************** */

SQ_CategoriesView::SQ_CategoriesView(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
    kdDebug() << "+SQ_CategoriesView" << endl;

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
    setAcceptDrops(true);
    root->setChildRecurse(true);
    root->setOpen(true);

    connect(this, SIGNAL(spacePressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(executed(QListViewItem*)), this, SLOT(slotItemExecuted(QListViewItem*)));
    connect(this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)), this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&)));

    menu = new KPopupMenu;

    menu->insertItem(SQ_IconLoader::instance()->loadIcon("folder_new", KIcon::Desktop, KIcon::SizeSmall), i18n("New category"), SQ_CategoriesBox::instance(), SLOT(slotNewCategory()));
    menu->insertSeparator();
    menu->insertItem(SQ_IconLoader::instance()->loadIcon("edittrash", KIcon::Desktop, KIcon::SizeSmall), i18n("Delete"), SQ_CategoriesBox::instance(), SLOT(slotDeleteItem()));
    menu->insertItem(SQ_IconLoader::instance()->loadIcon("info", KIcon::Desktop, KIcon::SizeSmall), i18n("Properties"), SQ_CategoriesBox::instance(), SLOT(slotItemProperties()));
}

SQ_CategoriesView::~SQ_CategoriesView()
{
    delete menu;

    kdDebug() << "-SQ_CategoriesView" << endl;
}

void SQ_CategoriesView::slotContextMenu(KListView *, QListViewItem *, const QPoint &p)
{
    menu->exec(p);
}

void SQ_CategoriesView::slotItemExecuted(QListViewItem *item)
{
    if(!item) return;

    KFileTreeViewItem *cur = dynamic_cast<KFileTreeViewItem *>(item);

    // file item
    if(cur && !cur->isDir())
    {
        QFile file(cur->path());

        if(file.open(IO_ReadOnly))
        {
            QByteArray ba = file.readAll();

            if(file.status() == IO_Ok)
            {
                QString intpath(ba);

                if(SQ_LibraryHandler::instance()->supports(intpath))
                    SQ_GLWidget::window()->slotStartDecoding(intpath, true);
            }

            file.close();
        }
    }
}

/* ************************************************************** */

SQ_CategoriesBox::SQ_CategoriesBox(QWidget *parent, const char *name) : QVBox(parent, name)
{
    kdDebug() << "+SQ_CategoriesBox" << endl;

    sing = this;

    lastdir = i18n("New Category");

    view = new SQ_CategoriesView(this);
    toolbar = new KToolBar(this);

    connect(view, SIGNAL(dropped(QDropEvent*, QListViewItem*)), this, SLOT(slotDropped(QDropEvent*, QListViewItem*)));

    SQ_Dir dir(SQ_Dir::Categories);

    menu = new SQ_CategoryBrowserMenu(dir.root(), NULL, "Categories menu");

    toolbar->setIconSize(KIcon::SizeSmall);

    toolbar->insertButton("folder_new", 0, SIGNAL(clicked()), this, SLOT(slotNewCategory()), true, i18n("New category"));
    toolbar->insertButton("edittrash", 0, SIGNAL(clicked()), this, SLOT(slotDeleteItem()), true, i18n("Delete"));
    toolbar->insertButton("info", 0, SIGNAL(clicked()), this, SLOT(slotItemProperties()), true, i18n("Properties"));
    toolbar->insertButton("bookmark_add", 0, SIGNAL(clicked()), this, SLOT(slotDefaultCategories()), true, i18n("Create default categories"));
}

SQ_CategoriesBox::~SQ_CategoriesBox()
{
    kdDebug() << "-SQ_CategoriesBox" << endl;
}

void SQ_CategoriesBox::addToCategory(const QString &path)
{
    KFileItemList *selected = const_cast<KFileItemList *>(SQ_WidgetStack::instance()->selectedItems());

    if(!selected) return;

    KFileItem *item;

    item = selected->first();

    while(item)
    {
        if(item->isFile())
        {
            writeCategoryFile(path + QString::fromLatin1("/") + item->name(),
                item->url().path(), item->url().path().length());
        }

        item = selected->next();
    }
}

void SQ_CategoriesBox::writeCategoryFile(const QString &path, const QString &inpath, const int len)
{
    KMD5 md5(QFile::encodeName(inpath));
    QFile file(path + QString::fromLatin1(".") + QString(md5.hexDigest()));

    if(file.open(IO_WriteOnly))
    {
        file.writeBlock(inpath, len);
        file.close();
    }
}

void SQ_CategoriesBox::slotDefaultCategories()
{
    if(KMessageBox::questionYesNo(KSquirrel::app(),
        i18n("It will create default categories: Concerts, Pets, Home, Friends, Free time, Traveling, Nature. Continue ?"),
        i18n("Create default categories"))
        == KMessageBox::Yes)
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

    QString tmp = KInputDialog::getText(i18n("New Category"), i18n("Create new category"),
                                         lastdir, &ok, this);
    if(ok)
    {
        lastdir = tmp;
        KIO::mkdir(cur->path() + QString::fromLatin1("/") + lastdir);
    }
}

void SQ_CategoriesBox::slotDropped(QDropEvent *e, QListViewItem *item)
{
    if(!item) return;

    KFileTreeViewItem *cur = dynamic_cast<KFileTreeViewItem *>(item);

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
                writeCategoryFile(path + QString::fromLatin1("/") + (*it).fileName(),
                    (*it).path(), (*it).path().length());
        }
}

void SQ_CategoriesBox::slotDeleteItem()
{
    KFileTreeViewItem *cur = view->currentKFileTreeViewItem();

    if(!cur) return;

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
    KIO::del(tmp);
}

void SQ_CategoriesBox::slotItemProperties()
{
    KFileTreeViewItem *cur = view->currentKFileTreeViewItem();

    if(!cur) return;

    // directory - juat show its properties
    if(cur->isDir())
        (void)new KPropertiesDialog(cur->url(), KSquirrel::app());
    // link to real file
    else
    {
        QFile file(cur->path());

        if(file.open(IO_ReadOnly))
        {
            QByteArray ba = file.readAll();

            if(file.status() == IO_Ok)
            {
                KURL url;
                QString intpath(ba);
                url.setPath(intpath);
                (void)new KPropertiesDialog(url, KSquirrel::app());
            }

            file.close();
        }
    }
}

#include "sq_categoriesview.moc"
