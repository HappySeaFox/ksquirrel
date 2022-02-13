#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_convertwidget.ui'
**
** Created: Fri Mar 19 17:27:20 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_convertwidget.h"

#include <qvariant.h>
#include <kde/kfiledialog.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
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

    pushOpenDir = new QPushButton( buttonGroup4, "pushOpenDir" );
    pushOpenDir->setGeometry( QRect( 430, 116, 69, 29 ) );
    pushOpenDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pushOpenDir->sizePolicy().hasHeightForWidth() ) );

    linePutHere = new QLineEdit( buttonGroup4, "linePutHere" );
    linePutHere->setEnabled( FALSE );
    linePutHere->setGeometry( QRect( 75, 120, 345, 21 ) );

    checkReplace = new QCheckBox( buttonGroup4, "checkReplace" );
    checkReplace->setGeometry( QRect( 3, 25, 210, 21 ) );

    checkAskReplace = new QCheckBox( buttonGroup4, "checkAskReplace" );
    checkAskReplace->setGeometry( QRect( 3, 52, 210, 21 ) );

    checkPut = new QCheckBox( buttonGroup4, "checkPut" );
    checkPut->setGeometry( QRect( 3, 79, 210, 21 ) );

    SQ_ConvertWidgetLayout->addMultiCellWidget( buttonGroup4, 2, 2, 0, 2 );
    QSpacerItem* spacer_3 = new QSpacerItem( 311, 25, QSizePolicy::Expanding, QSizePolicy::Minimum );
    SQ_ConvertWidgetLayout->addItem( spacer_3, 3, 0 );

    pushOK = new QPushButton( this, "pushOK" );

    SQ_ConvertWidgetLayout->addWidget( pushOK, 3, 2 );

    pushCancel = new QPushButton( this, "pushCancel" );
    pushCancel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, pushCancel->sizePolicy().hasHeightForWidth() ) );

    SQ_ConvertWidgetLayout->addWidget( pushCancel, 3, 1 );
    languageChange();
    resize( QSize(535, 386).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( checkPut, SIGNAL( toggled(bool) ), linePutHere, SLOT( setEnabled(bool) ) );
    connect( pushOpenDir, SIGNAL( clicked() ), this, SLOT( slotOpenDir() ) );
    connect( pushOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
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
    listFiles->clear();
    listFiles->insertItem( tr2i18n( "file1" ) );
    listFiles->insertItem( tr2i18n( "file2" ) );
    listFiles->insertItem( tr2i18n( "file3" ) );
    listFiles->setCurrentItem( 0 );
    listFormats->clear();
    listFormats->insertItem( tr2i18n( "bmp" ) );
    listFormats->insertItem( tr2i18n( "tga" ) );
    listFormats->insertItem( tr2i18n( "jpeg" ) );
    listFormats->setCurrentItem( 0 );
    buttonGroup4->setTitle( tr2i18n( "Options" ) );
    pushOpenDir->setText( tr2i18n( "Open" ) );
    checkReplace->setText( tr2i18n( "Replace original" ) );
    checkAskReplace->setText( tr2i18n( "Ask for replace" ) );
    checkPut->setText( tr2i18n( "Put convetred to" ) );
    pushOK->setText( tr2i18n( "OK" ) );
    pushCancel->setText( tr2i18n( "Cancel" ) );
}

#include "sq_convertwidget.moc"
