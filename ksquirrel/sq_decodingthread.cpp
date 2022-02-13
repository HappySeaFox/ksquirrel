/***************************************************************************
                          sq_decodingthread.cpp  -  description
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
#include <qsocket.h>
#include <qtextstream.h>

#include <klocale.h>

#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_progress.h"
#include "sq_libraryhandler.h"
#include "sq_progress.h"
#include "sq_decodingthread.h"
#include "sq_server.h"

SQ_DecodingThread::SQ_DecodingThread(SQ_GLWidget *glw) : QObject(this), QThread(), connected(false), m_stop(false)
{
	socket = new QSocket(this);
	os = new QTextStream(socket);
	p = 0L;
	ww = glw;

	socket->connectToHost("localhost", glw->server->port());

	connect(socket, SIGNAL(connected()), SLOT(slotConnected()));

	pdraw = new SQ_Progress(glw);
}

SQ_DecodingThread::~SQ_DecodingThread()
{
	if(running())
	{
		stop();
		wait();
	}

	delete pdraw;
}

void SQ_DecodingThread::run()
{
	unsigned int i;

	if(!lib || !finfo || !next) return;

	indicator =  sqConfig->readBoolEntry("GL view", "progress", true);

	pdraw->setShown(indicator);
	pdraw->setTotalSteps(finfo->h);
	pdraw->move(2, ww->height() - 22);

	for(i = 0;i < finfo->h;i++)
	{
		if(m_stop)
		{
			m_stop = false;
			pdraw->hide();
//			lib->fmt_close(finfo);
			return;
		}

		lib->fmt_read_scanline(finfo, next + realW*i);

		if(indicator)
			pdraw->advance(1);
	}

	lib->fmt_close(finfo);

	pdraw->hide();
	pdraw->flush();

	if(connected)
		*os << "1";
}

void SQ_DecodingThread::setParams(SQ_LIBRARY *l, fmt_info *f, unsigned long rw, RGBA *n)
{
	lib = l;
	finfo = f;
	realW = rw;
	next = n;
}

void SQ_DecodingThread::slotConnected()
{
	connected = true;
}

void SQ_DecodingThread::stop()
{
	m_stop = true;
}

void SQ_DecodingThread::moveProgressBar()
{
	pdraw->move(2, ww->height() - 22);
}
