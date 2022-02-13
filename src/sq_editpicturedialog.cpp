/***************************************************************************
                          sq_editpicturedialog.cpp  -  description
                             -------------------
    begin                : ??? ??? 5 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@yandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sq_editpicturedialog.h"

SQ_EditPictureDialog::SQ_EditPictureDialog(QWidget* parent, const char* name, bool modal, WFlags fl) : QDialog(parent, name, modal, fl)
{
	if ( !name )
		setName("EditPicture");
		
	setCaption("Standart edition of picture");
	setSizeGripEnabled(true);
	EditPictureLayout = new QVBoxLayout(this, 7, 8, "EditPictureLayout");

	tabWidget = new QTabWidget(this, "tabWidget" );
	tabWidget->setFocusPolicy(QTabWidget::NoFocus);
	tabWidget->setTabPosition(QTabWidget::Top);

	ResizeWidget = new SQ_ResizeWidget(tabWidget, "ResizeWidget");
	tabWidget->insertTab(ResizeWidget, "Resize");

	ConvertWidget = new QWidget(tabWidget, "ConvertWidget");
	tabWidget->insertTab(ConvertWidget, "Convert to ...");
	EditPictureLayout->addWidget(tabWidget);

	Layout1 = new QHBoxLayout(0, 0, 6, "Layout1");

	buttonHelp = new QPushButton("&Help", this, "buttonHelp");
	buttonHelp->setAutoDefault(true);
	buttonHelp->setAccel(QKeySequence("F1"));

	Layout1->addWidget( buttonHelp );
	QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	Layout1->addItem( spacer );

	buttonOk = new QPushButton("&OK", this, "buttonOk");
	buttonOk->setAutoDefault(true);
	buttonOk->setDefault(true);
	Layout1->addWidget(buttonOk);

	buttonCancel = new QPushButton("&Cancel", this, "buttonCancel" );
	buttonCancel->setAutoDefault(true);
	Layout1->addWidget(buttonCancel);
	EditPictureLayout->addLayout(Layout1);

	resize(QSize(528, 368).expandedTo(minimumSizeHint()));

	connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
	connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


SQ_EditPictureDialog::~SQ_EditPictureDialog()
{}
