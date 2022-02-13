/***************************************************************************
                          sq_fileiconview.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
    email                : squirrel-sf@yandex.ru
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

#include <kfileiconview.h>

#include <qwidget.h>
#include <qpoint.h>

class SQ_FileIconView : public KFileIconView
{
    Q_OBJECT

	public:
		SQ_FileIconView(QWidget *parent = 0, const char *name = "");
		~SQ_FileIconView();

		KFileIconViewItem* viewItem(KFileItem *item);

	protected:
		QDragObject *dragObject();
		void dragEnterEvent(QDragEnterEvent *);
		void dropEvent(QDropEvent *);

	protected slots:
		void slotSelected(QIconViewItem *item, const QPoint &point);

};


#endif
