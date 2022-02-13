/****************************************************************************
** Form interface generated from reading ui file './sq_convertwidget.ui'
**
** Created: Fri Mar 19 17:26:56 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SQ_CONVERTWIDGET_H
#define SQ_CONVERTWIDGET_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QCheckBox;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QPushButton;

class SQ_ConvertWidget : public QDialog
{
    Q_OBJECT

public:
    SQ_ConvertWidget( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SQ_ConvertWidget();

    QListBox* listFiles;
    QListBox* listFormats;
    QButtonGroup* buttonGroup4;
    QPushButton* pushOpenDir;
    QLineEdit* linePutHere;
    QCheckBox* checkReplace;
    QCheckBox* checkAskReplace;
    QCheckBox* checkPut;
    QPushButton* pushOK;
    QPushButton* pushCancel;

public slots:
    virtual void slotOpenDir();

protected:
    QGridLayout* SQ_ConvertWidgetLayout;
    QHBoxLayout* layout5;

protected slots:
    virtual void languageChange();
};

#endif // SQ_CONVERTWIDGET_H