/***************************************************************************
                          sq_fileiconview.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by Baryshev Dmitry
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

#ifndef SQ_FILETHUMBVIEW_H
#define SQ_FILETHUMBVIEW_H

#include <qguardedptr.h>
#include <qsize.h>

#include "sq_fileiconviewbase.h"
#include "sq_thumbnailinfo.h"

class QHBox;
class QToolButton;
class QTimer;

class SQ_ProgressBox;
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

        bool lazy() const;

        void setLazy(bool l, int delay);

        void setAddRows(int rows);

        void rebuildCachedPixmaps();

        /*
         *  Reimplement insertItem() to enable/disable inserting
         *  directories (depends on current settings) and inserting thumbnails.
         */
        virtual void insertItem(KFileItem *i);

        /*
         *  Append new items to thumbnail generating job.
         */
        void addItemsToJob(const KFileItemList &items, bool append = true);

        /*
         *  Delete item from thumbnail generating job.
         */
        void itemRemoved(KFileItem *i);
        void itemsRemoved(const KFileItemList &);

        /*
         *  Internal.
         */
        virtual void updateView(bool)
        {}

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
        void doStartThumbnailUpdate(const KFileItemList &list);

        /*
         *  Is thumbnail job running ?
         */
        bool updateRunning() const;

        /*
         *  Insert ".." item.
         */
        virtual void insertCdUpItem(const KURL &base);

        /*
         *  Don't start thumbnail job until thumbnail view is hidden.
         */
        void waitForShowEvent();

        SQ_ProgressBox* progressBox() const;

    protected:
        virtual void resizeEvent(QResizeEvent *);
        virtual void startDrag();

        /*
         *  Show event. Let's start thumbnail job, if needed.
         */
        virtual void showEvent(QShowEvent *);

    private:

        /*
         *  rebuild "pending" thumbnail for supported items
         */
        void rebuildPendingPixmap(bool dir = false);

        KFileItemList itemsToUpdate(bool fromAll = false);

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
        /*
         *  Start or stop thumbnail update.
         */
        void slotThumbnailUpdateToggle();

    private slots:

        void slotContentsMoving(int, int);
        void slotDelayedContentsMoving();
        void slotDelayedAddItems();

    public:
        /*
         *  Layout box with progress bar and "stop" button.
         */
        SQ_ProgressBox    *m_progressBox;

        /*
         *  "Stop" button.
         */
        QToolButton    *buttonStop;

    private:
        QGuardedPtr<SQ_ThumbnailLoadJob> thumbJob;

        /*
         *  "Pending" thumbnail.
         */
        QPixmap pending;

        QTimer      *timerScroll, *timerAdd;

        QPixmap    directoryCache,  pendingCache;
        QSize    pixelSize;

        KFileItemList newItems;
        bool          newItemsAppend;

        /*
         *  Thumbnail job won't start until isPending
         *  is false. It means, that thumbnail view currently is hidden.
         */
        bool    isPending;
        bool    m_lazy;
        int     lazyDelay;
        int     m_rows;
};

inline
void SQ_FileThumbView::waitForShowEvent()
{
    isPending = true;
}

inline
SQ_ProgressBox* SQ_FileThumbView::progressBox() const
{
    return m_progressBox;
}

inline
bool SQ_FileThumbView::lazy() const
{
    return m_lazy;
}

inline
void SQ_FileThumbView::setAddRows(int rows)
{
    m_rows = rows;
}

#endif
