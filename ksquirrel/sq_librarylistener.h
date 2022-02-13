/***************************************************************************
                          sq_librarylistener.h  -  description
                             -------------------
    begin                : Fri Mar 26 2004
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

#ifndef SQ_LIBRARYLISTENER_H
#define SQ_LIBRARYLISTENER_H

#include <qobject.h>
#include <kdirlister.h>

/**
  *@author ckult
  */

class SQ_LibraryListener : public KDirLister
{
	Q_OBJECT

	public: 
		SQ_LibraryListener(bool = false);
		~SQ_LibraryListener();

	public slots:
		void slotStarted(const KURL &);
		void slotCompleted();
		void slotNewItems(const KFileItemList &);
		void slotDeleteItem(KFileItem *);
		void slotOpenURL(const KURL&, bool, bool);
		void slotShowInfo(const QStringList &linfo, bool added);

	signals:
		void showInfo(const QStringList &linfo, bool added);
		void finishedInit();

	private:
		KURL			url;
		QStringList	list;
		bool 			operation;

};

#endif
