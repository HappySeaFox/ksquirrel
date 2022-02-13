/***************************************************************************
                          sq_runprocess.cpp  -  description
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
#include <qimage.h>
#include <qmessagebox.h>
#include <qvaluevector.h>
#include <qprocess.h>

#include "sq_runprocess.h"
#include "ksquirrel.h"

SQ_RunProcess::SQ_RunProcess()
{
    plist = new QValueVector<PROCESSITEM>;

    QObject::QObject();
}


SQ_RunProcess::~SQ_RunProcess()
{}

void SQ_RunProcess::AddItem(const QString &_name, const QString &_program)
{
	PROCESSITEM item;
   	static QImage im1, im2;
	static QPixmap px;

	px = sqLoader->loadIcon(_program, KIcon::Desktop, KIcon::SizeSmall);

	im1 = px;
	im2 = sqLoader->loadIcon("unknown", KIcon::Desktop, KIcon::SizeSmall);
    
	item.name =  _name;
	item.program = _program;
	item.px = (im1 == im2)?QPixmap(0):px;

	plist->append(item);
}

int SQ_RunProcess::GetCount() const
{
    return plist->count();
}

QString SQ_RunProcess::GetName(unsigned int index) const
{
    QString tmp = (*plist)[index].name;
    return tmp;
}

QString SQ_RunProcess::GetProgName(unsigned int index) const
{
    QString tmp = (*plist)[index].program;
    return tmp;
}

void SQ_RunProcess::slotRunCommand(int index)
{
	QString program = (*plist)[index].program;
	QProcess tmp(program);

	if(!tmp.start())
		QMessageBox::warning(sqApp, "SQuirrel", "Process \""+program+"\"could not be started.\nTry \"whereis "+program+"\" to find it.", QMessageBox::Ok, QMessageBox::NoButton);
}

QPixmap SQ_RunProcess::GetPixmap(unsigned int index) const
{
    QPixmap tmp = (*plist)[index].px;
    return tmp;
}
