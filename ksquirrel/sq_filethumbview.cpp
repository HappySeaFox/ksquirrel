/***************************************************************************
                          sq_fileiconview.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qpainter.h>
#include <qhbox.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qapplication.h>

#include <kstandarddirs.h>
#include <kstringhandler.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kapplication.h>
#include <kglobal.h>

#include "ksquirrel.h"
#include "sq_iconloader.h"
#include "sq_config.h"
#include "sq_dir.h"
#include "sq_filethumbview.h"
#include "sq_libraryhandler.h"
#include "sq_thumbnailloadjob.h"
#include "sq_thumbnailsize.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_pixmapcache.h"
#include "sq_progress.h"
#include "sq_progressbox.h"
#include "sq_filethumbviewitem.h"
#include "sq_dragprovider.h"

SQ_FileThumbView::SQ_FileThumbView(QWidget *parent, const char *name) : SQ_FileIconViewBase(parent, name), isPending(false)
{
    // create progress bar
    m_progressBox = new SQ_ProgressBox(this);

    timerScroll = new QTimer(this);
    connect(timerScroll, SIGNAL(timeout()), this, SLOT(slotDelayedContentsMoving()));

    timerAdd = new QTimer(this);
    connect(timerAdd, SIGNAL(timeout()), this, SLOT(slotDelayedAddItems()));

    // setup cache limit
    SQ_Config::instance()->setGroup("Thumbnails");
    SQ_PixmapCache::instance()->setCacheLimit(SQ_Config::instance()->readNumEntry("cache", 1024*10));
    m_lazy = SQ_Config::instance()->readBoolEntry("lazy", true);
    lazyDelay = SQ_Config::instance()->readNumEntry("lazy_delay", 500);
    if(lazyDelay <= 0) lazyDelay = 500;

    setResizeMode(QIconView::Adjust);

    // load "pending" pixmaps
    pending = SQ_IconLoader::instance()->loadIcon("clock", KIcon::Desktop, 32);

    connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotContentsMoving(int, int)));

    rebuildCachedPixmaps();
}

SQ_FileThumbView::~SQ_FileThumbView()
{}

/*
 *  Reimplement insertItem() to enable/disable inserting
 *  directories (depends on current settings) and inserting thumbnails.
 */
void SQ_FileThumbView::insertItem(KFileItem *i)
{
    SQ_Config::instance()->setGroup("Fileview");

    // directores disabled ?
    if(i->isDir() && SQ_Config::instance()->readBoolEntry("disable_dirs", false))
        return;

    SQ_Config::instance()->setGroup("Thumbnails");

    SQ_FileThumbViewItem *item;

    if(SQ_Config::instance()->readBoolEntry("mark", false) && SQ_LibraryHandler::instance()->libraryForFile(i->url().path()))
    {
        item = new SQ_FileThumbViewItem(this, i->text(), pendingCache, i);
    }
    else
    {
        QPixmap mimeall = i->pixmap(SQ_ThumbnailSize::smallest());
        QPixmap thumbnail(pixelSize.width(), pixelSize.height());
        QPainter painter(&thumbnail);
        painter.setBrush(colorGroup().base());
        painter.setPen(colorGroup().highlight());
        painter.drawRect(0, 0, pixelSize.width(), pixelSize.height());
        painter.drawPixmap((pixelSize.width()-mimeall.width())/2, (pixelSize.height()-mimeall.height())/2, mimeall);
        item = new SQ_FileThumbViewItem(this, i->text(), thumbnail, i);
    }

    initItemMy(item, i);

    i->setExtraData(this, item);
}

/*
 *  One thumbnail is loaded. Let's update KFileItem's pixmap.
 */
void SQ_FileThumbView::setThumbnailPixmap(const KFileItem* fileItem, const SQ_Thumbnail &t)
{
    KFileIconViewItem *iconItem = viewItem(fileItem);

    if(!iconItem) return;

    SQ_FileThumbViewItem *item = static_cast<SQ_FileThumbViewItem*>(iconItem);

    if(!item) return;

    QPixmap newpix;

    // Extended thumbnail also have mime icon and dimensions
    if(SQ_ThumbnailSize::instance()->extended())
    {
        QSize sz = pixelSize;//SQ_ThumbnailSize::instance()->extendedSize();
        int W = sz.width(), H = sz.height();

        // erase original pixmap
        newpix.resize(W, H);

        QPainter painter;
        painter.begin(&newpix);

        painter.setPen(colorGroup().highlight());
        painter.setBrush(colorGroup().base());

        // draw bounding rect
        painter.drawRect(0, 0, W, H);

        painter.drawImage((W - t.thumbnail.width())/2, (W - t.thumbnail.height())/2, t.thumbnail);
        painter.drawPixmap(W-t.mime.width()-5, H-t.mime.height()-4, t.mime);
        painter.drawLine(3, W-1, W-4, W-1);

        QFont f = painter.font();
        f.setPixelSize(10);
        painter.setFont(f);

        int rest = H-W-2;

        painter.setPen(colorGroup().text());

        if(t.w && t.h) painter.drawText(4, W+rest/2-12, 100, 12, 0, QString::fromLatin1("%1x%2").arg(t.w).arg(t.h));

        painter.drawText(4, W+rest/2+1, 100, 12, 0, KIO::convertSize(fileItem->size()));
        painter.end();
    }
    else
    {
        newpix.resize(pixelSize.width(), pixelSize.height());

        QPainter painter;
        painter.begin(&newpix);

        painter.setPen(colorGroup().highlight());
        painter.setBrush(colorGroup().base());

        // draw bounding rect
        painter.drawRect(0, 0, pixelSize.width(), pixelSize.height());

        // draw pixmap
        painter.drawImage((pixelSize.width() - t.thumbnail.width())/2, (pixelSize.height() - t.thumbnail.height())/2, t.thumbnail);
        painter.end();
    }

    item->setPixmap(newpix);
    item->setListed(true);

    // update item
    item->repaint();
}

