#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_editpicture.ui'
**
** Created: Сбт Янв 24 15:51:36 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_editpicture.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/* 
 *  Constructs a EditPicture as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
EditPicture::EditPicture( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "EditPicture" );
    setSizeGripEnabled( TRUE );
    EditPictureLayout = new QVBoxLayout( this, 7, 8, "EditPictureLayout"); 

    tabWidget = new QTabWidget( this, "tabWidget" );
    tabWidget->setFocusPolicy( QTabWidget::NoFocus );
    tabWidget->setTabPosition( QTabWidget::Top );

    Widget9 = new QWidget( tabWidget, "Widget9" );
    tabWidget->insertTab( Widget9, "" );

    Widget8 = new QWidget( tabWidget, "Widget8" );
    tabWidget->insertTab( Widget8, "" );
    EditPictureLayout->addWidget( tabWidget );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );
    EditPictureLayout->addLayout( Layout1 );
    languageChange();
    resize( QSize(528, 368).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
EditPicture::~EditPicture()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void EditPicture::languageChange()
{
    setCaption( tr2i18n( "Standart edition of picture" ) );
    tabWidget->changeTab( Widget9, tr2i18n( "Resize" ) );
    tabWidget->changeTab( Widget8, tr2i18n( "Convert to ..." ) );
    buttonHelp->setText( tr2i18n( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr2i18n( "F1" ) ) );
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

#include "sq_editpicture.moc"
