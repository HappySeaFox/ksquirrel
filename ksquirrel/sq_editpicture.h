/****************************************************************************
** Form interface generated from reading ui file './sq_editpicture.ui'
**
** Created: Птн Янв 23 15:26:03 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef EDITPICTURE_H
#define EDITPICTURE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTabWidget;
class QWidget;

class EditPicture : public QDialog
{
    Q_OBJECT

public:
    EditPicture( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~EditPicture();

    QTabWidget* tabWidget;
    QWidget* Widget9;
    QWidget* Widget8;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

protected:
    QVBoxLayout* EditPictureLayout;
    QHBoxLayout* Layout1;

protected slots:
    virtual void languageChange();
};

#endif // EDITPICTURE_H
