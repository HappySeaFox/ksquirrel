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

#include <qpainter.h>
#include <qhbox.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qtimer.h>

#include <kurldrag.h>
#include <kstandarddirs.h>
#include <kstringhandler.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_iconloader.h"
#include "sq_config.h"
#include "sq_dir.h"
#include "sq_filethumbview.h"
#include "sq_libraryhandler.h"
#include "sq_thumbnailjob.h"
#include "sq_thumbnailsize.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_pixmapcache.h"
#include "sq_progress.h"
#include "sq_filethumbviewitem.h"

SQ_FileThumbView::SQ_FileThumbView(QWidget *parent, const char *name) : SQ_FileIconViewBase(parent, name), isPending(false)
{
    kdDebug() << "+SQ_FileThumbView" << endl;

    toolTip = NULL;
    disconnect(this, SIGNAL(clicked(QIconViewItem*, const QPoint&)), this, 0);

    // create layout box 
    progressBox = new QHBox(this);
    QToolBar *progressBoxBar = new QToolBar(QString::null, NULL, progressBox);
    buttonStop = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumb_resume.png")),
    QString::null, QString::null, this, SLOT(slotThumbnailUpdateToggle()), progressBoxBar);

    // create progress bar
    progress = new SQ_Progress(progressBox);

    // setup progress bar
    progressBox->setPalette(QPalette(QColor(255,255,255), QColor(255,255,255)));
    progressBox->setFrameShape(QFrame::Box);
    progressBox->setSpacing(3);
    progressBox->setStretchFactor(progress, 1);
    progressBox->setStretchFactor(progressBoxBar, 0);
    progressBox->setGeometry(5, 5, 235, 24);

    // create timer 
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTooltipDelay()));

    // setup cache limit
    SQ_Config::instance()->setGroup("Thumbnails");
    SQ_PixmapCache::instance()->setCacheLimit(SQ_Config::instance()->readNumEntry("cache", 1024*10));

    setResizeMode(QIconView::Adjust);

    // load "pending" pixmaps
    pending.insert("48",  QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_pending48.png")));
    pending.insert("64",  QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_pending64.png")));
    pending.insert("96",  QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_pending96.png")));
    pending.insert("128", QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_pending128.png")));

    // some hacks for tooltip support
    disconnect(this, SIGNAL(onViewport()), this, 0);
    disconnect(this, SIGNAL(onItem(QIconViewItem *)), this, 0);
    connect(this, SIGNAL(onItem(QIconViewItem *)), this, SLOT(slotShowToolTip(QIconViewItem *)));
    connect(this, SIGNAL(onViewport()), this, SLOT(slotRemoveToolTip()));

    // load "directory" pixmap
    dirPix = SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, 48);
    pixelSize = SQ_ThumbnailSize::instance()->currentPixelSize();
}

SQ_FileThumbView::~SQ_FileThumbView()
{
    kdDebug() << "-SQ_FileThumbView" << endl;

    pending.clear();

    slotRemoveToolTip();
}

void SQ_FileThumbView::slotSelected(QIconViewItem *item, const QPoint &point)
{
    emit doubleClicked(item, point);
}

/*
 *  Get KFileIconViewItem by KFileItem. All KFileItems store
 *  a pointer to appropriate KFileIconViewItem as extra data.
 *  See also KFileItem::setExtraData() and insertItem().
 */
KFileIconViewItem* SQ_FileThumbView::viewItem(const KFileItem *item)
{
    return item ? ((KFileIconViewItem *)item->extraData(this)) : NULL;
}

/*
 *  Reimplement insertItem() to enable/disable inserting
 *  directories (depends on current settings) and inserting thumbnails.
 */
void SQ_FileThumbView::insertItem(KFileItem *i)
{
//    int pixelSize = SQ_ThumbnailSize::instance()->currentPixelSize();

    SQ_Config::instance()->setGroup("Fileview");

    // directores disabled ?
    if(i->isDir() && SQ_Config::instance()->readBoolEntry("disable_dirs", false))
        return;

    SQ_Config::instance()->setGroup("Thumbnails");

    // mimetype disabled ?
    bool disable_mime = SQ_Config::instance()->readBoolEntry("disable_mime", false);

    // common icon for all mimetypes
    const QPixmap mimeall = SQ_IconLoader::instance()->loadIcon("unknown", KIcon::Desktop,
                pixelSize);

    SQ_FileThumbViewItem *item;

    QPixmap thumbnail(pixelSize, pixelSize);
    QPainter painter(&thumbnail);
    painter.eraseRect(0, 0, pixelSize, pixelSize);
    painter.setBrush(white);
    painter.setPen(colorGroup().highlight());
    painter.drawRect(0, 0, pixelSize, pixelSize);

    if(i->isDir())
        painter.drawPixmap((pixelSize-dirPix.width())/2, (pixelSize-dirPix.height())/2, dirPix);
    else
        painter.drawPixmap((pixelSize-mimeall.width())/2, (pixelSize-mimeall.height())/2,
                            (disable_mime ? mimeall : i->pixmap(pixelSize)));

    // "pending" thumbnail
    QPixmap p = pending[QString(SQ_ThumbnailSize::instance()->stringValue())];
    QPixmap p2(pixelSize, pixelSize);

    if(!p.isNull())
    {
        QPainter painter2(&p2);
        painter2.eraseRect(0, 0, pixelSize, pixelSize);
        painter2.setBrush(white);
        painter2.setPen(colorGroup().highlight());
        painter2.drawRect(0, 0, pixelSize, pixelSize);
        painter2.drawPixmap((pixelSize - p.width())/2, (pixelSize-p.height())/2, p);
    }

    if(i->isDir())
        item = new SQ_FileThumbViewItem(this, i->text(), thumbnail, i);
    else if(SQ_LibraryHandler::instance()->supports(i->url().path()) && !p2.isNull())
        item = new SQ_FileThumbViewItem(this, i->text(), p2, i);
    else if(disable_mime)
        item = new SQ_FileThumbViewItem(this, i->text(), mimeall, i);
    else
        item = new SQ_FileThumbViewItem(this, i->text(), thumbnail, i);

    initItem(item, i);

    i->setExtraData(this, item);
}

/*
 *  Internal. Set item's sorting key.
 */
void SQ_FileThumbView::initItem(KFileIconViewItem *item, const KFileItem *i)
{
    // determine current sorting type
    QDir::SortSpec spec = KFileView::sorting();

    if(spec & QDir::Time)
        item->setKey(sortingKey((unsigned long)i->time(KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
    else if(spec & QDir::Size)
        item->setKey(sortingKey(i->size(), i->isDir(), spec));
    else
        item->setKey(sortingKey(i->text(), i->isDir(), spec));
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

    int pixelSize = SQ_ThumbnailSize::instance()->currentPixelSize();

    // erase original pixmap
    QPainter painter(item->pixmap());
    painter.eraseRect(0, 0, pixelSize, pixelSize);
    painter.setPen(colorGroup().highlight());
    painter.setBrush(white);

    // draw bounding rect
    painter.drawRect(0, 0, pixelSize, pixelSize);

    // draw pixmap
    painter.drawImage((pixelSize - t.thumbnail.width())/2, (pixelSize - t.thumbnail.height())/2, t.thumbnail);

    // Extended thumbnail also have mime icon and dimensions
    if(SQ_ThumbnailSize::instance()->extended())
    {
        // draw mime icon
        if(!t.info.mime.isNull() && SQ_ThumbnailSize::instance()->extended())
        painter.drawImage(QPoint(pixelSize - t.info.mime.width() - 1, 1), t.info.mime);

        // draw dimensions
        QFont font = painter.font();
        font.setPixelSize(12);
        painter.setFont(font);
        painter.setPen(colorGroup().text());
        painter.drawText(0, pixelSize - SQ_ThumbnailSize::instance()->margin(), pixelSize, SQ_ThumbnailSize::instance()->margin(),
        Qt::AlignCenter, QString::fromLatin1("%1x%2").arg(t.info.dimensions).arg(t.info.bpp));
    }

    // store thumbnail information
    item->setInfo(t);

    // update item
    item->repaint();
}

void SQ_FileThumbView::startThumbnailUpdate()
{
    stopThumbnailUpdate();
    doStartThumbnailUpdate(items());
}

/*
 *  Create job, connect signals and start updating
 */
void SQ_FileThumbView::doStartThumbnailUpdate(const KFileItemList* list)
{
    // create new job
    thumbJob = new SQ_ThumbnailLoadJob(list);

    connect(thumbJob, SIGNAL(thumbnailLoaded(const KFileItem*, const SQ_Thumbnail &)),
    this, SLOT(setThumbnailPixmap(const KFileItem*, const SQ_Thumbnail&)));

    connect(thumbJob, SIGNAL(result(KIO::Job*)), SQ_WidgetStack::instance(), SLOT(thumbnailsUpdateEnded()));

    // update progress bar
    SQ_WidgetStack::instance()->thumbnailUpdateStart(list->count());

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
void SQ_FileThumbView::appendItems(const KFileItemList &items)
{
    // job is not running
    if(thumbJob.isNull())
        doStartThumbnailUpdate(&items);

    // add new items to running job
    else 
    thumbJob->appendItems(items);
}

void SQ_FileThumbView::updateView(bool b)
{
    if(!b)
        return;

    KFileIconViewItem *item = static_cast<KFileIconViewItem*>(QIconView::firstItem());

    if(item)
    {
        do
        {
            item->setPixmap((item->fileInfo())->pixmap(SQ_ThumbnailSize::instance()->pixelSize()));
            item = static_cast<KFileIconViewItem*>(item->nextItem());
        }while(item);
    }
}

void SQ_FileThumbView::updateView(const KFileItem *i)
{
     KFileIconViewItem *item = viewItem(i);

    if(item)
        initItem(item, i);
}

// Show extended tooltip for item under mouse cursor
void SQ_FileThumbView::slotShowToolTip(QIconViewItem *item)
{
    SQ_Config::instance()->setGroup("Thumbnails");

    if(!SQ_Config::instance()->readBoolEntry("tooltips", false))
        return;

    // remove previous tootip and stop timer
    slotRemoveToolTip();

    if(!item)
        return;

    SQ_FileThumbViewItem* fitem = dynamic_cast<SQ_FileThumbViewItem*>(item);

    if(!fitem)
        return;

    if(!SQ_LibraryHandler::instance()->supports(fitem->fileInfo()->url().path()))
        return;

    tooltipFor = fitem;

    timer->start(450, true);
}

/*
 *  Remove tootip and stop timer.
 */
void SQ_FileThumbView::slotRemoveToolTip()
{
    timer->stop();

    if(toolTip)
    {
        delete toolTip;
        toolTip = NULL;
    }
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
 *  Don't start thumbnail job until thumbnail view is hidden.
 */
void SQ_FileThumbView::waitForShowEvent()
{
    isPending = true;
}

/*
 *  "showEvent()"! Let's start thumbnail job, if needed.
 */
void SQ_FileThumbView::showEvent(QShowEvent *e)
{
    KFileIconView::showEvent(e);

    if(isPending)
    {
        isPending = false;
        startThumbnailUpdate();
    }
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

    // construct tooltip
    if(f->isLink())
        toolTip = new QLabel(i18n("File: %1\nSize: %2\nLink to: %3\n\n%4")
            .arg(KStringHandler::csqueeze(tooltipFor->text()))
            .arg(KIO::convertSize(f->size()))
            .arg(KStringHandler::csqueeze(f->linkDest()))
            .arg(tooltipFor->fullInfo()), 0, "myToolTip",
            WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder
            | WStyle_Tool | WX11BypassWM);
    else
        toolTip = new QLabel(i18n("File: %1\nSize: %2\n\n%3")
            .arg(KStringHandler::csqueeze(tooltipFor->text()))
            .arg(KIO::convertSize(f->size()))
            .arg(tooltipFor->fullInfo()), 0, "myToolTip",
            WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder
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

    pixelSize = SQ_ThumbnailSize::instance()->currentPixelSize();

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

    // create pixmap for ".."
    QPixmap thumbnail(pixelSize, pixelSize);
    QPainter painter(&thumbnail);
    painter.eraseRect(0, 0, pixelSize, pixelSize);
    painter.setBrush(white);
    painter.setPen(colorGroup().highlight());
    painter.drawRect(0, 0, pixelSize, pixelSize);

    // draw "directory" pixmap
    painter.drawPixmap((pixelSize-dirPix.width())/2, (pixelSize-dirPix.height())/2, dirPix);

    KFileItem *fi = new KFileItem(base.upURL(), QString::null, KFileItem::Unknown);

    // insert new item
    SQ_FileThumbViewItem *item = new SQ_FileThumbViewItem(this, dirup, thumbnail, fi);

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
