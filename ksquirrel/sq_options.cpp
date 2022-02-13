#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_options.ui'
**
** Created: Sat Mar 27 17:13:50 2004
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
    setMinimumSize( QSize( 400, 200 ) );
    setSizeGripEnabled( TRUE );
    SQ_OptionsLayout = new QGridLayout( this, 1, 1, 11, 6, "SQ_OptionsLayout"); 

    listMain = new QListView( this, "listMain" );
    listMain->addColumn( tr2i18n( "Column 1" ) );
    listMain->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, listMain->sizePolicy().hasHeightForWidth() ) );
    listMain->setFrameShape( QListView::StyledPanel );
    listMain->setFrameShadow( QListView::Sunken );

    SQ_OptionsLayout->addWidget( listMain, 0, 0 );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    layout1->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 336, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    layout1->addWidget( buttonCancel );

    SQ_OptionsLayout->addMultiCellLayout( layout1, 1, 1, 0, 1 );

    widgetStack1 = new QWidgetStack( this, "widgetStack1" );
    widgetStack1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, widgetStack1->sizePolicy().hasHeightForWidth() ) );
    widgetStack1->setFrameShape( QWidgetStack::Box );
    widgetStack1->setFrameShadow( QWidgetStack::Raised );

    page = new QWidget( widgetStack1, "page" );

    checkSplash = new QCheckBox( page, "checkSplash" );
    checkSplash->setGeometry( QRect( 10, 15, 265, 20 ) );

    checkRestart = new QCheckBox( page, "checkRestart" );
    checkRestart->setGeometry( QRect( 10, 40, 265, 20 ) );
    checkRestart->setChecked( FALSE );

    checkMinimize = new QCheckBox( page, "checkMinimize" );
    checkMinimize->setGeometry( QRect( 10, 65, 265, 20 ) );
    widgetStack1->addWidget( page, 0 );

    page_2 = new QWidget( widgetStack1, "page_2" );

    textLabel1_3 = new QLabel( page_2, "textLabel1_3" );
    textLabel1_3->setGeometry( QRect( 15, 300, 110, 16 ) );

    comboToolbarIconSize = new QComboBox( FALSE, page_2, "comboToolbarIconSize" );
    comboToolbarIconSize->setGeometry( QRect( 130, 300, 80, 25 ) );

    buttonGroupViewType = new QButtonGroup( page_2, "buttonGroupViewType" );
    buttonGroupViewType->setGeometry( QRect( 8, 8, 410, 165 ) );
    buttonGroupViewType->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupViewType->sizePolicy().hasHeightForWidth() ) );

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

    radioButton15 = new QRadioButton( buttonGroupCreateFirst, "radioButton15" );
    radioButton15->setGeometry( QRect( 10, 25, 80, 21 ) );
    buttonGroupCreateFirst->insert( radioButton15, 0 );

    radioButton14 = new QRadioButton( buttonGroupCreateFirst, "radioButton14" );
    radioButton14->setGeometry( QRect( 10, 45, 86, 21 ) );
    buttonGroupCreateFirst->insert( radioButton14, 1 );

    radioButton16 = new QRadioButton( buttonGroupCreateFirst, "radioButton16" );
    radioButton16->setGeometry( QRect( 10, 65, 106, 21 ) );
    widgetStack1->addWidget( page_2, 1 );

    page_3 = new QWidget( widgetStack1, "page_3" );
    pageLayout = new QGridLayout( page_3, 1, 1, 11, 6, "pageLayout"); 

    buttonGroupSetPath = new QButtonGroup( page_3, "buttonGroupSetPath" );
    buttonGroupSetPath->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupSetPath->sizePolicy().hasHeightForWidth() ) );
    buttonGroupSetPath->setColumnLayout(0, Qt::Vertical );
    buttonGroupSetPath->layout()->setSpacing( 6 );
    buttonGroupSetPath->layout()->setMargin( 11 );
    buttonGroupSetPathLayout = new QGridLayout( buttonGroupSetPath->layout() );
    buttonGroupSetPathLayout->setAlignment( Qt::AlignTop );

    radioSetLast = new QRadioButton( buttonGroupSetPath, "radioSetLast" );
    buttonGroupSetPath->insert( radioSetLast, 0 );

    buttonGroupSetPathLayout->addWidget( radioSetLast, 0, 0 );

    radioSetCurrent = new QRadioButton( buttonGroupSetPath, "radioSetCurrent" );
    buttonGroupSetPath->insert( radioSetCurrent, 1 );

    buttonGroupSetPathLayout->addWidget( radioSetCurrent, 1, 0 );

    radioSetThis = new QRadioButton( buttonGroupSetPath, "radioSetThis" );
    buttonGroupSetPath->insert( radioSetThis, 2 );

    buttonGroupSetPathLayout->addWidget( radioSetThis, 2, 0 );

    lineEditCustomDir = new QLineEdit( buttonGroupSetPath, "lineEditCustomDir" );
    lineEditCustomDir->setEnabled( FALSE );

    buttonGroupSetPathLayout->addWidget( lineEditCustomDir, 2, 1 );

    pushOpenDir = new QPushButton( buttonGroupSetPath, "pushOpenDir" );

    buttonGroupSetPathLayout->addWidget( pushOpenDir, 2, 2 );

    pageLayout->addMultiCellWidget( buttonGroupSetPath, 0, 0, 0, 4 );

    checkSyncTree = new QCheckBox( page_3, "checkSyncTree" );

    pageLayout->addMultiCellWidget( checkSyncTree, 3, 3, 0, 4 );

    textLabel2 = new QLabel( page_3, "textLabel2" );

    pageLayout->addMultiCellWidget( textLabel2, 2, 2, 0, 1 );

    checkRunUnknown = new QCheckBox( page_3, "checkRunUnknown" );

    pageLayout->addMultiCellWidget( checkRunUnknown, 4, 4, 0, 4 );

    textLabel1_2 = new QLabel( page_3, "textLabel1_2" );

    pageLayout->addMultiCellWidget( textLabel1_2, 1, 1, 0, 1 );

    comboIconIndex = new QComboBox( FALSE, page_3, "comboIconIndex" );
    comboIconIndex->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, comboIconIndex->sizePolicy().hasHeightForWidth() ) );
    comboIconIndex->setMaxCount( 20 );

    pageLayout->addMultiCellWidget( comboIconIndex, 1, 1, 2, 3 );

    comboListIndex = new QComboBox( FALSE, page_3, "comboListIndex" );
    comboListIndex->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, comboListIndex->sizePolicy().hasHeightForWidth() ) );

    pageLayout->addMultiCellWidget( comboListIndex, 2, 2, 2, 3 );
    QSpacerItem* spacer_2 = new QSpacerItem( 185, 16, QSizePolicy::Fixed, QSizePolicy::Minimum );
    pageLayout->addItem( spacer_2, 1, 4 );
    QSpacerItem* spacer_3 = new QSpacerItem( 185, 16, QSizePolicy::Fixed, QSizePolicy::Minimum );
    pageLayout->addItem( spacer_3, 2, 4 );
    QSpacerItem* spacer_4 = new QSpacerItem( 16, 40, QSizePolicy::Minimum, QSizePolicy::Minimum );
    pageLayout->addItem( spacer_4, 7, 4 );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 45, QSizePolicy::Fixed, QSizePolicy::Minimum );
    pageLayout->addItem( spacer_5, 6, 0 );

    checkClickSystem = new QCheckBox( page_3, "checkClickSystem" );

    pageLayout->addMultiCellWidget( checkClickSystem, 5, 5, 0, 4 );

    buttonGroupClickPolicy = new QButtonGroup( page_3, "buttonGroupClickPolicy" );
    buttonGroupClickPolicy->setEnabled( FALSE );
    buttonGroupClickPolicy->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, buttonGroupClickPolicy->sizePolicy().hasHeightForWidth() ) );
    buttonGroupClickPolicy->setFrameShape( QButtonGroup::NoFrame );
    buttonGroupClickPolicy->setFrameShadow( QButtonGroup::Sunken );

    radioButton18 = new QRadioButton( buttonGroupClickPolicy, "radioButton18" );
    radioButton18->setGeometry( QRect( 0, 20, 161, 21 ) );
    buttonGroupClickPolicy->insert( radioButton18, 1 );

    radioButton17 = new QRadioButton( buttonGroupClickPolicy, "radioButton17" );
    radioButton17->setGeometry( QRect( 0, 0, 157, 21 ) );
    radioButton17->setChecked( FALSE );
    buttonGroupClickPolicy->insert( radioButton17, 0 );

    pageLayout->addMultiCellWidget( buttonGroupClickPolicy, 6, 6, 1, 2 );
    QSpacerItem* spacer_6 = new QSpacerItem( 270, 40, QSizePolicy::Expanding, QSizePolicy::Minimum );
    pageLayout->addMultiCell( spacer_6, 6, 6, 3, 4 );
    widgetStack1->addWidget( page_3, 2 );

    page_4 = new QWidget( widgetStack1, "page_4" );
    pageLayout_2 = new QGridLayout( page_4, 1, 1, 11, 6, "pageLayout_2"); 

    textPrefix = new QLabel( page_4, "textPrefix" );
    textPrefix->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textPrefix->sizePolicy().hasHeightForWidth() ) );

    pageLayout_2->addWidget( textPrefix, 0, 1 );

    textLabel1 = new QLabel( page_4, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    pageLayout_2->addWidget( textLabel1, 0, 0 );

    checkFAMDetectNew = new QCheckBox( page_4, "checkFAMDetectNew" );

    pageLayout_2->addMultiCellWidget( checkFAMDetectNew, 1, 1, 0, 1 );

    checkFAMMessage = new QCheckBox( page_4, "checkFAMMessage" );
    checkFAMMessage->setEnabled( FALSE );

    pageLayout_2->addMultiCellWidget( checkFAMMessage, 2, 2, 0, 1 );

    checkShowLinks = new QCheckBox( page_4, "checkShowLinks" );
    checkShowLinks->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, checkShowLinks->sizePolicy().hasHeightForWidth() ) );
    checkShowLinks->setChecked( TRUE );

    pageLayout_2->addMultiCellWidget( checkShowLinks, 3, 3, 0, 1 );

    tableLib = new QListView( page_4, "tableLib" );
    tableLib->setFrameShape( QListView::NoFrame );

    pageLayout_2->addMultiCellWidget( tableLib, 5, 5, 0, 1 );

    line1 = new QFrame( page_4, "line1" );
    line1->setPaletteForegroundColor( QColor( 38, 255, 0 ) );
    line1->setPaletteBackgroundColor( QColor( 68, 255, 0 ) );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    pageLayout_2->addMultiCellWidget( line1, 4, 4, 0, 1 );
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
    widgetStack1->addWidget( page_5, 4 );

    page_6 = new QWidget( widgetStack1, "page_6" );
    pageLayout_3 = new QGridLayout( page_6, 1, 1, 11, 6, "pageLayout_3"); 

    tabWidget2 = new QTabWidget( page_6, "tabWidget2" );

    tab = new QWidget( tabWidget2, "tab" );

    buttonGroupShadeModel = new QButtonGroup( tab, "buttonGroupShadeModel" );
    buttonGroupShadeModel->setGeometry( QRect( 15, 125, 200, 80 ) );

    radioSmooth_2 = new QRadioButton( buttonGroupShadeModel, "radioSmooth_2" );
    radioSmooth_2->setGeometry( QRect( 10, 50, 130, 20 ) );
    buttonGroupShadeModel->insert( radioSmooth_2, 1 );

    radioFlat_2 = new QRadioButton( buttonGroupShadeModel, "radioFlat_2" );
    radioFlat_2->setGeometry( QRect( 10, 25, 130, 20 ) );
    buttonGroupShadeModel->insert( radioFlat_2, 0 );

    checkDrop = new QCheckBox( tab, "checkDrop" );
    checkDrop->setGeometry( QRect( 13, 215, 200, 21 ) );

    textLabel1_5 = new QLabel( tab, "textLabel1_5" );
    textLabel1_5->setGeometry( QRect( 15, 30, 75, 16 ) );

    checkAdjust = new QCheckBox( tab, "checkAdjust" );
    checkAdjust->setGeometry( QRect( 15, 80, 200, 20 ) );

    kColorGLbackground = new KColorButton( tab, "kColorGLbackground" );
    kColorGLbackground->setGeometry( QRect( 100, 20, 95, 35 ) );
    tabWidget2->insertTab( tab, "" );

    tab_2 = new QWidget( tabWidget2, "tab_2" );

    buttonGroupZoomModel = new QButtonGroup( tab_2, "buttonGroupZoomModel" );
    buttonGroupZoomModel->setGeometry( QRect( 10, 15, 200, 80 ) );

    radioLinear_2_2 = new QRadioButton( buttonGroupZoomModel, "radioLinear_2_2" );
    radioLinear_2_2->setGeometry( QRect( 10, 25, 130, 20 ) );
    buttonGroupZoomModel->insert( radioLinear_2_2, 0 );

    radioNearest_2_2 = new QRadioButton( buttonGroupZoomModel, "radioNearest_2_2" );
    radioNearest_2_2->setGeometry( QRect( 10, 50, 130, 20 ) );
    radioNearest_2_2->setChecked( FALSE );
    buttonGroupZoomModel->insert( radioNearest_2_2, 1 );

    checkZoomLock = new QCheckBox( tab_2, "checkZoomLock" );
    checkZoomLock->setGeometry( QRect( 8, 110, 200, 21 ) );

    textLabel1_4 = new QLabel( tab_2, "textLabel1_4" );
    textLabel1_4->setGeometry( QRect( 15, 145, 120, 25 ) );

    spinZoomFactor = new QSpinBox( tab_2, "spinZoomFactor" );
    spinZoomFactor->setGeometry( QRect( 140, 145, 65, 25 ) );
    spinZoomFactor->setMaxValue( 100 );
    spinZoomFactor->setValue( 25 );
    tabWidget2->insertTab( tab_2, "" );

    pageLayout_3->addWidget( tabWidget2, 0, 0 );
    widgetStack1->addWidget( page_6, 5 );

    SQ_OptionsLayout->addWidget( widgetStack1, 0, 1 );
    languageChange();
    resize( QSize(632, 468).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonGroupViewType, SIGNAL( clicked(int) ), this, SLOT( slotSetViewPixmap(int) ) );
    connect( pushOpenDir, SIGNAL( clicked() ), this, SLOT( slotOpenDir() ) );
    connect( checkCacheDisk, SIGNAL( toggled(bool) ), lineCachePath, SLOT( setEnabled(bool) ) );
    connect( pushOpenCacheDir, SIGNAL( released() ), this, SLOT( slotDirCache() ) );
    connect( radioSetThis, SIGNAL( toggled(bool) ), lineEditCustomDir, SLOT( setEnabled(bool) ) );
    connect( checkFAMDetectNew, SIGNAL( toggled(bool) ), checkFAMMessage, SLOT( setEnabled(bool) ) );
    connect( checkShowLinks, SIGNAL( toggled(bool) ), this, SLOT( slotShowLinks(bool) ) );
    connect( checkClickSystem, SIGNAL( toggled(bool) ), buttonGroupClickPolicy, SLOT( setEnabled(bool) ) );
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
    buttonHelp->setText( tr2i18n( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr2i18n( "F1" ) ) );
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
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
    radioButton15->setText( tr2i18n( "List View" ) );
    radioButton14->setText( tr2i18n( "Icon View" ) );
    radioButton16->setText( tr2i18n( "Detailed View" ) );
    buttonGroupSetPath->setTitle( tr2i18n( "Set path ..." ) );
    QToolTip::add( buttonGroupSetPath, QString::null );
    radioSetLast->setText( tr2i18n( "Set last" ) );
    radioSetCurrent->setText( tr2i18n( "Set current" ) );
    radioSetThis->setText( tr2i18n( "Set this" ) );
    lineEditCustomDir->setText( QString::null );
    pushOpenDir->setText( tr2i18n( "Open" ) );
    checkSyncTree->setText( tr2i18n( "<NI>Syncronize Tree i FIleView" ) );
    QToolTip::add( checkSyncTree, tr2i18n( "TreeView and FileView will have common path" ) );
    textLabel2->setText( tr2i18n( "default icon size for \"List View\"" ) );
    checkRunUnknown->setText( tr2i18n( "<NI>Run unknown file formats separately" ) );
    QToolTip::add( checkRunUnknown, tr2i18n( "For example, run unsupported \"png\" format in kuickshow" ) );
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
    checkClickSystem->setText( tr2i18n( "Use system settings for clicking policy" ) );
    QToolTip::add( checkClickSystem, tr2i18n( "if KDE uses single click to open item, SQuirrel will have too" ) );
    buttonGroupClickPolicy->setTitle( QString::null );
    radioButton18->setText( tr2i18n( "Use double click policy" ) );
    radioButton17->setText( tr2i18n( "Use single click policy" ) );
    textPrefix->setText( QString::null );
    QToolTip::add( textPrefix, tr2i18n( "Where SQuirrel should search libraries" ) );
    textLabel1->setText( tr2i18n( "$PREFIX:" ) );
    QToolTip::add( textLabel1, tr2i18n( "Where SQuirrel should search libraries" ) );
    checkFAMDetectNew->setText( tr2i18n( "<NI>detect new libraries automatically" ) );
    checkFAMMessage->setText( tr2i18n( "<NI>Show message" ) );
    checkShowLinks->setText( tr2i18n( "show 'symlink' libraries" ) );
    QToolTip::add( checkShowLinks, tr2i18n( "If not checked, this table will contain only real .so files, not symlinks" ) );
    QToolTip::add( tableLib, tr2i18n( "Contains information about all found libraries" ) );
    checkCacheDisk->setText( tr2i18n( "<NI>Cache on disk" ) );
    checkCacheGL->setText( tr2i18n( "<NI>Cache in OpenGL textures" ) );
    pushOpenCacheDir->setText( tr2i18n( "Open" ) );
    buttonGroupShadeModel->setTitle( tr2i18n( "Shade Model" ) );
    radioSmooth_2->setText( tr2i18n( "GL_SMOOTH" ) );
    radioFlat_2->setText( tr2i18n( "GL_FLAT" ) );
    checkDrop->setText( tr2i18n( "Enable drop" ) );
    textLabel1_5->setText( tr2i18n( "Background:" ) );
    checkAdjust->setText( tr2i18n( "<NI>adjust image" ) );
    QToolTip::add( checkAdjust, tr2i18n( "Adjust image to fit free space" ) );
    kColorGLbackground->setText( QString::null );
    QToolTip::add( kColorGLbackground, tr2i18n( "GLView will have such background color" ) );
    tabWidget2->changeTab( tab, tr2i18n( "Main" ) );
    buttonGroupZoomModel->setTitle( tr2i18n( "Zoom Model" ) );
    radioLinear_2_2->setText( tr2i18n( "GL_LINEAR" ) );
    radioNearest_2_2->setText( tr2i18n( "GL_NEAREST" ) );
    checkZoomLock->setText( tr2i18n( "<NI>Lock zoom" ) );
    textLabel1_4->setText( tr2i18n( "<NI>Zoom factor:" ) );
    spinZoomFactor->setSuffix( tr2i18n( " %" ) );
    spinZoomFactor->setSpecialValueText( tr2i18n( "None" ) );
    tabWidget2->changeTab( tab_2, tr2i18n( "Zoom" ) );
}

#include "sq_options.moc"
