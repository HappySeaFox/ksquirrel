/****************************************************************************
** Form interface generated from reading ui file './sq_externaltools.ui'
**
** Created: Срд Апр 7 18:27:54 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SQ_EXTERNALTOOLS_H
#define SQ_EXTERNALTOOLS_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QLabel;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QPushButton;

class SQ_ExternalTools : public QDialog
{
    Q_OBJECT

public:
    SQ_ExternalTools( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SQ_ExternalTools();

    QListBox* listNames;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QLineEdit* lineAction;
    QPushButton* pushTestAll;
    QButtonGroup* buttonGroup1;
    QLineEdit* lineNewName;
    QLabel* textLabel1_2;
    QLabel* textLabel2_2;
    QLineEdit* lineNewProgram;
    QPushButton* pushAddNew;
    QPushButton* pushButton3;

public slots:
    virtual void slotTestAll();

protected:
    QGridLayout* SQ_ExternalToolsLayout;
    QGridLayout* buttonGroup1Layout;

protected slots:
    virtual void languageChange();
private:
    void init();

};

#endif // SQ_EXTERNALTOOLS_H
