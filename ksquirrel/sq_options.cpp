#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_options.ui'
**
** Created: Срд Май 19 22:35:45 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_options.h"

#include <qvariant.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kurlcompletion.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
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
#include "sq_librarylistener.h"
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

    checkRestart = new QCheckBox( pageMain, "checkRestart" );
    checkRestart->setChecked( FALSE );

    pageMainLayout->addWidget( checkRestart, 0, 0 );

    checkMinimize = new QCheckBox( pageMain, "checkMinimize" );

    pageMainLayout->addWidget( checkMinimize, 1, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 250, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageMainLayout->addItem( spacer_2, 4, 0 );

    checkOneInstance = new QCheckBox( pageMain, "checkOneInstance" );

    pageMainLayout->addWidget( checkOneInstance, 2, 0 );

    checkSync = new QCheckBox( pageMain, "checkSync" );

    pageMainLayout->addWidget( checkSync, 3, 0 );
    widgetStack1->addWidget( pageMain, 0 );

    pageInterface = new QWidget( widgetStack1, "pageInterface" );
    pageInterfaceLayout = new QGridLayout( pageInterface, 1, 1, 11, 6, "pageInterfaceLayout"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 30, 15, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageInterfaceLayout->addItem( spacer_3, 4, 2 );

    buttonGroupCreateFirst = new QButtonGroup( pageInterface, "buttonGroupCreateFirst" );
    buttonGroupCreateFirst->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupCreateFirst->sizePolicy().hasHeightForWidth() ) );
    buttonGroupCreateFirst->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupCreateFirst->setColumnLayout(0, Qt::Vertical );
    buttonGroupCreateFirst->layout()->setSpacing( 1 );
    buttonGroupCreateFirst->layout()->setMargin( 9 );
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

    pageInterfaceLayout->addWidget( textLabel1_3, 3, 0 );
    QSpacerItem* spacer_4 = new QSpacerItem( 315, 16, QSizePolicy::Fixed, QSizePolicy::Minimum );
    pageInterfaceLayout->addItem( spacer_4, 3, 2 );

    comboToolbarIconSize = new QComboBox( FALSE, pageInterface, "comboToolbarIconSize" );
    comboToolbarIconSize->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, comboToolbarIconSize->sizePolicy().hasHeightForWidth() ) );

    pageInterfaceLayout->addWidget( comboToolbarIconSize, 3, 1 );

    buttonGroup14 = new QButtonGroup( pageInterface, "buttonGroup14" );
    buttonGroup14->setColumnLayout(0, Qt::Vertical );
    buttonGroup14->layout()->setSpacing( 2 );
    buttonGroup14->layout()->setMargin( 9 );
    buttonGroup14Layout = new QGridLayout( buttonGroup14->layout() );
    buttonGroup14Layout->setAlignment( Qt::AlignTop );

    checkSavePos = new QCheckBox( buttonGroup14, "checkSavePos" );

    buttonGroup14Layout->addWidget( checkSavePos, 0, 0 );

    checkSaveSize = new QCheckBox( buttonGroup14, "checkSaveSize" );

    buttonGroup14Layout->addWidget( checkSaveSize, 1, 0 );

    pageInterfaceLayout->addMultiCellWidget( buttonGroup14, 2, 2, 0, 2 );

    buttonGroupViewType = new QButtonGroup( pageInterface, "buttonGroupViewType" );
    buttonGroupViewType->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupViewType->sizePolicy().hasHeightForWidth() ) );
    buttonGroupViewType->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupViewType->setColumnLayout(0, Qt::Vertical );
    buttonGroupViewType->layout()->setSpacing( 1 );
    buttonGroupViewType->layout()->setMargin( 9 );
    buttonGroupViewTypeLayout = new QGridLayout( buttonGroupViewType->layout() );
    buttonGroupViewTypeLayout->setAlignment( Qt::AlignTop );

    radioSQuirrel = new QRadioButton( buttonGroupViewType, "radioSQuirrel" );
    radioSQuirrel->setChecked( FALSE );
    buttonGroupViewType->insert( radioSQuirrel, 0 );

    buttonGroupViewTypeLayout->addMultiCellWidget( radioSQuirrel, 0, 1, 0, 1 );

    radioGQview = new QRadioButton( buttonGroupViewType, "radioGQview" );
    buttonGroupViewType->insert( radioGQview, 1 );

    buttonGroupViewTypeLayout->addWidget( radioGQview, 2, 0 );

    radioKuickShow = new QRadioButton( buttonGroupViewType, "radioKuickShow" );
    buttonGroupViewType->insert( radioKuickShow, 2 );

    buttonGroupViewTypeLayout->addWidget( radioKuickShow, 3, 0 );

    radioButton29 = new QRadioButton( buttonGroupViewType, "radioButton29" );
    buttonGroupViewType->insert( radioButton29, 5 );

    buttonGroupViewTypeLayout->addWidget( radioButton29, 6, 0 );

    radioWinViewer = new QRadioButton( buttonGroupViewType, "radioWinViewer" );
    buttonGroupViewType->insert( radioWinViewer, 3 );

    buttonGroupViewTypeLayout->addWidget( radioWinViewer, 4, 0 );

    radioXnview = new QRadioButton( buttonGroupViewType, "radioXnview" );
    buttonGroupViewType->insert( radioXnview, 4 );

    buttonGroupViewTypeLayout->addWidget( radioXnview, 5, 0 );

    radioButton13 = new QRadioButton( buttonGroupViewType, "radioButton13" );
    radioButton13->setEnabled( TRUE );
    buttonGroupViewType->insert( radioButton13, 6 );

    buttonGroupViewTypeLayout->addWidget( radioButton13, 7, 0 );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 25, QSizePolicy::Minimum, QSizePolicy::Expanding );
    buttonGroupViewTypeLayout->addItem( spacer_5, 8, 0 );

    widgetStackView = new QWidgetStack( buttonGroupViewType, "widgetStackView" );
    widgetStackView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, widgetStackView->sizePolicy().hasHeightForWidth() ) );

    page = new QWidget( widgetStackView, "page" );
    pageLayout = new QGridLayout( page, 1, 1, 0, 0, "pageLayout"); 

    pixmapView1 = new QLabel( page, "pixmapView1" );
    pixmapView1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pixmapView1->sizePolicy().hasHeightForWidth() ) );
    pixmapView1->setScaledContents( TRUE );

    pageLayout->addWidget( pixmapView1, 0, 0 );
    widgetStackView->addWidget( page, 0 );

    page_2 = new QWidget( widgetStackView, "page_2" );
    pageLayout_2 = new QGridLayout( page_2, 1, 1, 0, 0, "pageLayout_2"); 

    pixmapView2 = new QLabel( page_2, "pixmapView2" );
    pixmapView2->setScaledContents( TRUE );

    pageLayout_2->addWidget( pixmapView2, 0, 0 );
    widgetStackView->addWidget( page_2, 1 );

    page_3 = new QWidget( widgetStackView, "page_3" );
    pageLayout_3 = new QGridLayout( page_3, 1, 1, 0, 0, "pageLayout_3"); 

    pixmapView3 = new QLabel( page_3, "pixmapView3" );
    pixmapView3->setScaledContents( TRUE );

    pageLayout_3->addWidget( pixmapView3, 0, 0 );
    widgetStackView->addWidget( page_3, 2 );

    page_4 = new QWidget( widgetStackView, "page_4" );
    pageLayout_4 = new QGridLayout( page_4, 1, 1, 0, 0, "pageLayout_4"); 

    pixmapView4 = new QLabel( page_4, "pixmapView4" );
    pixmapView4->setScaledContents( TRUE );

    pageLayout_4->addWidget( pixmapView4, 0, 0 );
    widgetStackView->addWidget( page_4, 3 );

    page_5 = new QWidget( widgetStackView, "page_5" );
    pageLayout_5 = new QGridLayout( page_5, 1, 1, 0, 0, "pageLayout_5"); 

    pixmapView5 = new QLabel( page_5, "pixmapView5" );
    pixmapView5->setScaledContents( TRUE );

    pageLayout_5->addWidget( pixmapView5, 0, 0 );
    widgetStackView->addWidget( page_5, 4 );

    page_6 = new QWidget( widgetStackView, "page_6" );
    pageLayout_6 = new QGridLayout( page_6, 1, 1, 0, 0, "pageLayout_6"); 

    pixmapView6 = new QLabel( page_6, "pixmapView6" );
    pixmapView6->setScaledContents( TRUE );

    pageLayout_6->addWidget( pixmapView6, 0, 0 );
    widgetStackView->addWidget( page_6, 5 );

    page_7 = new QWidget( widgetStackView, "page_7" );
    pageLayout_7 = new QGridLayout( page_7, 1, 1, 0, 0, "pageLayout_7"); 

    pixmapView7 = new QLabel( page_7, "pixmapView7" );
    pixmapView7->setScaledContents( TRUE );

    pageLayout_7->addWidget( pixmapView7, 0, 0 );
    widgetStackView->addWidget( page_7, 6 );

    buttonGroupViewTypeLayout->addMultiCellWidget( widgetStackView, 1, 8, 1, 1 );

    pageInterfaceLayout->addMultiCellWidget( buttonGroupViewType, 0, 0, 0, 2 );
    widgetStack1->addWidget( pageInterface, 1 );

    pageFileview = new QWidget( widgetStack1, "pageFileview" );
    pageFileviewLayout = new QGridLayout( pageFileview, 1, 1, 11, 6, "pageFileviewLayout"); 

    buttonGroupSetPath = new QButtonGroup( pageFileview, "buttonGroupSetPath" );
    buttonGroupSetPath->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupSetPath->sizePolicy().hasHeightForWidth() ) );
    buttonGroupSetPath->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupSetPath->setColumnLayout(0, Qt::Vertical );
    buttonGroupSetPath->layout()->setSpacing( 1 );
    buttonGroupSetPath->layout()->setMargin( 9 );
    buttonGroupSetPathLayout = new QGridLayout( buttonGroupSetPath->layout() );
    buttonGroupSetPathLayout->setAlignment( Qt::AlignTop );

    radioSetThis = new QRadioButton( buttonGroupSetPath, "radioSetThis" );
    buttonGroupSetPath->insert( radioSetThis, 2 );

    buttonGroupSetPathLayout->addWidget( radioSetThis, 2, 0 );

    radioSetLast = new QRadioButton( buttonGroupSetPath, "radioSetLast" );
    buttonGroupSetPath->insert( radioSetLast, 0 );

    buttonGroupSetPathLayout->addWidget( radioSetLast, 0, 0 );

    radioSetCurrent = new QRadioButton( buttonGroupSetPath, "radioSetCurrent" );
    buttonGroupSetPath->insert( radioSetCurrent, 1 );

    buttonGroupSetPathLayout->addWidget( radioSetCurrent, 1, 0 );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    lineEditCustomDir = new QLineEdit( buttonGroupSetPath, "lineEditCustomDir" );
    lineEditCustomDir->setEnabled( FALSE );
    lineEditCustomDir->setLineWidth( 2 );
    lineEditCustomDir->setMargin( 0 );
    layout4->addWidget( lineEditCustomDir );

    pushOpenDir = new QPushButton( buttonGroupSetPath, "pushOpenDir" );
    pushOpenDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pushOpenDir->sizePolicy().hasHeightForWidth() ) );
    pushOpenDir->setMinimumSize( QSize( 0, 0 ) );
    pushOpenDir->setFlat( FALSE );
    layout4->addWidget( pushOpenDir );

    buttonGroupSetPathLayout->addLayout( layout4, 3, 0 );

    pageFileviewLayout->addWidget( buttonGroupSetPath, 0, 0 );

    buttonGroup7 = new QButtonGroup( pageFileview, "buttonGroup7" );
    buttonGroup7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroup7->sizePolicy().hasHeightForWidth() ) );
    buttonGroup7->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroup7->setColumnLayout(0, Qt::Vertical );
    buttonGroup7->layout()->setSpacing( 1 );
    buttonGroup7->layout()->setMargin( 9 );
    buttonGroup7Layout = new QGridLayout( buttonGroup7->layout() );
    buttonGroup7Layout->setAlignment( Qt::AlignTop );
    QSpacerItem* spacer_6 = new QSpacerItem( 16, 35, QSizePolicy::Fixed, QSizePolicy::Minimum );
    buttonGroup7Layout->addItem( spacer_6, 1, 0 );

    buttonGroupClickPolicy = new QButtonGroup( buttonGroup7, "buttonGroupClickPolicy" );
    buttonGroupClickPolicy->setEnabled( TRUE );
    buttonGroupClickPolicy->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupClickPolicy->sizePolicy().hasHeightForWidth() ) );
    buttonGroupClickPolicy->setFrameShape( QButtonGroup::NoFrame );
    buttonGroupClickPolicy->setFrameShadow( QButtonGroup::Sunken );
    buttonGroupClickPolicy->setColumnLayout(0, Qt::Vertical );
    buttonGroupClickPolicy->layout()->setSpacing( 0 );
    buttonGroupClickPolicy->layout()->setMargin( 0 );
    buttonGroupClickPolicyLayout = new QGridLayout( buttonGroupClickPolicy->layout() );
    buttonGroupClickPolicyLayout->setAlignment( Qt::AlignTop );

    radioButton18 = new QRadioButton( buttonGroupClickPolicy, "radioButton18" );
    buttonGroupClickPolicy->insert( radioButton18, 1 );

    buttonGroupClickPolicyLayout->addWidget( radioButton18, 1, 0 );

    radioButton17 = new QRadioButton( buttonGroupClickPolicy, "radioButton17" );
    radioButton17->setChecked( FALSE );
    buttonGroupClickPolicy->insert( radioButton17, 0 );

    buttonGroupClickPolicyLayout->addWidget( radioButton17, 0, 0 );

    buttonGroup7Layout->addWidget( buttonGroupClickPolicy, 1, 1 );

    checkClickSystem = new QCheckBox( buttonGroup7, "checkClickSystem" );

    buttonGroup7Layout->addMultiCellWidget( checkClickSystem, 0, 0, 0, 1 );

    pageFileviewLayout->addWidget( buttonGroup7, 1, 0 );

    checkRunUnknown = new QCheckBox( pageFileview, "checkRunUnknown" );

    pageFileviewLayout->addWidget( checkRunUnknown, 3, 0 );

    buttonGroupSync = new QButtonGroup( pageFileview, "buttonGroupSync" );
    buttonGroupSync->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupSync->sizePolicy().hasHeightForWidth() ) );
    buttonGroupSync->setMargin( 0 );
    buttonGroupSync->setColumnLayout(0, Qt::Vertical );
    buttonGroupSync->layout()->setSpacing( 1 );
    buttonGroupSync->layout()->setMargin( 9 );
    buttonGroupSyncLayout = new QGridLayout( buttonGroupSync->layout() );
    buttonGroupSyncLayout->setAlignment( Qt::AlignTop );

    radioSyncBoth = new QRadioButton( buttonGroupSync, "radioSyncBoth" );

    buttonGroupSyncLayout->addWidget( radioSyncBoth, 0, 0 );

    radioSyncTreeWStack = new QRadioButton( buttonGroupSync, "radioSyncTreeWStack" );

    buttonGroupSyncLayout->addWidget( radioSyncTreeWStack, 1, 0 );

    radioSyncWStackTree = new QRadioButton( buttonGroupSync, "radioSyncWStackTree" );

    buttonGroupSyncLayout->addWidget( radioSyncWStackTree, 2, 0 );

    pageFileviewLayout->addWidget( buttonGroupSync, 2, 0 );

    checkSaveHistory = new QCheckBox( pageFileview, "checkSaveHistory" );

    pageFileviewLayout->addWidget( checkSaveHistory, 4, 0 );
    QSpacerItem* spacer_7 = new QSpacerItem( 95, 50, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageFileviewLayout->addItem( spacer_7, 5, 0 );
    widgetStack1->addWidget( pageFileview, 2 );

    pageGL = new QWidget( widgetStack1, "pageGL" );
    pageGLLayout = new QGridLayout( pageGL, 1, 1, 11, 6, "pageGLLayout"); 

    tabWidget2 = new QTabWidget( pageGL, "tabWidget2" );

    tab = new QWidget( tabWidget2, "tab" );
    tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

    buttonGroup15 = new QButtonGroup( tab, "buttonGroup15" );
    buttonGroup15->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroup15->sizePolicy().hasHeightForWidth() ) );
    buttonGroup15->setColumnLayout(0, Qt::Vertical );
    buttonGroup15->layout()->setSpacing( 2 );
    buttonGroup15->layout()->setMargin( 9 );
    buttonGroup15Layout = new QGridLayout( buttonGroup15->layout() );
    buttonGroup15Layout->setAlignment( Qt::AlignTop );

    checkGLSaveSize = new QCheckBox( buttonGroup15, "checkGLSaveSize" );

    buttonGroup15Layout->addWidget( checkGLSaveSize, 1, 0 );

    checkGLSavePos = new QCheckBox( buttonGroup15, "checkGLSavePos" );

    buttonGroup15Layout->addWidget( checkGLSavePos, 0, 0 );

    tabLayout->addWidget( buttonGroup15, 0, 0 );
    QSpacerItem* spacer_8 = new QSpacerItem( 20, 75, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addItem( spacer_8, 3, 0 );

    layout5 = new QHBoxLayout( 0, 0, 4, "layout5"); 

    textLabel1_5 = new QLabel( tab, "textLabel1_5" );
    textLabel1_5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_5->sizePolicy().hasHeightForWidth() ) );
    layout5->addWidget( textLabel1_5 );

    spinSwapFactor = new QSpinBox( tab, "spinSwapFactor" );
    spinSwapFactor->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, spinSwapFactor->sizePolicy().hasHeightForWidth() ) );
    spinSwapFactor->setBackgroundOrigin( QSpinBox::ParentOrigin );
    spinSwapFactor->setMaxValue( 100 );
    spinSwapFactor->setMinValue( 1 );
    spinSwapFactor->setValue( 1 );
    layout5->addWidget( spinSwapFactor );

    textLabel2 = new QLabel( tab, "textLabel2" );
    layout5->addWidget( textLabel2 );

    tabLayout->addLayout( layout5, 1, 0 );

    checkDrop = new QCheckBox( tab, "checkDrop" );

    tabLayout->addWidget( checkDrop, 2, 0 );
    tabWidget2->insertTab( tab, "" );

    tab_2 = new QWidget( tabWidget2, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 

    buttonGroup9 = new QButtonGroup( tab_2, "buttonGroup9" );
    buttonGroup9->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroup9->sizePolicy().hasHeightForWidth() ) );
    buttonGroup9->setColumnLayout(0, Qt::Vertical );
    buttonGroup9->layout()->setSpacing( 1 );
    buttonGroup9->layout()->setMargin( 9 );
    buttonGroup9Layout = new QGridLayout( buttonGroup9->layout() );
    buttonGroup9Layout->setAlignment( Qt::AlignTop );

    textLabel1_5_2 = new QLabel( buttonGroup9, "textLabel1_5_2" );
    textLabel1_5_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_5_2->sizePolicy().hasHeightForWidth() ) );

    buttonGroup9Layout->addWidget( textLabel1_5_2, 1, 0 );
    QSpacerItem* spacer_9 = new QSpacerItem( 310, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    buttonGroup9Layout->addItem( spacer_9, 1, 2 );

    kColorGLbackground = new KColorButton( buttonGroup9, "kColorGLbackground" );
    kColorGLbackground->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, kColorGLbackground->sizePolicy().hasHeightForWidth() ) );
    kColorGLbackground->setMinimumSize( QSize( 61, 25 ) );
    kColorGLbackground->setFlat( FALSE );

    buttonGroup9Layout->addWidget( kColorGLbackground, 1, 1 );

    checkSystemColor = new QCheckBox( buttonGroup9, "checkSystemColor" );
    checkSystemColor->setChecked( FALSE );

    buttonGroup9Layout->addMultiCellWidget( checkSystemColor, 0, 0, 0, 2 );

    tabLayout_2->addWidget( buttonGroup9, 0, 0 );
    QSpacerItem* spacer_10 = new QSpacerItem( 20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_2->addItem( spacer_10, 6, 0 );

    layout4_2 = new QGridLayout( 0, 1, 1, 0, 6, "layout4_2"); 

    textMove = new QLabel( tab_2, "textMove" );
    textMove->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textMove->sizePolicy().hasHeightForWidth() ) );
    textMove->setTextFormat( QLabel::AutoText );
    textMove->setIndent( 3 );

    layout4_2->addWidget( textMove, 1, 2 );

    textLabel2_3_2 = new QLabel( tab_2, "textLabel2_3_2" );
    textLabel2_3_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel2_3_2->sizePolicy().hasHeightForWidth() ) );

    layout4_2->addWidget( textLabel2_3_2, 1, 3 );

    textLabel1_6_2 = new QLabel( tab_2, "textLabel1_6_2" );
    textLabel1_6_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_6_2->sizePolicy().hasHeightForWidth() ) );

    layout4_2->addWidget( textLabel1_6_2, 0, 0 );

    sliderMove = new QSlider( tab_2, "sliderMove" );
    sliderMove->setFocusPolicy( QSlider::WheelFocus );
    sliderMove->setMinValue( 1 );
    sliderMove->setMaxValue( 100 );
    sliderMove->setPageStep( 1 );
    sliderMove->setTracking( TRUE );
    sliderMove->setOrientation( QSlider::Horizontal );
    sliderMove->setTickmarks( QSlider::NoMarks );
    sliderMove->setTickInterval( 1 );

    layout4_2->addWidget( sliderMove, 1, 1 );

    sliderAngle = new QSlider( tab_2, "sliderAngle" );
    sliderAngle->setFocusPolicy( QSlider::WheelFocus );
    sliderAngle->setMaxValue( 180 );
    sliderAngle->setPageStep( 5 );
    sliderAngle->setTracking( TRUE );
    sliderAngle->setOrientation( QSlider::Horizontal );
    sliderAngle->setTickmarks( QSlider::NoMarks );
    sliderAngle->setTickInterval( 1 );

    layout4_2->addWidget( sliderAngle, 0, 1 );

    textLabel1_7_2 = new QLabel( tab_2, "textLabel1_7_2" );
    textLabel1_7_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_7_2->sizePolicy().hasHeightForWidth() ) );

    layout4_2->addWidget( textLabel1_7_2, 1, 0 );

    textAngle = new QLabel( tab_2, "textAngle" );
    textAngle->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textAngle->sizePolicy().hasHeightForWidth() ) );
    textAngle->setIndent( 3 );

    layout4_2->addWidget( textAngle, 0, 2 );

    textLabel1_9_2 = new QLabel( tab_2, "textLabel1_9_2" );
    textLabel1_9_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_9_2->sizePolicy().hasHeightForWidth() ) );

    layout4_2->addWidget( textLabel1_9_2, 0, 3 );

    tabLayout_2->addLayout( layout4_2, 5, 0 );

    checkBorder = new QCheckBox( tab_2, "checkBorder" );

    tabLayout_2->addWidget( checkBorder, 4, 0 );

    checkBackgroundTransparent = new QCheckBox( tab_2, "checkBackgroundTransparent" );

    tabLayout_2->addWidget( checkBackgroundTransparent, 3, 0 );

    buttonGroupShadeModel = new QButtonGroup( tab_2, "buttonGroupShadeModel" );
    buttonGroupShadeModel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupShadeModel->sizePolicy().hasHeightForWidth() ) );
    buttonGroupShadeModel->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupShadeModel->setColumnLayout(0, Qt::Vertical );
    buttonGroupShadeModel->layout()->setSpacing( 1 );
    buttonGroupShadeModel->layout()->setMargin( 9 );
    buttonGroupShadeModelLayout = new QGridLayout( buttonGroupShadeModel->layout() );
    buttonGroupShadeModelLayout->setAlignment( Qt::AlignTop );

    radioSmooth = new QRadioButton( buttonGroupShadeModel, "radioSmooth" );
    buttonGroupShadeModel->insert( radioSmooth, 1 );

    buttonGroupShadeModelLayout->addWidget( radioSmooth, 1, 0 );

    radioFlat = new QRadioButton( buttonGroupShadeModel, "radioFlat" );
    buttonGroupShadeModel->insert( radioFlat, 0 );

    buttonGroupShadeModelLayout->addWidget( radioFlat, 0, 0 );

    tabLayout_2->addWidget( buttonGroupShadeModel, 1, 0 );

    buttonGroupCenter = new QButtonGroup( tab_2, "buttonGroupCenter" );
    buttonGroupCenter->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupCenter->sizePolicy().hasHeightForWidth() ) );
    buttonGroupCenter->setColumnLayout(0, Qt::Vertical );
    buttonGroupCenter->layout()->setSpacing( 1 );
    buttonGroupCenter->layout()->setMargin( 9 );
    buttonGroupCenterLayout = new QGridLayout( buttonGroupCenter->layout() );
    buttonGroupCenterLayout->setAlignment( Qt::AlignTop );

    radioCenterImage = new QRadioButton( buttonGroupCenter, "radioCenterImage" );

    buttonGroupCenterLayout->addWidget( radioCenterImage, 0, 0 );

    radioCenterAxes = new QRadioButton( buttonGroupCenter, "radioCenterAxes" );

    buttonGroupCenterLayout->addWidget( radioCenterAxes, 1, 0 );

    tabLayout_2->addWidget( buttonGroupCenter, 2, 0 );
    tabWidget2->insertTab( tab_2, "" );

    tab_3 = new QWidget( tabWidget2, "tab_3" );
    tabLayout_3 = new QGridLayout( tab_3, 1, 1, 11, 6, "tabLayout_3"); 

    buttonGroupZoomModel = new QButtonGroup( tab_3, "buttonGroupZoomModel" );
    buttonGroupZoomModel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupZoomModel->sizePolicy().hasHeightForWidth() ) );
    buttonGroupZoomModel->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupZoomModel->setColumnLayout(0, Qt::Vertical );
    buttonGroupZoomModel->layout()->setSpacing( 1 );
    buttonGroupZoomModel->layout()->setMargin( 9 );
    buttonGroupZoomModelLayout = new QGridLayout( buttonGroupZoomModel->layout() );
    buttonGroupZoomModelLayout->setAlignment( Qt::AlignTop );

    radioLinear = new QRadioButton( buttonGroupZoomModel, "radioLinear" );
    buttonGroupZoomModel->insert( radioLinear, 0 );

    buttonGroupZoomModelLayout->addWidget( radioLinear, 0, 0 );

    radioNearest = new QRadioButton( buttonGroupZoomModel, "radioNearest" );
    radioNearest->setChecked( FALSE );
    buttonGroupZoomModel->insert( radioNearest, 1 );

    buttonGroupZoomModelLayout->addWidget( radioNearest, 1, 0 );

    tabLayout_3->addMultiCellWidget( buttonGroupZoomModel, 0, 0, 0, 2 );

    textLabel1_4 = new QLabel( tab_3, "textLabel1_4" );
    textLabel1_4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_4->sizePolicy().hasHeightForWidth() ) );

    tabLayout_3->addWidget( textLabel1_4, 2, 0 );
    QSpacerItem* spacer_11 = new QSpacerItem( 320, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    tabLayout_3->addItem( spacer_11, 2, 2 );

    spinZoomFactor = new QSpinBox( tab_3, "spinZoomFactor" );
    spinZoomFactor->setMaxValue( 300 );
    spinZoomFactor->setValue( 25 );

    tabLayout_3->addWidget( spinZoomFactor, 2, 1 );

    buttonGroupZoomType = new QButtonGroup( tab_3, "buttonGroupZoomType" );
    buttonGroupZoomType->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, buttonGroupZoomType->sizePolicy().hasHeightForWidth() ) );
    buttonGroupZoomType->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroupZoomType->setColumnLayout(0, Qt::Vertical );
    buttonGroupZoomType->layout()->setSpacing( 1 );
    buttonGroupZoomType->layout()->setMargin( 9 );
    buttonGroupZoomTypeLayout = new QGridLayout( buttonGroupZoomType->layout() );
    buttonGroupZoomTypeLayout->setAlignment( Qt::AlignTop );

    radioZoomOriginal = new QRadioButton( buttonGroupZoomType, "radioZoomOriginal" );

    buttonGroupZoomTypeLayout->addWidget( radioZoomOriginal, 0, 0 );

    radioButton27 = new QRadioButton( buttonGroupZoomType, "radioButton27" );

    buttonGroupZoomTypeLayout->addWidget( radioButton27, 1, 0 );

    radioButton21 = new QRadioButton( buttonGroupZoomType, "radioButton21" );

    buttonGroupZoomTypeLayout->addWidget( radioButton21, 4, 0 );

    radioButton20 = new QRadioButton( buttonGroupZoomType, "radioButton20" );

    buttonGroupZoomTypeLayout->addWidget( radioButton20, 3, 0 );

    radioButton28 = new QRadioButton( buttonGroupZoomType, "radioButton28" );

    buttonGroupZoomTypeLayout->addWidget( radioButton28, 2, 0 );

    tabLayout_3->addMultiCellWidget( buttonGroupZoomType, 1, 1, 0, 2 );
    QSpacerItem* spacer_12 = new QSpacerItem( 70, 70, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_3->addItem( spacer_12, 3, 2 );
    tabWidget2->insertTab( tab_3, "" );

    tab_4 = new QWidget( tabWidget2, "tab_4" );
    tabLayout_4 = new QGridLayout( tab_4, 1, 1, 11, 6, "tabLayout_4"); 

    buttonGroupScrolling = new QButtonGroup( tab_4, "buttonGroupScrolling" );
    buttonGroupScrolling->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupScrolling->sizePolicy().hasHeightForWidth() ) );
    buttonGroupScrolling->setColumnLayout(0, Qt::Vertical );
    buttonGroupScrolling->layout()->setSpacing( 1 );
    buttonGroupScrolling->layout()->setMargin( 9 );
    buttonGroupScrollingLayout = new QGridLayout( buttonGroupScrolling->layout() );
    buttonGroupScrollingLayout->setAlignment( Qt::AlignTop );

    radioSCRZoom = new QRadioButton( buttonGroupScrolling, "radioSCRZoom" );

    buttonGroupScrollingLayout->addWidget( radioSCRZoom, 0, 0 );

    radioSRCNext = new QRadioButton( buttonGroupScrolling, "radioSRCNext" );

    buttonGroupScrollingLayout->addWidget( radioSRCNext, 1, 0 );

    tabLayout_4->addWidget( buttonGroupScrolling, 0, 0 );
    QSpacerItem* spacer_13 = new QSpacerItem( 30, 191, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_4->addItem( spacer_13, 1, 0 );
    tabWidget2->insertTab( tab_4, "" );

    pageGLLayout->addWidget( tabWidget2, 0, 0 );
    widgetStack1->addWidget( pageGL, 3 );

    pageLibraries = new QWidget( widgetStack1, "pageLibraries" );
    pageLibrariesLayout = new QGridLayout( pageLibraries, 1, 1, 11, 6, "pageLibrariesLayout"); 

    textLabel1 = new QLabel( pageLibraries, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    pageLibrariesLayout->addWidget( textLabel1, 0, 0 );

    buttonGroup12 = new QButtonGroup( pageLibraries, "buttonGroup12" );
    buttonGroup12->setFrameShape( QButtonGroup::NoFrame );
    buttonGroup12->setColumnLayout(0, Qt::Vertical );
    buttonGroup12->layout()->setSpacing( 2 );
    buttonGroup12->layout()->setMargin( 0 );
    buttonGroup12Layout = new QGridLayout( buttonGroup12->layout() );
    buttonGroup12Layout->setAlignment( Qt::AlignTop );

    checkMonitor = new QCheckBox( buttonGroup12, "checkMonitor" );

    buttonGroup12Layout->addMultiCellWidget( checkMonitor, 0, 0, 0, 1 );
    QSpacerItem* spacer_14 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    buttonGroup12Layout->addItem( spacer_14, 1, 0 );

    checkFAMMessage = new QCheckBox( buttonGroup12, "checkFAMMessage" );
    checkFAMMessage->setEnabled( FALSE );

    buttonGroup12Layout->addWidget( checkFAMMessage, 1, 1 );

    pageLibrariesLayout->addMultiCellWidget( buttonGroup12, 1, 1, 0, 2 );

    pushNewPrefix = new QPushButton( pageLibraries, "pushNewPrefix" );

    pageLibrariesLayout->addWidget( pushNewPrefix, 0, 2 );

    checkShowLinks = new QCheckBox( pageLibraries, "checkShowLinks" );
    checkShowLinks->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, checkShowLinks->sizePolicy().hasHeightForWidth() ) );
    checkShowLinks->setChecked( FALSE );

    pageLibrariesLayout->addMultiCellWidget( checkShowLinks, 3, 3, 0, 2 );

    checkContinue = new QCheckBox( pageLibraries, "checkContinue" );
    checkContinue->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, checkContinue->sizePolicy().hasHeightForWidth() ) );
    checkContinue->setAutoRepeat( FALSE );

    pageLibrariesLayout->addMultiCellWidget( checkContinue, 2, 2, 0, 2 );

    WST = new QWidgetStack( pageLibraries, "WST" );
    WST->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, WST->sizePolicy().hasHeightForWidth() ) );

    page_8 = new QWidget( WST, "page_8" );
    pageLayout_8 = new QGridLayout( page_8, 1, 1, 0, 0, "pageLayout_8"); 

    tableLib = new QListView( page_8, "tableLib" );
    tableLib->addColumn( tr2i18n( "Library" ) );
    tableLib->addColumn( tr2i18n( "Info" ) );
    tableLib->addColumn( tr2i18n( "Version" ) );
    tableLib->addColumn( tr2i18n( "Extensions" ) );
    tableLib->setFrameShape( QListView::ToolBarPanel );
    tableLib->setSelectionMode( QListView::Extended );
    tableLib->setAllColumnsShowFocus( TRUE );
    tableLib->setResizeMode( QListView::AllColumns );

    pageLayout_8->addWidget( tableLib, 0, 0 );
    WST->addWidget( page_8, 0 );

    page_9 = new QWidget( WST, "page_9" );
    pageLayout_9 = new QGridLayout( page_9, 1, 1, 11, 6, "pageLayout_9"); 

    pixmapNotFound = new QLabel( page_9, "pixmapNotFound" );
    pixmapNotFound->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pixmapNotFound->sizePolicy().hasHeightForWidth() ) );
    pixmapNotFound->setScaledContents( FALSE );
    pixmapNotFound->setAlignment( int( QLabel::AlignCenter ) );

    pageLayout_9->addWidget( pixmapNotFound, 0, 0 );
    WST->addWidget( page_9, 1 );

    pageLibrariesLayout->addMultiCellWidget( WST, 4, 4, 0, 2 );

    comboPrefix = new KHistoryCombo( pageLibraries, "comboPrefix" );
    comboPrefix->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, comboPrefix->sizePolicy().hasHeightForWidth() ) );

    pageLibrariesLayout->addWidget( comboPrefix, 0, 1 );
    widgetStack1->addWidget( pageLibraries, 4 );

    pageCaching = new QWidget( widgetStack1, "pageCaching" );
    pageCachingLayout = new QGridLayout( pageCaching, 1, 1, 11, 6, "pageCachingLayout"); 

    checkCacheEnable = new QCheckBox( pageCaching, "checkCacheEnable" );
    checkCacheEnable->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, checkCacheEnable->sizePolicy().hasHeightForWidth() ) );
    checkCacheEnable->setChecked( TRUE );

    pageCachingLayout->addWidget( checkCacheEnable, 0, 0 );
    QSpacerItem* spacer_15 = new QSpacerItem( 40, 75, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageCachingLayout->addItem( spacer_15, 2, 1 );

    buttonGroup13 = new QButtonGroup( pageCaching, "buttonGroup13" );
    buttonGroup13->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, buttonGroup13->sizePolicy().hasHeightForWidth() ) );
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
    buttonGroupCachePolicy->layout()->setSpacing( 1 );
    buttonGroupCachePolicy->layout()->setMargin( 9 );
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

    checkCacheMax = new QCheckBox( buttonGroup13, "checkCacheMax" );

    buttonGroup13Layout->addMultiCellWidget( checkCacheMax, 3, 3, 0, 1 );

    spinCacheMax = new QSpinBox( buttonGroup13, "spinCacheMax" );
    spinCacheMax->setMaxValue( 1000 );
    spinCacheMax->setMinValue( 0 );
    spinCacheMax->setValue( 0 );

    buttonGroup13Layout->addWidget( spinCacheMax, 4, 0 );

    textLabel1_2 = new QLabel( buttonGroup13, "textLabel1_2" );
    textLabel1_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_2->sizePolicy().hasHeightForWidth() ) );

    buttonGroup13Layout->addWidget( textLabel1_2, 4, 1 );

    pageCachingLayout->addMultiCellWidget( buttonGroup13, 1, 1, 0, 1 );
    widgetStack1->addWidget( pageCaching, 5 );

    SQ_OptionsLayout->addMultiCellWidget( widgetStack1, 0, 0, 1, 3 );

    listMain = new QListBox( this, "listMain" );
    listMain->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)3, 0, 0, listMain->sizePolicy().hasHeightForWidth() ) );
    listMain->setResizePolicy( QListBox::AutoOne );
    listMain->setColumnMode( QListBox::FixedNumber );
    listMain->setRowMode( QListBox::Variable );

    SQ_OptionsLayout->addWidget( listMain, 0, 0 );
    languageChange();
    resize( QSize(648, 494).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( checkCacheEnable, SIGNAL( toggled(bool) ), buttonGroup13, SLOT( setEnabled(bool) ) );
    connect( checkClickSystem, SIGNAL( toggled(bool) ), buttonGroupClickPolicy, SLOT( setDisabled(bool) ) );
    connect( checkMonitor, SIGNAL( toggled(bool) ), checkFAMMessage, SLOT( setEnabled(bool) ) );
    connect( checkShowLinks, SIGNAL( toggled(bool) ), this, SLOT( slotShowLinks(bool) ) );
    connect( listMain, SIGNAL( highlighted(int) ), widgetStack1, SLOT( raiseWidget(int) ) );
    connect( pushNewPrefix, SIGNAL( clicked() ), this, SLOT( slotNewPrefix() ) );
    connect( pushOpenCacheDir, SIGNAL( clicked() ), this, SLOT( slotDirCache() ) );
    connect( pushOpenDir, SIGNAL( clicked() ), this, SLOT( slotOpenDir() ) );
    connect( radioSetThis, SIGNAL( toggled(bool) ), lineEditCustomDir, SLOT( setEnabled(bool) ) );
    connect( checkCacheMax, SIGNAL( toggled(bool) ), spinCacheMax, SLOT( setEnabled(bool) ) );
    connect( checkSystemColor, SIGNAL( toggled(bool) ), kColorGLbackground, SLOT( setDisabled(bool) ) );
    connect( sliderAngle, SIGNAL( valueChanged(int) ), textAngle, SLOT( setNum(int) ) );
    connect( sliderMove, SIGNAL( valueChanged(int) ), textMove, SLOT( setNum(int) ) );
    connect( comboPrefix, SIGNAL( returnPressed(const QString&) ), comboPrefix, SLOT( addToHistory(const QString&) ) );
    connect( buttonGroupViewType, SIGNAL( clicked(int) ), widgetStackView, SLOT( raiseWidget(int) ) );
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
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    checkRestart->setText( tr2i18n( "<NI> Restart SQuirrel after saving options" ) );
    checkMinimize->setText( tr2i18n( "Minimize to tray when close()" ) );
    checkOneInstance->setText( tr2i18n( "Allow only one instanse (useful with tray support)" ) );
    checkSync->setText( tr2i18n( "Write configuration file to disk, when I press \"OK\"" ) );
    buttonGroupCreateFirst->setTitle( tr2i18n( "Create first ..." ) );
    radioButton16->setText( tr2i18n( "Detailed View" ) );
    radioButton14->setText( tr2i18n( "Icon View" ) );
    radioButton15->setText( tr2i18n( "List View" ) );
    textLabel1_3->setText( tr2i18n( "Toolbar icon size:" ) );
    comboToolbarIconSize->clear();
    comboToolbarIconSize->insertItem( tr2i18n( "16" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "22" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "32" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "48" ) );
    comboToolbarIconSize->setCurrentItem( 0 );
    buttonGroup14->setTitle( tr2i18n( "Main window" ) );
    checkSavePos->setText( tr2i18n( "Save last position" ) );
    checkSaveSize->setText( tr2i18n( "Save last size" ) );
    buttonGroupViewType->setTitle( tr2i18n( "Look like ..." ) );
    radioSQuirrel->setText( tr2i18n( "SQuirrel" ) );
    QToolTip::add( radioSQuirrel, tr2i18n( "Like <a href=\"ksquirrel.sf.net\">SQuirrel</a>. Has TreeView, FileView, separate GLView" ) );
    radioGQview->setText( tr2i18n( "GQview" ) );
    QToolTip::add( radioGQview, tr2i18n( "Like <a href=\"gqview.sf.net\">GQview</a>. Has FileView, built-in GLView" ) );
    radioKuickShow->setText( tr2i18n( "KuickShow" ) );
    QToolTip::add( radioKuickShow, tr2i18n( "Like Kuickshow. Has FileView, separate GLView" ) );
    radioButton29->setText( tr2i18n( "ShowImg" ) );
    QToolTip::add( radioButton29, tr2i18n( "Like ShowImg, has treeview, fileview, built-in GL viewer" ) );
    radioWinViewer->setText( tr2i18n( "WinViewer" ) );
    QToolTip::add( radioWinViewer, tr2i18n( "Like Windows Picture and Fax viewer. Has only built-in GLView" ) );
    radioXnview->setText( tr2i18n( "XnView" ) );
    QToolTip::add( radioXnview, tr2i18n( "Like XnView. Has TreeView, FileView, built-in GLView" ) );
    radioButton13->setText( tr2i18n( "Konqueror" ) );
    QToolTip::add( radioButton13, tr2i18n( "Like Konqueror. GLView toglles with FileView" ) );
    buttonGroupSetPath->setTitle( tr2i18n( "On starting open ..." ) );
    QToolTip::add( buttonGroupSetPath, QString::null );
    radioSetThis->setText( tr2i18n( "Custom directory:" ) );
    radioSetLast->setText( tr2i18n( "Last visited directory" ) );
    radioSetCurrent->setText( tr2i18n( "Current directory, where SQuirrel starts" ) );
    lineEditCustomDir->setText( QString::null );
    pushOpenDir->setText( tr2i18n( "Open" ) );
    buttonGroup7->setTitle( tr2i18n( "Clicking policy" ) );
    buttonGroupClickPolicy->setTitle( QString::null );
    radioButton18->setText( tr2i18n( "Use double click to open item" ) );
    radioButton17->setText( tr2i18n( "Use single click to open item" ) );
    checkClickSystem->setText( tr2i18n( "Use system settings" ) );
    QToolTip::add( checkClickSystem, tr2i18n( "if KDE uses single click to open item, SQuirrel will have too" ) );
    checkRunUnknown->setText( tr2i18n( "Run unknown file formats separately (with default application)" ) );
    QToolTip::add( checkRunUnknown, tr2i18n( "For example, run unsupported \"png\" format in kuickshow" ) );
    buttonGroupSync->setTitle( tr2i18n( "Synchronization" ) );
    radioSyncBoth->setText( tr2i18n( "<NI>Sync both" ) );
    radioSyncTreeWStack->setText( tr2i18n( "Tree -> sqWStack" ) );
    radioSyncWStackTree->setText( tr2i18n( "<NI>sqWStack->Tree" ) );
    checkSaveHistory->setText( tr2i18n( "Save history" ) );
    buttonGroup15->setTitle( tr2i18n( "If windowed" ) );
    checkGLSaveSize->setText( tr2i18n( "Save size" ) );
    checkGLSavePos->setText( tr2i18n( "Save position" ) );
    textLabel1_5->setText( tr2i18n( "Update viewport after" ) );
    textLabel2->setText( tr2i18n( "lines of image decoded" ) );
    checkDrop->setText( tr2i18n( "Enable drop files into viewport" ) );
    tabWidget2->changeTab( tab, tr2i18n( "General" ) );
    buttonGroup9->setTitle( tr2i18n( "Background color" ) );
    textLabel1_5_2->setText( tr2i18n( "custom:" ) );
    kColorGLbackground->setText( QString::null );
    QToolTip::add( kColorGLbackground, tr2i18n( "GLView will have such background color" ) );
    checkSystemColor->setText( tr2i18n( "use system color" ) );
    textMove->setText( QString::null );
    textLabel2_3_2->setText( tr2i18n( "pix" ) );
    textLabel1_6_2->setText( tr2i18n( "Rotating:" ) );
    textLabel1_7_2->setText( tr2i18n( "Moving:" ) );
    textAngle->setText( QString::null );
    textLabel1_9_2->setText( tr2i18n( "deg" ) );
    checkBorder->setText( tr2i18n( "<NI>Draw a border" ) );
    checkBackgroundTransparent->setText( tr2i18n( "<NI>Background for transparent images (usually quads)" ) );
    buttonGroupShadeModel->setTitle( tr2i18n( "Shade Model" ) );
    radioSmooth->setText( tr2i18n( "GL_SMOOTH" ) );
    radioFlat->setText( tr2i18n( "GL_FLAT" ) );
    buttonGroupCenter->setTitle( tr2i18n( "Flipping and rotating" ) );
    radioCenterImage->setText( tr2i18n( "Relative to image center" ) );
    radioCenterAxes->setText( tr2i18n( "Relative to axes" ) );
    tabWidget2->changeTab( tab_2, tr2i18n( "OpenGL" ) );
    buttonGroupZoomModel->setTitle( tr2i18n( "Zoom Model" ) );
    radioLinear->setText( tr2i18n( "GL_LINEAR" ) );
    radioNearest->setText( tr2i18n( "GL_NEAREST" ) );
    textLabel1_4->setText( tr2i18n( "Zoom koefficient:" ) );
    spinZoomFactor->setSuffix( tr2i18n( " %" ) );
    spinZoomFactor->setSpecialValueText( tr2i18n( "None" ) );
    buttonGroupZoomType->setTitle( tr2i18n( "<NI>Zoom type" ) );
    radioZoomOriginal->setText( tr2i18n( "Zoom to original" ) );
    radioButton27->setText( tr2i18n( "Fit width" ) );
    radioButton21->setText( tr2i18n( "Leave previous zoom" ) );
    radioButton20->setText( tr2i18n( "Fit image" ) );
    radioButton28->setText( tr2i18n( "Fit height" ) );
    tabWidget2->changeTab( tab_3, tr2i18n( "Zoom" ) );
    buttonGroupScrolling->setTitle( tr2i18n( "If scrolling event" ) );
    radioSCRZoom->setText( tr2i18n( "Zoom the scene" ) );
    radioSRCNext->setText( tr2i18n( "Load next/previos file" ) );
    tabWidget2->changeTab( tab_4, tr2i18n( "Scrolling" ) );
    textLabel1->setText( tr2i18n( "Libraries:" ) );
    QToolTip::add( textLabel1, tr2i18n( "Where SQuirrel should search libraries" ) );
    buttonGroup12->setTitle( QString::null );
    checkMonitor->setText( tr2i18n( "Monitor directory with libraries" ) );
    QToolTip::add( checkMonitor, tr2i18n( "Monitor directory with libraries, so new .so files can be dynamically added" ) );
    checkFAMMessage->setText( tr2i18n( "Show message" ) );
    QToolTip::add( checkFAMMessage, tr2i18n( "Show dialog with information about new or deleted libraries" ) );
    pushNewPrefix->setText( tr2i18n( "Browse" ) );
    checkShowLinks->setText( tr2i18n( "show 'symlink' libraries" ) );
    QToolTip::add( checkShowLinks, tr2i18n( "If not checked, this table will contain only real .so files, not symlinks" ) );
    checkContinue->setText( tr2i18n( "Continue after resolving errors" ) );
    QToolTip::add( checkContinue, tr2i18n( "Resolving functions will continue after errors, if checked. Normally library with any error will NOT be handled, and format won't be supported" ) );
    tableLib->header()->setLabel( 0, tr2i18n( "Library" ) );
    tableLib->header()->setLabel( 1, tr2i18n( "Info" ) );
    tableLib->header()->setLabel( 2, tr2i18n( "Version" ) );
    tableLib->header()->setLabel( 3, tr2i18n( "Extensions" ) );
    QToolTip::add( tableLib, tr2i18n( "Contains information about all found libraries" ) );
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
    checkCacheMax->setText( tr2i18n( "Maximum size" ) );
    spinCacheMax->setSpecialValueText( tr2i18n( "Unlimited" ) );
    textLabel1_2->setText( tr2i18n( "Mb" ) );
}

#include "sq_options.moc"
