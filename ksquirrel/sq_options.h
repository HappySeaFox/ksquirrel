/****************************************************************************
** Form interface generated from reading ui file './sq_options.ui'
**
** Created: Sat Mar 27 17:13:21 2004
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
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
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
    QRadioButton* radioButton15;
    QRadioButton* radioButton14;
    QRadioButton* radioButton16;
    QWidget* page_3;
    QButtonGroup* buttonGroupSetPath;
    QRadioButton* radioSetLast;
    QRadioButton* radioSetCurrent;
    QRadioButton* radioSetThis;
    QLineEdit* lineEditCustomDir;
    QPushButton* pushOpenDir;
    QCheckBox* checkSyncTree;
    QLabel* textLabel2;
    QCheckBox* checkRunUnknown;
    QLabel* textLabel1_2;
    QComboBox* comboIconIndex;
    QComboBox* comboListIndex;
    QCheckBox* checkClickSystem;
    QButtonGroup* buttonGroupClickPolicy;
    QRadioButton* radioButton18;
    QRadioButton* radioButton17;
    QWidget* page_4;
    QLabel* textPrefix;
    QLabel* textLabel1;
    QCheckBox* checkFAMDetectNew;
    QCheckBox* checkFAMMessage;
    QCheckBox* checkShowLinks;
    QListView* tableLib;
    QFrame* line1;
    QWidget* page_5;
    QCheckBox* checkCacheDisk;
    QCheckBox* checkCacheGL;
    QLineEdit* lineCachePath;
    QPushButton* pushOpenCacheDir;
    QWidget* page_6;
    QTabWidget* tabWidget2;
    QWidget* tab;
    QButtonGroup* buttonGroupShadeModel;
    QRadioButton* radioSmooth_2;
    QRadioButton* radioFlat_2;
    QCheckBox* checkDrop;
    QLabel* textLabel1_5;
    QCheckBox* checkAdjust;
    KColorButton* kColorGLbackground;
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
    QListViewItem *itemMain,*itemInterface,*itemFileView,*itemLibraries, *itemCache, *itemGLview;
    typedef struct { QListViewItem *item; int id;} ListViewItemID;
    bool showlinks;

    QGridLayout* SQ_OptionsLayout;
    QHBoxLayout* layout1;
    QGridLayout* pageLayout;
    QGridLayout* buttonGroupSetPathLayout;
    QGridLayout* pageLayout_2;
    QGridLayout* pageLayout_3;

protected slots:
    virtual void languageChange();
private:
    QPixmap a[6];

    void init();

};

#endif // SQ_OPTIONS_H
