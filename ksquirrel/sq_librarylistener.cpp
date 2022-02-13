/***************************************************************************
                          sq_librarylistener.cpp  -  description
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

#include "sq_librarylistener.h"

#include <qsocketnotifier.h>
#include <fam.h>
#include <qstring.h>
#include <fcntl.h>

#include "ksquirrel.h"

SQ_LibraryListener::SQ_LibraryListener(QObject * parent, const char *name) : QObject(parent, name)
{}

SQ_LibraryListener::~SQ_LibraryListener()
{}

bool SQ_LibraryListener::openFAM()
{
	fam = new FAMConnection;

	if(!fam) return false;

	if(FAMOpen2(fam, "ksquirrel") != 0)
	{
		delete fam;
		fam = 0;
		return false;
	}

	int famfd = FAMCONNECTION_GETFD(fam);
	int flags = fcntl(famfd, F_GETFL);
	fcntl(famfd, F_SETFL, flags | O_NONBLOCK);

	sn = new QSocketNotifier(famfd, QSocketNotifier::Read, 0);
	connect(sn, SIGNAL(activated(int)), SLOT(readFam(int)));

	return true;
}

void SQ_LibraryListener::readFAM(int id)
{
static const char *famevent[10] =
{
		"",
		"FAMChanged",
		"FAMDeleted",
		"FAMStartExecuting",
		"FAMStopExecuting",
		"FAMCreated",
		"FAMMoved",
		"FAMAcknowledge",
		"FAMExists",
		"FAMEndExist"
};

	printf("\nFAM: caught \"%s\" signal\n", famevent[id]);

	while (fam && FAMPending(fam))
	{
		FAMEvent ev;

		if (FAMNextEvent(fam, &ev) != 1)
 		{
			closeFAM();
			return;
		}

//		emit(signal_checkMail());
	}
}

void SQ_LibraryListener::closeFAM()
{
	if(!fam) return;

	delete sn;
	sn = 0;
	FAMClose(fam);
	delete fam;
	fam = 0;
}

void SQ_LibraryListener::startMonitoring(const QString &path)
{
	if(openFAM())
		FAMMonitorDirectory(fam, path.ascii(), 0, 0);
}
