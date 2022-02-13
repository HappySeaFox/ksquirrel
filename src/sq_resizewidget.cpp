#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './sq_resizewidget.ui'
**
** Created: Втр Фев 10 22:18:01 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sq_resizewidget.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "./sq_resizewidget.ui.h"

/* 
 *  Constructs a SQ_ResizeWidget as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
SQ_ResizeWidget::SQ_ResizeWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "SQ_ResizeWidget" );

    plistSizes = new QListBox( this, "plistSizes" );
    plistSizes->setGeometry( QRect( 20, 20, 180, 140 ) );

    pgroupOtherSizes = new QGroupBox( this, "pgroupOtherSizes" );
    pgroupOtherSizes->setEnabled( FALSE );
    pgroupOtherSizes->setGeometry( QRect( 21, 170, 180, 60 ) );
    pgroupOtherSizes->setFrameShadow( QGroupBox::Sunken );
    pgroupOtherSizes->setLineWidth( 0 );

    textLabel1 = new QLabel( pgroupOtherSizes, "textLabel1" );
    textLabel1->setGeometry( QRect( 76, 10, 16, 20 ) );

    checkBox1 = new QCheckBox( pgroupOtherSizes, "checkBox1" );
    checkBox1->setGeometry( QRect( 10, 40, 99, 20 ) );

    pspinY = new QSpinBox( pgroupOtherSizes, "pspinY" );
    pspinY->setGeometry( QRect( 95, 10, 60, 20 ) );
    pspinY->setMaxValue( 10000 );

    pspinX = new QSpinBox( pgroupOtherSizes, "pspinX" );
    pspinX->setGeometry( QRect( 10, 10, 60, 20 ) );
    pspinX->setMaxValue( 10000 );
    languageChange();
    resize( QSize(542, 345).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( plistSizes, SIGNAL( highlighted(const QString&) ), this, SLOT( slotEnableSizeSpins(const QString&) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SQ_ResizeWidget::~SQ_ResizeWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SQ_ResizeWidget::languageChange()
{
    setCaption( tr2i18n( "Resize" ) );
    plistSizes->clear();
    plistSizes->insertItem( tr2i18n( "320x200" ) );
    plistSizes->insertItem( tr2i18n( "640x480" ) );
    plistSizes->insertItem( tr2i18n( "800x600" ) );
    plistSizes->insertItem( tr2i18n( "1024x768" ) );
    plistSizes->insertItem( tr2i18n( "1280x1024" ) );
    plistSizes->insertItem( tr2i18n( "1600x1200" ) );
    plistSizes->insertItem( tr2i18n( "Other ..." ) );
    plistSizes->setCurrentItem( 3 );
    pgroupOtherSizes->setTitle( QString::null );
    textLabel1->setText( tr2i18n( "<p align=\"center\">X</p>" ) );
    checkBox1->setText( tr2i18n( "Save aspect" ) );
}

#include "sq_resizewidget.moc"
