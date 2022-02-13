#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './klc.ui'
**
** Created: Птн Дек 7 20:18:10 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "klc.h"

#include <qvariant.h>
#include <kdirlister.h>
#include <kio/job.h>
#include <qheader.h>
#include <qtimer.h>
#include <kio/netaccess.h>
#include <kapplication.h>
#include <dcopclient.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "./klc.ui.h"

/*
 *  Constructs a KLC as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
KLC::KLC( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KLC" );
    KLCLayout = new QGridLayout( this, 1, 1, 11, 6, "KLCLayout"); 

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 
    spacer1 = new QSpacerItem( 371, 26, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem( spacer1 );

    pushApply = new QPushButton( this, "pushApply" );
    layout1->addWidget( pushApply );

    pushClose = new QPushButton( this, "pushClose" );
    layout1->addWidget( pushClose );

    KLCLayout->addMultiCellLayout( layout1, 2, 2, 0, 1 );

    listDisabled = new QListView( this, "listDisabled" );
    listDisabled->addColumn( tr2i18n( "..." ) );
    listDisabled->setEnabled( FALSE );
    listDisabled->setAllColumnsShowFocus( TRUE );
    listDisabled->setResizeMode( QListView::AllColumns );

    KLCLayout->addWidget( listDisabled, 1, 1 );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );

    KLCLayout->addWidget( textLabel1_2, 0, 1 );

    textLabel1 = new QLabel( this, "textLabel1" );

    KLCLayout->addWidget( textLabel1, 0, 0 );

    listEnabled = new QListView( this, "listEnabled" );
    listEnabled->addColumn( tr2i18n( "..." ) );
    listEnabled->setEnabled( FALSE );
    listEnabled->setAllColumnsShowFocus( TRUE );
    listEnabled->setResizeMode( QListView::AllColumns );

    KLCLayout->addWidget( listEnabled, 1, 0 );
    languageChange();
    resize( QSize(586, 425).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushApply, SIGNAL( clicked() ), this, SLOT( slotApply() ) );
    connect( pushClose, SIGNAL( clicked() ), this, SLOT( close() ) );

    // tab order
    setTabOrder( listEnabled, listDisabled );
    setTabOrder( listDisabled, pushApply );
    setTabOrder( pushApply, pushClose );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
KLC::~KLC()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KLC::languageChange()
{
    setCaption( tr2i18n( "Codec manager" ) );
    pushApply->setText( tr2i18n( "Apply" ) );
    pushClose->setText( tr2i18n( "Close" ) );
    listDisabled->header()->setLabel( 0, tr2i18n( "..." ) );
    textLabel1_2->setText( tr2i18n( "<b>Select codecs to enable:</b>" ) );
    textLabel1->setText( tr2i18n( "<b>Select codecs to disable:</b>" ) );
    listEnabled->header()->setLabel( 0, tr2i18n( "..." ) );
}

#include "klc.moc"
