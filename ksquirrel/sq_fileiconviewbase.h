/***************************************************************************
                          sq_fileiconviewbase.h  -  description
                             -------------------
    begin                : ??? Feb 22 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_FILEICONVIEWBASE_H
#define SQ_FILEICONVIEWBASE_H

#include <kfileiconview.h>

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

    signals:
        void launch(KFileItem *);
        void highlighted(KFileItem *);

    protected:
        /*
         *  Accept dragging.
         */
        virtual void dragEnterEvent(QDragEnterEvent *);

        /*
         *  Handle double clicks.
         */
        virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);

    private:
        void exec(QIconViewItem *i, bool single, bool hl = false);

    private slots:
        void slotMouseButtonClicked(int, QIconViewItem *);
        void slotDoubleClicked(QIconViewItem *i);
        void slotCurrentChanged(QIconViewItem *i);
};

#endif
