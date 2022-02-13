/****************************************************************************
** Form interface generated from reading ui file './sq_options.ui'
**
** Created: Птн Апр 16 19:42:22 2004
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
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QWidgetStack* widgetStack1;
    QWidget* page;
    QCheckBox* checkOneInstance;
    QCheckBox* checkRestart;
    QCheckBox* checkMinimize;
    QWidget* page_2;
    QButtonGroup* buttonGroupViewType;
    QRadioButton* radioWinViewer;
    QRadioButton* radioXnview;
    QRadioButton* radioButton13;
    QRadioButton* radioGQview;
    QRadioButton* radioKuickShow;
    QLabel* pixmapShowView;
    QRadioButton* radioSQuirrel;
    QButtonGroup* buttonGroupCreateFirst;
    QRadioButton* radioButton16;
    QRadioButton* radioButton14;
    QRadioButton* radioButton15;
    QLabel* textLabel1_3;
    QComboBox* comboToolbarIconSize;
    QWidget* page_3;
    QButtonGroup* buttonGroupSync;
    QRadioButton* radioSyncBoth;
    QRadioButton* radioSyncTreeWStack;
    QRadioButton* radioSyncWStackTree;
    QLabel* textLabel2;
    QCheckBox* checkRunUnknown;
    QButtonGroup* buttonGroupSetPath;
    QRadioButton* radioSetThis;
    QLineEdit* lineEditCustomDir;
    QPushButton* pushOpenDir;
    QRadioButton* radioSetLast;
    QRadioButton* radioSetCurrent;
    QCheckBox* checkSaveHistory;
    QButtonGroup* buttonGroup7;
    QButtonGroup* buttonGroupClickPolicy;
    QRadioButton* radioButton18;
    QRadioButton* radioButton17;
    QCheckBox* checkClickSystem;
    QComboBox* comboListIndex;
    QLabel* textLabel1_2;
    QComboBox* comboIconIndex;
    QWidget* page_4;
    QLabel* textLabel1;
    QButtonGroup* buttonGroup12;
    QCheckBox* checkMonitor;
    QCheckBox* checkFAMMessage;
    QLineEdit* linePrefix;
    QPushButton* pushNewPrefix;
    QListView* tableLib;
    QCheckBox* checkShowLinks;
    QCheckBox* checkContinue;
    QWidget* page_5;
    QButtonGroup* buttonGroup11;
    QCheckBox* checkCacheGL;
    QCheckBox* checkCacheDisk;
    QPushButton* pushOpenCacheDir;
    QLineEdit* lineCachePath;
    QWidget* page_6;
    QTabWidget* tabWidget2;
    QWidget* tab;
    QButtonGroup* buttonGroup9;
    QLabel* textLabel1_5;
    QCheckBox* checkSystemColor;
    KColorButton* kColorGLbackground;
    QButtonGroup* buttonGroupShadeModel;
    QRadioButton* radioSmooth;
    QRadioButton* radioFlat;
    QLabel* textLabel1_6;
    QCheckBox* checkBorder;
    QCheckBox* checkBackgroundTransparent;
    QCheckBox* checkDrop;
    QCheckBox* checkStepByStep;
    QSpinBox* spinAngle;
    QWidget* tab_2;
    QLabel* textLabel1_4;
    QButtonGroup* buttonGroupZoomType;
    QRadioButton* radioButton21;
    QRadioButton* radioZoomOriginal;
    QRadioButton* radioButton20;
    QButtonGroup* buttonGroupZoomModel;
    QRadioButton* radioLinear;
    QRadioButton* radioNearest;
    QSpinBox* spinZoomFactor;

    virtual int start();

public slots:
    virtual void slotSelectPage( int page );
    virtual void slotSetViewPixmap( int id );
    virtual void slotClicked( QListViewItem * item );
    virtual void slotOpenDir();
    virtual void slotDirCache();
    virtual void slotShowLinks( bool showl );
    virtual void slotNewPrefix();

protected:
    QListViewItem *itemMain,*itemInterface,*itemFileView,*itemLibraries, *itemCache, *itemGLview;
    typedef struct { QListViewItem *item; int id;} ListViewItemID;
    bool showlinks;

    QGridLayout* SQ_OptionsLayout;
    QGridLayout* pageLayout;
    QGridLayout* pageLayout_2;
    QGridLayout* buttonGroupViewTypeLayout;
    QGridLayout* buttonGroupCreateFirstLayout;
    QGridLayout* pageLayout_3;
    QGridLayout* buttonGroupSyncLayout;
    QGridLayout* buttonGroupSetPathLayout;
    QGridLayout* buttonGroup7Layout;
    QGridLayout* pageLayout_4;
    QGridLayout* buttonGroup12Layout;
    QGridLayout* pageLayout_5;
    QGridLayout* buttonGroup11Layout;
    QGridLayout* pageLayout_6;
    QGridLayout* tabLayout;
    QGridLayout* buttonGroup9Layout;
    QGridLayout* buttonGroupShadeModelLayout;
    QGridLayout* tabLayout_2;
    QGridLayout* buttonGroupZoomTypeLayout;
    QGridLayout* buttonGroupZoomModelLayout;

protected slots:
    virtual void languageChange();
private:
    QPixmap a[6];

    void init();

};

#endif // SQ_OPTIONS_H
