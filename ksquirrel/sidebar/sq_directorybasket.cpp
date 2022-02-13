/***************************************************************************
                          sq_directorybasket.cpp  -  description
                             -------------------
    begin                : ??? Sep 29 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#include <qvaluevector.h>
#include <qstringlist.h>
#include <qheader.h>
#include <qcursor.h>
#include <qtimer.h>

#include <kglobal.h>
#include <klocale.h>
#include <kurldrag.h>
#include <kmimetype.h>
#include <kfiledialog.h>
#include <kinputdialog.h>
#include <kicondialog.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <kprogress.h>
#include <kglobalsettings.h>

#include "ksquirrel.h"
#include "sq_directorybasket.h"
#include "sq_storagefile.h"
#include "sq_iconloader.h"
#include "sq_treeviewmenu.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_navigatordropmenu.h"
#include "sq_iconloader.h"
#include "sq_dir.h"

SQ_DirectoryBasket * SQ_DirectoryBasket::m_inst = 0;

/* ******************************************************************************* */

SQ_DBMenu::SQ_DBMenu(QWidget *parent, const char *name) : SQ_TreeViewMenu(parent, name), item(0)
{
    insertSeparator();
    id_icon = insertItem(i18n("Change icon"), this, SLOT(slotChangeIcon()));
}

SQ_DBMenu::~SQ_DBMenu()
{}

void SQ_DBMenu::slotChangeIcon()
{
    KIconDialog dialog(SQ_IconLoader::instance());
    dialog.setup(KIcon::Desktop, KIcon::MimeType, true, KIcon::SizeSmall);
    QString result = dialog.openDialog();

    if(!result.isEmpty() && item)
    {
        item->setIcon(result);
        item->setPixmap(0, SQ_IconLoader::instance()->loadIcon(result, KIcon::Desktop, KIcon::SizeSmall));
    }
}

void SQ_DBMenu::updateDirActions(bool, bool isroot)
{
    setItemEnabled(id_new, isroot);
    setItemEnabled(id_clear, isroot);
    setItemEnabled(id_prop, !isroot);

    setItemEnabled(id_delete, !isroot);
    setItemEnabled(id_rename, !isroot);
    setItemEnabled(id_icon,   !isroot);
}

void SQ_DBMenu::slotDirectoryRename()
{
    if(item)
    {
        QString renameSrc = item->text(0);
        bool ok;

        QString mNewFilename = KInputDialog::getText(i18n("Rename Folder"),
                i18n("<p>Rename item <b>%1</b> to:</p>").arg(renameSrc),
                renameSrc, &ok, KSquirrel::app());

        if(ok)
        {
            item->setName(mNewFilename);
            item->setText(0, mNewFilename);
        }
    }
}

void SQ_DBMenu::slotDirectoryResult(KIO::Job *job)
{
    if(job && job->error())
        job->showErrorDialog(KSquirrel::app());
}

void SQ_DBMenu::slotDirectoryDelete()
{
    if(item)
    {
        KIO::Job *job = KIO::del(item->KFileTreeViewItem::url());

        connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotDirectoryResult(KIO::Job *)));
    }
}

/* ******************************************************************************* */

SQ_DirectoryItem::SQ_DirectoryItem(KFileTreeViewItem *parentItem, KFileItem *fileItem, KFileTreeBranch *parentBranch)
    : KFileTreeViewItem(parentItem, fileItem, parentBranch), m_index(0)
{}

SQ_DirectoryItem::SQ_DirectoryItem(KFileTreeView *parent, KFileItem *fileItem, KFileTreeBranch *parentBranch)
    : KFileTreeViewItem(parent, fileItem, parentBranch), m_index(0)
{}

SQ_DirectoryItem::~SQ_DirectoryItem()
{}

/* ******************************************************************************* */

SQ_DirectoryBasketBranch::SQ_DirectoryBasketBranch(KFileTreeView *parent, const KURL &url, const QString &name, const QPixmap &pix)
    : KFileTreeBranch(parent, url, name, pix)
{}

SQ_DirectoryBasketBranch::~SQ_DirectoryBasketBranch()
{}

KFileTreeViewItem* SQ_DirectoryBasketBranch::createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem)
{
    if(!parent || !fileItem)
        return 0;

    // hehe...
    fileItem->setMimeType("inode/directory");

    SQ_DirectoryItem *i = new SQ_DirectoryItem(parent, fileItem, this);

    if(i)
    {
        // inpath = "<URL><name><index>"
        QStringList list = QStringList::split(QChar('\n'), SQ_StorageFile::readStorageFileAsString(i->path()), true);

        if(list.count() < 4)
            return i;

        QStringList::iterator it = list.begin();

        bool ok;
        QString name, icon;

        // get url
        KURL inpath = KURL::fromPathOrURL(*it);
        ++it;

        // get name
        name = *it;
        ++it;

        // get icon
        icon = *it;
        ++it;

        // get index
        int index = (*it).toInt(&ok);

        i->setURL(inpath);

        if(name.isEmpty())
            i->setText(0, inpath.isLocalFile() ? inpath.path() : inpath.prettyURL());
        else
        {
            i->setText(0, name);
            i->setName(name);
        }

        if(!icon.isEmpty())
        {
            i->setIcon(icon);
            i->setPixmap(0, SQ_IconLoader::instance()->loadIcon(icon, KIcon::Desktop, KIcon::SizeSmall));
        }

        if(ok) i->setIndex(index);
    }

    return i;
}

