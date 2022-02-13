#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_options.ui'
**
** Created: Fri Mar 19 17:40:12 2004
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

    buttonGroupViewType = new QButtonGroup( page_2, "buttonGroupViewType" );
    buttonGroupViewType->setGeometry( QRect( 8, 8, 410, 160 ) );

    pixmapShowView = new QLabel( buttonGroupViewType, "pixmapShowView" );
    pixmapShowView->setGeometry( QRect( 155, 20, 241, 130 ) );
    pixmapShowView->setScaledContents( FALSE );

    radioSQuirrel = new QRadioButton( buttonGroupViewType, "radioSQuirrel" );
    radioSQuirrel->setGeometry( QRect( 15, 35, 120, 20 ) );
    radioSQuirrel->setChecked( FALSE );
    buttonGroupViewType->insert( radioSQuirrel, 0 );

    radioGQview = new QRadioButton( buttonGroupViewType, "radioGQview" );
    radioGQview->setGeometry( QRect( 15, 55, 120, 20 ) );
    buttonGroupViewType->insert( radioGQview, 1 );

    radioKuickShow = new QRadioButton( buttonGroupViewType, "radioKuickShow" );
    radioKuickShow->setGeometry( QRect( 15, 75, 120, 20 ) );
    buttonGroupViewType->insert( radioKuickShow, 2 );

    radioWinViewer = new QRadioButton( buttonGroupViewType, "radioWinViewer" );
    radioWinViewer->setGeometry( QRect( 15, 95, 120, 20 ) );
    buttonGroupViewType->insert( radioWinViewer, 3 );

    radioXnview = new QRadioButton( buttonGroupViewType, "radioXnview" );
    radioXnview->setGeometry( QRect( 15, 115, 120, 20 ) );
    buttonGroupViewType->insert( radioXnview, 4 );

    textLabel1_3 = new QLabel( page_2, "textLabel1_3" );
    textLabel1_3->setGeometry( QRect( 15, 190, 110, 16 ) );

    comboToolbarIconSize = new QComboBox( FALSE, page_2, "comboToolbarIconSize" );
    comboToolbarIconSize->setGeometry( QRect( 130, 190, 80, 25 ) );
    widgetStack1->addWidget( page_2, 1 );

    page_3 = new QWidget( widgetStack1, "page_3" );

    textLabel1_2 = new QLabel( page_3, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 15, 125, 184, 15 ) );

    textLabel2 = new QLabel( page_3, "textLabel2" );
    textLabel2->setGeometry( QRect( 15, 165, 180, 15 ) );

    buttonGroupSetPath = new QButtonGroup( page_3, "buttonGroupSetPath" );
    buttonGroupSetPath->setGeometry( QRect( 11, 7, 470, 105 ) );

    lineEditCustomDir = new QLineEdit( buttonGroupSetPath, "lineEditCustomDir" );
    lineEditCustomDir->setEnabled( FALSE );
    lineEditCustomDir->setGeometry( QRect( 105, 70, 290, 25 ) );

    pushOpenDir = new QPushButton( buttonGroupSetPath, "pushOpenDir" );
    pushOpenDir->setGeometry( QRect( 400, 70, 55, 26 ) );

    radioSetLast = new QRadioButton( buttonGroupSetPath, "radioSetLast" );
    radioSetLast->setGeometry( QRect( 10, 20, 90, 20 ) );

    radioSetCurrent = new QRadioButton( buttonGroupSetPath, "radioSetCurrent" );
    radioSetCurrent->setGeometry( QRect( 10, 45, 90, 20 ) );

    radioSetThis = new QRadioButton( buttonGroupSetPath, "radioSetThis" );
    radioSetThis->setGeometry( QRect( 10, 70, 90, 20 ) );

    checkSyncTree = new QCheckBox( page_3, "checkSyncTree" );
    checkSyncTree->setGeometry( QRect( 15, 210, 270, 20 ) );

    comboIconIndex = new QComboBox( FALSE, page_3, "comboIconIndex" );
    comboIconIndex->setGeometry( QRect( 210, 125, 80, 25 ) );
    comboIconIndex->setMaxCount( 20 );

    comboListIndex = new QComboBox( FALSE, page_3, "comboListIndex" );
    comboListIndex->setGeometry( QRect( 210, 165, 80, 25 ) );
    widgetStack1->addWidget( page_3, 2 );

    page_4 = new QWidget( widgetStack1, "page_4" );

    textPrefix = new QLabel( page_4, "textPrefix" );
    textPrefix->setGeometry( QRect( 75, 10, 385, 25 ) );
    textPrefix->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textPrefix->sizePolicy().hasHeightForWidth() ) );

    textLabel1 = new QLabel( page_4, "textLabel1" );
    textLabel1->setGeometry( QRect( 10, 10, 57, 25 ) );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    checkShowLinks = new QCheckBox( page_4, "checkShowLinks" );
    checkShowLinks->setGeometry( QRect( 10, 40, 185, 20 ) );
    checkShowLinks->setChecked( TRUE );

    tableLib = new QListView( page_4, "tableLib" );
    tableLib->setGeometry( QRect( 0, 70, 495, 305 ) );
    tableLib->setFrameShape( QListView::NoFrame );
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
    pageLayout = new QGridLayout( page_6, 1, 1, 11, 6, "pageLayout"); 

    tabWidget2 = new QTabWidget( page_6, "tabWidget2" );

    tab = new QWidget( tabWidget2, "tab" );

    checkAdjust = new QCheckBox( tab, "checkAdjust" );
    checkAdjust->setGeometry( QRect( 15, 80, 200, 20 ) );

    kColorGLbackground = new KColorButton( tab, "kColorGLbackground" );
    kColorGLbackground->setGeometry( QRect( 100, 20, 95, 35 ) );

    textLabel1_5 = new QLabel( tab, "textLabel1_5" );
    textLabel1_5->setGeometry( QRect( 15, 30, 75, 16 ) );

    buttonGroupShadeModel = new QButtonGroup( tab, "buttonGroupShadeModel" );
    buttonGroupShadeModel->setGeometry( QRect( 15, 125, 200, 80 ) );

    radioSmooth_2 = new QRadioButton( buttonGroupShadeModel, "radioSmooth_2" );
    radioSmooth_2->setGeometry( QRect( 10, 50, 130, 20 ) );
    buttonGroupShadeModel->insert( radioSmooth_2, 1 );

    radioFlat_2 = new QRadioButton( buttonGroupShadeModel, "radioFlat_2" );
    radioFlat_2->setGeometry( QRect( 10, 25, 130, 20 ) );
    buttonGroupShadeModel->insert( radioFlat_2, 0 );
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

    pageLayout->addWidget( tabWidget2, 0, 0 );
    widgetStack1->addWidget( page_6, 5 );

    SQ_OptionsLayout->addWidget( widgetStack1, 0, 1 );
    languageChange();
    resize( QSize(630, 443).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonGroupViewType, SIGNAL( clicked(int) ), this, SLOT( slotSetViewPixmap(int) ) );
    connect( pushOpenDir, SIGNAL( clicked() ), this, SLOT( slotOpenDir() ) );
    connect( checkCacheDisk, SIGNAL( toggled(bool) ), lineCachePath, SLOT( setEnabled(bool) ) );
    connect( pushOpenCacheDir, SIGNAL( released() ), this, SLOT( slotDirCache() ) );
    connect( checkShowLinks, SIGNAL( toggled(bool) ), this, SLOT( slotShowLinks(bool) ) );
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
    buttonGroupViewType->setTitle( tr2i18n( "Look like ..." ) );
    QToolTip::add( buttonGroupViewType, tr2i18n( "Change appearance" ) );
    radioSQuirrel->setText( tr2i18n( "SQuirrel" ) );
    QToolTip::add( radioSQuirrel, tr2i18n( "Like SQuirrel. Has TreeView, FileView, separate GLView" ) );
    radioGQview->setText( tr2i18n( "GQview" ) );
    QToolTip::add( radioGQview, tr2i18n( "Like GQview. Has FileView, built-in GLView" ) );
    radioKuickShow->setText( tr2i18n( "KuickShow" ) );
    QToolTip::add( radioKuickShow, tr2i18n( "Like Kuickshow. Has FileView, separate GLView" ) );
    radioWinViewer->setText( tr2i18n( "WinViewer" ) );
    QToolTip::add( radioWinViewer, tr2i18n( "Like Windows Picture and Fax viewer. Has only built-in GLView" ) );
    radioXnview->setText( tr2i18n( "XnView" ) );
    QToolTip::add( radioXnview, tr2i18n( "Like XnView. Has TreeView, FileView, built-in GLView" ) );
    textLabel1_3->setText( tr2i18n( "Toolbar icon size" ) );
    comboToolbarIconSize->clear();
    comboToolbarIconSize->insertItem( tr2i18n( "16" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "22" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "32" ) );
    comboToolbarIconSize->insertItem( tr2i18n( "48" ) );
    comboToolbarIconSize->setCurrentItem( 0 );
    textLabel1_2->setText( tr2i18n( "default icon size for \"Icon View\"" ) );
    textLabel2->setText( tr2i18n( "default icon size for \"List View\"" ) );
    buttonGroupSetPath->setTitle( tr2i18n( "<NI>Set path ..." ) );
    QToolTip::add( buttonGroupSetPath, QString::null );
    lineEditCustomDir->setText( QString::null );
    pushOpenDir->setText( tr2i18n( "Open" ) );
    radioSetLast->setText( tr2i18n( "Set last" ) );
    radioSetCurrent->setText( tr2i18n( "Set current" ) );
    radioSetThis->setText( tr2i18n( "Set this" ) );
    checkSyncTree->setText( tr2i18n( "<NI>Syncronize Tree i FIleView" ) );
    QToolTip::add( checkSyncTree, tr2i18n( "Change path in TreeVire and FileView <>" ) );
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
    textPrefix->setText( QString::null );
    QToolTip::add( textPrefix, tr2i18n( "Where SQuirrel should search libraries" ) );
    textLabel1->setText( tr2i18n( "$PREFIX:" ) );
    QToolTip::add( textLabel1, tr2i18n( "Where SQuirrel should search libraries" ) );
    checkShowLinks->setText( tr2i18n( "show 'symlink' libraries" ) );
    QToolTip::add( checkShowLinks, tr2i18n( "If not checked, this table will contain only real .so files, not symlinks" ) );
    QToolTip::add( tableLib, tr2i18n( "Contains information about all found libraries" ) );
    checkCacheDisk->setText( tr2i18n( "<NI>Cache on disk" ) );
    checkCacheGL->setText( tr2i18n( "<NI>Cache in OpenGL textures" ) );
    pushOpenCacheDir->setText( tr2i18n( "Open" ) );
    checkAdjust->setText( tr2i18n( "<NI>adjust image" ) );
    QToolTip::add( checkAdjust, tr2i18n( "Adjust image to fit free space" ) );
    kColorGLbackground->setText( QString::null );
    QToolTip::add( kColorGLbackground, tr2i18n( "GLView will have such background color" ) );
    textLabel1_5->setText( tr2i18n( "Background:" ) );
    buttonGroupShadeModel->setTitle( tr2i18n( "Shade Model" ) );
    radioSmooth_2->setText( tr2i18n( "GL_SMOOTH" ) );
    radioFlat_2->setText( tr2i18n( "GL_FLAT" ) );
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
