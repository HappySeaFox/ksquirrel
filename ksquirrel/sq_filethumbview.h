/***************************************************************************
                          sq_fileiconview.h  -  description
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

#ifndef SQ_FILETHUMBVIEW_H
#define SQ_FILETHUMBVIEW_H

#include "sq_fileiconviewbase.h"

#include <qguardedptr.h>
#include <qmap.h>

#include "sq_thumbnailinfo.h"

class QHBox;
class QToolButton;
class QTimer;

class SQ_Progress;
class SQ_ThumbnailLoadJob;
class SQ_FileThumbViewItem;

/*
 *  SQ_FileThumbView represents thumbnail view. Can also update thumbnails (with the help of
 *  SQ_ThumbnailLoadJob) and show extended tooltips.
 */

class SQ_FileThumbView : public SQ_FileIconViewBase
{
    Q_OBJECT

    public:
        SQ_FileThumbView(QWidget *parent = 0, const char *name = "");
        ~SQ_FileThumbView();

        /*
         *  Get KFileIconViewItem by KFileItem. All KFileItems store
         *  a pointer to appropriate KFileIconViewItem as extra data.
         *  See also KFileItem::setExtraData() and insertItem().
         */
        KFileIconViewItem* viewItem(const KFileItem *item);

        /*
         *  Reimplement insertItem() to enable/disable inserting
         *  directories (depends on current settings) and inserting thumbnails.
         */
        virtual void insertItem(KFileItem *i);

        /*
         *  Append new items to thumbnail generating job.
         */
        void appendItems(const KFileItemList &items);

        /*
         *  Internal.
         */
        virtual void updateView(const KFileItem *i);
        virtual void updateView(bool b);

        /*
         *  Clear current view and insert "..".
         */
        virtual void clearView();

        /*
         *  All files are listed. Do something important.
         */
        virtual void listingCompleted();

        /*
         *  Create job, connect signals and start updating
         */
        void doStartThumbnailUpdate(const KFileItemList* list);

        /*
         *  Don't start thumbnail job until thumbnail view is hidden.
         */
        void waitForShowEvent();

        /*
         *  Is thumbnail job running ?
         */
        bool updateRunning() const;

        /*
         *  Insert ".." item.
         */
        virtual void insertCdUpItem(const KURL &base);

    protected:
        /*
         *  For tooltip support.
         */
        virtual bool eventFilter(QObject *o, QEvent *e);
        virtual void hideEvent(QHideEvent *);

        /*
         *  Show event! Let's start thumbnail job, if needed.
         */
        virtual void showEvent(QShowEvent *);

    private:
        /*
         *  Internal. Set item's sorting key.
         */
        void initItem(KFileIconViewItem *item, const KFileItem *i);

    public slots:
        /*
         *  Start and stop thumbnail update job.
         */
        void startThumbnailUpdate();
        void stopThumbnailUpdate();

        /*
         *  One thumbnail is loaded. Let's update KFileItem's pixmap.
         */
        void setThumbnailPixmap(const KFileItem* fileItem, const SQ_Thumbnail&);

    protected slots:
        void slotSelected(QIconViewItem *item, const QPoint &point);

        /*
         *  Start or stop thumbnail update.
         */
        void slotThumbnailUpdateToggle();

    private slots:
        /*
         *  Show extended tooltip for 'item'.
         */
        void slotShowToolTip(QIconViewItem *item);

        /*
         *  Remove tootip and stop timer.
         */
        void slotRemoveToolTip();

        /*
         *  Delayed tooltip
         */
        void slotTooltipDelay();

    public:
        /*
         *  Layout box with progress bar and "stop" button.
         */
        QHBox    *progressBox;

        /*
         *  Progress bar. It uses QPainter to draw progress line,
         *  which is MUCH faster, than using QProgressBar.
         */
        SQ_Progress    *progress;

        /*
         *  "Stop" button.
         */
        QToolButton    *buttonStop;

    private:
        QGuardedPtr<SQ_ThumbnailLoadJob> thumbJob;

        /*
         *  "Pending" thumbnails.
         */
        QMap<QString, QPixmap> pending;

        /*
         *  Tooltip for thumbnail item.
         */
        QLabel     *toolTip;

        SQ_FileThumbViewItem    *tooltipFor;
        QTimer     *timer;

        /*
         *  Thumbnail job won't start until isPending
         *  is false. It means, that thumbnail view currently is hidden.
         */
        bool    isPending;

        QPixmap    dirPix;
        int    pixelSize;
};

inline
void SQ_FileThumbView::waitForShowEvent()
{
    isPending = true;
}

#endif
