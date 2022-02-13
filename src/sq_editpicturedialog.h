#ifndef  EDITPICTURE_H
#define EDITPICTURE_H

#include <qvariant.h>
#include <qdialog.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include "sq_resizewidget.h"

class SQ_EditPictureDialog : public QDialog
{
	Q_OBJECT

	public:
		SQ_EditPictureDialog(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
		~SQ_EditPictureDialog();

	private:		
		QTabWidget* tabWidget;
		SQ_ResizeWidget *ResizeWidget;
		QWidget* ConvertWidget;
		QPushButton* buttonHelp;
		QPushButton* buttonOk;
		QPushButton* buttonCancel;

	protected:
		QVBoxLayout* EditPictureLayout;
		QHBoxLayout* Layout1;
};

#endif
