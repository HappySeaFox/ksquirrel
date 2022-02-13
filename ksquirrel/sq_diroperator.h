/***************************************************************************
                          sq_diroperator.h  -  description
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

#ifndef SQ_DIROPERATOR_H
#define SQ_DIROPERATOR_H

#include "sq_diroperatorbase.h"

class SQ_DirOperator : public SQ_DirOperatorBase
{
    Q_OBJECT

    public:
        SQ_DirOperator(const KURL &url = KURL(), ViewT type_ = SQ_DirOperator::TypeList,
                                    QWidget *parent = 0, const char *name = 0);
        virtual ~SQ_DirOperator();

        /*
         *  If current view type is "thumbnail view", then
         *     - if thumbnail view is visible (QWidget::isVisible()), start
         *       thumbnail update;
         *     - if thumbnail view is not visible, keep in mind
         *       (SQ_FileThumbView::waitForShowEvent()) that we should start
         *       thumbnail update later.
         */
        void startOrNotThumbnailUpdate();

        /*
         *  Smart update. Store all file items, reset view,
         *  and transfer all items back.
         */
        void smartUpdate();

        /*
         *  Remove ".." item from current view
         */
        void removeCdUpItem();

    private:
        /*
         *  Update current file's icon and name
         *  in statusbar. Will be called after some item
         *  has been selected.
         */
        void statusFile(KFileItem *);

    public slots:
        /*
         *  Invoked, when current directory has been loaded.
         */
        void slotFinishedLoading();

        /*
         *  Select first supported image in current directory.
         */
        void slotDelayedFinishedLoading();

        /*
         *  Change thumbnail size.
         */
        void slotSetThumbSize(const QString&);

        /*
         *  Invoked, when user selected some external tool in menu.
         */
        void slotActivateExternalTool(int index);

    protected slots:

        /*
         *  Invoked, when some item has been deleted. We should
         *  remove appropriate thumbnail from pixmap cache.
         */
        void slotItemDeleted(KFileItem *);

        void slotUpdateInformation(int,int);

        /*
         *  Some item has been selected.
         */
        void slotSelected(QIconViewItem*);
        void slotSelected(QListViewItem*);
};

#endif
