#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_externaltools.ui'
**
** Created: Птн Апр 16 21:24:10 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_externaltools.h"

#include <qvariant.h>
#include <qframe.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_externaltool.h"
#include "./sq_externaltools.ui.h"

/* 
 *  Constructs a SQ_ExternalTools as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SQ_ExternalTools::SQ_ExternalTools( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "SQ_ExternalTools" );
    setSizeGripEnabled( TRUE );
    SQ_ExternalToolsLayout = new QGridLayout( this, 1, 1, 11, 6, "SQ_ExternalToolsLayout"); 

    pushCancel = new QPushButton( this, "pushCancel" );
    pushCancel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, pushCancel->sizePolicy().hasHeightForWidth() ) );

    SQ_ExternalToolsLayout->addWidget( pushCancel, 1, 2 );
    QSpacerItem* spacer = new QSpacerItem( 65, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    SQ_ExternalToolsLayout->addItem( spacer, 1, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 185, 35, QSizePolicy::Minimum, QSizePolicy::Fixed );
    SQ_ExternalToolsLayout->addItem( spacer_2, 1, 0 );

    pushOK = new QPushButton( this, "pushOK" );
    pushOK->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, pushOK->sizePolicy().hasHeightForWidth() ) );
    pushOK->setDefault( TRUE );

    SQ_ExternalToolsLayout->addWidget( pushOK, 1, 3 );

    frame3 = new QFrame( this, "frame3" );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );
    frame3Layout = new QGridLayout( frame3, 1, 1, 11, 6, "frame3Layout"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 30, 25, QSizePolicy::Minimum, QSizePolicy::Fixed );
    frame3Layout->addItem( spacer_3, 2, 3 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Fixed );
    frame3Layout->addItem( spacer_4, 0, 3 );

    pushNewTool = new QPushButton( frame3, "pushNewTool" );

    frame3Layout->addWidget( pushNewTool, 2, 0 );

    pushToolClear = new QPushButton( frame3, "pushToolClear" );

    frame3Layout->addWidget( pushToolClear, 2, 1 );

    pushToolClearAll = new QPushButton( frame3, "pushToolClearAll" );

    frame3Layout->addWidget( pushToolClearAll, 2, 2 );

    layout4 = new QGridLayout( 0, 1, 1, 0, 6, "layout4"); 
    QSpacerItem* spacer_5 = new QSpacerItem( 16, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout4->addItem( spacer_5, 1, 0 );

    pushToolUp = new QPushButton( frame3, "pushToolUp" );
    pushToolUp->setEnabled( TRUE );
    pushToolUp->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)1, 0, 0, pushToolUp->sizePolicy().hasHeightForWidth() ) );

    layout4->addWidget( pushToolUp, 0, 0 );

    pushToolDown = new QPushButton( frame3, "pushToolDown" );
    pushToolDown->setEnabled( TRUE );
    pushToolDown->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)1, 0, 0, pushToolDown->sizePolicy().hasHeightForWidth() ) );

    layout4->addWidget( pushToolDown, 2, 0 );

    frame3Layout->addLayout( layout4, 1, 3 );

    listTools = new QListView( frame3, "listTools" );
    listTools->addColumn( tr2i18n( "Name" ) );
    listTools->addColumn( tr2i18n( "Extensions" ) );
    listTools->setResizePolicy( QScrollView::Manual );
    listTools->setSelectionMode( QListView::Single );
    listTools->setAllColumnsShowFocus( TRUE );
    listTools->setShowSortIndicator( FALSE );
    listTools->setItemMargin( 1 );
    listTools->setResizeMode( QListView::AllColumns );
    listTools->setDefaultRenameAction( QListView::Reject );

    frame3Layout->addMultiCellWidget( listTools, 1, 1, 0, 2 );

    textLabel1 = new QLabel( frame3, "textLabel1" );

    frame3Layout->addWidget( textLabel1, 0, 0 );

    SQ_ExternalToolsLayout->addMultiCellWidget( frame3, 0, 0, 0, 3 );
    languageChange();
    resize( QSize(555, 428).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( pushNewTool, SIGNAL( clicked() ), this, SLOT( slotNewTool() ) );
    connect( pushToolClear, SIGNAL( clicked() ), this, SLOT( slotToolClear() ) );
    connect( pushToolClearAll, SIGNAL( clicked() ), this, SLOT( slotToolClearAll() ) );
    connect( pushToolUp, SIGNAL( clicked() ), this, SLOT( slotToolUp() ) );
    connect( pushToolDown, SIGNAL( clicked() ), this, SLOT( slotToolDown() ) );
    connect( listTools, SIGNAL( rightButtonClicked(QListViewItem*,const QPoint&,int) ), this, SLOT( slotToolRenameRequest(QListViewItem*,const QPoint&,int) ) );
    connect( pushOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
SQ_ExternalTools::~SQ_ExternalTools()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SQ_ExternalTools::languageChange()
{
    setCaption( tr2i18n( "Adjust External Tools" ) );
    pushCancel->setText( tr2i18n( "Cancel" ) );
    pushOK->setText( tr2i18n( " OK " ) );
    pushNewTool->setText( tr2i18n( "New tool" ) );
    pushToolClear->setText( tr2i18n( "Delete tool" ) );
    pushToolClearAll->setText( tr2i18n( "Clear All" ) );
    pushToolUp->setText( QString::null );
    pushToolDown->setText( QString::null );
    listTools->header()->setLabel( 0, tr2i18n( "Name" ) );
    listTools->header()->setLabel( 1, tr2i18n( "Extensions" ) );
    textLabel1->setText( tr2i18n( "Hint: %s expands to file name" ) );
}

#include "sq_externaltools.moc"
