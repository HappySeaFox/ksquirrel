/***************************************************************************
                          sq_fileiconviewbase.h  -  description
                             -------------------
    begin                : ??? Feb 22 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_FILEICONVIEWBASE_H
#define SQ_FILEICONVIEWBASE_H

#include <kfileiconview.h>

class KonqFileTip;

/*
 *  SQ_FileIconViewBase is a base class for icon view, list view and
 *  thumbnail view with drag-and-drop support.
 */

class SQ_FileIconViewBase : public KFileIconView
{
    Q_OBJECT

    public: 
        SQ_FileIconViewBase(QWidget *parent = 0, const char *name = 0);
        virtual ~SQ_FileIconViewBase();

        virtual void insertCdUpItem(const KURL &base) = 0;
        virtual void updateView(const KFileItem *i);

    protected:

        /*
         *  Get KFileIconViewItem by KFileItem. All KFileItems store
         *  a pointer to appropriate KFileIconViewItem as extra data.
         *  See also KFileItem::setExtraData() and insertItem().
         */
        KFileIconViewItem* viewItem(const KFileItem *item);
        /*
         *  Accept dragging.
         */
        virtual void dragEnterEvent(QDragEnterEvent *);

        /*
         *  Handle double clicks.
         */
        virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);

        virtual bool eventFilter(QObject *o, QEvent *e);

        void initItemMy(KFileIconViewItem *item, const KFileItem *i, bool upd = false);

    signals:
        void invokeBrowser();

    public slots:
        void slotResetToolTip();

    protected slots:
        /*
         *  Show extended tooltip for 'item'.
         */
        void slotShowToolTip(QIconViewItem *item);

        /*
         *  Remove tootip and stop timer.
         */
        void slotRemoveToolTip();

    private:
        /*
         *  Tooltip for file item.
         */
        KonqFileTip *toolTip;
};

#endif