void SQ_FileThumbView::startThumbnailUpdate()
{
    stopThumbnailUpdate();
    doStartThumbnailUpdate(itemsToUpdate());
}

KFileItemList SQ_FileThumbView::itemsToUpdate(bool fromAll)
{
    // non-lazy mode - simply return all items
    if(!m_lazy)
        return *items();

    // hehe, lazy mode
    KFileItemList list;

    QRect rect(contentsX(), contentsY(), viewport()->width(), viewport()->height());
    //printf("** FIND in %d,%d %dx%d\n", rect.x(), rect.y(), rect.width(), rect.height());
    QIconViewItem *first = fromAll ? firstItem() : findFirstVisibleItem(rect);
    QIconViewItem *last  = fromAll ? lastItem()  : findLastVisibleItem(rect);

    if(first && last)
    {
        last = last->nextItem(); // next item or 0
        SQ_FileThumbViewItem *tfi;

        for(QIconViewItem *item = first;(item && item != last);item = item->nextItem())
        {
            tfi = dynamic_cast<SQ_FileThumbViewItem *>(item);

            if(tfi && !tfi->listed())
            {
                list.append(tfi->fileInfo());
                //printf("** %s\n", tfi->text().ascii());
            }
        }
    }

    return list;
}

void SQ_FileThumbView::slotContentsMoving(int, int)
{
    timerScroll->start(lazyDelay, true);
}

void SQ_FileThumbView::slotDelayedContentsMoving()
{
    // restart generator in lazy mode
    if(m_lazy)
    {
        stopThumbnailUpdate();
        doStartThumbnailUpdate(itemsToUpdate());
    }
    // make visible items first items in the job
    else
    {
        // force itemsToUpdate() return only visible items
        // that need update
        m_lazy = true;
        KFileItemList visibleItems = itemsToUpdate();

        if(updateRunning())
            thumbJob->pop(visibleItems);

        // restore lazy mode
        m_lazy = false;
    }
}

/*
 *  Create job, connect signals and start updating
 */
void SQ_FileThumbView::doStartThumbnailUpdate(const KFileItemList &list)
{
    if(list.isEmpty())
        return;

    // update progress bar
    SQ_WidgetStack::instance()->thumbnailUpdateStart(list.count());

    // create new job
    thumbJob = new SQ_ThumbnailLoadJob(list, this);

    connect(thumbJob, SIGNAL(thumbnailLoaded(const KFileItem*, const SQ_Thumbnail &)),
            this, SLOT(setThumbnailPixmap(const KFileItem*, const SQ_Thumbnail&)));

    connect(thumbJob, SIGNAL(result(KIO::Job*)), SQ_WidgetStack::instance(), SLOT(thumbnailsUpdateEnded()));

    // start!
    thumbJob->start();
}

// Stop thumbnail update: delete job, reset progress bar
void SQ_FileThumbView::stopThumbnailUpdate()
{
    if(!thumbJob.isNull())
    {
        thumbJob->kill();
        SQ_WidgetStack::instance()->thumbnailsUpdateEnded();
    }
}

// Start/stop thumbnail job
void SQ_FileThumbView::slotThumbnailUpdateToggle()
{
    if(!thumbJob.isNull())
        stopThumbnailUpdate();
    else
        startThumbnailUpdate();
}

/*
 *  Append new items to thumbnail generating job.
 */
void SQ_FileThumbView::addItemsToJob(const KFileItemList &items, bool append)
{
    newItemsAppend = append;

    KFileItemListIterator it(items);
    KFileItem *fi;

    while((fi = it.current()))
    {
        newItems.append(fi);
        ++it;
    }

    // don't confuse user with multiple updates
    timerAdd->start(500, true);
}

