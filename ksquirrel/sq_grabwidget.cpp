#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_grabwidget.ui'
**
** Created: Птн Янв 9 03:50:35 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_grabwidget.h"

#include <qapplication.h>
#include <qvariant.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qwmatrix.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qdatetime.h>

#include <kfiledialog.h>
#include "ksquirrel.h"

SQ_GrabWidget::SQ_GrabWidget(QWidget* parent, const char* name, bool modal, WFlags fl) : QDialog(parent, name, modal, fl)
{
	if (!name) setName("SQ_GrabWidget");

	groupBox2 = new QGroupBox(this, "groupBox2");
	groupBox2->setGeometry( QRect( 11, 225, 391, 109 ) );

	textLabel = new QLabel( groupBox2, "textLabel" );
	textLabel->setGeometry( QRect( 10, 30, 41, 15 ) );

	groupBox1 = new QGroupBox( this, "groupBox1" );
	groupBox1->setGeometry( QRect( 11, 11, 391, 208 ) );

	lScreenShot = new QLabel(groupBox1, "lScreenShot");
	lScreenShot->setGeometry(QRect( 10, 20, 210, 170));
	lScreenShot->setScaledContents(true);
	QWhatsThis::add(lScreenShot, QString("Current screenshot"));

	grabButton = new QPushButton(groupBox1, "grabButton");
	grabButton->setGeometry(QRect(250, 30, 130, 30));
	QWhatsThis::add(grabButton, QString("Create new screenshot"));

	hideButton = new QPushButton(groupBox1, "hideButton");
	hideButton->setGeometry(QRect( 250, 60, 130, 30));
	hideButton->setToggleButton(true);
	QWhatsThis::add(hideButton, QString("Hides main window"));
	
	quickSave = new QPushButton(groupBox1, "quick save");
	quickSave->setGeometry(QRect(250, 120, 130, 30));
	QWhatsThis::add(quickSave, QString("Quick save screenshot to $HOME/squirrel_screen-DAY-MONTH-YEAR-HOUR-MINUTE-SECOND-MILISECOND.png"));
	
	saveAs = new QPushButton(groupBox1, "save as");
	saveAs->setGeometry(QRect(250, 150, 130, 30));
	QWhatsThis::add(saveAs, QString("Browse file to save into"));

	secondsSpin = new QSpinBox( groupBox2, "secondsSpin" );
	secondsSpin->setGeometry( QRect( 60, 30, 60, 21 ) );

	grabwindowCheck = new QCheckBox( groupBox2, "grabwindowCheck" );
	grabwindowCheck->setGeometry( QRect( 10, 60, 165, 20 ) );
	
	closeButton = new QPushButton( this, "closeButton" );
	closeButton->setGeometry( QRect( 150, 340, 120, 28 ) );
	QWhatsThis::add(closeButton, QString("Close this window"));

	setCaption("Grabbing");
	textLabel->setText("Delay:");
	grabwindowCheck->setText("Grab window with cursor");
	groupBox1->setTitle("Current shot");
	groupBox2->setTitle("Settings");
	closeButton->setText("Close");
	grabButton->setText("New shot");
	saveAs->setText("Save As ...");
	quickSave->setText("Quick save");
	hideButton->setText("Hide SQ");

	setMinimumSize(QSize(414, 379));
	setMaximumSize(QSize(414, 379));

	connect(grabButton, SIGNAL(clicked()), this, SLOT(slotStartGrab()));
	connect(quickSave, SIGNAL(clicked()), this, SLOT(slotQuickSave()));
	connect(saveAs, SIGNAL(clicked()), this, SLOT(slotSaveAs()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(hideButton, SIGNAL(toggled(bool)), sqApp, SLOT(setHidden(bool)));

	slotStartGrab();
}

SQ_GrabWidget::~SQ_GrabWidget()
{}

void SQ_GrabWidget::slotStartGrab()
{
	hide();
	
	int sec = secondsSpin->value();

	if(sec)
       	QTimer::singleShot(sec*1000, this, SLOT(slotGrab()));
	else
       	QTimer::singleShot(100, this, SLOT(slotGrab()));
}

void SQ_GrabWidget::slotGrab()
{
	QPainter painter;
	QWMatrix m;

	double dx , dy;

	m.reset();

	screenshot = QPixmap::grabWindow(QApplication::desktop()->winId());

       int scaleX = lScreenShot->width();
       int scaleY = lScreenShot->height();

	dx = (double(scaleX) / screenshot.width());
	dy = (double(scaleY) / screenshot.height());

	m.scale(dx, dy);

	QPixmap *mappedS = new QPixmap(scaleX, scaleY);

	painter.begin(mappedS);
	painter.setWorldMatrix(m);
	painter.drawPixmap(0, 0, screenshot);
	painter.end();

	lScreenShot->setPixmap(*mappedS);
	show();
}

void SQ_GrabWidget::slotQuickSave(QString filename)
{
	QString suffix;
	QTime t = QTime::currentTime(Qt::LocalTime);
	QDate d = QDate::currentDate(Qt::LocalTime);
	suffix.sprintf("-%d.%d.%d-%d.%d.%d.%d", d.day(), d.month(), d.year(), t.hour(), t.minute(), t.second(), t.msec());

	QString savename = ((filename == "")?(QDir().home().absPath() + "/" + QString("squirrel_screen") + suffix + QString(".png")):filename);
	
	if(screenshot.save(savename, "PNG") == false)
	{
       	QMessageBox m("Saving screenshot", "unable to save a file", QMessageBox::Warning, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);
        	m.exec();
	}
}

void SQ_GrabWidget::slotSaveAs()
{
	QString s = KFileDialog::getSaveFileName("/", QString::null, this, "Choose a file");

	if(s == QString("") || s == QString::null) return;

	slotQuickSave(s);
}
