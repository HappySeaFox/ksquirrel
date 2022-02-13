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
#include <kpopupmenu.h>
#include <kfileiconview.h>
#include <kpropertiesdialog.h>

#include "ksquirrel.h"
#include "sq_imagebasket.h"
#include "sq_storagefile.h"
#include "sq_dir.h"

SQ_ImageBasket * SQ_ImageBasket::m_inst;

SQ_ImageBasket::SQ_ImageBasket(QWidget *parent, const char *name) : KDirOperator(SQ_Dir(SQ_Dir::Basket).root(), parent, name)
{
        m_inst = this;

        setupMenu(KDirOperator::AllActions ^ KDirOperator::NavActions);

        connect(this, SIGNAL(dropped(const KFileItem *, QDropEvent*, const KURL::List&)),
                        this, SLOT(slotDropped(const KFileItem *, QDropEvent*, const KURL::List&)));

//        disconnect(dirLister(), SIGNAL(started(const KURL&)), 0, 0);
//        disconnect(dirLister(), SIGNAL(completed()), 0, 0);
//        disconnect(dirLister(), SIGNAL(percent(int)), 0, 0);
        disconnect(dirLister(), SIGNAL(refreshItems(const KFileItemList &)), 0, 0);

        // redirect "Properties" dialog
        disconnect(actionCollection()->action("properties"), 0, 0, 0);
        connect(actionCollection()->action("properties"), SIGNAL(activated()), this, SLOT(slotBasketProperties()));

        disconnect(dirLister(), SIGNAL(newItems(const KFileItemList &)), 0, 0);
        connect(dirLister(), SIGNAL(newItems(const KFileItemList &)), this, SLOT(insertNewFiles(const KFileItemList &)));

        connect(this, SIGNAL(viewChanged(KFileView *)), this, SLOT(slotViewChanged(KFileView *)));

        setView(KFile::Simple);
        setMode(KFile::Files);

        setAcceptDrops(true);

        KActionMenu *pADirOperatorMenu = dynamic_cast<KActionMenu *>(actionCollection()->action("popupMenu"));
        pADirOperatorMenu->popupMenu()->insertItem(i18n("Synchronize"), this, SLOT(slotSync()), 0, -1, 0);
        pADirOperatorMenu->popupMenu()->insertSeparator(1);
}

SQ_ImageBasket::~SQ_ImageBasket()
{}

void SQ_ImageBasket::insertNewFiles(const KFileItemList &list)
{
        KFileItem *tmp;
        KFileItemList newlist;
        QString n;
        int ind;

        for(KFileItemListIterator it(list); (tmp = it.current()); ++it)
        {
                n = tmp->name();
                ind = n.findRev('.');

                // OOPS
                if(ind != -1)
                        n.truncate(ind);

                tmp->setName(n);
                newlist.append(tmp);
        }

        //KDirOperator::insertNewFiles(newlist);
        view()->addItemList(newlist);
}

void SQ_ImageBasket::add(const KFileItemList &list)
{
        KFileItem *tmp;
        QString name;

        for(KFileItemListIterator it(list); (tmp = it.current()); ++it)
        {
                name = url().path() + QDir::separator() + tmp->name();
                SQ_StorageFile::writeStorageFile(name, tmp->url().path());
        }
}

void SQ_ImageBasket::slotDropped(const KFileItem *, QDropEvent*, const KURL::List &list)
{
        QString name;
        KURL::List::const_iterator itEnd = list.end();

        for(KURL::List::const_iterator it = list.begin();it != itEnd;++it)
        {
                name = url().path() + QDir::separator() + (*it).fileName();
                SQ_StorageFile::writeStorageFile(name, (*it).path());
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
                        SQ_StorageFile::readStorageFIle(item->url().path()));

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
                    SQ_StorageFile::readStorageFIle(item->url().path()));

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
        QString path;

        for((item = fileView->firstFileItem()); item; item = fileView->nextItem(item))
        {
            path = SQ_StorageFile::readStorageFIle(item->url().path());

            if(!QFile::exists(path))
                QFile::remove(item->url().path());
        }
    }
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

#include "sq_imagebasket.moc"
