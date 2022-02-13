/***************************************************************************
                          SQ_DirOperatorBase.h  -  description
                             -------------------
    begin                : Sep 8 2004
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

#ifndef SQ_DIROPERATORBASE_H
#define SQ_DIROPERATORBASE_H

#include <kurl.h>
#include <kfileview.h>
#include <kdiroperator.h>

class QIconViewItem;
class QListViewItem;
class QTimer;

class KAction;

class SQ_FileIconView;
class SQ_FileDetailView;
class SQ_FileThumbView;

class SQ_DirOperatorBase : public KDirOperator
{
	Q_OBJECT

	public:
		enum VV {TypeList = 0, TypeIcon, TypeDetail, TypeThumbs };

		SQ_DirOperatorBase(const KURL &url = KURL(), VV type_ = SQ_DirOperatorBase::TypeList, QWidget *parent = 0, const char *name = "");
		virtual ~SQ_DirOperatorBase();

		void setCurrentItem(KFileItem *item);
		bool isThumbView() const;
		void reconnectClick(bool firstconnect = false);

	private:
		void setupActions();

	protected:
		KFileView* createView(QWidget *parent, KFile::FileView view);

	public slots:
		void slotActivateExternalTool(int index);

	protected slots:
		void slotReturnPressed(QIconViewItem *item);
		void slotReturnPressed(QListViewItem *item);
		void slotExecuted(QIconViewItem *item);
		void slotExecuted(QListViewItem *item);
		void slotSelected(QIconViewItem*);
		void slotSelected(QListViewItem*);
		void slotUrlEntered(const KURL&);
		void slotDelayedDecode();

	signals:
		void tryUnpack(KFileItem *item);

	public:
		KFileView 				*fileview;
		SQ_FileThumbView		*tv;
		SQ_FileIconView			*iv;
		SQ_FileDetailView		*dv;
		bool 					sing;

	protected:
		KActionMenu 		*pADirOperatorMenu, *pAFileActions;
		KAction				*pARunSeparately;

		VV 					type;
		QString				tobeDecoded;
		QTimer				*timer;
};

#endif
