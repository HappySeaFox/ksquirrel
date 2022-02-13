/***************************************************************************
                          sq_filedetailview.h  -  description
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

#ifndef SQ_FILEDETAILVIEW_H
#define SQ_FILEDETAILVIEW_H

#include <qwidget.h>
#include <kfiledetailview.h>

class SQ_FileDetailView : public KFileDetailView
{
    Q_OBJECT

	public:
		SQ_FileDetailView(QWidget* parent, const char* name);
		~SQ_FileDetailView();

	protected slots:
		void slotSelected(QListViewItem* item, const QPoint &, int c);

	protected:
		QDragObject *dragObject();
};

#endif
