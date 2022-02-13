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

#ifndef SQ_FILETHUMBVIEW_H
#define SQ_FILETHUMBVIEW_H

#include "sq_fileiconviewbase.h"

#include <qguardedptr.h>
#include <qmap.h>

#include "sq_thumbnailinfo.h"

class QHBox;
class QToolButton;
class QTimer;

class SQ_Progress;
class SQ_ThumbnailLoadJob;
class SQ_FileThumbViewItem;

class SQ_FileThumbView : public SQ_FileIconViewBase
{
	Q_OBJECT

	public:
		SQ_FileThumbView(QWidget *parent = 0, const char *name = "");
		~SQ_FileThumbView();

		KFileIconViewItem* viewItem(const KFileItem *item);
		virtual void insertItem(KFileItem *i);
		void appendItems(const KFileItemList &items);

		virtual void updateView(const KFileItem *i);
		virtual void updateView(bool b);
		virtual void clearView();
		virtual void listingCompleted();

		void doStartThumbnailUpdate(const KFileItemList* list);
		void waitForShowEvent();
		bool updateRunning() const;
		virtual void insertCdUpItem(const KURL &base);

	protected:
		virtual bool eventFilter(QObject *o, QEvent *e);
		virtual void hideEvent(QHideEvent *);
		virtual void showEvent(QShowEvent *);

	private:
		void initItem(KFileIconViewItem *item, const KFileItem *i);

	public slots:
		void startThumbnailUpdate();
		void stopThumbnailUpdate();
		void setThumbnailPixmap(const KFileItem* fileItem,const SQ_Thumbnail&);

	protected slots:
		void slotSelected(QIconViewItem *item, const QPoint &point);
		void slotThumbnailUpdateToggle();

	private slots:
		void slotShowToolTip(QIconViewItem *item);
		void slotRemoveToolTip();
		void slotTooltipDelay();

	public:
		QHBox			*progressBox;
		SQ_Progress		*progress;
		QToolButton		*buttonStop;

	private:
		QGuardedPtr<SQ_ThumbnailLoadJob> thumbJob;
		QMap<QString, QPixmap> pending;
		QLabel 					*toolTip;
		SQ_FileThumbViewItem	*tooltipFor;
		QTimer 					*timer;
		bool						isPending;
		QPixmap					dirPix;
		int							pixelSize;
};


#endif
