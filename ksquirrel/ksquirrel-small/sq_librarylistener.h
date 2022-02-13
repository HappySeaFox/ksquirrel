/***************************************************************************
                          sq_librarylistener.h  -  description
                             -------------------
    begin                : Fri Mar 26 2004
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

#ifndef SQ_LIBRARYLISTENER_H
#define SQ_LIBRARYLISTENER_H

#include <qobject.h>
#include <kdirlister.h>

/*
 *  SQ_LibraryListener monitors dirctory with libraries, and
 *  reports if it has changed. It will call SQ_LibraryHandler::add(),
 *  if somebody copied new libraries, and SQ_LibraryHandler::remove(), if
 *  deleted.
 */

class SQ_LibraryListener : public KDirLister
{
	Q_OBJECT

	public: 
		SQ_LibraryListener(bool = false);
		~SQ_LibraryListener();

		static SQ_LibraryListener* instance();

	public slots:
		/*
		 *  Start monitor given directory.
		 */
		void slotOpenURL(const KURL&, bool, bool);

	private slots:
		/*
		 *  All operations completed.
		 */
		void slotCompleted();

		/*
		 *  New libraries found!
		 */
		void slotNewItems(const KFileItemList &);

		/*
		 *  An item deleted.
		 */
		void slotDeleteItem(KFileItem *);
		
		/*
		 *  Show dialog with information on new or dleetd libraries.
		 */
		void slotShowInfo(const QStringList &linfo, bool added);

	signals:
		void showInfo(const QStringList &linfo, bool added);

		/*
		 *  Done loading/deleting.
		 */
		void finishedInit();

	private:
		QStringList	list;
		bool 		operation;
		static SQ_LibraryListener *listener;
};

#endif
