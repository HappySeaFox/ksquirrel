/****************************************************************************
** Form interface generated from reading ui file './sq_grabwidget.ui'
**
** Created: Чтв Янв 8 05:26:35 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SQ_GRABWIDGET_H
#define SQ_GRABWIDGET_H

#include <qvariant.h>
#include <qdialog.h>
#include <qpixmap.h>
#include <qstring.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;

class SQ_GrabWidget : public QDialog
{
	Q_OBJECT

	private:
		QPixmap screenshot;
	public:
		SQ_GrabWidget( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
		~SQ_GrabWidget();

		QPushButton* closeButton;
		QGroupBox* groupBox2;
		QLabel* textLabel;
		QSpinBox* secondsSpin;
		QCheckBox* grabwindowCheck;
		QGroupBox* groupBox1;
		QPushButton* grabButton;
		QPushButton* saveAs, *quickSave, *hideButton;
		QLabel* lScreenShot;

	public slots:
		void slotStartGrab();
		void slotGrab();
		void slotQuickSave(QString filename = QString(""));
		void slotSaveAs();		

	protected:
		QGridLayout* SQ_GrabWidgetLayout;
		
};

#endif
