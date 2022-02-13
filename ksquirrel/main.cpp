/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <dcopclient.h>

#include "ksquirrel.h"
#include "sq_about.h"

#include "sq_version.h"

#include "sq_application.h"

/////////////////////////////////////////////////////////////////////////////////////

static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};         

int main(int argc, char *argv[])
{
	aboutData.addAuthor("CKulT", 0, "squirrel-sf@yandex.ru");
	aboutData.addCredit("", "", "", 0);
	
	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	SQ_Application a;

	char name_app[100];
	sprintf(name_app, "%s%d", APP, version);

	Squirrel *SQ = new Squirrel;
	SQ->setCaption(name_app);

	a.setMainWidget(SQ);

	a.dcopClient()->registerAs("ksquirrel");

	return a.exec();
}
