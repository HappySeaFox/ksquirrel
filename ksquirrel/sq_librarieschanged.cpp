#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_librarieschanged.ui'
**
** Created: Tue Mar 30 00:59:48 2004
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

    pushOK = new QPushButton( this, "pushOK" );
    pushOK->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pushOK->sizePolicy().hasHeightForWidth() ) );
    pushOK->setCursor( QCursor( 0 ) );

    SQ_LibrariesChangedLayout->addWidget( pushOK, 2, 1 );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    SQ_LibrariesChangedLayout->addWidget( textLabel1, 0, 0 );

    listNewLibs = new QListBox( this, "listNewLibs" );
    listNewLibs->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, listNewLibs->sizePolicy().hasHeightForWidth() ) );
    listNewLibs->setFrameShape( QListBox::LineEditPanel );
    listNewLibs->setFrameShadow( QListBox::Sunken );

    SQ_LibrariesChangedLayout->addMultiCellWidget( listNewLibs, 1, 1, 0, 1 );
    languageChange();
    resize( QSize(471, 302).expandedTo(minimumSizeHint()) );

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
    pushOK->setText( tr2i18n( "OK" ) );
    textLabel1->setText( tr2i18n( "Libs" ) );
    listNewLibs->setCurrentItem( -1 );
}

#include "sq_librarieschanged.moc"
