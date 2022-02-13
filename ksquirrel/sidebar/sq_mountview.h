/***************************************************************************
                          sq_mountview.h  -  description
                             -------------------
    begin                : ??? Nov 29 2005
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

#ifndef SQ_MOUNTVIEW_H
#define SQ_MOUNTVIEW_H

#include <kfileiconview.h>


/*
 *  SQ_MountView is an icon view representing mount points.
 */

class SQ_MountView : public KFileIconView
{
    Q_OBJECT

    public: 
        SQ_MountView(QWidget *parent = 0, const char *name = 0);
	~SQ_MountView();

    private slots:

        /*
         *  Item executed. We should emit path() signal.
         */
        void slotExecuted(QIconViewItem *i);

        /*
         *  Reload current view (reread currently mounted fs).
         */
        void slotReload();

    signals:
        void path(const QString &);
};

#endif
