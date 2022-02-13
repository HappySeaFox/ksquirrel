#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_convertwidget.ui'
**
** Created: Tue Mar 30 01:46:06 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_convertwidget.h"

#include <qvariant.h>
#include <kfiledialog.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "./sq_convertwidget.ui.h"

/* 
 *  Constructs a SQ_ConvertWidget as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SQ_ConvertWidget::SQ_ConvertWidget( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "SQ_ConvertWidget" );
    setSizeGripEnabled( TRUE );
    SQ_ConvertWidgetLayout = new QGridLayout( this, 1, 1, 11, 6, "SQ_ConvertWidgetLayout"); 

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 

    listFiles = new QListBox( this, "listFiles" );
    listFiles->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, listFiles->sizePolicy().hasHeightForWidth() ) );
    listFiles->setSelectionMode( QListBox::Single );
    layout5->addWidget( listFiles );
    QSpacerItem* spacer = new QSpacerItem( 31, 16, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout5->addItem( spacer );

    listFormats = new QListBox( this, "listFormats" );
    listFormats->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, listFormats->sizePolicy().hasHeightForWidth() ) );
    layout5->addWidget( listFormats );

    SQ_ConvertWidgetLayout->addMultiCellLayout( layout5, 0, 0, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 16, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
    SQ_ConvertWidgetLayout->addItem( spacer_2, 1, 0 );

    buttonGroup4 = new QButtonGroup( this, "buttonGroup4" );
    buttonGroup4->setColumnLayout(0, Qt::Vertical );
    buttonGroup4->layout()->setSpacing( 6 );
    buttonGroup4->layout()->setMargin( 11 );
    buttonGroup4Layout = new QGridLayout( buttonGroup4->layout() );
    buttonGroup4Layout->setAlignment( Qt::AlignTop );
    QSpacerItem* spacer_3 = new QSpacerItem( 66, 21, QSizePolicy::Fixed, QSizePolicy::Minimum );
    buttonGroup4Layout->addItem( spacer_3, 3, 0 );

    linePutHere = new QLineEdit( buttonGroup4, "linePutHere" );
    linePutHere->setEnabled( FALSE );

    buttonGroup4Layout->addMultiCellWidget( linePutHere, 3, 3, 1, 2 );
    QSpacerItem* spacer_4 = new QSpacerItem( 340, 16, QSizePolicy::Expanding, QSizePolicy::Minimum );
    buttonGroup4Layout->addMultiCell( spacer_4, 0, 0, 2, 3 );

    checkReplace = new QCheckBox( buttonGroup4, "checkReplace" );

    buttonGroup4Layout->addMultiCellWidget( checkReplace, 0, 0, 0, 1 );

    checkAskReplace = new QCheckBox( buttonGroup4, "checkAskReplace" );

    buttonGroup4Layout->addMultiCellWidget( checkAskReplace, 1, 1, 0, 1 );

    checkPut = new QCheckBox( buttonGroup4, "checkPut" );

    buttonGroup4Layout->addMultiCellWidget( checkPut, 2, 2, 0, 1 );
    QSpacerItem* spacer_5 = new QSpacerItem( 340, 16, QSizePolicy::Expanding, QSizePolicy::Minimum );
    buttonGroup4Layout->addMultiCell( spacer_5, 1, 1, 2, 3 );
    QSpacerItem* spacer_6 = new QSpacerItem( 340, 16, QSizePolicy::Expanding, QSizePolicy::Minimum );
    buttonGroup4Layout->addMultiCell( spacer_6, 2, 2, 2, 3 );

    pushOpenDir = new QPushButton( buttonGroup4, "pushOpenDir" );
    pushOpenDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pushOpenDir->sizePolicy().hasHeightForWidth() ) );

    buttonGroup4Layout->addWidget( pushOpenDir, 3, 3 );

    SQ_ConvertWidgetLayout->addMultiCellWidget( buttonGroup4, 2, 2, 0, 2 );
    QSpacerItem* spacer_7 = new QSpacerItem( 311, 25, QSizePolicy::Expanding, QSizePolicy::Minimum );
    SQ_ConvertWidgetLayout->addItem( spacer_7, 3, 0 );

    pushOK = new QPushButton( this, "pushOK" );

    SQ_ConvertWidgetLayout->addWidget( pushOK, 3, 2 );

    pushCancel = new QPushButton( this, "pushCancel" );
    pushCancel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, pushCancel->sizePolicy().hasHeightForWidth() ) );

    SQ_ConvertWidgetLayout->addWidget( pushCancel, 3, 1 );
    languageChange();
    resize( QSize(552, 404).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( checkPut, SIGNAL( toggled(bool) ), linePutHere, SLOT( setEnabled(bool) ) );
    connect( pushOpenDir, SIGNAL( clicked() ), this, SLOT( slotOpenDir() ) );
    connect( pushOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
SQ_ConvertWidget::~SQ_ConvertWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SQ_ConvertWidget::languageChange()
{
    setCaption( tr2i18n( "Convert" ) );
    listFiles->setCurrentItem( -1 );
    listFormats->setCurrentItem( -1 );
    buttonGroup4->setTitle( tr2i18n( "Options" ) );
    checkReplace->setText( tr2i18n( "Replace original" ) );
    checkAskReplace->setText( tr2i18n( "Ask for replace" ) );
    checkPut->setText( tr2i18n( "Put convetred to" ) );
    pushOpenDir->setText( tr2i18n( "Open" ) );
    pushOK->setText( tr2i18n( "OK" ) );
    pushCancel->setText( tr2i18n( "Cancel" ) );
}

#include "sq_convertwidget.moc"
