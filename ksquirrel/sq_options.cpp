#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_options.ui'
**
** Created: Птн Янв 23 15:28:41 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_options.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "./sq_options.ui.h"

/* 
 *  Constructs a SQ_Options as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SQ_Options::SQ_Options( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "SQ_Options" );
    setSizeGripEnabled( TRUE );
    SQ_OptionsLayout = new QGridLayout( this, 1, 1, 11, 6, "SQ_OptionsLayout"); 

    listBox = new QListBox( this, "listBox" );
    listBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, listBox->sizePolicy().hasHeightForWidth() ) );

    SQ_OptionsLayout->addWidget( listBox, 0, 0 );

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

    SQ_OptionsLayout->addMultiCellLayout( Layout1, 1, 1, 0, 1 );

    widgetStack1 = new QWidgetStack( this, "widgetStack1" );
    widgetStack1->setFrameShape( QWidgetStack::Box );
    widgetStack1->setFrameShadow( QWidgetStack::Raised );

    page = new QWidget( widgetStack1, "page" );
    widgetStack1->addWidget( page, 0 );

    page_2 = new QWidget( widgetStack1, "page_2" );

    checkBox4 = new QCheckBox( page_2, "checkBox4" );
    checkBox4->setGeometry( QRect( 40, 170, 139, 20 ) );
    checkBox4->setChecked( TRUE );
    checkBox4->setTristate( FALSE );

    buttonGroup1 = new QButtonGroup( page_2, "buttonGroup1" );
    buttonGroup1->setGeometry( QRect( 30, 100, 120, 60 ) );
    buttonGroup1->setFrameShape( QButtonGroup::NoFrame );
    buttonGroup1->setFrameShadow( QButtonGroup::Plain );

    radioButton1 = new QRadioButton( buttonGroup1, "radioButton1" );
    radioButton1->setGeometry( QRect( 10, 0, 80, 20 ) );
    radioButton1->setChecked( TRUE );

    radioButton3 = new QRadioButton( buttonGroup1, "radioButton3" );
    radioButton3->setGeometry( QRect( 10, 40, 103, 20 ) );

    radioButton2 = new QRadioButton( buttonGroup1, "radioButton2" );
    radioButton2->setGeometry( QRect( 10, 20, 76, 20 ) );

    checkBox1 = new QCheckBox( page_2, "checkBox1" );
    checkBox1->setGeometry( QRect( 10, 10, 147, 20 ) );

    checkBox3 = new QCheckBox( page_2, "checkBox3" );
    checkBox3->setGeometry( QRect( 10, 70, 94, 20 ) );
    checkBox3->setChecked( TRUE );

    checkBox5 = new QCheckBox( page_2, "checkBox5" );
    checkBox5->setGeometry( QRect( 10, 40, 141, 20 ) );
    widgetStack1->addWidget( page_2, 1 );

    SQ_OptionsLayout->addWidget( widgetStack1, 0, 1 );
    languageChange();
    resize( QSize(597, 370).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( checkBox3, SIGNAL( toggled(bool) ), this, SLOT( slotCreatePageToggle(bool) ) );
    connect( listBox, SIGNAL( selected(int) ), this, SLOT( slotSelectPage(int) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SQ_Options::~SQ_Options()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SQ_Options::languageChange()
{
    setCaption( tr2i18n( "Options" ) );
    listBox->clear();
    listBox->insertItem( tr2i18n( "Main" ) );
    listBox->insertItem( tr2i18n( "Interface" ) );
    listBox->setCurrentItem( 0 );
    buttonHelp->setText( tr2i18n( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr2i18n( "F1" ) ) );
    buttonOk->setText( tr2i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr2i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    checkBox4->setText( tr2i18n( "Set last path visited" ) );
    buttonGroup1->setTitle( QString::null );
    radioButton1->setText( tr2i18n( "Icon view" ) );
    radioButton3->setText( tr2i18n( "Detailed view" ) );
    radioButton2->setText( tr2i18n( "List view" ) );
    checkBox1->setText( tr2i18n( "don't create tree view" ) );
    checkBox3->setText( tr2i18n( "create page" ) );
    checkBox5->setText( tr2i18n( "don't create preview" ) );
}

#include "sq_options.moc"
