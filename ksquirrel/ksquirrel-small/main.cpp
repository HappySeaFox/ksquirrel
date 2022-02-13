/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Dec 10 2003
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

#include <qgl.h>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kapplication.h>

#include <stdlib.h>

#include "sq_glview.h"
#include "sq_about.h"
#include "sq_hloptions.h"

/////////////////////////////////////////////////////////////////////////////////////

// Our command line options
static KCmdLineOptions options[] =
{
	{"+[file or folder to open]", I18N_NOOP("File or folder to be opened at startup."), 0},
	KCmdLineLastOption
};

int main(int argc, char *argv[])
{
	SQ_GLView		*SQ;
	SQ_HLOptions		*high;

	aboutData.addAuthor("Dmitry Baryshev aka Krasu", "Author", "ksquirrel@tut.by", QString::null);
        aboutData.addCredit("Andrey Rahmatullin aka wrar", I18N_NOOP("Bug reports, patches"), "wrar@altlinux.ru", QString::null);
        aboutData.addCredit("JaguarWan", I18N_NOOP("Bug reports"), "jaguarwan@gmail.com", QString::null);
	aboutData.addCredit("NightGoblin", I18N_NOOP("Translation help"), 0, "http://nightgoblin.info");
        aboutData.addCredit(I18N_NOOP("TiamaT"), I18N_NOOP("Initial artwork for edit tools"), "plasticfantasy@tut.by", "http://www.livejournal.com/users/tiamatik/");
        aboutData.addCredit(I18N_NOOP("Fera"), I18N_NOOP("Great artwork for edit tools"), "morrigan171@mail.ru", QString::null);
	aboutData.addCredit(I18N_NOOP("OpenGL forum at"), 0, 0, "http://opengl.org");
	aboutData.addCredit(I18N_NOOP("GameDev forum at"), 0, 0, "http://gamedev.ru");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	KCmdLineArgs *sq_args = KCmdLineArgs::parsedArgs();

	high = new SQ_HLOptions;

	if(sq_args->count())
		high->path = sq_args->url(0).path();

	KApplication		a;

	if(!QGLFormat::hasOpenGL())
	{
		qWarning("KSquirrel: this system has no OpenGL support. Exiting." );
		exit(1);
	}

	SQ = new SQ_GLView;

	a.setMainWidget(SQ);

	return a.exec();
}
