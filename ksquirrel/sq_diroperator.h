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
#include <kaction.h>

class QIconViewItem;
class QListViewItem;

class SQ_DirOperator : public KDirOperator
{
	Q_OBJECT

	private:
		KFile::FileView View;
		bool sing;

		KActionMenu 			*pADirOperatorMenu;
		KAction				*pARunSeparately, *pAShowEMenu;
		int					toolsId;

	public:
		SQ_DirOperator(const KURL &url = KURL(), QWidget *parent = 0, const char *name = "");
		virtual ~SQ_DirOperator();
		void setIconSize(int sz);
                void reInitToolsMenu();

		KFile::FileView 		getViewType();
		KFileView 			*fileview;

	protected:
		KFileView* createView(QWidget *parent, KFile::FileView view);

	protected slots:
		void slotDoubleClicked(QIconViewItem *item);
		void slotDoubleClicked(QListViewItem *item);
		void slotSelected(QIconViewItem*);
		void slotSelected(QListViewItem*);

	public slots:
		void setShowHiddenFilesF(bool s);
		void slotRunSeparately();
		void slotActivateExternalTool(int index);
		void slotFinishedLoading();
		void slotShowExternalToolsMenu();

		void emitSelected(const QString &file);
		void emitNextSelected();
		void emitPreviousSelected();
};

#endif
