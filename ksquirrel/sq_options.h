/****************************************************************************
** Form interface generated from reading ui file './sq_options.ui'
**
** Created: Срд Май 19 22:34:49 2004
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
class KHistoryCombo;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QListView;
class QListViewItem;
class QPushButton;
class QRadioButton;
class QSlider;
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

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QWidgetStack* widgetStack1;
    QWidget* pageMain;
    QCheckBox* checkRestart;
    QCheckBox* checkMinimize;
    QCheckBox* checkOneInstance;
    QCheckBox* checkSync;
    QWidget* pageInterface;
    QButtonGroup* buttonGroupCreateFirst;
    QRadioButton* radioButton16;
    QRadioButton* radioButton14;
    QRadioButton* radioButton15;
    QLabel* textLabel1_3;
    QComboBox* comboToolbarIconSize;
    QButtonGroup* buttonGroup14;
    QCheckBox* checkSavePos;
    QCheckBox* checkSaveSize;
    QButtonGroup* buttonGroupViewType;
    QRadioButton* radioSQuirrel;
    QRadioButton* radioGQview;
    QRadioButton* radioKuickShow;
    QRadioButton* radioButton29;
    QRadioButton* radioWinViewer;
    QRadioButton* radioXnview;
    QRadioButton* radioButton13;
    QWidgetStack* widgetStackView;
    QWidget* page;
    QLabel* pixmapView1;
    QWidget* page_2;
    QLabel* pixmapView2;
    QWidget* page_3;
    QLabel* pixmapView3;
    QWidget* page_4;
    QLabel* pixmapView4;
    QWidget* page_5;
    QLabel* pixmapView5;
    QWidget* page_6;
    QLabel* pixmapView6;
    QWidget* page_7;
    QLabel* pixmapView7;
    QWidget* pageFileview;
    QButtonGroup* buttonGroupSetPath;
    QRadioButton* radioSetThis;
    QRadioButton* radioSetLast;
    QRadioButton* radioSetCurrent;
    QLineEdit* lineEditCustomDir;
    QPushButton* pushOpenDir;
    QButtonGroup* buttonGroup7;
    QButtonGroup* buttonGroupClickPolicy;
    QRadioButton* radioButton18;
    QRadioButton* radioButton17;
    QCheckBox* checkClickSystem;
    QCheckBox* checkRunUnknown;
    QButtonGroup* buttonGroupSync;
    QRadioButton* radioSyncBoth;
    QRadioButton* radioSyncTreeWStack;
    QRadioButton* radioSyncWStackTree;
    QCheckBox* checkSaveHistory;
    QWidget* pageGL;
    QTabWidget* tabWidget2;
    QWidget* tab;
    QButtonGroup* buttonGroup15;
    QCheckBox* checkGLSaveSize;
    QCheckBox* checkGLSavePos;
    QLabel* textLabel1_5;
    QSpinBox* spinSwapFactor;
    QLabel* textLabel2;
    QCheckBox* checkDrop;
    QWidget* tab_2;
    QButtonGroup* buttonGroup9;
    QLabel* textLabel1_5_2;
    KColorButton* kColorGLbackground;
    QCheckBox* checkSystemColor;
    QLabel* textMove;
    QLabel* textLabel2_3_2;
    QLabel* textLabel1_6_2;
    QSlider* sliderMove;
    QSlider* sliderAngle;
    QLabel* textLabel1_7_2;
    QLabel* textAngle;
    QLabel* textLabel1_9_2;
    QCheckBox* checkBorder;
    QCheckBox* checkBackgroundTransparent;
    QButtonGroup* buttonGroupShadeModel;
    QRadioButton* radioSmooth;
    QRadioButton* radioFlat;
    QButtonGroup* buttonGroupCenter;
    QRadioButton* radioCenterImage;
    QRadioButton* radioCenterAxes;
    QWidget* tab_3;
    QButtonGroup* buttonGroupZoomModel;
    QRadioButton* radioLinear;
    QRadioButton* radioNearest;
    QLabel* textLabel1_4;
    QSpinBox* spinZoomFactor;
    QButtonGroup* buttonGroupZoomType;
    QRadioButton* radioZoomOriginal;
    QRadioButton* radioButton27;
    QRadioButton* radioButton21;
    QRadioButton* radioButton20;
    QRadioButton* radioButton28;
    QWidget* tab_4;
    QButtonGroup* buttonGroupScrolling;
    QRadioButton* radioSCRZoom;
    QRadioButton* radioSRCNext;
    QWidget* pageLibraries;
    QLabel* textLabel1;
    QButtonGroup* buttonGroup12;
    QCheckBox* checkMonitor;
    QCheckBox* checkFAMMessage;
    QPushButton* pushNewPrefix;
    QCheckBox* checkShowLinks;
    QCheckBox* checkContinue;
    QWidgetStack* WST;
    QWidget* page_8;
    QListView* tableLib;
    QWidget* page_9;
    QLabel* pixmapNotFound;
    KHistoryCombo* comboPrefix;
    QWidget* pageCaching;
    QCheckBox* checkCacheEnable;
    QButtonGroup* buttonGroup13;
    QLabel* textLabel1_8;
    QPushButton* pushOpenCacheDir;
    QLineEdit* lineCachePath;
    QButtonGroup* buttonGroupCachePolicy;
    QSpinBox* spinCacheImageLarger;
    QLabel* textLabel3;
    QSpinBox* spinCacheImageX;
    QLabel* textLabel2_2;
    QSpinBox* spinCacheImageY;
    QRadioButton* radioCacheWhichLarger;
    QRadioButton* radioCacheWhichTakeMore;
    QCheckBox* checkCacheGZIP;
    QCheckBox* checkCacheMax;
    QSpinBox* spinCacheMax;
    QLabel* textLabel1_2;
    QListBox* listMain;

    virtual int start();

public slots:
    virtual void slotOpenDir();
    virtual void slotDirCache();
    virtual void slotShowLinks( bool showl );
    virtual void slotNewPrefix();

protected:
    QString libPrefix;
    typedef struct { QListViewItem *item; int id;} ListViewItemID;
    bool showlinks;

    QGridLayout* SQ_OptionsLayout;
    QGridLayout* pageMainLayout;
    QGridLayout* pageInterfaceLayout;
    QGridLayout* buttonGroupCreateFirstLayout;
    QGridLayout* buttonGroup14Layout;
    QGridLayout* buttonGroupViewTypeLayout;
    QGridLayout* pageLayout;
    QGridLayout* pageLayout_2;
    QGridLayout* pageLayout_3;
    QGridLayout* pageLayout_4;
    QGridLayout* pageLayout_5;
    QGridLayout* pageLayout_6;
    QGridLayout* pageLayout_7;
    QGridLayout* pageFileviewLayout;
    QGridLayout* buttonGroupSetPathLayout;
    QHBoxLayout* layout4;
    QGridLayout* buttonGroup7Layout;
    QGridLayout* buttonGroupClickPolicyLayout;
    QGridLayout* buttonGroupSyncLayout;
    QGridLayout* pageGLLayout;
    QGridLayout* tabLayout;
    QGridLayout* buttonGroup15Layout;
    QHBoxLayout* layout5;
    QGridLayout* tabLayout_2;
    QGridLayout* buttonGroup9Layout;
    QGridLayout* layout4_2;
    QGridLayout* buttonGroupShadeModelLayout;
    QGridLayout* buttonGroupCenterLayout;
    QGridLayout* tabLayout_3;
    QGridLayout* buttonGroupZoomModelLayout;
    QGridLayout* buttonGroupZoomTypeLayout;
    QGridLayout* tabLayout_4;
    QGridLayout* buttonGroupScrollingLayout;
    QGridLayout* pageLibrariesLayout;
    QGridLayout* buttonGroup12Layout;
    QGridLayout* pageLayout_8;
    QGridLayout* pageLayout_9;
    QGridLayout* pageCachingLayout;
    QGridLayout* buttonGroup13Layout;
    QGridLayout* buttonGroupCachePolicyLayout;
    QHBoxLayout* layout2;
    QHBoxLayout* layout1;

protected slots:
    virtual void languageChange();
private:
    void init();

};

#endif // SQ_OPTIONS_H
