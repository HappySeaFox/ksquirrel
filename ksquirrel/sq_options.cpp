#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_options.ui'
**
** Created: Tue Mar 30 01:30:59 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_options.h"

#include <qvariant.h>
#include <kcolorbutton.h>
#include <kde/kfiledialog.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qframe.h>
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
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 608, 459 ) );
    setMaximumSize( QSize( 608, 459 ) );
    setSizeGripEnabled( FALSE );

    listMain = new QListView( this, "listMain" );
    listMain->addColumn( tr2i18n( "Column 1" ) );
    listMain->setGeometry( QRect( 11, 11, 90, 406 ) );
    listMain->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, listMain->sizePolicy().hasHeightForWidth() ) );
    listMain->setFrameShape( QListView::StyledPanel );
    listMain->setFrameShadow( QListView::Sunken );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setGeometry( QRect( 454, 424, 70, 25 ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    buttonOk->setFlat( FALSE );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setGeometry( QRect( 530, 424, 70, 25 ) );
    buttonCancel->setAutoDefault( TRUE );
    buttonCancel->setFlat( FALSE );

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setGeometry( QRect( 10, 425, 70, 25 ) );
    buttonHelp->setAutoDefault( FALSE );
    buttonHelp->setFlat( FALSE );

    widgetStack1 = new QWidgetStack( this, "widgetStack1" );
    widgetStack1->setGeometry( QRect( 105, 10, 495, 406 ) );
    widgetStack1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, widgetStack1->sizePolicy().hasHeightForWidth() ) );
    widgetStack1->setFrameShape( QWidgetStack::ToolBarPanel );
    widgetStack1->setFrameShadow( QWidgetStack::Sunken );

    page = new QWidget( widgetStack1, "page" );

    checkSplash = new QCheckBox( page, "checkSplash" );
    checkSplash->setGeometry( QRect( 10, 15, 355, 20 ) );

    checkRestart = new QCheckBox( page, "checkRestart" );
    checkRestart->setGeometry( QRect( 10, 40, 355, 20 ) );
    checkRestart->setChecked( FALSE );

    checkMinimize = new QCheckBox( page, "checkMinimize" );
    checkMinimize->setGeometry( QRect( 10, 65, 355, 20 ) );
    widgetStack1->addWidget( page, 0 );

    page_2 = new QWidget( widgetStack1, "page_2" );

    textLabel1_3 = new QLabel( page_2, "textLabel1_3" );
    textLabel1_3->setGeometry( QRect( 15, 300, 110, 16 ) );

    comboToolbarIconSize = new QComboBox( FALSE, page_2, "comboToolbarIconSize" );
    comboToolbarIconSize->setGeometry( QRect( 130, 300, 80, 25 ) );

    buttonGroupViewType = new QButtonGroup( page_2, "buttonGroupViewType" );
    buttonGroupViewType->setGeometry( QRect( 8, 8, 410, 165 ) );
    buttonGroupViewType->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupViewType->sizePolicy().hasHeightForWidth() ) );
    buttonGroupViewType->setFrameShape( QButtonGroup::LineEditPanel );

    radioSQuirrel = new QRadioButton( buttonGroupViewType, "radioSQuirrel" );
    radioSQuirrel->setGeometry( QRect( 15, 30, 120, 20 ) );
    radioSQuirrel->setChecked( FALSE );
    buttonGroupViewType->insert( radioSQuirrel, 0 );

    radioWinViewer = new QRadioButton( buttonGroupViewType, "radioWinViewer" );
    radioWinViewer->setGeometry( QRect( 15, 90, 120, 20 ) );
    buttonGroupViewType->insert( radioWinViewer, 3 );

    radioXnview = new QRadioButton( buttonGroupViewType, "radioXnview" );
    radioXnview->setGeometry( QRect( 15, 110, 120, 20 ) );
    buttonGroupViewType->insert( radioXnview, 4 );

    radioButton13 = new QRadioButton( buttonGroupViewType, "radioButton13" );
    radioButton13->setGeometry( QRect( 15, 130, 120, 20 ) );

    radioGQview = new QRadioButton( buttonGroupViewType, "radioGQview" );
    radioGQview->setGeometry( QRect( 15, 50, 120, 20 ) );
    buttonGroupViewType->insert( radioGQview, 1 );

    radioKuickShow = new QRadioButton( buttonGroupViewType, "radioKuickShow" );
    radioKuickShow->setGeometry( QRect( 15, 70, 120, 20 ) );
    buttonGroupViewType->insert( radioKuickShow, 2 );

    pixmapShowView = new QLabel( buttonGroupViewType, "pixmapShowView" );
    pixmapShowView->setGeometry( QRect( 155, 20, 241, 130 ) );
    pixmapShowView->setScaledContents( FALSE );

    buttonGroupCreateFirst = new QButtonGroup( page_2, "buttonGroupCreateFirst" );
    buttonGroupCreateFirst->setGeometry( QRect( 10, 180, 405, 100 ) );
    buttonGroupCreateFirst->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupCreateFirst->sizePolicy().hasHeightForWidth() ) );
    buttonGroupCreateFirst->setFrameShape( QButtonGroup::LineEditPanel );

    radioButton16 = new QRadioButton( buttonGroupCreateFirst, "radioButton16" );
    radioButton16->setGeometry( QRect( 10, 65, 170, 21 ) );
    buttonGroupCreateFirst->insert( radioButton16, 2 );

    radioButton14 = new QRadioButton( buttonGroupCreateFirst, "radioButton14" );
    radioButton14->setGeometry( QRect( 10, 45, 170, 21 ) );
    buttonGroupCreateFirst->insert( radioButton14, 1 );

    radioButton15 = new QRadioButton( buttonGroupCreateFirst, "radioButton15" );
    radioButton15->setGeometry( QRect( 10, 25, 170, 21 ) );
    buttonGroupCreateFirst->insert( radioButton15, 0 );
    widgetStack1->addWidget( page_2, 1 );

    page_3 = new QWidget( widgetStack1, "page_3" );

    textLabel2 = new QLabel( page_3, "textLabel2" );
    textLabel2->setGeometry( QRect( 11, 282, 189, 27 ) );

    textLabel1_2 = new QLabel( page_3, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 11, 249, 189, 27 ) );

    comboIconIndex = new QComboBox( FALSE, page_3, "comboIconIndex" );
    comboIconIndex->setGeometry( QRect( 206, 249, 90, 27 ) );
    comboIconIndex->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, comboIconIndex->sizePolicy().hasHeightForWidth() ) );
    comboIconIndex->setMaxCount( 20 );

    comboListIndex = new QComboBox( FALSE, page_3, "comboListIndex" );
    comboListIndex->setGeometry( QRect( 206, 282, 90, 27 ) );
    comboListIndex->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, comboListIndex->sizePolicy().hasHeightForWidth() ) );

    checkRunUnknown = new QCheckBox( page_3, "checkRunUnknown" );
    checkRunUnknown->setGeometry( QRect( 11, 352, 285, 21 ) );

    checkSyncTree = new QCheckBox( page_3, "checkSyncTree" );
    checkSyncTree->setGeometry( QRect( 11, 325, 285, 21 ) );

    buttonGroup7 = new QButtonGroup( page_3, "buttonGroup7" );
    buttonGroup7->setGeometry( QRect( 11, 133, 470, 105 ) );
    buttonGroup7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroup7->sizePolicy().hasHeightForWidth() ) );
    buttonGroup7->setFrameShape( QButtonGroup::LineEditPanel );

    buttonGroupClickPolicy = new QButtonGroup( buttonGroup7, "buttonGroupClickPolicy" );
    buttonGroupClickPolicy->setEnabled( TRUE );
    buttonGroupClickPolicy->setGeometry( QRect( 33, 49, 395, 45 ) );
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

    checkClickSystem = new QCheckBox( buttonGroup7, "checkClickSystem" );
    checkClickSystem->setGeometry( QRect( 11, 22, 345, 21 ) );

    buttonGroupSetPath = new QButtonGroup( page_3, "buttonGroupSetPath" );
    buttonGroupSetPath->setGeometry( QRect( 11, 11, 470, 110 ) );
    buttonGroupSetPath->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupSetPath->sizePolicy().hasHeightForWidth() ) );
    buttonGroupSetPath->setFrameShape( QButtonGroup::LineEditPanel );

    radioSetThis = new QRadioButton( buttonGroupSetPath, "radioSetThis" );
    radioSetThis->setGeometry( QRect( 10, 70, 91, 21 ) );
    buttonGroupSetPath->insert( radioSetThis, 2 );

    radioSetLast = new QRadioButton( buttonGroupSetPath, "radioSetLast" );
    radioSetLast->setGeometry( QRect( 10, 20, 91, 21 ) );
    buttonGroupSetPath->insert( radioSetLast, 0 );

    radioSetCurrent = new QRadioButton( buttonGroupSetPath, "radioSetCurrent" );
    radioSetCurrent->setGeometry( QRect( 10, 45, 91, 21 ) );
    buttonGroupSetPath->insert( radioSetCurrent, 1 );

    lineEditCustomDir = new QLineEdit( buttonGroupSetPath, "lineEditCustomDir" );
    lineEditCustomDir->setEnabled( FALSE );
    lineEditCustomDir->setGeometry( QRect( 113, 72, 270, 25 ) );

    pushOpenDir = new QPushButton( buttonGroupSetPath, "pushOpenDir" );
    pushOpenDir->setGeometry( QRect( 390, 70, 70, 25 ) );
    pushOpenDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pushOpenDir->sizePolicy().hasHeightForWidth() ) );
    pushOpenDir->setFlat( TRUE );
    widgetStack1->addWidget( page_3, 2 );

    page_4 = new QWidget( widgetStack1, "page_4" );

    textPrefix = new QLabel( page_4, "textPrefix" );
    textPrefix->setGeometry( QRect( 80, 11, 449, 16 ) );
    textPrefix->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textPrefix->sizePolicy().hasHeightForWidth() ) );

    textLabel1 = new QLabel( page_4, "textLabel1" );
    textLabel1->setGeometry( QRect( 11, 11, 63, 16 ) );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    checkMonitor = new QCheckBox( page_4, "checkMonitor" );
    checkMonitor->setGeometry( QRect( 11, 33, 300, 21 ) );

    checkFAMMessage = new QCheckBox( page_4, "checkFAMMessage" );
    checkFAMMessage->setEnabled( FALSE );
    checkFAMMessage->setGeometry( QRect( 26, 60, 285, 21 ) );

    tableLib = new QListView( page_4, "tableLib" );
    tableLib->setGeometry( QRect( 10, 130, 475, 265 ) );
    tableLib->setFrameShape( QListView::ToolBarPanel );

    checkShowLinks = new QCheckBox( page_4, "checkShowLinks" );
    checkShowLinks->setGeometry( QRect( 11, 102, 300, 21 ) );
    checkShowLinks->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, checkShowLinks->sizePolicy().hasHeightForWidth() ) );
    checkShowLinks->setChecked( TRUE );

    line1 = new QFrame( page_4, "line1" );
    line1->setGeometry( QRect( 9, 84, 475, 16 ) );
    QPalette pal;
    QColorGroup cg;
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 138, 137, 225) );
    cg.setColor( QColorGroup::Light, QColor( 237, 237, 255) );
    cg.setColor( QColorGroup::Midlight, QColor( 187, 187, 240) );
    cg.setColor( QColorGroup::Dark, QColor( 69, 68, 112) );
    cg.setColor( QColorGroup::Mid, QColor( 92, 91, 150) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 239, 239, 239) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    cg.setColor( QColorGroup::Link, black );
    cg.setColor( QColorGroup::LinkVisited, black );
    pal.setActive( cg );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 138, 137, 225) );
    cg.setColor( QColorGroup::Light, QColor( 237, 237, 255) );
    cg.setColor( QColorGroup::Midlight, QColor( 160, 158, 255) );
    cg.setColor( QColorGroup::Dark, QColor( 69, 68, 112) );
    cg.setColor( QColorGroup::Mid, QColor( 92, 91, 150) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 239, 239, 239) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    cg.setColor( QColorGroup::Link, QColor( 0, 0, 192) );
    cg.setColor( QColorGroup::LinkVisited, QColor( 128, 0, 128) );
    pal.setInactive( cg );
    cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Button, QColor( 138, 137, 225) );
    cg.setColor( QColorGroup::Light, QColor( 237, 237, 255) );
    cg.setColor( QColorGroup::Midlight, QColor( 160, 158, 255) );
    cg.setColor( QColorGroup::Dark, QColor( 69, 68, 112) );
    cg.setColor( QColorGroup::Mid, QColor( 92, 91, 150) );
    cg.setColor( QColorGroup::Text, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 239, 239, 239) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    cg.setColor( QColorGroup::Link, QColor( 0, 0, 192) );
    cg.setColor( QColorGroup::LinkVisited, QColor( 128, 0, 128) );
    pal.setDisabled( cg );
    line1->setPalette( pal );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );
    widgetStack1->addWidget( page_4, 3 );

    page_5 = new QWidget( widgetStack1, "page_5" );

    checkCacheDisk = new QCheckBox( page_5, "checkCacheDisk" );
    checkCacheDisk->setGeometry( QRect( 10, 40, 165, 20 ) );

    checkCacheGL = new QCheckBox( page_5, "checkCacheGL" );
    checkCacheGL->setGeometry( QRect( 10, 15, 225, 20 ) );

    lineCachePath = new QLineEdit( page_5, "lineCachePath" );
    lineCachePath->setEnabled( FALSE );
    lineCachePath->setGeometry( QRect( 180, 40, 235, 25 ) );

    pushOpenCacheDir = new QPushButton( page_5, "pushOpenCacheDir" );
    pushOpenCacheDir->setGeometry( QRect( 420, 40, 65, 26 ) );
    pushOpenCacheDir->setFlat( TRUE );
    widgetStack1->addWidget( page_5, 4 );

    page_6 = new QWidget( widgetStack1, "page_6" );

    tabWidget2 = new QTabWidget( page_6, "tabWidget2" );
    tabWidget2->setGeometry( QRect( 5, 20, 480, 380 ) );

    tab = new QWidget( tabWidget2, "tab" );

    textLabel1_5 = new QLabel( tab, "textLabel1_5" );
    textLabel1_5->setGeometry( QRect( 15, 30, 75, 16 ) );

    buttonGroupShadeModel = new QButtonGroup( tab, "buttonGroupShadeModel" );
    buttonGroupShadeModel->setGeometry( QRect( 15, 80, 200, 80 ) );
    buttonGroupShadeModel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupShadeModel->sizePolicy().hasHeightForWidth() ) );
    buttonGroupShadeModel->setFrameShape( QButtonGroup::LineEditPanel );

    radioSmooth_2 = new QRadioButton( buttonGroupShadeModel, "radioSmooth_2" );
    radioSmooth_2->setGeometry( QRect( 10, 50, 130, 20 ) );
    buttonGroupShadeModel->insert( radioSmooth_2, 1 );

    radioFlat_2 = new QRadioButton( buttonGroupShadeModel, "radioFlat_2" );
    radioFlat_2->setGeometry( QRect( 10, 25, 130, 20 ) );
    buttonGroupShadeModel->insert( radioFlat_2, 0 );

    checkStepByStep = new QCheckBox( tab, "checkStepByStep" );
    checkStepByStep->setGeometry( QRect( 15, 175, 262, 21 ) );
    checkStepByStep->setChecked( TRUE );

    checkDrop = new QCheckBox( tab, "checkDrop" );
    checkDrop->setGeometry( QRect( 15, 205, 265, 21 ) );

    kColorGLbackground = new KColorButton( tab, "kColorGLbackground" );
    kColorGLbackground->setGeometry( QRect( 100, 20, 95, 35 ) );
    kColorGLbackground->setFlat( TRUE );
    tabWidget2->insertTab( tab, "" );

    tab_2 = new QWidget( tabWidget2, "tab_2" );

    buttonGroupZoomModel = new QButtonGroup( tab_2, "buttonGroupZoomModel" );
    buttonGroupZoomModel->setGeometry( QRect( 10, 15, 200, 80 ) );
    buttonGroupZoomModel->setFrameShape( QButtonGroup::LineEditPanel );

    radioLinear_2_2 = new QRadioButton( buttonGroupZoomModel, "radioLinear_2_2" );
    radioLinear_2_2->setGeometry( QRect( 10, 25, 130, 20 ) );
    buttonGroupZoomModel->insert( radioLinear_2_2, 0 );

    radioNearest_2_2 = new QRadioButton( buttonGroupZoomModel, "radioNearest_2_2" );
    radioNearest_2_2->setGeometry( QRect( 10, 50, 130, 20 ) );
    radioNearest_2_2->setChecked( FALSE );
    buttonGroupZoomModel->insert( radioNearest_2_2, 1 );

    textLabel1_4 = new QLabel( tab_2, "textLabel1_4" );
    textLabel1_4->setGeometry( QRect( 15, 245, 145, 25 ) );

    spinZoomFactor = new QSpinBox( tab_2, "spinZoomFactor" );
    spinZoomFactor->setGeometry( QRect( 160, 245, 65, 25 ) );
    spinZoomFactor->setMaxValue( 100 );
    spinZoomFactor->setValue( 25 );

    buttonGroupZoomType = new QButtonGroup( tab_2, "buttonGroupZoomType" );
    buttonGroupZoomType->setGeometry( QRect( 10, 110, 220, 110 ) );
    buttonGroupZoomType->setFrameShape( QButtonGroup::LineEditPanel );

    radioZoomOriginal = new QRadioButton( buttonGroupZoomType, "radioZoomOriginal" );
    radioZoomOriginal->setGeometry( QRect( 10, 25, 180, 21 ) );

    radioButton20 = new QRadioButton( buttonGroupZoomType, "radioButton20" );
    radioButton20->setGeometry( QRect( 10, 50, 180, 21 ) );

    radioButton21 = new QRadioButton( buttonGroupZoomType, "radioButton21" );
    radioButton21->setGeometry( QRect( 10, 75, 180, 21 ) );
    tabWidget2->insertTab( tab_2, "" );
    widgetStack1->addWidget( page_6, 5 );
    languageChange();
    resize( QSize(608, 459).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonGroupViewType, SIGNAL( clicked(int) ), this, SLOT( slotSetViewPixmap(int) ) );
    connect( pushOpenDir, SIGNAL( clicked() ), this, SLOT( slotOpenDir() ) );
    connect( checkCacheDisk, SIGNAL( toggled(bool) ), lineCachePath, SLOT( setEnabled(bool) ) );
    connect( pushOpenCacheDir, SIGNAL( released() ), this, SLOT( slotDirCache() ) );
    connect( radioSetThis, SIGNAL( toggled(bool) ), lineEditCustomDir, SLOT( setEnabled(bool) ) );
    connect( checkMonitor, SIGNAL( toggled(bool) ), checkFAMMessage, SLOT( setEnabled(bool) ) );
    connect( checkShowLinks, SIGNAL( toggled(bool) ), this, SLOT( slotShowLinks(bool) ) );
    connect( checkClickSystem, SIGNAL( toggled(bool) ), buttonGroupClickPolicy, SLOT( setDisabled(bool) ) );
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
    buttonHelp->setText( tr2i18n( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr2i18n( "F1" ) ) );
    checkSplash->setText( tr2i18n( "show splash screen" ) );
    QToolTip::add( checkSplash, tr2i18n( "Show splash screen when SQuireel starts" ) );
    checkRestart->setText( tr2i18n( "<NI> Restart SQuirrel after saving options" ) );
    checkMinimize->setText( tr2i18n( "Minimize to tray when close()" ) );
    textLabel1_3->setText( tr2i18n( "Toolbar icon size" ) );
    comboToolbarIconSize->clear();
    comboToolbarIconSize->insertItem( tr2i18n( "16" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "22" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "32" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "48" ) );
    comboToolbarIconSize->setCurrentItem( 0 );
    buttonGroupViewType->setTitle( tr2i18n( "Look like ..." ) );
    radioSQuirrel->setText( tr2i18n( "SQuirrel" ) );
    QToolTip::add( radioSQuirrel, tr2i18n( "Like SQuirrel. Has TreeView, FileView, separate GLView" ) );
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
    buttonGroupCreateFirst->setTitle( tr2i18n( "Create first ..." ) );
    radioButton16->setText( tr2i18n( "Detailed View" ) );
    radioButton14->setText( tr2i18n( "Icon View" ) );
    radioButton15->setText( tr2i18n( "List View" ) );
    textLabel2->setText( tr2i18n( "default icon size for \"List View\"" ) );
    textLabel1_2->setText( tr2i18n( "default icon size for \"Icon View\"" ) );
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
    checkRunUnknown->setText( tr2i18n( "<NI>Run unknown file formats separately" ) );
    QToolTip::add( checkRunUnknown, tr2i18n( "For example, run unsupported \"png\" format in kuickshow" ) );
    checkSyncTree->setText( tr2i18n( "<NI>Syncronize Tree i FIleView" ) );
    QToolTip::add( checkSyncTree, tr2i18n( "TreeView and FileView will have common path" ) );
    buttonGroup7->setTitle( tr2i18n( "Clicking policy" ) );
    buttonGroupClickPolicy->setTitle( QString::null );
    radioButton18->setText( tr2i18n( "Use double click policy" ) );
    radioButton17->setText( tr2i18n( "Use single click policy" ) );
    checkClickSystem->setText( tr2i18n( "Use system settings" ) );
    QToolTip::add( checkClickSystem, tr2i18n( "if KDE uses single click to open item, SQuirrel will have too" ) );
    buttonGroupSetPath->setTitle( tr2i18n( "Set path ..." ) );
    QToolTip::add( buttonGroupSetPath, QString::null );
    radioSetThis->setText( tr2i18n( "Set this" ) );
    radioSetLast->setText( tr2i18n( "Set last" ) );
    radioSetCurrent->setText( tr2i18n( "Set current" ) );
    lineEditCustomDir->setText( QString::null );
    pushOpenDir->setText( tr2i18n( "Open" ) );
    textPrefix->setText( QString::null );
    QToolTip::add( textPrefix, tr2i18n( "Where SQuirrel should search libraries" ) );
    textLabel1->setText( tr2i18n( "$PREFIX:" ) );
    QToolTip::add( textLabel1, tr2i18n( "Where SQuirrel should search libraries" ) );
    checkMonitor->setText( tr2i18n( "Monitor $PREFIX directory" ) );
    QToolTip::add( checkMonitor, tr2i18n( "Monitor $PREFIX directory, so new .so files can be dynamically added" ) );
    checkFAMMessage->setText( tr2i18n( "Show message" ) );
    QToolTip::add( checkFAMMessage, tr2i18n( "Show dialog with information about new or deleted libraries" ) );
    QToolTip::add( tableLib, tr2i18n( "Contains information about all found libraries" ) );
    checkShowLinks->setText( tr2i18n( "show 'symlink' libraries" ) );
    QToolTip::add( checkShowLinks, tr2i18n( "If not checked, this table will contain only real .so files, not symlinks" ) );
    checkCacheDisk->setText( tr2i18n( "<NI>Cache on disk" ) );
    checkCacheGL->setText( tr2i18n( "<NI>Cache in OpenGL textures" ) );
    pushOpenCacheDir->setText( tr2i18n( "Open" ) );
    textLabel1_5->setText( tr2i18n( "Background:" ) );
    buttonGroupShadeModel->setTitle( tr2i18n( "Shade Model" ) );
    radioSmooth_2->setText( tr2i18n( "GL_SMOOTH" ) );
    radioFlat_2->setText( tr2i18n( "GL_FLAT" ) );
    checkStepByStep->setText( tr2i18n( "<NI>Step-by-step decoding and showing" ) );
    checkDrop->setText( tr2i18n( "Enable drop" ) );
    kColorGLbackground->setText( QString::null );
    QToolTip::add( kColorGLbackground, tr2i18n( "GLView will have such background color" ) );
    tabWidget2->changeTab( tab, tr2i18n( "Main" ) );
    buttonGroupZoomModel->setTitle( tr2i18n( "Zoom Model" ) );
    radioLinear_2_2->setText( tr2i18n( "GL_LINEAR" ) );
    radioNearest_2_2->setText( tr2i18n( "GL_NEAREST" ) );
    textLabel1_4->setText( tr2i18n( "<NI>Zoom increment:" ) );
    spinZoomFactor->setSuffix( tr2i18n( " %" ) );
    spinZoomFactor->setSpecialValueText( tr2i18n( "None" ) );
    buttonGroupZoomType->setTitle( tr2i18n( "<NI>Zoom type" ) );
    radioZoomOriginal->setText( tr2i18n( "Zoom to original" ) );
    radioButton20->setText( tr2i18n( "Fit to window" ) );
    radioButton21->setText( tr2i18n( "Leave previous zoom" ) );
    tabWidget2->changeTab( tab_2, tr2i18n( "Zoom" ) );
}

#include "sq_options.moc"
