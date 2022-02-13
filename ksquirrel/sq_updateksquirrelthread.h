/***************************************************************************
                          sq_updateksquirrelthread.h  -  description
                             -------------------
    begin                : ??? ??? 26 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_UPDATEKSQUIRRELTHREAD_H
#define SQ_UPDATEKSQUIRRELTHREAD_H

#include <qthread.h>
#include <qobject.h>

class QNetworkOperation;

/**
  *@author Baryshev Dmitry
  */

class SQ_UpdateKsquirrelThread : public QObject, public QThread
{
	Q_OBJECT

	public: 
		SQ_UpdateKsquirrelThread();
		~SQ_UpdateKsquirrelThread();

	protected:
		virtual void run();

	private slots:
		void slotFinished(QNetworkOperation *netop);

	signals:
		void needUpdate(const QString &ver);

	private:
		QString file;
};

#endif
