#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_filters.ui'
**
** Created: Птн Апр 16 19:42:23 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_filters.h"

#include <qvariant.h>
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

    pushCancel = new QPushButton( this, "pushCancel" );
    pushCancel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, pushCancel->sizePolicy().hasHeightForWidth() ) );

    SQ_FiltersLayout->addWidget( pushCancel, 1, 2 );
    QSpacerItem* spacer = new QSpacerItem( 65, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    SQ_FiltersLayout->addItem( spacer, 1, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 185, 35, QSizePolicy::Minimum, QSizePolicy::Fixed );
    SQ_FiltersLayout->addItem( spacer_2, 1, 0 );

    pushOK = new QPushButton( this, "pushOK" );
    pushOK->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, pushOK->sizePolicy().hasHeightForWidth() ) );
    pushOK->setDefault( TRUE );

    SQ_FiltersLayout->addWidget( pushOK, 1, 3 );

    frame3 = new QFrame( this, "frame3" );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );
    frame3Layout = new QGridLayout( frame3, 1, 1, 11, 6, "frame3Layout"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 30, 25, QSizePolicy::Minimum, QSizePolicy::Fixed );
    frame3Layout->addItem( spacer_3, 2, 3 );

    listFilters = new QListView( frame3, "listFilters" );
    listFilters->addColumn( tr2i18n( "Name" ) );
    listFilters->addColumn( tr2i18n( "Extensions" ) );
    listFilters->setResizePolicy( QListView::Default );
    listFilters->setSelectionMode( QListView::Single );
    listFilters->setAllColumnsShowFocus( TRUE );
    listFilters->setShowSortIndicator( FALSE );
    listFilters->setItemMargin( 1 );
    listFilters->setResizeMode( QListView::AllColumns );
    listFilters->setDefaultRenameAction( QListView::Reject );

    frame3Layout->addMultiCellWidget( listFilters, 0, 1, 0, 2 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Fixed );
    frame3Layout->addItem( spacer_4, 0, 3 );

    pushNewFilter = new QPushButton( frame3, "pushNewFilter" );

    frame3Layout->addWidget( pushNewFilter, 2, 0 );

    pushFilterClear = new QPushButton( frame3, "pushFilterClear" );

    frame3Layout->addWidget( pushFilterClear, 2, 1 );

    pushFilterClearAll = new QPushButton( frame3, "pushFilterClearAll" );

    frame3Layout->addWidget( pushFilterClearAll, 2, 2 );

    layout4 = new QGridLayout( 0, 1, 1, 0, 6, "layout4"); 
    QSpacerItem* spacer_5 = new QSpacerItem( 16, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout4->addItem( spacer_5, 1, 0 );

    pushFilterUp = new QPushButton( frame3, "pushFilterUp" );
    pushFilterUp->setEnabled( TRUE );
    pushFilterUp->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)1, 0, 0, pushFilterUp->sizePolicy().hasHeightForWidth() ) );

    layout4->addWidget( pushFilterUp, 0, 0 );

    pushFilterDown = new QPushButton( frame3, "pushFilterDown" );
    pushFilterDown->setEnabled( TRUE );
    pushFilterDown->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)1, 0, 0, pushFilterDown->sizePolicy().hasHeightForWidth() ) );

    layout4->addWidget( pushFilterDown, 2, 0 );

    frame3Layout->addLayout( layout4, 1, 3 );

    SQ_FiltersLayout->addMultiCellWidget( frame3, 0, 0, 0, 3 );
    languageChange();
    resize( QSize(536, 404).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( pushNewFilter, SIGNAL( clicked() ), this, SLOT( slotNewFilter() ) );
    connect( pushFilterClear, SIGNAL( clicked() ), this, SLOT( slotFilterClear() ) );
    connect( pushFilterClearAll, SIGNAL( clicked() ), this, SLOT( slotFilterClearAll() ) );
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
    pushCancel->setText( tr2i18n( "Cancel" ) );
    pushOK->setText( tr2i18n( " OK " ) );
    listFilters->header()->setLabel( 0, tr2i18n( "Name" ) );
    listFilters->header()->setLabel( 1, tr2i18n( "Extensions" ) );
    pushNewFilter->setText( tr2i18n( "New filter" ) );
    pushFilterClear->setText( tr2i18n( "Delete filter" ) );
    pushFilterClearAll->setText( tr2i18n( "Clear All" ) );
    pushFilterUp->setText( QString::null );
    pushFilterDown->setText( QString::null );
}

#include "sq_filters.moc"
