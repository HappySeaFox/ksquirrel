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

SQ_LibraryHandler::SQ_LibraryHandler(QStringList &foundLibraries, QObject *parent, const char *name) : QObject(parent, name), currentlib(0)
{
	libs = new QValueList<SQ_LIBRARY>;

	QValueList<QString>::iterator   BEGIN = foundLibraries.begin();
	QValueList<QString>::iterator      END = foundLibraries.end();

	if(BEGIN == END) return;

	for(QValueList<QString>::iterator it = BEGIN;it != END;it++)
	{
//		QString tmp = *it;
		SQ_LIBRARY libtmp;

//		tmp = tmp.replace("/usr/lib/squirrel/libSQ_serve_", "");
//		tmp = tmp.replace(".so", "");
		
		libtmp.lib = new QLibrary(*it);
		libtmp.libpath = *it;
		//@todo 'Preload' runlevel needs load(), otherwise - not.
		libtmp.lib->load();
		libtmp.readformat = (int (*)(const char*, PICTURE **))(libtmp.lib)->resolve("readformat");
		libtmp.writeformat = (int (*)(const char*, PICTURE **))(libtmp.lib)->resolve("writeformat");
		libtmp.readable = (int (*)())(libtmp.lib)->resolve("readable");
		libtmp.writeable = (int (*)())(libtmp.lib)->resolve("writeable");
		libtmp.version = (char* (*)())(libtmp.lib)->resolve("version");
		libtmp.info = (char* (*)())(libtmp.lib)->resolve("info");
		libtmp.extension = (char* (*)())(libtmp.lib)->resolve("extension");
		libtmp.sinfo = QString(libtmp.extension());

		libs->append(libtmp);
	}
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
