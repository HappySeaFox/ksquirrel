/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Dec 10 2003
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
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
#include <qdir.h>
#include <qfile.h>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kconfig.h>
#include <dcopclient.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <cstdlib>

#include "ksquirrel.h"
#include "sq_splashscreen.h"
#include "sq_hloptions.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* ****************************************************************** */

// Our command line options
static KCmdLineOptions options[] =
{
    {"+[file to open]", I18N_NOOP("File to be opened at startup."), 0},
    {"l", I18N_NOOP("Print found libraries and exit."), 0},
    {"d", 0, 0},
    {"print-dcop", I18N_NOOP("Print available DCOP parameters"), 0},
    {"nodirectorybasket", I18N_NOOP("Folder basket"), 0},
    {"noimagebasket", I18N_NOOP("Image basket"), 0},
    {"nocategories", I18N_NOOP("Categories"), 0},
    {"nomountview", I18N_NOOP("Mount view"), 0},

    KCmdLineLastOption
};

int main(int argc, char *argv[])
{
    KSquirrel            *SQ;
    SQ_HLOptions         *high;
    const QCString App = "ksquirrel";

    KAboutData aboutData(
            "ksquirrel",
            "KSquirrel",
            VERSION,
            I18N_NOOP("KSquirrel - image viewer for KDE"),
            KAboutData::License_GPL,
            "(c) 2003-2007 Baryshev Dmitry",
            QString::null,
            "http://ksquirrel.sourceforge.net",
            "ksquirrel.iv@gmail.com");

    // setup 'About' dialog
    aboutData.addAuthor("Dmitry Baryshev aka Krasu", "Author", "ksquirrel.iv@gmail.com", QString::null);
    aboutData.addCredit("Andrey Rahmatullin aka wrar", I18N_NOOP("Bug reports, patches"), "wrar@altlinux.ru", QString::null);
    aboutData.addCredit("SeaJey", I18N_NOOP("Testing"), "seajey.serg@gmail.com", QString::null);
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
        high->param = QFile::decodeName(sq_args->arg(0));

    high->showLibsAndExit = sq_args->isSet("l");
    bool printDCOP = sq_args->isSet("d");

    high->have_directorybasket = sq_args->isSet("directorybasket");
    high->have_imagebasket     = sq_args->isSet("imagebasket");
    high->have_categories      = sq_args->isSet("categories");
    high->have_mountview       = sq_args->isSet("mountview");

    KApplication    a;

    if(!QGLFormat::hasOpenGL())
    {
        qWarning("KSquirrel: this system has no OpenGL support. Exiting." );
        exit(1);
    }

    // don't even try to connect to DCOP if -l specified - 
    // anyway we will exit later
    if(!high->showLibsAndExit)
    {
        bool reg = a.dcopClient()->isApplicationRegistered(App);

        // Check if KSquirrel already registered.
        // If registered, send an url to it.
        if(reg && !high->param.isEmpty())
        {
            // Yes, it is registered. Let's send a message to it.
            QCString replyType;
            QByteArray data, replyData;
            QDataStream dataStream(data, IO_WriteOnly);

            dataStream << high->param;

            if(!a.dcopClient()->call(App, App, "load(QString)", data, replyType, replyData))
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
    }

    KGlobal::dirs()->addResourceType("data", KStandardDirs::kde_default("data") + QString::fromLatin1("ksquirrel"));

    SQ_SplashScreen *splash = 0;

    // should we show a splash screen ?
    KConfig *config = new KConfig("ksquirrelrc");
    config->setGroup("Main");

    if(config->readBoolEntry("splash", true))
    {
        splash = new SQ_SplashScreen(0, "ksquirrel-splash-screen");
        if(!high->showLibsAndExit) splash->show(); // don't show splash when -l
        KApplication::flush();
    }

    delete config;

    // connect to DCOP server and register KSquirrel. Now we can
    // send messages to KSquirrel (see README for parameters)
    if(a.dcopClient()->attach())
        a.dcopClient()->registerAs(App, false);

    // create an instance
    SQ = new KSquirrel(0, App);

    if(printDCOP)
        SQ->printDCOP();

    a.setMainWidget(SQ);

    sq_args->clear();

    return a.exec();
}
