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

class QSocketNotifier;
class QString;
class FAMConnection;

/**
  *@author ckult
  */

class SQ_LibraryListener : public QObject
{
	Q_OBJECT

	private:
		FAMConnection	*fam;
		QSocketNotifier	*sn;

		bool openFAM();

	public: 
		SQ_LibraryListener(QObject * parent = 0, const char *name = 0);
		~SQ_LibraryListener();

		void readFAM();
		void closeFAM();
		void startMonitoring(const QString &);

	public slots:
		void readFAM(int);
};

#endif
