/****************************************************************************
** Form interface generated from reading ui file './sq_options.ui'
**
** Created: Fri Mar 19 17:39:48 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SQ_OPTIONS_H
#define SQ_OPTIONS_H

#include <qvariant.h>
#include <qdialog.h>
#include <qpixmap.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class KColorButton;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTabWidget;
class QWidget;
class QWidgetStack;

class SQ_Options : public QDialog
{
    Q_OBJECT

public:
    SQ_Options( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SQ_Options();

    QListView* listMain;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QWidgetStack* widgetStack1;
    QWidget* page;
    QCheckBox* checkSplash;
    QCheckBox* checkRestart;
    QCheckBox* checkMinimize;
    QWidget* page_2;
    QButtonGroup* buttonGroupViewType;
    QLabel* pixmapShowView;
    QRadioButton* radioSQuirrel;
    QRadioButton* radioGQview;
    QRadioButton* radioKuickShow;
    QRadioButton* radioWinViewer;
    QRadioButton* radioXnview;
    QLabel* textLabel1_3;
    QComboBox* comboToolbarIconSize;
    QWidget* page_3;
    QLabel* textLabel1_2;
    QLabel* textLabel2;
    QButtonGroup* buttonGroupSetPath;
    QLineEdit* lineEditCustomDir;
    QPushButton* pushOpenDir;
    QRadioButton* radioSetLast;
    QRadioButton* radioSetCurrent;
    QRadioButton* radioSetThis;
    QCheckBox* checkSyncTree;
    QComboBox* comboIconIndex;
    QComboBox* comboListIndex;
    QWidget* page_4;
    QLabel* textPrefix;
    QLabel* textLabel1;
    QCheckBox* checkShowLinks;
    QListView* tableLib;
    QWidget* page_5;
    QCheckBox* checkCacheDisk;
    QCheckBox* checkCacheGL;
    QLineEdit* lineCachePath;
    QPushButton* pushOpenCacheDir;
    QWidget* page_6;
    QTabWidget* tabWidget2;
    QWidget* tab;
    QCheckBox* checkAdjust;
    KColorButton* kColorGLbackground;
    QLabel* textLabel1_5;
    QButtonGroup* buttonGroupShadeModel;
    QRadioButton* radioSmooth_2;
    QRadioButton* radioFlat_2;
    QWidget* tab_2;
    QButtonGroup* buttonGroupZoomModel;
    QRadioButton* radioLinear_2_2;
    QRadioButton* radioNearest_2_2;
    QCheckBox* checkZoomLock;
    QLabel* textLabel1_4;
    QSpinBox* spinZoomFactor;

    virtual void start();

public slots:
    virtual void slotSelectPage( int page );
    virtual void slotSetViewPixmap( int id );
    virtual void slotClicked( QListViewItem * item );
    virtual void slotOpenDir();
    virtual void slotDirCache();
    virtual void slotShowLinks( bool show );

protected:
    bool showlinks;
    typedef struct { QListViewItem *item; int id;} ListViewItemID;
    QListViewItem *itemMain,*itemInterface,*itemFileView,*itemLibraries, *itemCache, *itemGLview;

    QGridLayout* SQ_OptionsLayout;
    QHBoxLayout* layout1;
    QGridLayout* pageLayout;

protected slots:
    virtual void languageChange();
private:
    QPixmap a[5];

    void init();

};

#endif // SQ_OPTIONS_H
