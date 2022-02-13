#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_librarieschanged.ui'
**
** Created: Втр Апр 27 19:00:38 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_librarieschanged.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "./sq_librarieschanged.ui.h"

/* 
 *  Constructs a SQ_LibrariesChanged as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SQ_LibrariesChanged::SQ_LibrariesChanged( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "SQ_LibrariesChanged" );
    setCursor( QCursor( 0 ) );
    SQ_LibrariesChangedLayout = new QGridLayout( this, 1, 1, 11, 6, "SQ_LibrariesChangedLayout"); 

    listNewLibs = new QListBox( this, "listNewLibs" );
    listNewLibs->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, listNewLibs->sizePolicy().hasHeightForWidth() ) );
    listNewLibs->setFrameShape( QListBox::LineEditPanel );
    listNewLibs->setFrameShadow( QListBox::Sunken );

    SQ_LibrariesChangedLayout->addMultiCellWidget( listNewLibs, 1, 1, 0, 1 );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    SQ_LibrariesChangedLayout->addMultiCellWidget( textLabel1, 0, 0, 0, 1 );

    pushOK = new QPushButton( this, "pushOK" );
    pushOK->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pushOK->sizePolicy().hasHeightForWidth() ) );
    pushOK->setCursor( QCursor( 0 ) );
    pushOK->setDefault( TRUE );

    SQ_LibrariesChangedLayout->addWidget( pushOK, 2, 1 );
    QSpacerItem* spacer = new QSpacerItem( 345, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    SQ_LibrariesChangedLayout->addItem( spacer, 2, 0 );
    languageChange();
    resize( QSize(472, 322).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( pushOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SQ_LibrariesChanged::~SQ_LibrariesChanged()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SQ_LibrariesChanged::languageChange()
{
    setCaption( tr2i18n( "Libraries" ) );
    listNewLibs->setCurrentItem( -1 );
    textLabel1->setText( QString::null );
    pushOK->setText( tr2i18n( "OK" ) );
}

#include "sq_librarieschanged.moc"
