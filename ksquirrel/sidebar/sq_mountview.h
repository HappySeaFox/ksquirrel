/***************************************************************************
                          sq_mountview.h  -  description
                             -------------------
    begin                : ??? Nov 29 2005
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

#ifndef SQ_MOUNTVIEW_H
#define SQ_MOUNTVIEW_H

#include <qstringlist.h>

#include <klistview.h>

class KPopupMenu;

class SQ_MountViewItem;

/*
 *  SQ_MountView is an detailed view representing mount points.
 */

class SQ_MountView : public KListView
{
    Q_OBJECT

    public: 
        SQ_MountView(QWidget *parent = 0, const char *name = 0);
	~SQ_MountView();

        enum { OPT_COL_MOUNTPOINT = 1, OPT_COL_DEVICE = 2, OPT_COL_FSTYPE = 4, OPT_COL_OPTIONS = 8 };

        static SQ_MountView* instance() { return m_inst; }

        void setupColumns();

        void reload(bool current = true);

    private:
        void setColumns(int cols);
        bool exists(const QString &);

    private slots:

        void slotContextMenu(KListView *, QListViewItem *i, const QPoint &p);
        /*
         *  Item executed. We should emit path() signal.
         */
        void slotExecuted(QListViewItem *i);

        // for context menu
        void slotRefresh();
        void slotMount();
        void slotUnmount();
        void slotUnmountFinished();
        void slotMountFinished();
        void slotMountFinished2();
        void slotMountError();

    signals:
        void path(const QString &);

    private:
        int m_columns;
        SQ_MountViewItem *mountItem, *citem;
        QStringList mounted;
        KPopupMenu *popup;
        int id_mount, id_unmount;

        static SQ_MountView *m_inst;
};

#endif
