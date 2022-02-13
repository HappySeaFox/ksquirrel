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
#include <dcopclient.h>

#include <cstdlib>

#include "ksquirrel.h"
#include "sq_about.h"
#include "sq_hloptions.h"

/////////////////////////////////////////////////////////////////////////////////////

// Our command line options
static KCmdLineOptions options[] =
{
    {"+[file or folder to open]", I18N_NOOP("File or folder to be opened at startup."), 0},
    {"l", I18N_NOOP("Print found libraries and exit."), 0},
    {"t <starting folder>", I18N_NOOP("Find all supported images on disk and create thumbnails."), "/"},
    {"r", I18N_NOOP("Recursively scan (with -t option)."), "/"},
    KCmdLineLastOption
};

int main(int argc, char *argv[])
{
    KSquirrel             *SQ;
    SQ_HLOptions        *high;
    const QCString App = "ksquirrel";

    // setup 'About' dialog
    aboutData.addAuthor("Dmitry Baryshev aka Krasu", "Author", "ksquirrel@tut.by", QString::null);
    aboutData.addCredit("Andrey Rahmatullin aka wrar", I18N_NOOP("Bug reports, patches"), "wrar@altlinux.ru", QString::null);
    aboutData.addCredit("JaguarWan", I18N_NOOP("Bug reports"), "jaguarwan@gmail.com", QString::null);
    aboutData.addCredit("NightGoblin", I18N_NOOP("Translation help"), 0, "http://nightgoblin.info");
    aboutData.addCredit(I18N_NOOP("TiamaT"), I18N_NOOP("Initial artwork for edit tools"), "plasticfantasy@tut.by", "http://www.livejournal.com/users/tiamatik/");
    aboutData.addCredit(I18N_NOOP("Fera"), I18N_NOOP("Great artwork for edit tools"), "morrigan171@mail.ru", QString::null);
    aboutData.addCredit(I18N_NOOP("OpenGL forum at"), 0, 0, "http://opengl.org");
    aboutData.addCredit(I18N_NOOP("GameDev forum at"), 0, 0, "http://gamedev.ru");

    // parse command line
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *sq_args = KCmdLineArgs::parsedArgs();

    //create high level options
    high = new SQ_HLOptions;

    if(sq_args->count())
        high->path = sq_args->url(0).path();

    KApplication    a;

    if(!QGLFormat::hasOpenGL())
    {
        qWarning("KSquirrel: this system has no OpenGL support. Exiting." );
        exit(1);
    }

    bool reg = a.dcopClient()->isApplicationRegistered(App);

    // Check if KSquirrel already registered.
    // If registered, send an url to it.
    if(reg && !high->path.isEmpty())
    {
        // Yes, it is registered. Let's send a message to it.
        QCString replyType;
        QByteArray data, replyData;
        QDataStream dataStream(data, IO_WriteOnly);

        dataStream << high->path;

        if(!a.dcopClient()->call(App, App, "load_image(QString)", data, replyType, replyData))
            qDebug("\nUnable to send data to old instance of KSquirrel: exiting.\n");

        sq_args->clear();
        delete high;

        exit(0);
    }
    // If registered, but no url was specified in command line
    else if(reg)
    {
        QString data;

        if(!a.dcopClient()->send(App, App, "activate()", data))
            qDebug("\nUnable to send data to old instance of KSquirrel: exiting.\n");

        sq_args->clear();
        delete high;

        exit(0);
    }

    high->showLibsAndExit = sq_args->isSet("l");
    high->thumbs = sq_args->isSet("t");
    high->recurs = (high->thumbs) ? sq_args->isSet("r") : false;

    if(high->thumbs)
        high->thumbs_p = sq_args->getOption("t");

    // connect to DCOP server and register KSquirrel. Now we can
    // send messages to KSquirrel (see README for parameters)
    if(a.dcopClient()->attach())
        a.dcopClient()->registerAs(App, false);

    // create instance
    SQ = new KSquirrel(NULL, App);

    a.setMainWidget(SQ);

    sq_args->clear();

    return a.exec();
}
