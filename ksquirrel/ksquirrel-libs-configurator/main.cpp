/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Dec 03 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <klocale.h>

#include "klc.h"

static KCmdLineOptions options[] =
{
    KCmdLineLastOption
};

int main(int argc, char *argv[])
{
    const QCString App = "ksquirrel-libs-configurator";

    KAboutData aboutData(
            "ksquirrel-libs-configurator",
            "ksquirrel-libs-configurator",
            "0.1",
            I18N_NOOP("ksquirrel-libs-configurator"),
            KAboutData::License_GPL,
            "(c) 2007 Baryshev Dmitry",
            QString::null,
            "http://ksquirrel.sourceforge.net",
            "ksquirrel.iv@gmail.com");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    KGlobal::locale()->insertCatalogue("ksquirrel");
    KGlobal::locale()->setActiveCatalogue("ksquirrel");

    KLC *klc = new KLC(0, App);

    app.setMainWidget(klc);

    klc->show();

    return app.exec();
}
