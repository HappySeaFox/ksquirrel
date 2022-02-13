/****************************************************************************
** Form interface generated from reading ui file './sq_librarieschanged.ui'
**
** Created: Птн Апр 16 19:42:22 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SQ_LIBRARIESCHANGED_H
#define SQ_LIBRARIESCHANGED_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;

class SQ_LibrariesChanged : public QDialog
{
    Q_OBJECT

public:
    SQ_LibrariesChanged( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SQ_LibrariesChanged();

    QListBox* listNewLibs;
    QLabel* textLabel1;
    QPushButton* pushOK;

public slots:
    void setLibsInfo( const QStringList & l, bool added );

protected:
    QGridLayout* SQ_LibrariesChangedLayout;

protected slots:
    virtual void languageChange();
};

#endif // SQ_LIBRARIESCHANGED_H
