#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_options.ui'
**
** Created: Втр Апр 27 19:08:11 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_options.h"

#include <qvariant.h>
#include <kcolorbutton.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_libraryhandler.h"
#include "./sq_options.ui.h"

/* 
 *  Constructs a SQ_Options as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SQ_Options::SQ_Options( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "SQ_Options" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 200, 100 ) );
    setMaximumSize( QSize( 10000, 10000 ) );
    setSizeGripEnabled( TRUE );
    SQ_OptionsLayout = new QGridLayout( this, 1, 1, 11, 6, "SQ_OptionsLayout"); 

    listMain = new QListView( this, "listMain" );
    listMain->addColumn( tr2i18n( "Column 1" ) );
    listMain->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, listMain->sizePolicy().hasHeightForWidth() ) );
    listMain->setFrameShape( QListView::StyledPanel );
    listMain->setFrameShadow( QListView::Sunken );

    SQ_OptionsLayout->addWidget( listMain, 0, 0 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( FALSE );
    buttonOk->setDefault( TRUE );
    buttonOk->setFlat( FALSE );

    SQ_OptionsLayout->addWidget( buttonOk, 1, 2 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    buttonCancel->setFlat( FALSE );

    SQ_OptionsLayout->addWidget( buttonCancel, 1, 3 );
    QSpacerItem* spacer = new QSpacerItem( 395, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    SQ_OptionsLayout->addMultiCell( spacer, 1, 1, 0, 1 );

    widgetStack1 = new QWidgetStack( this, "widgetStack1" );
    widgetStack1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, widgetStack1->sizePolicy().hasHeightForWidth() ) );
    widgetStack1->setFrameShape( QWidgetStack::GroupBoxPanel );
    widgetStack1->setFrameShadow( QWidgetStack::Sunken );
    widgetStack1->setMargin( 0 );

    pageMain = new QWidget( widgetStack1, "pageMain" );
    pageMainLayout = new QGridLayout( pageMain, 1, 1, 11, 6, "pageMainLayout"); 

    checkOneInstance = new QCheckBox( pageMain, "checkOneInstance" );

    pageMainLayout->addWidget( checkOneInstance, 2, 0 );

    checkRestart = new QCheckBox( pageMain, "checkRestart" );
    checkRestart->setChecked( FALSE );

    pageMainLayout->addWidget( checkRestart, 0, 0 );

    checkMinimize = new QCheckBox( pageMain, "checkMinimize" );

    pageMainLayout->addWidget( checkMinimize, 1, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 295, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageMainLayout->addItem( spacer_2, 3, 0 );
    widgetStack1->addWidget( pageMain, 0 );

    pageInterface = new QWidget( widgetStack1, "pageInterface" );
    pageInterfaceLayout = new QGridLayout( pageInterface, 1, 1, 11, 6, "pageInterfaceLayout"); 

    buttonGroupViewType = new QButtonGroup( pageInterface, "buttonGroupViewType" );
    buttonGroupViewType->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupViewType->sizePolicy().hasHeightForWidth() ) );
    buttonGroupViewType->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupViewType->setColumnLayout(0, Qt::Vertical );
    buttonGroupViewType->layout()->setSpacing( 0 );
    buttonGroupViewType->layout()->setMargin( 8 );
    buttonGroupViewTypeLayout = new QGridLayout( buttonGroupViewType->layout() );
    buttonGroupViewTypeLayout->setAlignment( Qt::AlignTop );

    radioWinViewer = new QRadioButton( buttonGroupViewType, "radioWinViewer" );
    buttonGroupViewType->insert( radioWinViewer, 3 );

    buttonGroupViewTypeLayout->addWidget( radioWinViewer, 3, 0 );

    radioXnview = new QRadioButton( buttonGroupViewType, "radioXnview" );
    buttonGroupViewType->insert( radioXnview, 4 );

    buttonGroupViewTypeLayout->addWidget( radioXnview, 4, 0 );

    radioButton13 = new QRadioButton( buttonGroupViewType, "radioButton13" );
    radioButton13->setEnabled( FALSE );
    buttonGroupViewType->insert( radioButton13, 5 );

    buttonGroupViewTypeLayout->addWidget( radioButton13, 5, 0 );

    radioGQview = new QRadioButton( buttonGroupViewType, "radioGQview" );
    buttonGroupViewType->insert( radioGQview, 1 );

    buttonGroupViewTypeLayout->addWidget( radioGQview, 1, 0 );

    radioKuickShow = new QRadioButton( buttonGroupViewType, "radioKuickShow" );
    buttonGroupViewType->insert( radioKuickShow, 2 );

    buttonGroupViewTypeLayout->addWidget( radioKuickShow, 2, 0 );

    pixmapShowView = new QLabel( buttonGroupViewType, "pixmapShowView" );
    pixmapShowView->setScaledContents( FALSE );

    buttonGroupViewTypeLayout->addMultiCellWidget( pixmapShowView, 0, 5, 1, 1 );

    radioSQuirrel = new QRadioButton( buttonGroupViewType, "radioSQuirrel" );
    radioSQuirrel->setChecked( FALSE );
    buttonGroupViewType->insert( radioSQuirrel, 0 );

    buttonGroupViewTypeLayout->addWidget( radioSQuirrel, 0, 0 );

    pageInterfaceLayout->addMultiCellWidget( buttonGroupViewType, 0, 0, 0, 2 );

    buttonGroupCreateFirst = new QButtonGroup( pageInterface, "buttonGroupCreateFirst" );
    buttonGroupCreateFirst->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupCreateFirst->sizePolicy().hasHeightForWidth() ) );
    buttonGroupCreateFirst->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupCreateFirst->setColumnLayout(0, Qt::Vertical );
    buttonGroupCreateFirst->layout()->setSpacing( 0 );
    buttonGroupCreateFirst->layout()->setMargin( 8 );
    buttonGroupCreateFirstLayout = new QGridLayout( buttonGroupCreateFirst->layout() );
    buttonGroupCreateFirstLayout->setAlignment( Qt::AlignTop );

    radioButton16 = new QRadioButton( buttonGroupCreateFirst, "radioButton16" );
    buttonGroupCreateFirst->insert( radioButton16, 2 );

    buttonGroupCreateFirstLayout->addWidget( radioButton16, 2, 0 );

    radioButton14 = new QRadioButton( buttonGroupCreateFirst, "radioButton14" );
    buttonGroupCreateFirst->insert( radioButton14, 1 );

    buttonGroupCreateFirstLayout->addWidget( radioButton14, 1, 0 );

    radioButton15 = new QRadioButton( buttonGroupCreateFirst, "radioButton15" );
    buttonGroupCreateFirst->insert( radioButton15, 0 );

    buttonGroupCreateFirstLayout->addWidget( radioButton15, 0, 0 );

    pageInterfaceLayout->addMultiCellWidget( buttonGroupCreateFirst, 1, 1, 0, 2 );

    textLabel1_3 = new QLabel( pageInterface, "textLabel1_3" );
    textLabel1_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_3->sizePolicy().hasHeightForWidth() ) );

    pageInterfaceLayout->addWidget( textLabel1_3, 4, 0 );

    checkSavePos = new QCheckBox( pageInterface, "checkSavePos" );

    pageInterfaceLayout->addMultiCellWidget( checkSavePos, 2, 2, 0, 2 );

    checkSaveSize = new QCheckBox( pageInterface, "checkSaveSize" );

    pageInterfaceLayout->addMultiCellWidget( checkSaveSize, 3, 3, 0, 2 );

    comboToolbarIconSize = new QComboBox( FALSE, pageInterface, "comboToolbarIconSize" );
    comboToolbarIconSize->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, comboToolbarIconSize->sizePolicy().hasHeightForWidth() ) );

    pageInterfaceLayout->addWidget( comboToolbarIconSize, 4, 1 );
    QSpacerItem* spacer_3 = new QSpacerItem( 315, 16, QSizePolicy::Fixed, QSizePolicy::Minimum );
    pageInterfaceLayout->addItem( spacer_3, 4, 2 );
    QSpacerItem* spacer_4 = new QSpacerItem( 30, 91, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageInterfaceLayout->addItem( spacer_4, 5, 2 );
    widgetStack1->addWidget( pageInterface, 1 );

    pageFileview = new QWidget( widgetStack1, "pageFileview" );
    pageFileviewLayout = new QGridLayout( pageFileview, 1, 1, 11, 6, "pageFileviewLayout"); 

    buttonGroupSync = new QButtonGroup( pageFileview, "buttonGroupSync" );
    buttonGroupSync->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupSync->sizePolicy().hasHeightForWidth() ) );
    buttonGroupSync->setMargin( 0 );
    buttonGroupSync->setColumnLayout(0, Qt::Vertical );
    buttonGroupSync->layout()->setSpacing( 0 );
    buttonGroupSync->layout()->setMargin( 8 );
    buttonGroupSyncLayout = new QGridLayout( buttonGroupSync->layout() );
    buttonGroupSyncLayout->setAlignment( Qt::AlignTop );

    radioSyncBoth = new QRadioButton( buttonGroupSync, "radioSyncBoth" );

    buttonGroupSyncLayout->addWidget( radioSyncBoth, 0, 0 );

    radioSyncTreeWStack = new QRadioButton( buttonGroupSync, "radioSyncTreeWStack" );

    buttonGroupSyncLayout->addWidget( radioSyncTreeWStack, 1, 0 );

    radioSyncWStackTree = new QRadioButton( buttonGroupSync, "radioSyncWStackTree" );

    buttonGroupSyncLayout->addWidget( radioSyncWStackTree, 2, 0 );

    pageFileviewLayout->addMultiCellWidget( buttonGroupSync, 2, 2, 0, 3 );

    textLabel2 = new QLabel( pageFileview, "textLabel2" );
    textLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel2->sizePolicy().hasHeightForWidth() ) );

    pageFileviewLayout->addWidget( textLabel2, 3, 2 );

    checkRunUnknown = new QCheckBox( pageFileview, "checkRunUnknown" );

    pageFileviewLayout->addMultiCellWidget( checkRunUnknown, 4, 4, 0, 3 );

    buttonGroupSetPath = new QButtonGroup( pageFileview, "buttonGroupSetPath" );
    buttonGroupSetPath->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupSetPath->sizePolicy().hasHeightForWidth() ) );
    buttonGroupSetPath->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupSetPath->setColumnLayout(0, Qt::Vertical );
    buttonGroupSetPath->layout()->setSpacing( 0 );
    buttonGroupSetPath->layout()->setMargin( 8 );
    buttonGroupSetPathLayout = new QGridLayout( buttonGroupSetPath->layout() );
    buttonGroupSetPathLayout->setAlignment( Qt::AlignTop );

    radioSetThis = new QRadioButton( buttonGroupSetPath, "radioSetThis" );
    buttonGroupSetPath->insert( radioSetThis, 2 );

    buttonGroupSetPathLayout->addWidget( radioSetThis, 2, 0 );

    lineEditCustomDir = new QLineEdit( buttonGroupSetPath, "lineEditCustomDir" );
    lineEditCustomDir->setEnabled( FALSE );
    lineEditCustomDir->setLineWidth( 2 );
    lineEditCustomDir->setMargin( 0 );

    buttonGroupSetPathLayout->addWidget( lineEditCustomDir, 2, 1 );

    pushOpenDir = new QPushButton( buttonGroupSetPath, "pushOpenDir" );
    pushOpenDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pushOpenDir->sizePolicy().hasHeightForWidth() ) );
    pushOpenDir->setMinimumSize( QSize( 0, 0 ) );
    pushOpenDir->setFlat( FALSE );

    buttonGroupSetPathLayout->addWidget( pushOpenDir, 2, 2 );

    radioSetLast = new QRadioButton( buttonGroupSetPath, "radioSetLast" );
    buttonGroupSetPath->insert( radioSetLast, 0 );

    buttonGroupSetPathLayout->addMultiCellWidget( radioSetLast, 0, 0, 0, 1 );

    radioSetCurrent = new QRadioButton( buttonGroupSetPath, "radioSetCurrent" );
    buttonGroupSetPath->insert( radioSetCurrent, 1 );

    buttonGroupSetPathLayout->addMultiCellWidget( radioSetCurrent, 1, 1, 0, 1 );

    pageFileviewLayout->addMultiCellWidget( buttonGroupSetPath, 0, 0, 0, 3 );

    checkSaveHistory = new QCheckBox( pageFileview, "checkSaveHistory" );

    pageFileviewLayout->addMultiCellWidget( checkSaveHistory, 5, 5, 0, 3 );

    buttonGroup7 = new QButtonGroup( pageFileview, "buttonGroup7" );
    buttonGroup7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroup7->sizePolicy().hasHeightForWidth() ) );
    buttonGroup7->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroup7->setColumnLayout(0, Qt::Vertical );
    buttonGroup7->layout()->setSpacing( 0 );
    buttonGroup7->layout()->setMargin( 8 );
    buttonGroup7Layout = new QGridLayout( buttonGroup7->layout() );
    buttonGroup7Layout->setAlignment( Qt::AlignTop );
    QSpacerItem* spacer_5 = new QSpacerItem( 16, 35, QSizePolicy::Fixed, QSizePolicy::Minimum );
    buttonGroup7Layout->addItem( spacer_5, 1, 0 );

    buttonGroupClickPolicy = new QButtonGroup( buttonGroup7, "buttonGroupClickPolicy" );
    buttonGroupClickPolicy->setEnabled( TRUE );
    buttonGroupClickPolicy->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupClickPolicy->sizePolicy().hasHeightForWidth() ) );
    buttonGroupClickPolicy->setFrameShape( QButtonGroup::NoFrame );
    buttonGroupClickPolicy->setFrameShadow( QButtonGroup::Sunken );

    radioButton18 = new QRadioButton( buttonGroupClickPolicy, "radioButton18" );
    radioButton18->setGeometry( QRect( 0, 20, 325, 21 ) );
    buttonGroupClickPolicy->insert( radioButton18, 1 );

    radioButton17 = new QRadioButton( buttonGroupClickPolicy, "radioButton17" );
    radioButton17->setGeometry( QRect( 0, 0, 325, 21 ) );
    radioButton17->setChecked( FALSE );
    buttonGroupClickPolicy->insert( radioButton17, 0 );

    buttonGroup7Layout->addWidget( buttonGroupClickPolicy, 1, 1 );

    checkClickSystem = new QCheckBox( buttonGroup7, "checkClickSystem" );

    buttonGroup7Layout->addMultiCellWidget( checkClickSystem, 0, 0, 0, 1 );

    pageFileviewLayout->addMultiCellWidget( buttonGroup7, 1, 1, 0, 3 );

    comboListIndex = new QComboBox( FALSE, pageFileview, "comboListIndex" );
    comboListIndex->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, comboListIndex->sizePolicy().hasHeightForWidth() ) );

    pageFileviewLayout->addWidget( comboListIndex, 3, 3 );

    textLabel1_2 = new QLabel( pageFileview, "textLabel1_2" );
    textLabel1_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_2->sizePolicy().hasHeightForWidth() ) );

    pageFileviewLayout->addWidget( textLabel1_2, 3, 0 );
    QSpacerItem* spacer_6 = new QSpacerItem( 470, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageFileviewLayout->addMultiCell( spacer_6, 6, 6, 0, 3 );

    comboIconIndex = new QComboBox( FALSE, pageFileview, "comboIconIndex" );
    comboIconIndex->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, comboIconIndex->sizePolicy().hasHeightForWidth() ) );
    comboIconIndex->setMaxCount( 20 );

    pageFileviewLayout->addWidget( comboIconIndex, 3, 1 );
    widgetStack1->addWidget( pageFileview, 2 );

    pageGL = new QWidget( widgetStack1, "pageGL" );
    pageGLLayout = new QGridLayout( pageGL, 1, 1, 11, 6, "pageGLLayout"); 

    tabWidget2 = new QTabWidget( pageGL, "tabWidget2" );

    tab = new QWidget( tabWidget2, "tab" );
    tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

    buttonGroup9 = new QButtonGroup( tab, "buttonGroup9" );
    buttonGroup9->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroup9->sizePolicy().hasHeightForWidth() ) );
    buttonGroup9->setColumnLayout(0, Qt::Vertical );
    buttonGroup9->layout()->setSpacing( 6 );
    buttonGroup9->layout()->setMargin( 11 );
    buttonGroup9Layout = new QGridLayout( buttonGroup9->layout() );
    buttonGroup9Layout->setAlignment( Qt::AlignTop );

    textLabel1_5 = new QLabel( buttonGroup9, "textLabel1_5" );
    textLabel1_5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_5->sizePolicy().hasHeightForWidth() ) );

    buttonGroup9Layout->addWidget( textLabel1_5, 1, 0 );

    checkSystemColor = new QCheckBox( buttonGroup9, "checkSystemColor" );
    checkSystemColor->setChecked( FALSE );

    buttonGroup9Layout->addMultiCellWidget( checkSystemColor, 0, 0, 0, 2 );

    kColorGLbackground = new KColorButton( buttonGroup9, "kColorGLbackground" );
    kColorGLbackground->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, kColorGLbackground->sizePolicy().hasHeightForWidth() ) );
    kColorGLbackground->setMinimumSize( QSize( 61, 25 ) );
    kColorGLbackground->setFlat( FALSE );

    buttonGroup9Layout->addWidget( kColorGLbackground, 1, 1 );
    QSpacerItem* spacer_7 = new QSpacerItem( 310, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    buttonGroup9Layout->addItem( spacer_7, 1, 2 );

    tabLayout->addMultiCellWidget( buttonGroup9, 0, 0, 0, 2 );

    buttonGroupShadeModel = new QButtonGroup( tab, "buttonGroupShadeModel" );
    buttonGroupShadeModel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupShadeModel->sizePolicy().hasHeightForWidth() ) );
    buttonGroupShadeModel->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupShadeModel->setColumnLayout(0, Qt::Vertical );
    buttonGroupShadeModel->layout()->setSpacing( 6 );
    buttonGroupShadeModel->layout()->setMargin( 11 );
    buttonGroupShadeModelLayout = new QGridLayout( buttonGroupShadeModel->layout() );
    buttonGroupShadeModelLayout->setAlignment( Qt::AlignTop );

    radioSmooth = new QRadioButton( buttonGroupShadeModel, "radioSmooth" );
    buttonGroupShadeModel->insert( radioSmooth, 1 );

    buttonGroupShadeModelLayout->addWidget( radioSmooth, 1, 0 );

    radioFlat = new QRadioButton( buttonGroupShadeModel, "radioFlat" );
    buttonGroupShadeModel->insert( radioFlat, 0 );

    buttonGroupShadeModelLayout->addWidget( radioFlat, 0, 0 );

    tabLayout->addMultiCellWidget( buttonGroupShadeModel, 1, 1, 0, 2 );

    textLabel1_6 = new QLabel( tab, "textLabel1_6" );
    textLabel1_6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_6->sizePolicy().hasHeightForWidth() ) );

    tabLayout->addWidget( textLabel1_6, 5, 0 );

    checkBorder = new QCheckBox( tab, "checkBorder" );

    tabLayout->addMultiCellWidget( checkBorder, 4, 4, 0, 2 );

    checkBackgroundTransparent = new QCheckBox( tab, "checkBackgroundTransparent" );

    tabLayout->addMultiCellWidget( checkBackgroundTransparent, 3, 3, 0, 2 );

    checkDrop = new QCheckBox( tab, "checkDrop" );

    tabLayout->addMultiCellWidget( checkDrop, 2, 2, 0, 2 );

    spinAngle = new QSpinBox( tab, "spinAngle" );
    spinAngle->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, spinAngle->sizePolicy().hasHeightForWidth() ) );
    spinAngle->setMaxValue( 360 );
    spinAngle->setMinValue( 0 );
    spinAngle->setLineStep( 1 );
    spinAngle->setValue( 0 );

    tabLayout->addWidget( spinAngle, 5, 1 );

    spinMoveFactor = new QSpinBox( tab, "spinMoveFactor" );
    spinMoveFactor->setMinValue( 1 );

    tabLayout->addWidget( spinMoveFactor, 6, 1 );

    textLabel1_7 = new QLabel( tab, "textLabel1_7" );
    textLabel1_7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_7->sizePolicy().hasHeightForWidth() ) );

    tabLayout->addWidget( textLabel1_7, 6, 0 );
    QSpacerItem* spacer_8 = new QSpacerItem( 295, 58, QSizePolicy::Fixed, QSizePolicy::Minimum );
    tabLayout->addMultiCell( spacer_8, 5, 6, 2, 2 );
    QSpacerItem* spacer_9 = new QSpacerItem( 465, 46, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addMultiCell( spacer_9, 7, 7, 0, 2 );
    tabWidget2->insertTab( tab, "" );

    tab_2 = new QWidget( tabWidget2, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 

    textLabel1_4 = new QLabel( tab_2, "textLabel1_4" );
    textLabel1_4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_4->sizePolicy().hasHeightForWidth() ) );

    tabLayout_2->addWidget( textLabel1_4, 2, 0 );

    buttonGroupZoomType = new QButtonGroup( tab_2, "buttonGroupZoomType" );
    buttonGroupZoomType->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, buttonGroupZoomType->sizePolicy().hasHeightForWidth() ) );
    buttonGroupZoomType->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupZoomType->setColumnLayout(0, Qt::Vertical );
    buttonGroupZoomType->layout()->setSpacing( 1 );
    buttonGroupZoomType->layout()->setMargin( 11 );
    buttonGroupZoomTypeLayout = new QGridLayout( buttonGroupZoomType->layout() );
    buttonGroupZoomTypeLayout->setAlignment( Qt::AlignTop );

    radioButton21 = new QRadioButton( buttonGroupZoomType, "radioButton21" );

    buttonGroupZoomTypeLayout->addWidget( radioButton21, 2, 0 );

    radioZoomOriginal = new QRadioButton( buttonGroupZoomType, "radioZoomOriginal" );

    buttonGroupZoomTypeLayout->addWidget( radioZoomOriginal, 0, 0 );

    radioButton20 = new QRadioButton( buttonGroupZoomType, "radioButton20" );

    buttonGroupZoomTypeLayout->addWidget( radioButton20, 1, 0 );

    tabLayout_2->addMultiCellWidget( buttonGroupZoomType, 1, 1, 0, 2 );

    buttonGroupZoomModel = new QButtonGroup( tab_2, "buttonGroupZoomModel" );
    buttonGroupZoomModel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupZoomModel->sizePolicy().hasHeightForWidth() ) );
    buttonGroupZoomModel->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupZoomModel->setColumnLayout(0, Qt::Vertical );
    buttonGroupZoomModel->layout()->setSpacing( 1 );
    buttonGroupZoomModel->layout()->setMargin( 11 );
    buttonGroupZoomModelLayout = new QGridLayout( buttonGroupZoomModel->layout() );
    buttonGroupZoomModelLayout->setAlignment( Qt::AlignTop );

    radioLinear = new QRadioButton( buttonGroupZoomModel, "radioLinear" );
    buttonGroupZoomModel->insert( radioLinear, 0 );

    buttonGroupZoomModelLayout->addWidget( radioLinear, 0, 0 );

    radioNearest = new QRadioButton( buttonGroupZoomModel, "radioNearest" );
    radioNearest->setChecked( FALSE );
    buttonGroupZoomModel->insert( radioNearest, 1 );

    buttonGroupZoomModelLayout->addWidget( radioNearest, 1, 0 );

    tabLayout_2->addMultiCellWidget( buttonGroupZoomModel, 0, 0, 0, 2 );

    spinZoomFactor = new QSpinBox( tab_2, "spinZoomFactor" );
    spinZoomFactor->setMaxValue( 300 );
    spinZoomFactor->setValue( 25 );

    tabLayout_2->addWidget( spinZoomFactor, 2, 1 );
    QSpacerItem* spacer_10 = new QSpacerItem( 320, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    tabLayout_2->addItem( spacer_10, 2, 2 );
    QSpacerItem* spacer_11 = new QSpacerItem( 70, 199, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_2->addItem( spacer_11, 3, 2 );
    tabWidget2->insertTab( tab_2, "" );

    pageGLLayout->addWidget( tabWidget2, 0, 0 );
    widgetStack1->addWidget( pageGL, 3 );

    pageLibraries = new QWidget( widgetStack1, "pageLibraries" );
    pageLibrariesLayout = new QGridLayout( pageLibraries, 1, 1, 11, 6, "pageLibrariesLayout"); 

    textLabel1 = new QLabel( pageLibraries, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    pageLibrariesLayout->addWidget( textLabel1, 0, 0 );
    QSpacerItem* spacer_12 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Fixed );
    pageLibrariesLayout->addItem( spacer_12, 5, 2 );

    buttonGroup12 = new QButtonGroup( pageLibraries, "buttonGroup12" );
    buttonGroup12->setFrameShape( QButtonGroup::NoFrame );
    buttonGroup12->setColumnLayout(0, Qt::Vertical );
    buttonGroup12->layout()->setSpacing( 2 );
    buttonGroup12->layout()->setMargin( 0 );
    buttonGroup12Layout = new QGridLayout( buttonGroup12->layout() );
    buttonGroup12Layout->setAlignment( Qt::AlignTop );

    checkMonitor = new QCheckBox( buttonGroup12, "checkMonitor" );

    buttonGroup12Layout->addMultiCellWidget( checkMonitor, 0, 0, 0, 1 );
    QSpacerItem* spacer_13 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    buttonGroup12Layout->addItem( spacer_13, 1, 0 );

    checkFAMMessage = new QCheckBox( buttonGroup12, "checkFAMMessage" );
    checkFAMMessage->setEnabled( FALSE );

    buttonGroup12Layout->addWidget( checkFAMMessage, 1, 1 );

    pageLibrariesLayout->addMultiCellWidget( buttonGroup12, 1, 1, 0, 2 );

    linePrefix = new QLineEdit( pageLibraries, "linePrefix" );

    pageLibrariesLayout->addWidget( linePrefix, 0, 1 );

    pushNewPrefix = new QPushButton( pageLibraries, "pushNewPrefix" );

    pageLibrariesLayout->addWidget( pushNewPrefix, 0, 2 );
    QSpacerItem* spacer_14 = new QSpacerItem( 100, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    pageLibrariesLayout->addItem( spacer_14, 0, 3 );

    tableLib = new QListView( pageLibraries, "tableLib" );
    tableLib->addColumn( tr2i18n( "Library" ) );
    tableLib->addColumn( tr2i18n( "Info" ) );
    tableLib->addColumn( tr2i18n( "Version" ) );
    tableLib->addColumn( tr2i18n( "Extensions" ) );
    tableLib->setFrameShape( QListView::ToolBarPanel );
    tableLib->setSelectionMode( QListView::Extended );
    tableLib->setAllColumnsShowFocus( TRUE );
    tableLib->setResizeMode( QListView::AllColumns );

    pageLibrariesLayout->addMultiCellWidget( tableLib, 4, 4, 0, 3 );

    checkShowLinks = new QCheckBox( pageLibraries, "checkShowLinks" );
    checkShowLinks->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, checkShowLinks->sizePolicy().hasHeightForWidth() ) );
    checkShowLinks->setChecked( FALSE );

    pageLibrariesLayout->addMultiCellWidget( checkShowLinks, 3, 3, 0, 3 );

    checkContinue = new QCheckBox( pageLibraries, "checkContinue" );
    checkContinue->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, checkContinue->sizePolicy().hasHeightForWidth() ) );
    checkContinue->setAutoRepeat( FALSE );

    pageLibrariesLayout->addMultiCellWidget( checkContinue, 2, 2, 0, 3 );
    widgetStack1->addWidget( pageLibraries, 4 );

    pageCaching = new QWidget( widgetStack1, "pageCaching" );
    pageCachingLayout = new QGridLayout( pageCaching, 1, 1, 11, 6, "pageCachingLayout"); 
    QSpacerItem* spacer_15 = new QSpacerItem( 2, 55, QSizePolicy::Fixed, QSizePolicy::Minimum );
    pageCachingLayout->addItem( spacer_15, 1, 2 );

    checkCacheEnable = new QCheckBox( pageCaching, "checkCacheEnable" );
    checkCacheEnable->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, checkCacheEnable->sizePolicy().hasHeightForWidth() ) );
    checkCacheEnable->setChecked( TRUE );

    pageCachingLayout->addWidget( checkCacheEnable, 0, 0 );

    buttonGroup13 = new QButtonGroup( pageCaching, "buttonGroup13" );
    buttonGroup13->setFrameShape( QButtonGroup::NoFrame );
    buttonGroup13->setColumnLayout(0, Qt::Vertical );
    buttonGroup13->layout()->setSpacing( 6 );
    buttonGroup13->layout()->setMargin( 11 );
    buttonGroup13Layout = new QGridLayout( buttonGroup13->layout() );
    buttonGroup13Layout->setAlignment( Qt::AlignTop );

    textLabel1_8 = new QLabel( buttonGroup13, "textLabel1_8" );

    buttonGroup13Layout->addWidget( textLabel1_8, 0, 0 );

    pushOpenCacheDir = new QPushButton( buttonGroup13, "pushOpenCacheDir" );
    pushOpenCacheDir->setFlat( FALSE );

    buttonGroup13Layout->addWidget( pushOpenCacheDir, 0, 2 );

    lineCachePath = new QLineEdit( buttonGroup13, "lineCachePath" );
    lineCachePath->setEnabled( TRUE );

    buttonGroup13Layout->addWidget( lineCachePath, 0, 1 );

    buttonGroupCachePolicy = new QButtonGroup( buttonGroup13, "buttonGroupCachePolicy" );
    buttonGroupCachePolicy->setColumnLayout(0, Qt::Vertical );
    buttonGroupCachePolicy->layout()->setSpacing( 6 );
    buttonGroupCachePolicy->layout()->setMargin( 11 );
    buttonGroupCachePolicyLayout = new QGridLayout( buttonGroupCachePolicy->layout() );
    buttonGroupCachePolicyLayout->setAlignment( Qt::AlignTop );

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    spinCacheImageLarger = new QSpinBox( buttonGroupCachePolicy, "spinCacheImageLarger" );
    spinCacheImageLarger->setMaxValue( 65535 );
    spinCacheImageLarger->setMinValue( 1 );
    layout2->addWidget( spinCacheImageLarger );

    textLabel3 = new QLabel( buttonGroupCachePolicy, "textLabel3" );
    layout2->addWidget( textLabel3 );

    buttonGroupCachePolicyLayout->addLayout( layout2, 1, 1 );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    spinCacheImageX = new QSpinBox( buttonGroupCachePolicy, "spinCacheImageX" );
    spinCacheImageX->setMaxValue( 65535 );
    spinCacheImageX->setMinValue( 1 );
    layout1->addWidget( spinCacheImageX );

    textLabel2_2 = new QLabel( buttonGroupCachePolicy, "textLabel2_2" );
    textLabel2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel2_2->sizePolicy().hasHeightForWidth() ) );
    layout1->addWidget( textLabel2_2 );

    spinCacheImageY = new QSpinBox( buttonGroupCachePolicy, "spinCacheImageY" );
    spinCacheImageY->setMaxValue( 65535 );
    spinCacheImageY->setMinValue( 1 );
    layout1->addWidget( spinCacheImageY );

    buttonGroupCachePolicyLayout->addLayout( layout1, 0, 1 );

    radioCacheWhichLarger = new QRadioButton( buttonGroupCachePolicy, "radioCacheWhichLarger" );
    buttonGroupCachePolicy->insert( radioCacheWhichLarger, 0 );

    buttonGroupCachePolicyLayout->addWidget( radioCacheWhichLarger, 0, 0 );

    radioCacheWhichTakeMore = new QRadioButton( buttonGroupCachePolicy, "radioCacheWhichTakeMore" );
    buttonGroupCachePolicy->insert( radioCacheWhichTakeMore, 1 );

    buttonGroupCachePolicyLayout->addWidget( radioCacheWhichTakeMore, 1, 0 );

    buttonGroup13Layout->addMultiCellWidget( buttonGroupCachePolicy, 1, 1, 0, 1 );

    checkCacheGZIP = new QCheckBox( buttonGroup13, "checkCacheGZIP" );

    buttonGroup13Layout->addMultiCellWidget( checkCacheGZIP, 2, 2, 0, 2 );

    pageCachingLayout->addMultiCellWidget( buttonGroup13, 1, 1, 0, 1 );
    QSpacerItem* spacer_16 = new QSpacerItem( 40, 210, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageCachingLayout->addItem( spacer_16, 2, 1 );
    widgetStack1->addWidget( pageCaching, 5 );

    SQ_OptionsLayout->addMultiCellWidget( widgetStack1, 0, 0, 1, 3 );
    languageChange();
    resize( QSize(624, 482).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonGroupViewType, SIGNAL( clicked(int) ), this, SLOT( slotSetViewPixmap(int) ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( checkCacheEnable, SIGNAL( toggled(bool) ), buttonGroup13, SLOT( setEnabled(bool) ) );
    connect( checkClickSystem, SIGNAL( toggled(bool) ), buttonGroupClickPolicy, SLOT( setDisabled(bool) ) );
    connect( checkMonitor, SIGNAL( toggled(bool) ), checkFAMMessage, SLOT( setEnabled(bool) ) );
    connect( checkShowLinks, SIGNAL( toggled(bool) ), this, SLOT( slotShowLinks(bool) ) );
    connect( checkSystemColor, SIGNAL( toggled(bool) ), kColorGLbackground, SLOT( setDisabled(bool) ) );
    connect( pushNewPrefix, SIGNAL( clicked() ), this, SLOT( slotNewPrefix() ) );
    connect( pushOpenCacheDir, SIGNAL( released() ), this, SLOT( slotDirCache() ) );
    connect( pushOpenDir, SIGNAL( clicked() ), this, SLOT( slotOpenDir() ) );
    connect( radioSetThis, SIGNAL( toggled(bool) ), lineEditCustomDir, SLOT( setEnabled(bool) ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
SQ_Options::~SQ_Options()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SQ_Options::languageChange()
{
    setCaption( tr2i18n( "Options" ) );
    QToolTip::add( this, QString::null );
    listMain->header()->setLabel( 0, tr2i18n( "Column 1" ) );
    QToolTip::add( listMain, tr2i18n( "Main options" ) );
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    checkOneInstance->setText( tr2i18n( "Allow only one instanse (useful with tray support)" ) );
    checkRestart->setText( tr2i18n( "<NI> Restart SQuirrel after saving options" ) );
    checkMinimize->setText( tr2i18n( "Minimize to tray when close()" ) );
    buttonGroupViewType->setTitle( tr2i18n( "Look like ..." ) );
    radioWinViewer->setText( tr2i18n( "WinViewer" ) );
    QToolTip::add( radioWinViewer, tr2i18n( "Like Windows Picture and Fax viewer. Has only built-in GLView" ) );
    radioXnview->setText( tr2i18n( "XnView" ) );
    QToolTip::add( radioXnview, tr2i18n( "Like XnView. Has TreeView, FileView, built-in GLView" ) );
    radioButton13->setText( tr2i18n( "<NI>Konqueror" ) );
    QToolTip::add( radioButton13, tr2i18n( "Like Konqueror. GLView toglles with FileView" ) );
    radioGQview->setText( tr2i18n( "GQview" ) );
    QToolTip::add( radioGQview, tr2i18n( "Like GQview. Has FileView, built-in GLView" ) );
    radioKuickShow->setText( tr2i18n( "KuickShow" ) );
    QToolTip::add( radioKuickShow, tr2i18n( "Like Kuickshow. Has FileView, separate GLView" ) );
    radioSQuirrel->setText( tr2i18n( "SQuirrel" ) );
    QToolTip::add( radioSQuirrel, tr2i18n( "Like SQuirrel. Has TreeView, FileView, separate GLView" ) );
    buttonGroupCreateFirst->setTitle( tr2i18n( "Create first ..." ) );
    radioButton16->setText( tr2i18n( "Detailed View" ) );
    radioButton14->setText( tr2i18n( "Icon View" ) );
    radioButton15->setText( tr2i18n( "List View" ) );
    textLabel1_3->setText( tr2i18n( "Toolbar icon size:" ) );
    checkSavePos->setText( tr2i18n( "Save last position" ) );
    checkSaveSize->setText( tr2i18n( "Save last size" ) );
    comboToolbarIconSize->clear();
    comboToolbarIconSize->insertItem( tr2i18n( "16" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "22" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "32" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "48" ) );
    comboToolbarIconSize->setCurrentItem( 0 );
    buttonGroupSync->setTitle( tr2i18n( "Synchronization" ) );
    radioSyncBoth->setText( tr2i18n( "<NI>Sync both" ) );
    radioSyncTreeWStack->setText( tr2i18n( "Tree -> sqWStack" ) );
    radioSyncWStackTree->setText( tr2i18n( "<NI>sqWStack->Tree" ) );
    textLabel2->setText( tr2i18n( "Default icon size for \"List View\"" ) );
    checkRunUnknown->setText( tr2i18n( "Run unknown file formats separately (with default application)" ) );
    QToolTip::add( checkRunUnknown, tr2i18n( "For example, run unsupported \"png\" format in kuickshow" ) );
    buttonGroupSetPath->setTitle( tr2i18n( "Set path ..." ) );
    QToolTip::add( buttonGroupSetPath, QString::null );
    radioSetThis->setText( tr2i18n( "Set this" ) );
    lineEditCustomDir->setText( QString::null );
    pushOpenDir->setText( tr2i18n( "Open" ) );
    radioSetLast->setText( tr2i18n( "Set last" ) );
    radioSetCurrent->setText( tr2i18n( "Set current" ) );
    checkSaveHistory->setText( tr2i18n( "Save history" ) );
    buttonGroup7->setTitle( tr2i18n( "Clicking policy" ) );
    buttonGroupClickPolicy->setTitle( QString::null );
    radioButton18->setText( tr2i18n( "Use double click policy" ) );
    radioButton17->setText( tr2i18n( "Use single click policy" ) );
    checkClickSystem->setText( tr2i18n( "Use system settings" ) );
    QToolTip::add( checkClickSystem, tr2i18n( "if KDE uses single click to open item, SQuirrel will have too" ) );
    comboListIndex->clear();
    comboListIndex->insertItem( tr2i18n( "16" ) );
    comboListIndex->insertItem( tr2i18n( "22" ) );
    comboListIndex->insertItem( tr2i18n( "32" ) );
    comboListIndex->insertItem( tr2i18n( "48" ) );
    comboListIndex->insertItem( tr2i18n( "64" ) );
    comboListIndex->insertItem( tr2i18n( "96" ) );
    comboListIndex->insertItem( tr2i18n( "128" ) );
    comboListIndex->insertItem( tr2i18n( "192" ) );
    comboListIndex->insertItem( tr2i18n( "256" ) );
    comboListIndex->setCurrentItem( 0 );
    textLabel1_2->setText( tr2i18n( "Default icon size for \"Icon View\"" ) );
    comboIconIndex->clear();
    comboIconIndex->insertItem( tr2i18n( "16" ) );
    comboIconIndex->insertItem( tr2i18n( "22" ) );
    comboIconIndex->insertItem( tr2i18n( "32" ) );
    comboIconIndex->insertItem( tr2i18n( "48" ) );
    comboIconIndex->insertItem( tr2i18n( "64" ) );
    comboIconIndex->insertItem( tr2i18n( "96" ) );
    comboIconIndex->insertItem( tr2i18n( "128" ) );
    comboIconIndex->insertItem( tr2i18n( "192" ) );
    comboIconIndex->insertItem( tr2i18n( "256" ) );
    comboIconIndex->setCurrentItem( 0 );
    buttonGroup9->setTitle( tr2i18n( "Background color" ) );
    textLabel1_5->setText( tr2i18n( "custom:" ) );
    checkSystemColor->setText( tr2i18n( "use system color instead" ) );
    kColorGLbackground->setText( QString::null );
    QToolTip::add( kColorGLbackground, tr2i18n( "GLView will have such background color" ) );
    buttonGroupShadeModel->setTitle( tr2i18n( "Shade Model" ) );
    radioSmooth->setText( tr2i18n( "GL_SMOOTH" ) );
    radioFlat->setText( tr2i18n( "GL_FLAT" ) );
    textLabel1_6->setText( tr2i18n( "Rotation angle:" ) );
    checkBorder->setText( tr2i18n( "<NI>Draw a border" ) );
    checkBackgroundTransparent->setText( tr2i18n( "<NI>Background for transparent images (usually quads)" ) );
    checkDrop->setText( tr2i18n( "Enable drop" ) );
    spinAngle->setPrefix( QString::null );
    spinAngle->setSuffix( tr2i18n( " degrees" ) );
    spinAngle->setSpecialValueText( tr2i18n( "no rotation" ) );
    spinMoveFactor->setSuffix( tr2i18n( " pixels" ) );
    textLabel1_7->setText( tr2i18n( "Move factor:" ) );
    tabWidget2->changeTab( tab, tr2i18n( "General" ) );
    textLabel1_4->setText( tr2i18n( "Zoom koefficient:" ) );
    buttonGroupZoomType->setTitle( tr2i18n( "<NI>Zoom type" ) );
    radioButton21->setText( tr2i18n( "Leave previous zoom" ) );
    radioZoomOriginal->setText( tr2i18n( "Zoom to original" ) );
    radioButton20->setText( tr2i18n( "Fit to window" ) );
    buttonGroupZoomModel->setTitle( tr2i18n( "Zoom Model" ) );
    radioLinear->setText( tr2i18n( "GL_LINEAR" ) );
    radioNearest->setText( tr2i18n( "GL_NEAREST" ) );
    spinZoomFactor->setSuffix( tr2i18n( " %" ) );
    spinZoomFactor->setSpecialValueText( tr2i18n( "None" ) );
    tabWidget2->changeTab( tab_2, tr2i18n( "Zoom" ) );
    textLabel1->setText( tr2i18n( "Libraries here:" ) );
    QToolTip::add( textLabel1, tr2i18n( "Where SQuirrel should search libraries" ) );
    buttonGroup12->setTitle( QString::null );
    checkMonitor->setText( tr2i18n( "Monitor directory with libraries" ) );
    QToolTip::add( checkMonitor, tr2i18n( "Monitor directory with libraries, so new .so files can be dynamically added" ) );
    checkFAMMessage->setText( tr2i18n( "Show message" ) );
    QToolTip::add( checkFAMMessage, tr2i18n( "Show dialog with information about new or deleted libraries" ) );
    pushNewPrefix->setText( tr2i18n( "New" ) );
    tableLib->header()->setLabel( 0, tr2i18n( "Library" ) );
    tableLib->header()->setLabel( 1, tr2i18n( "Info" ) );
    tableLib->header()->setLabel( 2, tr2i18n( "Version" ) );
    tableLib->header()->setLabel( 3, tr2i18n( "Extensions" ) );
    QToolTip::add( tableLib, tr2i18n( "Contains information about all found libraries" ) );
    checkShowLinks->setText( tr2i18n( "show 'symlink' libraries" ) );
    QToolTip::add( checkShowLinks, tr2i18n( "If not checked, this table will contain only real .so files, not symlinks" ) );
    checkContinue->setText( tr2i18n( "Continue after resolving errors" ) );
    QToolTip::add( checkContinue, tr2i18n( "Resolving functions will continue after errors, if checked. Normally library with any error will NOT be handled, and format won't be supported" ) );
    checkCacheEnable->setText( tr2i18n( "<NI>Enable caching" ) );
    buttonGroup13->setTitle( QString::null );
    textLabel1_8->setText( tr2i18n( "Cache is placed here:" ) );
    pushOpenCacheDir->setText( tr2i18n( "Open" ) );
    buttonGroupCachePolicy->setTitle( tr2i18n( "Caching policy" ) );
    textLabel3->setText( tr2i18n( "Mb of memory" ) );
    textLabel2_2->setText( tr2i18n( "X" ) );
    radioCacheWhichLarger->setText( tr2i18n( "Cache images, which larger than" ) );
    radioCacheWhichTakeMore->setText( tr2i18n( "Cache images, which take more than " ) );
    checkCacheGZIP->setText( tr2i18n( "GZIP cache files" ) );
}

#include "sq_options.moc"
