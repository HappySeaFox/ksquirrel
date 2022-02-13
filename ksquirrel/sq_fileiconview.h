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

#ifndef SQ_FILEICONVIEW_H
#define SQ_FILEICONVIEW_H

#include "sq_fileiconviewbase.h"

#include <qwidget.h>

class QPoint;

/*
 *  SQ_FileIconView represents icon view and list view in filemanager.
 */

/*
 *  SQ_FileIconViewItem represents one item in icon/list view.
 */
class SQ_FileIconViewItem : public KFileIconViewItem
{
    public:
        SQ_FileIconViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi);
        ~SQ_FileIconViewItem();

    protected:
        /*
         *  Reimplement paintFocus() to ignore painting focus.
         */
        virtual void paintFocus(QPainter *p, const QColorGroup &cg);
};

class SQ_FileIconView : public SQ_FileIconViewBase
{
    Q_OBJECT

    public:
        SQ_FileIconView(QWidget *parent = 0, const char *name = "");
        ~SQ_FileIconView();

        /*
         *  Get SQ_FileIconViewItem by KFileItem. All KFileItems store
         *  a pointer to appropriate SQ_FileIconViewItem as extra data.
         *  See also KFileItem::setExtraData() and insertItem().
         */
        SQ_FileIconViewItem* viewItem(const KFileItem *item);

        /*
         *  Internal.
         */
        virtual void updateView(const KFileItem *i);
        virtual void updateView(bool b);

        /*
         *  Reimplement insertItem() to enable/disable inserting
         *  directories (depends on current settings).
         */
        virtual void insertItem(KFileItem *i);

        /*
         *  Clear current view and insert "..".
         */
        virtual void clearView();

        /*
         *  All files are listed. Do something important.
         */
        virtual void listingCompleted();

        /*
         *  Insert ".." item.
         */
        virtual void insertCdUpItem(const KURL &baseurl);

    private:
        /*
         *  Internal. Set item's sorting key.
         */
        void initItem(SQ_FileIconViewItem *item, const KFileItem *i);

    protected slots:
        void slotSelected(QIconViewItem *item, const QPoint &point);

    private:
        QPixmap    dirPix;
};

#endif
