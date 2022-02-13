/***************************************************************************
                          sq_imagebasket.cpp  -  description
                             -------------------
    begin                : ??? Feb 24 2007
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

#include <qfile.h>

#include <kmdcodec.h>
#include <kfileview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kpopupmenu.h>
#include <kfileiconview.h>
#include <kpropertiesdialog.h>
#include <kio/job.h>
#include <kapplication.h>
#include <kmimetype.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_imagebasket.h"
#include "sq_storagefile.h"
#include "sq_dir.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"

SQ_ImageBasket * SQ_ImageBasket::m_inst;

SQ_ImageBasket::SQ_ImageBasket(QWidget *parent, const char *name) : KDirOperator((dir = new SQ_Dir(SQ_Dir::Basket))->root(), parent, name)
{
    m_inst = this;

    connect(this, SIGNAL(dropped(const KFileItem *, QDropEvent*, const KURL::List&)),
            this, SLOT(slotDropped(const KFileItem *, QDropEvent*, const KURL::List&)));

    disconnect(dirLister(), SIGNAL(refreshItems(const KFileItemList &)), 0, 0);

    // redirect "Properties" dialog
    disconnect(actionCollection()->action("properties"), 0, 0, 0);
    connect(actionCollection()->action("properties"), SIGNAL(activated()), this, SLOT(slotBasketProperties()));

    disconnect(dirLister(), SIGNAL(newItems(const KFileItemList &)), 0, 0);
    connect(dirLister(), SIGNAL(newItems(const KFileItemList &)), this, SLOT(insertNewFiles(const KFileItemList &)));

    connect(this, SIGNAL(viewChanged(KFileView *)), this, SLOT(slotViewChanged(KFileView *)));
    connect(this, SIGNAL(fileSelected(const KFileItem *)), this, SLOT(slotExecuted(const KFileItem *)));

    setView(KFile::Simple);
    setMode(KFile::Files);

    setAcceptDrops(true);

    SQ_Config::instance()->setGroup("Fileview");
    int sorting = 0;

    if(SQ_Config::instance()->readBoolEntry("basket_sorting_name", true))      sorting |= QDir::Name;
    if(SQ_Config::instance()->readBoolEntry("basket_sorting_time", false))     sorting |= QDir::Time;
    if(SQ_Config::instance()->readBoolEntry("basket_sorting_size", false))     sorting |= QDir::Size;
    if(SQ_Config::instance()->readBoolEntry("basket_sorting_dirs", true))      sorting |= QDir::DirsFirst;
    if(SQ_Config::instance()->readBoolEntry("basket_sorting_reverse", false))  sorting |= QDir::Reversed;
    if(SQ_Config::instance()->readBoolEntry("basket_sorting_ignore", false))   sorting |= QDir::IgnoreCase;

    setSorting(static_cast<QDir::SortSpec>(sorting));
}

SQ_ImageBasket::~SQ_ImageBasket()
{
    delete dir;
}

void SQ_ImageBasket::insertNewFiles(const KFileItemList &list)
{
    QString n;
    int ind;
    KFileItemListIterator it(list);
    KFileItem *tmp;

    for(; (tmp = it.current()); ++it)
    {
        n = tmp->name();
        ind = n.findRev('.');

        // OOPS
        if(ind != -1)
            n.truncate(ind);

        // force determining mimetype
        (void)tmp->mimetype();
        tmp->setName(n);

        QStringList list = QStringList::split(QChar('\n'), SQ_StorageFile::readStorageFileAsString(tmp->url().path()), true);
        QStringList::iterator it = list.begin();

        if(list.count() > 1)
        {
            ++it; // skip url
            tmp->setMimeType(*it);
        }
        else
        {
            KURL url = KURL::fromPathOrURL(*it);
            QString mime = KMimeType::findByURL(url)->name();
            tmp->setMimeType(mime);

            static const QString &nl = KGlobal::staticQString("\n");

            QString inurl = url.prettyURL() + nl + mime;

            SQ_StorageFile::writeStorageFileAsString(
                    dir->root() + QDir::separator() + url.fileName(),
                    url, inurl);
        }
    }

    view()->addItemList(list);
}

void SQ_ImageBasket::add(const KFileItemList &list)
{
    KFileItem *tmp;
    static const QString &nl = KGlobal::staticQString("\n");

    for(KFileItemListIterator it(list); (tmp = it.current()); ++it)
    {
        if(tmp->isFile())
        {
            QString inurl = tmp->url().prettyURL() + nl + tmp->mimetype();

            SQ_StorageFile::writeStorageFileAsString(
                    dir->root() + QDir::separator() + tmp->name(),
                    tmp->url(), inurl);
        }
    }
}

void SQ_ImageBasket::slotDropped(const KFileItem *, QDropEvent*, const KURL::List &list)
{
    QString name;
    KURL::List::const_iterator itEnd = list.end();
    static const QString &nl = KGlobal::staticQString("\n");

    for(KURL::List::const_iterator it = list.begin();it != itEnd;++it)
    {
        QString inurl = (*it).prettyURL() + nl + KMimeType::findByURL(*it)->name();

        SQ_StorageFile::writeStorageFileAsString(
                dir->root() + QDir::separator() + (*it).fileName(),
                *it, inurl);
    }
}

void SQ_ImageBasket::slotBasketProperties()
{
    KFileView *fileView = view();

    if(fileView)
    {
        KFileItemList newlist;
        KFileItem *item = 0;

        newlist.setAutoDelete(true);

        for((item = fileView->firstFileItem()); item; item = fileView->nextItem(item))
        {
            if(fileView->isSelected(item))
            {
                KFileItem *realFile = new KFileItem(KFileItem::Unknown, KFileItem::Unknown,
                        SQ_StorageFile::readStorageFile(item->url().path()));

                newlist.append(realFile);
            }
        }

        if (!newlist.isEmpty())
            (void)new KPropertiesDialog(newlist, KSquirrel::app(), "props dlg", true);
    }
}

KFileItemList SQ_ImageBasket::realItems() const
{
    KFileView *fileView = view();
    KFileItemList newlist;

    newlist.setAutoDelete(true);

    if(fileView)
    {
        KFileItem *item = 0;

        for((item = fileView->firstFileItem()); item; item = fileView->nextItem(item))
        {
            KFileItem *realFile = new KFileItem(KFileItem::Unknown, KFileItem::Unknown,
                    SQ_StorageFile::readStorageFile(item->url().path()));

            newlist.append(realFile);
        }
    }

    return newlist;
}

void SQ_ImageBasket::slotSync()
{
    KFileView *fileView = view();

    if(fileView)
    {
        KFileItem *item = 0;
        KURL path;
        KIO::Job *j;

        for((item = fileView->firstFileItem()); item; item = fileView->nextItem(item))
        {
            path = SQ_StorageFile::readStorageFile(item->url().path());

            j = KIO::stat(path, false);
            connect(j, SIGNAL(result(KIO::Job *)), this, SLOT(slotStatResult(KIO::Job *)));

            m_stat = false;
            m_exist = true;

            while(!m_stat)
                kapp->processOneEvent();

            if(!m_exist)
                QFile::remove(item->url().path());
        }
    }
}

void SQ_ImageBasket::slotStatResult(KIO::Job *job)
{
    if(job->error())
        m_exist = false;

    m_stat = true;
}

void SQ_ImageBasket::slotViewChanged(KFileView *v)
{
    KFileIconView *iv = dynamic_cast<KFileIconView *>(v);

    if(iv)
    {
        KAction *a;

        a = iv->actionCollection()->action("zoomIn");
        if(a) a->setShortcut(0);

        a = iv->actionCollection()->action("zoomOut");
        if(a) a->setShortcut(0);

        a = iv->actionCollection()->action("show previews");
        if(a) a->setShortcut(0);
    }
}

void SQ_ImageBasket::slotExecuted(const KFileItem *fi)
{
    if(!fi)
        return;

    KURL inpath = SQ_StorageFile::readStorageFile(fi->url().path());

    KFileItem f(KFileItem::Unknown, KFileItem::Unknown, inpath);

    SQ_WidgetStack::instance()->diroperator()->execute(&f);
}

void SQ_ImageBasket::activatedMenu(const KFileItem *, const QPoint &pos)
{
    setupMenu(KDirOperator::AllActions ^ KDirOperator::NavActions ^ KDirOperator::ViewActions);
    updateSelectionDependentActions();

    KActionMenu *pADirOperatorMenu = dynamic_cast<KActionMenu *>(actionCollection()->action("popupMenu"));
    pADirOperatorMenu->popupMenu()->insertItem(i18n("Synchronize"), this, SLOT(slotSync()), 0, -1, 0);
    pADirOperatorMenu->popupMenu()->insertSeparator(1);

    pADirOperatorMenu->popup(pos);
}

void SQ_ImageBasket::saveConfig()
{
    QDir::SortSpec sort = sorting();

    SQ_Config::instance()->writeEntry("basket_sorting_name", KFile::isSortByName(sort));
    SQ_Config::instance()->writeEntry("basket_sorting_time", KFile::isSortByDate(sort));
    SQ_Config::instance()->writeEntry("basket_sorting_size", KFile::isSortBySize(sort));
    SQ_Config::instance()->writeEntry("basket_sorting_dirs", KFile::isSortDirsFirst(sort));
    SQ_Config::instance()->writeEntry("basket_sorting_reverse", (sort & QDir::Reversed) == QDir::Reversed);
    SQ_Config::instance()->writeEntry("basket_sorting_ignore", KFile::isSortCaseInsensitive(sort));

    SQ_Config::instance()->writeEntry("show hidden", showHiddenFiles());
}

#include "sq_imagebasket.moc"
