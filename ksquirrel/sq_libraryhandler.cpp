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

#include <qmessagebox.h>
#include <qvaluevector.h>

#include "sq_libraryhandler.h"
#include "ksquirrel.h"
#include "sq_config.h"

SQ_LibraryHandler::SQ_LibraryHandler(QStringList *foundLibraries, QObject *parent, const char *name) : QObject(parent, name), currentlib(0)
{
	libs = new QValueVector<SQ_LIBRARY>;

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
	QValueVector<SQ_LIBRARY>::iterator   BEGIN = libs->begin();
	QValueVector<SQ_LIBRARY>::iterator      END = libs->end();

	for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;it++)
		if(((*it).sinfo).find(name, 0, false) > 0)
		{
			currentlib = &(*it);
			break;
		}
}

SQ_LIBRARY* SQ_LibraryHandler::getCurrentLibrary()
{
	return currentlib;
}

SQ_LIBRARY SQ_LibraryHandler::getLibByIndex(const int &i)
{
	return ((*libs)[i]);
}

bool SQ_LibraryHandler::supports(const QString &format) const
{
	QValueVector<SQ_LIBRARY>::iterator   BEGIN = libs->begin();
	QValueVector<SQ_LIBRARY>::iterator      END = libs->end();

	for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;it++)
		if(((*it).sinfo).find(format, 0, false) > 0)
			return true;

	return false;
}

QString SQ_LibraryHandler::allSupportedForFilter() const
{
	QString ret = "";

	QValueVector<SQ_LIBRARY>::iterator   BEGIN = libs->begin();
	QValueVector<SQ_LIBRARY>::iterator      END = libs->end();

	for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;it++)
		ret = ret + (*it).sinfo + " ";

	return ret;
}

void SQ_LibraryHandler::clear()
{
	libs->clear();
}

void SQ_LibraryHandler::reInit(QStringList *foundLibraries)
{
	libs->clear();
	add(foundLibraries);
}

void SQ_LibraryHandler::add(QStringList *foundLibraries)
{
	QValueList<QString>::iterator   BEGIN = foundLibraries->begin();
	QValueList<QString>::iterator      END = foundLibraries->end();

	if(BEGIN == END) return;

	for(QValueList<QString>::iterator it = BEGIN;it != END;it++)
	{
		SQ_LIBRARY libtmp;

		libtmp.lib = new QLibrary(*it);
		libtmp.libpath = *it;
		libtmp.lib->load();

		libtmp.fmt_init = (int (*)(fmt_info **, const char *))(libtmp.lib)->resolve("fmt_init");
		libtmp.fmt_read_info = (int (*)(fmt_info *))(libtmp.lib)->resolve("fmt_read_info");
		libtmp.fmt_read_scanline = (int (*)(fmt_info *, RGBA*))(libtmp.lib)->resolve("fmt_read_scanline");
		libtmp.fmt_version = (char* (*)())(libtmp.lib)->resolve("fmt_version");
		libtmp.fmt_quickinfo = (char* (*)())(libtmp.lib)->resolve("fmt_quickinfo");
		libtmp.fmt_extension = (char* (*)())(libtmp.lib)->resolve("fmt_extension");
		libtmp.fmt_close = (int* (*)(fmt_info*))(libtmp.lib)->resolve("fmt_close");

		if(libtmp.fmt_init == 0 || libtmp.fmt_read_info == 0 || libtmp.fmt_read_scanline == 0 || libtmp.fmt_version == 0 || libtmp.fmt_quickinfo == 0 || libtmp.fmt_extension == 0 || libtmp.fmt_close == 0)
			if(sqConfig->readBoolEntry("Libraries", "continue", true))
				;
			else break;
		else
        	{
	 		libtmp.sinfo = QString(libtmp.fmt_extension());
			libs->append(libtmp);
		}
	}
}

void SQ_LibraryHandler::remove(QStringList *foundLibraries)
{
	QValueList<QString>::iterator   BEGIN = foundLibraries->begin();
	QValueList<QString>::iterator      END = foundLibraries->end();

	QValueVector<SQ_LIBRARY>::iterator   vBEGIN = libs->begin();
	QValueVector<SQ_LIBRARY>::iterator      vEND = libs->end();

}

int SQ_LibraryHandler::count() const
{
	return libs->count();
}
