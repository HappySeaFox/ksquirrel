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
#include <qtooltip.h>
#include <qtimer.h>
#include <qapplication.h>

#include <kurldrag.h>
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

SQ_FileThumbView::SQ_FileThumbView(QWidget *parent, const char *name) : SQ_FileIconViewBase(parent, name), isPending(false)
{
    toolTip = 0;

    // create progress bar
    m_progressBox = new SQ_ProgressBox(this);

    // create timer 
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTooltipDelay()));

    // setup cache limit
    SQ_Config::instance()->setGroup("Thumbnails");
    SQ_PixmapCache::instance()->setCacheLimit(SQ_Config::instance()->readNumEntry("cache", 1024*10));

    setResizeMode(QIconView::Adjust);

    // load "pending" pixmaps
    pending = SQ_IconLoader::instance()->loadIcon("clock", KIcon::Desktop, 32);

    // some hacks for tooltip support
    disconnect(this, SIGNAL(onViewport()), this, 0);
    disconnect(this, SIGNAL(onItem(QIconViewItem *)), this, 0);
    connect(this, SIGNAL(onItem(QIconViewItem *)), this, SLOT(slotShowToolTip(QIconViewItem *)));
    connect(this, SIGNAL(onViewport()), this, SLOT(slotRemoveToolTip()));

    rebuildCachedPixmaps();
}

SQ_FileThumbView::~SQ_FileThumbView()
{
    slotRemoveToolTip();
}

void SQ_FileThumbView::slotSelected(QIconViewItem *item, const QPoint &point)
{
    emit doubleClicked(item, point);
}

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

    // update item
    item->repaint();
}

void SQ_FileThumbView::startThumbnailUpdate()
{
    stopThumbnailUpdate();
    doStartThumbnailUpdate(*items());
}

/*
 *  Create job, connect signals and start updating
 */
void SQ_FileThumbView::doStartThumbnailUpdate(const KFileItemList &list)
{
    // update progress bar
    SQ_WidgetStack::instance()->thumbnailUpdateStart(list.count());

    // create new job
    thumbJob = new SQ_ThumbnailLoadJob(list);

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
    // job is not running
    if(thumbJob.isNull()){ //printf("Starting job %d\n", items.count());
        doStartThumbnailUpdate(items); }
    // add new items to running job
    else { //printf("Appending to job %d\n", items.count());
    m_progressBox->addSteps(items.count());
//    QApplication::flush();
        if(append)thumbJob->appendItems(items);else thumbJob->prependItems(items);}
}

// Show extended tooltip for item under mouse cursor
void SQ_FileThumbView::slotShowToolTip(QIconViewItem *item)
{
    SQ_Config::instance()->setGroup("Thumbnails");

    if(!SQ_Config::instance()->readBoolEntry("tooltips", false) ||
        (!KSquirrel::app()->isActiveWindow() && SQ_Config::instance()->readBoolEntry("tooltips_inactive", true)))
        return;

    // remove previous tootip and stop timer
    slotRemoveToolTip();

    SQ_FileThumbViewItem* fitem = dynamic_cast<SQ_FileThumbViewItem*>(item);

    if(!fitem)
        return;

    if(!SQ_LibraryHandler::instance()->libraryForFile(fitem->fileInfo()->url().path()))
        return;

    tooltipFor = fitem;

    timer->start(600, true);
}

/*
 *  Remove tootip and stop timer.
 */
void SQ_FileThumbView::slotRemoveToolTip()
{
    timer->stop();

    delete toolTip;
    toolTip = 0;
}

bool SQ_FileThumbView::eventFilter(QObject *o, QEvent *e)
{
    if(o == viewport() || o == this)
    {
        int type = e->type();

        if(type == QEvent::Leave || type == QEvent::FocusOut)
            slotRemoveToolTip();
    }

    return KFileIconView::eventFilter(o, e);
}

/*
 *  On "hideEvent()" delete tooltip.
 */
void SQ_FileThumbView::hideEvent(QHideEvent *e)
{
    slotRemoveToolTip();

    KFileIconView::hideEvent(e);
}

/*
 *  Delayed tooltip
 */
void SQ_FileThumbView::slotTooltipDelay()
{
    if(!tooltipFor)
        return;

    KFileItem *f = tooltipFor->fileInfo();

    if(!f)
        return;

    toolTip = new QLabel(i18n("<table cellspacing=0><tr><td align=left>File:</td><td align=left><b>%1</b></td></tr><tr><td align=left>Size:</td><td align=left><b>%2</b></td></tr>%3</table>")
        .arg(KStringHandler::csqueeze(f->name()))
        .arg(KIO::convertSize(f->size()))
        .arg(f->isLink() ? i18n("<tr><td align=left>Link destination:</td><td align=left>%1</td></tr>").arg(KStringHandler::csqueeze(f->linkDest())) : QString::null)
        ,
        0, "myToolTip", WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder
        | WStyle_Tool | WX11BypassWM);

    // setup tooltip
    toolTip->setFrameStyle(QFrame::Plain | QFrame::Box);
    toolTip->setLineWidth(1);
    toolTip->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    toolTip->move(QCursor::pos() + QPoint(5, 5));
    toolTip->adjustSize();

    QRect screen = KGlobalSettings::desktopGeometry(KSquirrel::app());

    if(toolTip->x() + toolTip->width() > screen.right())
        toolTip->move(toolTip->x()+screen.right()-toolTip->x()-toolTip->width(), toolTip->y());

    if(toolTip->y() + toolTip->height() > screen.bottom())
        toolTip->move(toolTip->x(), screen.bottom()-toolTip->y()-toolTip->height()+toolTip->y());

    toolTip->setFont(QToolTip::font());
    toolTip->setPaletteBackgroundColor(colorGroup().highlight());
    toolTip->setPaletteForegroundColor(colorGroup().highlightedText());

    // set original palette
    toolTip->setPalette(QToolTip::palette());

    // finally, show tootip
    toolTip->show();
}

/*
 *  Clear current view and insert "..".
 */
void SQ_FileThumbView::clearView()
{
    // stop job
    stopThumbnailUpdate();

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

#include "sq_filethumbview.moc"
