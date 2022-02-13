/****************************************************************************
** Form interface generated from reading ui file './sq_filters.ui'
**
** Created: Срд Май 5 19:46:54 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SQ_FILTERS_H
#define SQ_FILTERS_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QFrame;
class QListView;
class QListViewItem;
class QPushButton;

class SQ_Filters : public QDialog
{
    Q_OBJECT

public:
    SQ_Filters( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SQ_Filters();

    QPushButton* pushOK;
    QPushButton* pushCancel;
    QFrame* frame3;
    QPushButton* pushNewFilter;
    QPushButton* pushFilterClear;
    QPushButton* pushFilterClearAll;
    QListView* listFilters;
    QPushButton* pushFilterDown;
    QPushButton* pushFilterUp;
    QCheckBox* checkBoth;

public slots:
    virtual void slotNewFilter();
    virtual void slotFilterClear();
    virtual void slotFilterUp();
    virtual void slotFilterDown();
    virtual int start();
    virtual void slotFilterRenameRequest( QListViewItem * item, const QPoint & point, int pos );

protected:
    QGridLayout* SQ_FiltersLayout;
    QGridLayout* frame3Layout;

protected slots:
    virtual void languageChange();
private:
    void init();

};

#endif // SQ_FILTERS_H
