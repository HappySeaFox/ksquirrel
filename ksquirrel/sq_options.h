/****************************************************************************
** Form interface generated from reading ui file './sq_options.ui'
**
** Created: Tue Mar 30 01:30:27 2004
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
class QFrame;
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
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonHelp;
    QWidgetStack* widgetStack1;
    QWidget* page;
    QCheckBox* checkSplash;
    QCheckBox* checkRestart;
    QCheckBox* checkMinimize;
    QWidget* page_2;
    QLabel* textLabel1_3;
    QComboBox* comboToolbarIconSize;
    QButtonGroup* buttonGroupViewType;
    QRadioButton* radioSQuirrel;
    QRadioButton* radioWinViewer;
    QRadioButton* radioXnview;
    QRadioButton* radioButton13;
    QRadioButton* radioGQview;
    QRadioButton* radioKuickShow;
    QLabel* pixmapShowView;
    QButtonGroup* buttonGroupCreateFirst;
    QRadioButton* radioButton16;
    QRadioButton* radioButton14;
    QRadioButton* radioButton15;
    QWidget* page_3;
    QLabel* textLabel2;
    QLabel* textLabel1_2;
    QComboBox* comboIconIndex;
    QComboBox* comboListIndex;
    QCheckBox* checkRunUnknown;
    QCheckBox* checkSyncTree;
    QButtonGroup* buttonGroup7;
    QButtonGroup* buttonGroupClickPolicy;
    QRadioButton* radioButton18;
    QRadioButton* radioButton17;
    QCheckBox* checkClickSystem;
    QButtonGroup* buttonGroupSetPath;
    QRadioButton* radioSetThis;
    QRadioButton* radioSetLast;
    QRadioButton* radioSetCurrent;
    QLineEdit* lineEditCustomDir;
    QPushButton* pushOpenDir;
    QWidget* page_4;
    QLabel* textPrefix;
    QLabel* textLabel1;
    QCheckBox* checkMonitor;
    QCheckBox* checkFAMMessage;
    QListView* tableLib;
    QCheckBox* checkShowLinks;
    QFrame* line1;
    QWidget* page_5;
    QCheckBox* checkCacheDisk;
    QCheckBox* checkCacheGL;
    QLineEdit* lineCachePath;
    QPushButton* pushOpenCacheDir;
    QWidget* page_6;
    QTabWidget* tabWidget2;
    QWidget* tab;
    QLabel* textLabel1_5;
    QButtonGroup* buttonGroupShadeModel;
    QRadioButton* radioSmooth_2;
    QRadioButton* radioFlat_2;
    QCheckBox* checkStepByStep;
    QCheckBox* checkDrop;
    KColorButton* kColorGLbackground;
    QWidget* tab_2;
    QButtonGroup* buttonGroupZoomModel;
    QRadioButton* radioLinear_2_2;
    QRadioButton* radioNearest_2_2;
    QLabel* textLabel1_4;
    QSpinBox* spinZoomFactor;
    QButtonGroup* buttonGroupZoomType;
    QRadioButton* radioZoomOriginal;
    QRadioButton* radioButton20;
    QRadioButton* radioButton21;

    virtual void start();

public slots:
    virtual void slotSelectPage( int page );
    virtual void slotSetViewPixmap( int id );
    virtual void slotClicked( QListViewItem * item );
    virtual void slotOpenDir();
    virtual void slotDirCache();
    virtual void slotShowLinks( bool show );

protected:
    QListViewItem *itemMain,*itemInterface,*itemFileView,*itemLibraries, *itemCache, *itemGLview;
    typedef struct { QListViewItem *item; int id;} ListViewItemID;
    bool showlinks;


protected slots:
    virtual void languageChange();
private:
    QPixmap a[6];

    void init();

};

#endif // SQ_OPTIONS_H
