/***************************************************************************
                          sq_dir.cpp  -  description
                             -------------------
    begin                : Tue Mar 2 2004
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

#include "sq_dir.h"

SQ_Dir::SQ_Dir() : QDir()
{
}

SQ_Dir::SQ_Dir(const QString & path, const QString & nameFilter, int sortSpec, int filterSpec) : QDir(path,nameFilter,sortSpec,filterSpec)
{}

SQ_Dir::SQ_Dir(const QDir & d) : QDir(d)
{}
                
QString SQ_Dir::operator[] (int index) const
{
	int cnt = count();

	if(cnt == 0)
		return QString::null;

	if(index < 0)
		return (*this)[0];
	else if(index >= cnt)
		return (*this)[cnt-1];
	else
		return (*this)[index];
}

SQ_Dir::~SQ_Dir()
{}
