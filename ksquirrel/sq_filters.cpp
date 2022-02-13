#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_filters.ui'
**
** Created: Срд Май 5 19:50:18 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_filters.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "ksquirrel.h"
#include "sq_config.h"
#include "sq_fileviewfilter.h"
#include "./sq_filters.ui.h"

/* 
 *  Constructs a SQ_Filters as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SQ_Filters::SQ_Filters( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "SQ_Filters" );
    setSizeGripEnabled( TRUE );
    SQ_FiltersLayout = new QGridLayout( this, 1, 1, 11, 6, "SQ_FiltersLayout"); 

    pushOK = new QPushButton( this, "pushOK" );
    pushOK->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, pushOK->sizePolicy().hasHeightForWidth() ) );
    pushOK->setDefault( TRUE );

    SQ_FiltersLayout->addWidget( pushOK, 1, 2 );

    pushCancel = new QPushButton( this, "pushCancel" );
    pushCancel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, pushCancel->sizePolicy().hasHeightForWidth() ) );

    SQ_FiltersLayout->addWidget( pushCancel, 1, 3 );
    QSpacerItem* spacer = new QSpacerItem( 155, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    SQ_FiltersLayout->addItem( spacer, 1, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 185, 35, QSizePolicy::Minimum, QSizePolicy::Fixed );
    SQ_FiltersLayout->addItem( spacer_2, 1, 0 );

    frame3 = new QFrame( this, "frame3" );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );
    frame3Layout = new QGridLayout( frame3, 1, 1, 11, 6, "frame3Layout"); 

    pushNewFilter = new QPushButton( frame3, "pushNewFilter" );

    frame3Layout->addWidget( pushNewFilter, 3, 0 );

    pushFilterClear = new QPushButton( frame3, "pushFilterClear" );

    frame3Layout->addWidget( pushFilterClear, 3, 1 );

    pushFilterClearAll = new QPushButton( frame3, "pushFilterClearAll" );

    frame3Layout->addWidget( pushFilterClearAll, 3, 2 );

    listFilters = new QListView( frame3, "listFilters" );
    listFilters->addColumn( tr2i18n( "Name" ) );
    listFilters->addColumn( tr2i18n( "Extensions" ) );
    listFilters->setResizePolicy( QScrollView::Manual );
    listFilters->setSelectionMode( QListView::Single );
    listFilters->setAllColumnsShowFocus( TRUE );
    listFilters->setShowSortIndicator( FALSE );
    listFilters->setItemMargin( 1 );
    listFilters->setResizeMode( QListView::AllColumns );
    listFilters->setDefaultRenameAction( QListView::Reject );

    frame3Layout->addMultiCellWidget( listFilters, 0, 2, 0, 2 );

    pushFilterDown = new QPushButton( frame3, "pushFilterDown" );
    pushFilterDown->setEnabled( TRUE );
    pushFilterDown->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pushFilterDown->sizePolicy().hasHeightForWidth() ) );
    pushFilterDown->setMinimumSize( QSize( 22, 22 ) );

    frame3Layout->addWidget( pushFilterDown, 1, 3 );

    pushFilterUp = new QPushButton( frame3, "pushFilterUp" );
    pushFilterUp->setEnabled( TRUE );
    pushFilterUp->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pushFilterUp->sizePolicy().hasHeightForWidth() ) );
    pushFilterUp->setMinimumSize( QSize( 22, 22 ) );

    frame3Layout->addWidget( pushFilterUp, 0, 3 );
    QSpacerItem* spacer_3 = new QSpacerItem( 30, 180, QSizePolicy::Minimum, QSizePolicy::Expanding );
    frame3Layout->addMultiCell( spacer_3, 2, 3, 3, 3 );

    checkBoth = new QCheckBox( frame3, "checkBoth" );

    frame3Layout->addMultiCellWidget( checkBoth, 4, 4, 0, 1 );

    SQ_FiltersLayout->addMultiCellWidget( frame3, 0, 0, 0, 3 );
    languageChange();
    resize( QSize(536, 428).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( pushNewFilter, SIGNAL( clicked() ), this, SLOT( slotNewFilter() ) );
    connect( pushFilterClear, SIGNAL( clicked() ), this, SLOT( slotFilterClear() ) );
    connect( pushFilterClearAll, SIGNAL( clicked() ), listFilters, SLOT( clear() ) );
    connect( pushFilterUp, SIGNAL( clicked() ), this, SLOT( slotFilterUp() ) );
    connect( pushFilterDown, SIGNAL( clicked() ), this, SLOT( slotFilterDown() ) );
    connect( listFilters, SIGNAL( rightButtonClicked(QListViewItem*,const QPoint&,int) ), this, SLOT( slotFilterRenameRequest(QListViewItem*,const QPoint&,int) ) );
    connect( pushOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
SQ_Filters::~SQ_Filters()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SQ_Filters::languageChange()
{
    setCaption( tr2i18n( "Adjust Filters" ) );
    pushOK->setText( tr2i18n( " OK " ) );
    pushCancel->setText( tr2i18n( "Cancel" ) );
    pushNewFilter->setText( tr2i18n( "New filter" ) );
    pushFilterClear->setText( tr2i18n( "Delete filter" ) );
    pushFilterClearAll->setText( tr2i18n( "Clear All" ) );
    listFilters->header()->setLabel( 0, tr2i18n( "Name" ) );
    listFilters->header()->setLabel( 1, tr2i18n( "Extensions" ) );
    pushFilterDown->setText( QString::null );
    pushFilterUp->setText( QString::null );
    checkBoth->setText( tr2i18n( "Menuitem contains both name and extension" ) );
}

#include "sq_filters.moc"
