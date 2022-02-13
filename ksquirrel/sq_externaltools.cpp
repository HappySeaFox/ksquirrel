#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_externaltools.ui'
**
** Created: Втр Апр 6 01:48:42 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_externaltools.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "ksquirrel.h"
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

    listNames = new QListBox( this, "listNames" );
    listNames->setFrameShape( QListBox::LineEditPanel );

    SQ_ExternalToolsLayout->addMultiCellWidget( listNames, 1, 4, 0, 0 );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    SQ_ExternalToolsLayout->addWidget( textLabel1, 0, 0 );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, textLabel2->sizePolicy().hasHeightForWidth() ) );

    SQ_ExternalToolsLayout->addMultiCellWidget( textLabel2, 0, 0, 1, 2 );

    lineAction = new QLineEdit( this, "lineAction" );

    SQ_ExternalToolsLayout->addMultiCellWidget( lineAction, 1, 1, 1, 2 );

    pushTestAll = new QPushButton( this, "pushTestAll" );

    SQ_ExternalToolsLayout->addMultiCellWidget( pushTestAll, 2, 2, 1, 2 );
    QSpacerItem* spacer = new QSpacerItem( 65, 55, QSizePolicy::Fixed, QSizePolicy::Minimum );
    SQ_ExternalToolsLayout->addMultiCell( spacer, 1, 2, 3, 3 );
    QSpacerItem* spacer_2 = new QSpacerItem( 215, 56, QSizePolicy::Minimum, QSizePolicy::Fixed );
    SQ_ExternalToolsLayout->addMultiCell( spacer_2, 3, 3, 1, 3 );

    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setColumnLayout(0, Qt::Vertical );
    buttonGroup1->layout()->setSpacing( 6 );
    buttonGroup1->layout()->setMargin( 11 );
    buttonGroup1Layout = new QGridLayout( buttonGroup1->layout() );
    buttonGroup1Layout->setAlignment( Qt::AlignTop );

    lineNewName = new QLineEdit( buttonGroup1, "lineNewName" );

    buttonGroup1Layout->addWidget( lineNewName, 0, 1 );

    textLabel1_2 = new QLabel( buttonGroup1, "textLabel1_2" );

    buttonGroup1Layout->addWidget( textLabel1_2, 0, 0 );

    textLabel2_2 = new QLabel( buttonGroup1, "textLabel2_2" );

    buttonGroup1Layout->addWidget( textLabel2_2, 1, 0 );

    lineNewProgram = new QLineEdit( buttonGroup1, "lineNewProgram" );

    buttonGroup1Layout->addWidget( lineNewProgram, 1, 1 );

    pushAddNew = new QPushButton( buttonGroup1, "pushAddNew" );

    buttonGroup1Layout->addMultiCellWidget( pushAddNew, 2, 2, 0, 1 );

    SQ_ExternalToolsLayout->addMultiCellWidget( buttonGroup1, 4, 4, 1, 3 );
    QSpacerItem* spacer_3 = new QSpacerItem( 145, 16, QSizePolicy::Minimum, QSizePolicy::Fixed );
    SQ_ExternalToolsLayout->addItem( spacer_3, 5, 0 );

    pushButton3 = new QPushButton( this, "pushButton3" );
    pushButton3->setDefault( TRUE );

    SQ_ExternalToolsLayout->addMultiCellWidget( pushButton3, 5, 5, 2, 3 );
    QSpacerItem* spacer_4 = new QSpacerItem( 125, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    SQ_ExternalToolsLayout->addItem( spacer_4, 5, 1 );
    languageChange();
    resize( QSize(410, 303).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( pushTestAll, SIGNAL( clicked() ), this, SLOT( slotTestAll() ) );
    connect( pushButton3, SIGNAL( clicked() ), this, SLOT( accept() ) );
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
    setCaption( tr2i18n( "<NI>Configure external tools" ) );
    listNames->clear();
    listNames->insertItem( tr2i18n( "Open with GIMP" ) );
    listNames->insertItem( tr2i18n( "Open with konqueror" ) );
    textLabel1->setText( tr2i18n( "Name" ) );
    textLabel2->setText( tr2i18n( "Action" ) );
    lineAction->setText( tr2i18n( "gimp-remote %s" ) );
    pushTestAll->setText( tr2i18n( "Test all" ) );
    buttonGroup1->setTitle( tr2i18n( "Add new tool" ) );
    textLabel1_2->setText( tr2i18n( "Name:" ) );
    textLabel2_2->setText( tr2i18n( "Program:" ) );
    pushAddNew->setText( tr2i18n( "Add" ) );
    pushButton3->setText( tr2i18n( "OK" ) );
}

#include "sq_externaltools.moc"
