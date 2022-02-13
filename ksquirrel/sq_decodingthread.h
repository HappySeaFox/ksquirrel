/***************************************************************************
                          sq_decodingthread.h  -  description
                             -------------------
    begin                : ??? ??? 4 2004
    copyright            : (C) 2004 by CKulT
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

#ifndef SQ_DECODINGTHREAD_H
#define SQ_DECODINGTHREAD_H

#include <qthread.h>
#include <qobject.h>

#include "defs.h"

/**
  *@author CKulT
  */

class QSocket;
class QTextStream;
class SQ_LIBRARY;
class QPainter;
class SQ_Progress;
class SQ_GLWidget;

class SQ_DecodingThread : public QObject, public QThread
{
	Q_OBJECT

	public: 
		SQ_DecodingThread(SQ_GLWidget *);
		~SQ_DecodingThread();

		void setParams(SQ_LIBRARY *, fmt_info *, unsigned long, RGBA *);
		void stop();
		void moveProgressBar();

	public slots:
		void slotConnected();

	protected:
		virtual void run();

	private:
		fmt_info 		*finfo;
		unsigned long	realW;
		RGBA		*next;
		SQ_LIBRARY	*lib;
		QSocket		*socket;
		bool			connected;
		QTextStream 	*os;
		QPainter		*p;
		bool			m_stop, indicator;
		SQ_Progress *pdraw;
		SQ_GLWidget	*ww;
};

#endif
