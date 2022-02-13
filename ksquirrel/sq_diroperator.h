/***************************************************************************
                          sq_diroperator.h  -  description
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

#ifndef SQ_DIROPERATOR_H
#define SQ_DIROPERATOR_H

#include <kurl.h>
#include <kfileview.h>
#include <kdiroperator.h>

class QIconViewItem;
class QListViewItem;
class KAction;

class SQ_FileIconView;
class SQ_FileDetailView;
class SQ_FileThumbView;
class SQ_ThumbnailThread;

class SQ_DirOperator : public KDirOperator
{
	Q_OBJECT

	public:
		enum VV {TypeList = 0, TypeIcon, TypeDetail, TypeThumbs };
		SQ_DirOperator(const KURL &url = KURL(), VV type_ = SQ_DirOperator::TypeList, QWidget *parent = 0, const char *name = "");
		virtual ~SQ_DirOperator();

		void setIconSize(int sz);
		void reconnectClick();
		void setCurrentItem(KFileItem *item);
		bool isThumbView() const;

	protected:
		KFileView* createView(QWidget *parent, KFile::FileView view);

	public slots:
		void setShowHiddenFilesF(bool s);
		void slotRunSeparately();
		void slotActivateExternalTool(int index);
		void slotFinishedLoading();
		void slotDelayedFinishedLoading();
		void slotShowExternalToolsMenu();
		void slotUrlEntered(const KURL&);

		void emitSelected(const QString &file);
		void slotSetThumbSize(const QString&);

	protected slots:
		void slotReturnPressed(QIconViewItem *item);
		void slotReturnPressed(QListViewItem *item);
		void slotExecuted(QIconViewItem *item);
		void slotExecuted(QListViewItem *item);
		void slotSelected(QIconViewItem*);
		void slotSelected(QListViewItem*);
		void slotNewItems(const KFileItemList&);
		void slotDeletedItem(KFileItem *);
		void slotUpdateInformation(int,int);

	signals:
		void tryUnpack(const QString &path);

	public:
		KFileView 			*fileview;
		SQ_FileThumbView	*tv;
		SQ_FileIconView		*iv;
		SQ_FileDetailView		*dv;

	private:
		bool 			sing;
		KActionMenu 		*pADirOperatorMenu;
		KAction			*pARunSeparately, *pAShowEMenu;
		SQ_ThumbnailThread *thumbThread;
		VV type;
};

#endif
