/***************************************************************************
                          SQ_LibraryHandler.cpp  -  description
                             -------------------
    begin                :  Mar 5 2004
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

#include "sq_libraryhandler.h"

#include <qmessagebox.h>

SQ_LibraryHandler::SQ_LibraryHandler(QStringList *foundLibraries, QObject *parent, const char *name) : QObject(parent, name), currentlib(0)
{
	libs = new QValueList<SQ_LIBRARY>;

	if(foundLibraries)
		reInit(foundLibraries);
}

SQ_LibraryHandler::~SQ_LibraryHandler()
{
	for(unsigned int i = 0;i < libs->count();i++)
		(*libs)[i].lib->unload();
}

void SQ_LibraryHandler::setCurrentLibrary(const QString &name)
{
	QValueList<SQ_LIBRARY>::iterator   BEGIN = libs->begin();
	QValueList<SQ_LIBRARY>::iterator      END = libs->end();

	for(QValueList<SQ_LIBRARY>::iterator it = BEGIN;it != END;it++)
		if(((*it).sinfo).find(name, 0, false) > 0)
		{
			currentlib = &(*it);
			break;
		}
}

int SQ_LibraryHandler::callReadFormat(const QString &file, PICTURE **pic)
{
//	QMessageBox::information(0, "", (const char*)file, 1);

	int ret = currentlib->readformat(file, pic);

	return ret;
}

SQ_LIBRARY SQ_LibraryHandler::getLibByIndex(const int &i)
{
	return ((*libs)[i]);
}

bool SQ_LibraryHandler::supports(const QString &format) const
{
	QValueList<SQ_LIBRARY>::iterator   BEGIN = libs->begin();
	QValueList<SQ_LIBRARY>::iterator      END = libs->end();

	for(QValueList<SQ_LIBRARY>::iterator it = BEGIN;it != END;it++)
		if(((*it).sinfo).find(format, 0, false) > 0)
			return true;

	return false;
}

QString SQ_LibraryHandler::allSupportedForFilter() const
{
	QString ret = "";

	QValueList<SQ_LIBRARY>::iterator   BEGIN = libs->begin();
	QValueList<SQ_LIBRARY>::iterator      END = libs->end();

	for(QValueList<SQ_LIBRARY>::iterator it = BEGIN;it != END;it++)
		ret = ret + QString((*it).sinfo) + " ";

	return ret;
}

void SQ_LibraryHandler::clear()
{
	libs->clear();
}

void SQ_LibraryHandler::reInit(QStringList *foundLibraries)
{
	libs->clear();

	QValueList<QString>::iterator   BEGIN = foundLibraries->begin();
	QValueList<QString>::iterator      END = foundLibraries->end();

	if(BEGIN == END) return;

	for(QValueList<QString>::iterator it = BEGIN;it != END;it++)
	{
		SQ_LIBRARY libtmp;

		libtmp.lib = new QLibrary(*it);
		libtmp.libpath = *it;
		libtmp.lib->load();

		libtmp.fmt_init = (int (*)(fmt_info *, const char *))(libtmp.lib)->resolve("fmt_init");
		libtmp.fmt_read_info = (int (*)(fmt_info *))(libtmp.lib)->resolve("fmt_read_info");
		libtmp.fmt_read_scanline = (int (*)(fmt_info *, RGBA*))(libtmp.lib)->resolve("fmt_read_scanline");

		libtmp.readformat = (int (*)(const char*, PICTURE **))(libtmp.lib)->resolve("readformat");
		libtmp.writeformat = (int (*)(const char*, PICTURE **))(libtmp.lib)->resolve("writeformat");
		libtmp.fmt_readable = (int (*)())(libtmp.lib)->resolve("fmt_readable");
		libtmp.fmt_writeable = (int (*)())(libtmp.lib)->resolve("fmt_writeable");
		libtmp.fmt_version = (char* (*)())(libtmp.lib)->resolve("fmt_version");
		libtmp.fmt_quickinfo = (char* (*)())(libtmp.lib)->resolve("fmt_quickinfo");
		libtmp.fmt_extension = (char* (*)())(libtmp.lib)->resolve("fmt_extension");
		libtmp.sinfo = QString(libtmp.fmt_extension());

		if(libtmp.readformat == 0)
			if(QMessageBox::warning(0, "SQ_LibHandler", "Library \""+libtmp.libpath+"\" doesn't contain one of fmt_* function.", QMessageBox::Ignore, QMessageBox::Abort) == QMessageBox::Abort)
				break;
			else;
      		else
			libs->append(libtmp);
	}
}
