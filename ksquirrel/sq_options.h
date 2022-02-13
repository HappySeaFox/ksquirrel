/****************************************************************************
** Form interface generated from reading ui file './sq_options.ui'
**
** Created: Срд Янв 21 12:40:23 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SQ_OPTIONS_H
#define SQ_OPTIONS_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QCheckBox;
class QListBox;
class QListBoxItem;
class QPushButton;
class QRadioButton;
class QWidget;
class QWidgetStack;

class SQ_Options : public QDialog
{
    Q_OBJECT

public:
    SQ_Options( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SQ_Options();

    QListBox* listBox;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QWidgetStack* widgetStack1;
    QWidget* page;
    QWidget* page_2;
    QCheckBox* checkBox3;
    QButtonGroup* buttonGroup1;
    QRadioButton* radioButton1;
    QRadioButton* radioButton2;
    QRadioButton* radioButton3;
    QCheckBox* checkBox4;
    QCheckBox* checkBox1;

public slots:
    virtual void slotCreatePageToggle( bool is );
    virtual void slotSelectPage( int page );

protected:
    QGridLayout* SQ_OptionsLayout;
    QHBoxLayout* Layout1;

protected slots:
    virtual void languageChange();
};

#endif // SQ_OPTIONS_H
