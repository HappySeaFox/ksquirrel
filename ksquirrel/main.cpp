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

#include <qvariant.h>

#include "ksquirrel.h"
#include "sq_about.h"
#include "sq_application.h"
#include "sq_hloptions.h"

#include <X11/Xlib.h>

/////////////////////////////////////////////////////////////////////////////////////

static KCmdLineOptions options[] =
{
	{"f <url_file_or_directory>", I18N_NOOP(""), 0},
	{ 0, 0, 0 }
};         

int main(int argc, char *argv[])
{
	const QCString App = "ksquirrel";

	XInitThreads();

	Squirrel 			*SQ;
	SQ_HLOptions		*high;

	aboutData.addAuthor("Dmitry Baryshev aka CKulT", "Author, first programming, code cleanups, fisrt GL viewer.", "squirrel-sf@yandex.ru", "http://ckult.narod.ru");
	aboutData.addCredit("OpenGL forum at", 0, 0, "http://opengl.org");
	aboutData.addCredit("OpenGL forum at", 0, 0, "http://opengl.org.ru");
	aboutData.addCredit("A great description of various file formats at", 0, 0, "http://www.wotsit.org");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	KCmdLineArgs *sq_args = KCmdLineArgs::parsedArgs();

	// create "high level options" instance, and parse cmdline args
	// we need "high level options", because we have to ignore some options, that were set through "Options" dialog, but now set through cmdline.
	// as an example - SQ_WidgetStack sets initial path to SQ_DirOperator to "high->HL_url", if it is not empty, and reads general options otherwise (from sqConfig)
	high = new SQ_HLOptions;
	high->HL_url = QVariant(sq_args->getOption("f")).toString();

	// create app after KCmdLineArgs::init(...) !
	SQ_Application	a;

	KConfig *tmpConfig = new KConfig(QString("ksquirrelrc"));

	// check for another SQuirrel, if exists - send SQ_ACTIVATE message
	tmpConfig->setGroup("Main");
	if(a.dcopClient()->isApplicationRegistered(App) && tmpConfig->readBoolEntry("activate another", true))
	{
		QCString replyType;
		QByteArray data, replyData;
		QDataStream dataStream(data, IO_WriteOnly);

		if(!high->HL_url.isEmpty())
			dataStream << (QString("SQ_ACTIVATE_WITH_FILE") + high->HL_url.path());
		else
			dataStream << QString("SQ_ACTIVATE");

		if(a.dcopClient()->call(App, "KMainWindow", "control(QString)", data, replyType, replyData) == false)
			printf("\nUnable to send data to old instance of SQuirrel: exiting anyway.\n");

		sq_args->clear();
		delete tmpConfig;
		return 0;
	}

	delete tmpConfig;

	// create instance
	SQ = new Squirrel(high, 0, "KMainWindow");

	a.setMainWidget(SQ);

	if(a.dcopClient()->attach())
		a.dcopClient()->registerAs(App, false);

	sq_args->clear();

	return a.exec();
}
