/***************************************************************************
                          sq_splash.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
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

#include <qapplication.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>

#include <kglobal.h>
#include <kiconloader.h>

#include "sq_splash.h"


SQ_Splash::SQ_Splash(QWidget *parent, const char *name) : QVBox(parent, name, WType_Modal | WStyle_Customize | WStyle_NoBorder | WDestructiveClose | WStyle_StaysOnTop)
{
	KIconLoader *loader = new KIconLoader(*(KGlobal::iconLoader()));

	picLabel = new QLabel(this);
	QPixmap pixmap = loader->loadIcon("folder", KIcon::Desktop, 256);

	picLabel->setPixmap(pixmap);
	move(QApplication::desktop()->width() / 2 - pixmap.width() / 2, QApplication::desktop()->height() / 2 - height() / 2);
}


SQ_Splash::~SQ_Splash() 
{}


void SQ_Splash::mousePressEvent(QMouseEvent *)
{
	hide();
}
