/***************************************************************************
                          sq_diroperator.h  -  description
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

#ifndef SQ_DIROPERATOR_H
#define SQ_DIROPERATOR_H

#include "sq_diroperatorbase.h"

class SQ_DirOperator : public SQ_DirOperatorBase
{
	Q_OBJECT

	public:
		SQ_DirOperator(const KURL &url = KURL(), VV type_ = SQ_DirOperator::TypeList,
						QWidget *parent = 0, const char *name = 0);
		virtual ~SQ_DirOperator();

		void setDirLister(KDirLister *lister);

	private:
		/*
		 *  Update current file's icon and name
		 *  in statusbar. Will be called after some item
		 *  has been selected.
		 */
		void statusFile(KFileItem *);

	public slots:
		/*
		 *  Invoked, when current directory has been loaded.
		 */
		void slotFinishedLoading();

		/*
		 *  Select first supported image in current directory.
		 */
		void slotDelayedFinishedLoading();

		/*
		 *  Change thumbnail size.
		 */
		void slotSetThumbSize(const QString&);

	protected slots:
		/*
		 *  Invoked, when some item has been deleted. We should
		 *  remove appropriate thumbnail from pixmap cache.
		 */
		void slotDeletedItem(KFileItem *);
		void slotUpdateInformation(int,int);

		/*
		 *  Some item has been selected.
		 */
		void slotSelected(QIconViewItem*);
		void slotSelected(QListViewItem*);
};

#endif