void SQ_FileThumbView::slotDelayedAddItems()
{
    KFileItemList _newItems = newItems;
    newItems.clear();

    if(m_lazy)
    {
        KFileItemList visItems = itemsToUpdate();
        KFileItemListIterator it(_newItems);
        KFileItem *fi;

        while((fi = it.current()))
        {
            if(visItems.findRef(fi) == -1)
                _newItems.removeRef(fi); // also does ++it
            else
                ++it;
        }
    }

    // job is not running
    if(thumbJob.isNull())
    {
        //printf("Starting job %d\n", items.count());
        doStartThumbnailUpdate(_newItems);
    }
    // add new items to running job
    else
    {
        m_progressBox->addSteps(_newItems.count());

        if(newItemsAppend)
            thumbJob->appendItems(_newItems);
        else
            thumbJob->prependItems(_newItems);
    }
}

/*
 *  Clear current view and insert "..".
 */
void SQ_FileThumbView::clearView()
{
    // stop job
    stopThumbnailUpdate();
    slotRemoveToolTip();

    pixelSize = SQ_ThumbnailSize::instance()->extended() ?
            SQ_ThumbnailSize::instance()->extendedSize() : QSize(SQ_ThumbnailSize::instance()->pixelSize()+2,SQ_ThumbnailSize::instance()->pixelSize()+2);

    // clear
    KFileIconView::clearView();

    // insert ".."
    insertCdUpItem(SQ_WidgetStack::instance()->url());
}

/*
 *  Is thumbnail job running ?
 */
bool SQ_FileThumbView::updateRunning() const
{
    return !thumbJob.isNull();
}

/*
 *  Insert ".." item.
 */
void SQ_FileThumbView::insertCdUpItem(const KURL &base)
{
    static const QString &dirup = KGlobal::staticQString("..");

    KFileItem *fi = new KFileItem(base.upURL(), "inode/directory", S_IFDIR);

    // insert new item
    SQ_FileThumbViewItem *item = new SQ_FileThumbViewItem(this, dirup, directoryCache, fi);

    // item ".." won't be selectable
    item->setSelectable(false);

    fi->setExtraData(this, item);
}

/*
 *  All files are listed. Do something important.
 */
void SQ_FileThumbView::listingCompleted()
{
    arrangeItemsInGrid();
}

void SQ_FileThumbView::rebuildPendingPixmap(bool dir)
{
    QPixmap pixmapDir;

    if(dir)
        pixmapDir = SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, 48);

    QPixmap *p = dir ? &directoryCache : &pendingCache;
    QPixmap *w = dir ? &pixmapDir : &pending;

    p->resize(pixelSize.width(), pixelSize.height());

    QPainter painter(p);
    painter.setBrush(colorGroup().base());
    painter.setPen(colorGroup().highlight());
    painter.drawRect(0, 0, pixelSize.width(), pixelSize.height());
    painter.drawPixmap((pixelSize.width() - w->width())/2, (pixelSize.height() - w->height())/2, *w);
}

void SQ_FileThumbView::rebuildCachedPixmaps()
{
    pixelSize = SQ_ThumbnailSize::instance()->extended() ?
            SQ_ThumbnailSize::instance()->extendedSize() : QSize(SQ_ThumbnailSize::instance()->pixelSize()+2,SQ_ThumbnailSize::instance()->pixelSize()+2);

    // rebuild "pending" thumbnail
    rebuildPendingPixmap();

    // rebuild directory pixmap
    rebuildPendingPixmap(true);
}

void SQ_FileThumbView::itemRemoved(KFileItem *i)
{
    thumbJob->itemRemoved(i);
}

void SQ_FileThumbView::itemsRemoved(const KFileItemList &l)
{
    thumbJob->itemsRemoved(l);
}

void SQ_FileThumbView::showEvent(QShowEvent *e)
{
    KFileIconView::showEvent(e);

    if(isPending)
    {
        isPending = false;
        startThumbnailUpdate();
    }
}

void SQ_FileThumbView::startDrag()
{
    SQ_Config::instance()->setGroup("Fileview");

    if(SQ_Config::instance()->readBoolEntry("drag", true))
    {
        SQ_DragProvider::instance()->setParams(this, *KFileView::selectedItems(), SQ_DragProvider::Thumbnails);
        SQ_DragProvider::instance()->start();
    }
    else
        KFileIconView::startDrag();
}

void SQ_FileThumbView::setLazy(bool l, int delay)
{
    m_lazy = l;
    lazyDelay = delay <= 0 ? 500 : delay;

    // non-lazy mode requires to update all thumbnails
    if(!m_lazy)
    {
        m_lazy = true;

        stopThumbnailUpdate();
        doStartThumbnailUpdate(itemsToUpdate(true));

        // restore lazy mode
        m_lazy = false;
    }
}

void SQ_FileThumbView::resizeEvent(QResizeEvent *e)
{
    timerScroll->start(lazyDelay, true);

    KFileIconView::resizeEvent(e);
}

#include "sq_filethumbview.moc"
