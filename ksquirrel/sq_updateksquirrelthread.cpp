/***************************************************************************
                          sq_updateksquirrelthread.cpp  -  description
                             -------------------
    begin                : Jan 26 2005
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

#include <qurloperator.h>
#include <qdir.h>

#include "sq_updateksquirrelthread.h"
#include "sq_about.h"

SQ_UpdateKsquirrelThread::SQ_UpdateKsquirrelThread() : QObject(), QThread()
{}

SQ_UpdateKsquirrelThread::~SQ_UpdateKsquirrelThread()
{}

void SQ_UpdateKsquirrelThread::run()
{
	QUrlOperator *op = new QUrlOperator();

	connect(op, SIGNAL(finished(QNetworkOperation*)), this, SLOT(slotFinished(QNetworkOperation*)));

	op->copy(QString::fromLatin1("http://ksquirrel.sourceforge.net/sq_version"), QString::fromLatin1("file:/%1").arg(QDir::homeDirPath()));
}

void SQ_UpdateKsquirrelThread::slotFinished(QNetworkOperation *netop)
{
	if(netop->state() == QNetworkProtocol::StDone)
	{
		QString file = QString::fromLatin1("%1/sq_version").arg(QDir::homeDirPath());

		QFile f(file);

		if(f.open(IO_ReadOnly))
		{
			QString ver;
			f.readLine(ver, 100);

			if(ver != QString(SQ_VERSION))
				emit needUpdate(ver);

			f.close();
		}
	}
}
