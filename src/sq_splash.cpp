#include <qapplication.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>

#include <kglobal.h>
#include <kiconloader.h>

#include "sq_splash.h"


SQ_Splash::SQ_Splash(QWidget *parent, const char *name) : QVBox(parent, name, WType_Modal | WStyle_Customize | WStyle_NoBorder | WDestructiveClose | WStyle_StaysOnTop)
{
	KIconLoader *loader = new KIconLoader(*(KGlobal::iconLoader()));

	picLabel = new QLabel(this);
	QPixmap pixmap = loader->loadIcon("folder", KIcon::Desktop, 256);

	picLabel->setPixmap(pixmap);

	pLInfo = new QLabel(this);
	pLInfo->setMargin(2);
	pLInfo->setPaletteBackgroundColor(QColor(0,0,0));
	pLInfo->setPaletteForegroundColor(QColor(10,255,10));

	move(QApplication::desktop()->width() / 2 - pixmap.width() / 2, QApplication::desktop()->height() / 2 - height() / 2);
	show();
}


SQ_Splash::~SQ_Splash() 
{}


void SQ_Splash::mousePressEvent(QMouseEvent *)
{
	hide();
}


void SQ_Splash::addInfo(const QString &s) 
{
	pLInfo->setText(s);
	qApp->processEvents();
}
