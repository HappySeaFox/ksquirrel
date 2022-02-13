/****************************************************************************
** Form interface generated from reading ui file './sq_options.ui'
**
** Created: Сбт Май 15 16:56:11 2004
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
    QCheckBox* checkOneInstance;
    QCheckBox* checkRestart;
    QCheckBox* checkMinimize;
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
    QRadioButton* radioKuickShow;
    QRadioButton* radioSQuirrel;
    QRadioButton* radioGQview;
    QRadioButton* radioButton13;
    QRadioButton* radioWinViewer;
    QRadioButton* radioXnview;
    QRadioButton* radioButton29;
    QLabel* pixmapShowView;
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
    QButtonGroup* buttonGroupShadeModel;
    QRadioButton* radioSmooth;
    QRadioButton* radioFlat;
    QButtonGroup* buttonGroup9;
    QLabel* textLabel1_5_2;
    KColorButton* kColorGLbackground;
    QCheckBox* checkSystemColor;
    QCheckBox* checkBackgroundTransparent;
    QCheckBox* checkBorder;
    QLabel* textMove;
    QLabel* textLabel2_3_2;
    QLabel* textLabel1_6_2;
    QSlider* sliderMove;
    QSlider* sliderAngle;
    QLabel* textLabel1_7_2;
    QLabel* textAngle;
    QLabel* textLabel1_9_2;
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
    QWidget* pageLibraries;
    QLabel* textLabel1;
    QButtonGroup* buttonGroup12;
    QCheckBox* checkMonitor;
    QCheckBox* checkFAMMessage;
    QLineEdit* linePrefix;
    QPushButton* pushNewPrefix;
    QListView* tableLib;
    QCheckBox* checkShowLinks;
    QCheckBox* checkContinue;
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
    virtual void slotSelectPage( int page );
    virtual void slotSetViewPixmap( int id );
    virtual void slotOpenDir();
    virtual void slotDirCache();
    virtual void slotShowLinks( bool showl );
    virtual void slotNewPrefix();

protected:
    bool showlinks;
    typedef struct { QListViewItem *item; int id;} ListViewItemID;
    QString libPrefix;

    QGridLayout* SQ_OptionsLayout;
    QGridLayout* pageMainLayout;
    QGridLayout* pageInterfaceLayout;
    QGridLayout* buttonGroupCreateFirstLayout;
    QGridLayout* buttonGroup14Layout;
    QGridLayout* buttonGroupViewTypeLayout;
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
    QGridLayout* buttonGroupShadeModelLayout;
    QGridLayout* buttonGroup9Layout;
    QGridLayout* layout4_2;
    QGridLayout* tabLayout_3;
    QGridLayout* buttonGroupZoomModelLayout;
    QGridLayout* buttonGroupZoomTypeLayout;
    QGridLayout* pageLibrariesLayout;
    QGridLayout* buttonGroup12Layout;
    QGridLayout* pageCachingLayout;
    QGridLayout* buttonGroup13Layout;
    QGridLayout* buttonGroupCachePolicyLayout;
    QHBoxLayout* layout2;
    QHBoxLayout* layout1;

protected slots:
    virtual void languageChange();
private:
    QPixmap a[6];

    void init();

};

#endif // SQ_OPTIONS_H
