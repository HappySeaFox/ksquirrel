#include <qapplication.h>
#include <string.h>
#include <qtimer.h>

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <dcopclient.h>

#include "ksquirrel.h"
#include "sq_about.h"

#include "sq_version.h"

/////////////////////////////////////////////////////////////////////////////////////

static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};         

int main(int argc, char *argv[])
{
	aboutData.addAuthor("CKulT", 0, "ckult@yandex.ru");
	aboutData.addCredit("", "", "", 0);
	
	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication a;

	char name_app[100];
	sprintf(name_app, "%s%d", APP, version);

	Squirrel *SQ = new Squirrel;
	SQ->setCaption(name_app);

	a.setMainWidget(SQ);

	a.dcopClient()->registerAs("ksquirrel");

	int result = a.exec();

	delete SQ;

	return result;
}