/* ******************************************************************************* */

SQ_DirectoryBasket::SQ_DirectoryBasket(QWidget *parent, const char *name) : KFileTreeView(parent, name)
{
    m_inst = this;

    progressAdd = new KProgress(0, "progress add", Qt::WStyle_StaysOnTop | Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WX11BypassWM);

    menu = new SQ_DBMenu(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotSortReal()));

    timerAdd = new QTimer(this);
    connect(timerAdd, SIGNAL(timeout()), this, SLOT(slotDelayedShowAdd()));

    setSorting(-1);
    setAcceptDrops(true);

    dir = new SQ_Dir(SQ_Dir::DirectoryBasket);

    // create custom branch
    root = new SQ_DirectoryBasketBranch(this, dir->root(), QString::null, 
            SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, KIcon::SizeSmall));

    // some hacks to create our SQ_TreeViewItem as root item
    SQ_DirectoryItem  *ritem = new SQ_DirectoryItem(this,
                                        new KFileItem(dir->root(),
                                        "inode/directory", S_IFDIR),
                                        root);

    ritem->setText(0, i18n("Folders"));
    ritem->setExpandable(true);
    ritem->setURL(dir->root());
    delete root->root();
    root->setRoot(ritem);

    addBranch(root);

    disconnect(root, SIGNAL(refreshItems(const KFileItemList &)), 0, 0);

    header()->hide();
    addColumn(i18n("File"));
    setDirOnlyMode(root, false);
    setCurrentItem(root->root());
    root->setOpen(true);
    setRootIsDecorated(false);

    menu->reconnect(SQ_TreeViewMenu::New, this, SLOT(slotNewDirectory()));

    connect(this, SIGNAL(spacePressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SIGNAL(executed(QListViewItem*)));
    connect(this, SIGNAL(executed(QListViewItem*)), this, SLOT(slotItemExecuted(QListViewItem*)));
    connect(this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)), this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&)));
    connect(this, SIGNAL(dropped(QDropEvent*, QListViewItem*, QListViewItem*)), this, SLOT(slotDropped(QDropEvent*, QListViewItem*, QListViewItem*)));
    connect(this, SIGNAL(itemRenamed(QListViewItem *, int, const QString &)), this, SLOT(slotItemRenamedMy(QListViewItem *, int, const QString &)));
    connect(this, SIGNAL(itemAdded(QListViewItem *)), this, SLOT(slotSort()));
    connect(this, SIGNAL(moved()), this, SLOT(slotReindex()));
}

SQ_DirectoryBasket::~SQ_DirectoryBasket()
{
    SQ_DirectoryItem *item = static_cast<SQ_DirectoryItem *>(root->root()->firstChild());

    static const QString &nl = KGlobal::staticQString("\n");

    if(item)
    {
        QString url;
        int index = 0;

        do
        {
            url = item->url().prettyURL() + nl + item->name() + nl + item->icon() + nl + QString::number(index);

            SQ_StorageFile::writeStorageFileAsString(
                    dir->root() + QDir::separator() + item->url().fileName(),
                    item->url(), url);

            ++index;
        }
        while((item = static_cast<SQ_DirectoryItem *>(item->nextSibling())));
    }

    delete dir;
    delete progressAdd;
}

void SQ_DirectoryBasket::slotNewDirectory()
{
    static const QString &nl = KGlobal::staticQString("\n");

    KURL url = KFileDialog::getExistingURL(QString::null, KSquirrel::app());

    if(url.isEmpty())
        return;

     SQ_StorageFile::writeStorageFileAsString(
            dir->root() + QDir::separator() + url.fileName(),
            url,
            (url.prettyURL() + nl + nl + nl + QString::number(root->root()->childCount())));
}

