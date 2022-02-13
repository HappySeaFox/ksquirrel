/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kapplication.h>

#include "ksquirrel.h"
#include "sq_about.h"
#include "sq_hloptions.h"

/////////////////////////////////////////////////////////////////////////////////////

static KCmdLineOptions options[] =
{
	{I18N_NOOP("+[file or folder to open]"), I18N_NOOP("File or folder to be opened at startup."), 0},
	{"l", I18N_NOOP("Print found libraries and exit."), 0},
	KCmdLineLastOption
};         

int main(int argc, char *argv[])
{
	KSquirrel 			*SQ;
	SQ_HLOptions		*high;

	aboutData.addAuthor("Dmitry Baryshev aka Krasu", "Author", "ksquirrel@tut.by", QString::null);
	aboutData.addCredit("NightGoblin", I18N_NOOP("Translation help"), 0, "http://nightgoblin.info");
	aboutData.addCredit(I18N_NOOP("OpenGL forum at"), 0, 0, "http://opengl.org");
	aboutData.addCredit(I18N_NOOP("GameDev forum at"), 0, 0, "http://gamedev.ru");
	aboutData.addCredit(I18N_NOOP("A great description of various file formats at"), 0, 0, "http://www.wotsit.org");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication	a;

	KCmdLineArgs *sq_args = KCmdLineArgs::parsedArgs();

	high = new SQ_HLOptions;

	if(!sq_args->isSet("l"))
	{
		if(sq_args->count() > 0)
			high->path = sq_args->url(0).path();
	}
	else
		high->showLibsAndExit = true;

	SQ = new KSquirrel(high, 0, "KMainWindow");

	a.setMainWidget(SQ);

	sq_args->clear();

	return a.exec();
}
