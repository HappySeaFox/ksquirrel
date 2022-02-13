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

#ifndef SQ_FILETHUMBVIEW_H
#define SQ_FILETHUMBVIEW_H

#include <kfileiconview.h>

#include <qguardedptr.h>

class QHBox;
class QToolButton;

class SQ_Progress;
class SQ_ThumbnailLoadJob;

class SQ_FileThumbView : public KFileIconView
{
    Q_OBJECT

	public:
		SQ_FileThumbView(QWidget *parent = 0, const char *name = "");
		~SQ_FileThumbView();

		KFileIconViewItem* viewItem(const KFileItem *item);
		virtual void insertItem(KFileItem *i);
		void appendItems(const KFileItemList &items);

	protected:
		QDragObject *dragObject();
		void dragEnterEvent(QDragEnterEvent *);
		void dropEvent(QDropEvent *);

	private:
		void initItem(KFileIconViewItem *item, const KFileItem *i);
		void doStartThumbnailUpdate(const KFileItemList* list);

	protected slots:
		void slotSelected(QIconViewItem *item, const QPoint &point);
		void setThumbnailPixmap(const KFileItem* fileItem,const QPixmap &);
		void slotThumbnailUpdateToggle();

	public slots:
		void startThumbnailUpdate();
		void stopThumbnailUpdate();

	public:
 		QHBox			*progressBox;
		SQ_Progress		*sqProgress;
		QToolButton		*buttonStop;

	private:
		QGuardedPtr<SQ_ThumbnailLoadJob> thumbJob;
};


#endif
