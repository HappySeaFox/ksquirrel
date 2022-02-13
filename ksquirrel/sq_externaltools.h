/****************************************************************************
** Form interface generated from reading ui file './sq_externaltools.ui'
**
** Created: Втр Апр 27 19:00:36 2004
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
class QFrame;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;

class SQ_ExternalTools : public QDialog
{
    Q_OBJECT

public:
    SQ_ExternalTools( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SQ_ExternalTools();

    QFrame* frame3;
    QPushButton* pushNewTool;
    QPushButton* pushToolClear;
    QPushButton* pushToolClearAll;
    QLabel* textLabel1;
    QPushButton* pushToolUp;
    QPushButton* pushToolDown;
    QListView* listTools;
    QPushButton* pushCancel;
    QPushButton* pushOK;

public slots:
    virtual void slotNewTool();
    virtual void slotToolClear();
    virtual void slotToolClearAll();
    virtual void slotToolUp();
    virtual void slotToolDown();
    virtual int start();
    virtual void slotToolRenameRequest( QListViewItem * item, const QPoint & point, int pos );

protected:
    QGridLayout* SQ_ExternalToolsLayout;
    QGridLayout* frame3Layout;

protected slots:
    virtual void languageChange();
private:
    void init();

};

#endif // SQ_EXTERNALTOOLS_H
