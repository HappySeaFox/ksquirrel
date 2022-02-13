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
		SQ_DirOperator(const KURL &url = KURL(), VV type_ = SQ_DirOperator::TypeList, QWidget *parent = 0, const char *name = "");
		virtual ~SQ_DirOperator();

		void setDirLister(KDirLister *lister);

	public slots:
		void slotFinishedLoading();
		void slotDelayedFinishedLoading();
		void slotSetThumbSize(const QString&);

	protected slots:
		void slotDeletedItem(KFileItem *);
		void slotUpdateInformation(int,int);
		void slotSelected(QIconViewItem*);
		void slotSelected(QListViewItem*);
		void slotUrlEntered(const KURL&);
};

#endif
