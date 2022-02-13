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
#include <kconfig.h>
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
	const QCString App = "ksquirrel";

	QString			name_app;
	Squirrel 			*SQ;

	aboutData.addAuthor("CKulT", "Author, first programming, code cleanups,\nfisrt GL viewer.", "squirrel-sf@yandex.ru", "http://ckult.narod.ru");
	aboutData.addCredit("OpenGL forum at", 0, 0, "http://opengl.org.ru");
	aboutData.addCredit("A great description of various file formats at", 0, 0, "http://www.wotsit.org");
	aboutData.addCredit("Special thanks to: Axiome, Converter,");
	aboutData.addCredit("Front Line Assembly, Hocico, Hybrids,");
	aboutData.addCredit("Hypnoskull, Mlada Fronta, Mono No Aware,");
	aboutData.addCredit("Shinjuku Thief, Synapscape, Dive,");
	aboutData.addCredit("Noise Unit, Sonar, Somatic Responses,");
	aboutData.addCredit("Gods Tower, Dimmu Borgir, Nine Inch Nails,");
	aboutData.addCredit("Therion, Brujeria, Children of Bodom,");
	aboutData.addCredit("In Extremo, Rammstein, Fear Factory,");
	aboutData.addCredit("and other mad industrial & rock'n'roll men.");
	aboutData.addCredit("Your music is a great support for me.");
	aboutData.addCredit(" ");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	KCmdLineArgs *sq_args = KCmdLineArgs::parsedArgs();

	// create app after KCmdLineArgs::init(...) !
	SQ_Application	a;

	KConfig *tmpConfig = new KConfig(QString("ksquirrelrc"));

	// check for another SQuirrel
	tmpConfig->setGroup("Main");
	if(a.dcopClient()->isApplicationRegistered(App) && tmpConfig->readBoolEntry("activate another", true))
	{
		QCString replyType;
		QByteArray data, replyData;
		QDataStream dataStream(data, IO_WriteOnly);
		dataStream << QString("ACTIVATE");

		if(a.dcopClient()->call(App, "KMainWindow", "control(QString)", data, replyType, replyData) == false)
			printf("\nUnable to send data to old SQuirrel: exiting anyway.\n");

		sq_args->clear();
		delete tmpConfig;
		return 0;
	}

	delete tmpConfig;

	// create instance
	SQ = new Squirrel(0, "KMainWindow");
	name_app.sprintf("%s%d", APP, version);
	SQ->setCaption(name_app);

	a.setMainWidget(SQ);

	if(a.dcopClient()->attach())
		a.dcopClient()->registerAs(App, false);

	sq_args->clear();

	return a.exec();
}