void SQ_DirectoryBasket::slotDropped(QDropEvent *e, QListViewItem *_parent, QListViewItem *_item)
{
    if(!_parent) return;
    if(!_item) _item = _parent;

    SQ_DirectoryItem *item = static_cast<SQ_DirectoryItem *>(_item);
    SQ_DirectoryItem *parent = static_cast<SQ_DirectoryItem *>(_parent);

    KURL::List list;
    KURLDrag::decode(e, list);

    // drag'n'drop inside basket
    if(e->source() == this)
    {
        SQ_DirectoryItem *tomove = static_cast<SQ_DirectoryItem *>(root->findTVIByURL(list.first()));

        if(!tomove || tomove == root->root()) return;

        SQ_DirectoryItem *it;

        if(item->index() < tomove->index() && item != root->root())
            it = static_cast<SQ_DirectoryItem *>(item->itemAbove());
        else
            it = static_cast<SQ_DirectoryItem *>(item);

        if(it)
        {
            if(it != root->root())
                moveItem(tomove, parent, it);
            else
                moveItem(tomove, parent, 0);

            emit moved();

            setCurrentItem(tomove);
            setSelected(tomove, true);
        }
    }
    else if(item == root->root()) // some files were dropped from another source
    {
        KURL::List::iterator itEnd = list.end();
        KFileItemList flist;
        KIO::UDSEntry entry;

        progressAdd->setTotalSteps(list.count());
        timerAdd->start(1000, true);

        for(KURL::List::iterator it = list.begin();it != itEnd;++it)
        {
            if(KIO::NetAccess::stat(*it, entry, KSquirrel::app()))
                flist.append(new KFileItem(entry, *it));

            progressAdd->advance(1);
        }

        timerAdd->stop();
        progressAdd->hide();

        add(flist);
        flist.setAutoDelete(true);
    }
    else
    {
        SQ_NavigatorDropMenu::instance()->setupFiles(list, item->url());
        SQ_NavigatorDropMenu::instance()->exec(QCursor::pos(), true);
    }
}

void SQ_DirectoryBasket::slotItemExecuted(QListViewItem *item)
{
    if(!item) return;

    if(item == root->root())
    {
        root->setOpen(true);
        return;
    }

    KFileTreeViewItem *cur = static_cast<KFileTreeViewItem *>(item);

    if(cur && !cur->isDir())
    {
        KURL inpath = SQ_StorageFile::readStorageFile(cur->path());
        SQ_WidgetStack::instance()->diroperator()->setURL(inpath, true);
    }
}

void SQ_DirectoryBasket::slotContextMenu(KListView *, QListViewItem *item, const QPoint &p)
{
    if(item)
    {
        SQ_DirectoryItem *kfi = static_cast<SQ_DirectoryItem *>(item);

        if(kfi)
        {
            menu->updateDirActions(true, item == root->root());
            menu->setURL(kfi->url());
            menu->setItem(kfi);
            menu->exec(p);
        }
    }
}

void SQ_DirectoryBasket::add(const KFileItemList &list)
{
    static const QString &nl = KGlobal::staticQString("\n");

    KFileItemListIterator it(list);
    KFileItem *fi;
    QString url;

    while((fi = it.current()))
    {
        if(fi->isDir())
        {
            url = fi->url().prettyURL() + nl + nl + nl + QString::number(root->root()->childCount());

            SQ_StorageFile::writeStorageFileAsString(
                dir->root() + QDir::separator() + fi->url().fileName(),
                fi->url(),
                url);
        }

        ++it;
    }
}

void SQ_DirectoryBasket::slotSort()
{
    timer->start(100, true);
}

void SQ_DirectoryBasket::slotSortReal()
{
    sort();
}

struct SortableItem
{
    SortableItem(QListViewItem *i, int ind) : item(i), index(ind)
    {}

    SortableItem() : item(0), index(0)
    {}

    bool operator< (const SortableItem &i)
    {
        return index > i.index;
    }

    QListViewItem *item;
    int index;
};

void SQ_DirectoryBasket::sort()
{
    QListViewItemIterator it(this);
    SQ_DirectoryItem *item;

    QValueVector<SortableItem> items;
    int i = 0;
    ++it;

    while((item = static_cast<SQ_DirectoryItem *>(it.current())))
    {
        items.append(SortableItem(item, item->index()));
        ++it;
    }

    const int nChildren = items.count();

    for(i = 0;i < nChildren;i++)
        root->root()->takeItem(items[i].item);

    qHeapSort(items);

    blockSignals(true);
    for(i = 0;i < nChildren;i++)
        root->root()->insertItem(items[i].item);
    blockSignals(false);
}

void SQ_DirectoryBasket::slotReindex()
{
    SQ_DirectoryItem *item = static_cast<SQ_DirectoryItem *>(root->root()->firstChild());

    if(item)
    {
        int index = 0;

        do
        {
            item->setIndex(index++);
        }
        while((item = static_cast<SQ_DirectoryItem *>(item->nextSibling())));
    }
}

void SQ_DirectoryBasket::slotItemRenamedMy(QListViewItem *_item, int, const QString &name)
{
    SQ_DirectoryItem *item = static_cast<SQ_DirectoryItem *>(_item);

    if(item)
        item->setName(name);
}

void SQ_DirectoryBasket::slotDelayedShowAdd()
{
    int w = 200, h = 32;

    QRect rc = KGlobalSettings::splashScreenDesktopGeometry();

    progressAdd->setGeometry(rc.center().x() - w/2, rc.center().y() - h/2, w, h);
    progressAdd->show();
}

#include "sq_directorybasket.moc"
