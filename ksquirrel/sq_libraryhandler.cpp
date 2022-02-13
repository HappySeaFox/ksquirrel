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
#include <qregexp.h>
#include <qstringlist.h>
#include <qlibrary.h>

#include "sq_libraryhandler.h"
#include "ksquirrel.h"
#include "sq_config.h"

SQ_LibraryHandler::SQ_LibraryHandler(QStringList *foundLibraries, QObject *parent, const char *name) : QObject(parent, name), currentlib(0)
{
	if(foundLibraries)
		reInit(foundLibraries);
}

SQ_LibraryHandler::~SQ_LibraryHandler()
{}

SQ_LIBRARY* SQ_LibraryHandler::setCurrentLibrary(const QString &name)
{
	if(name.isEmpty())
		return 0L;

	currentlib = &map[name.lower()];

	if(currentlib->libpath.isEmpty())
		return 0L;

	return currentlib;
}

bool SQ_LibraryHandler::supports(const QString &format) const
{
	if(format.isEmpty() || !count()) return false;

	return !(map[format.lower()]).filter.isEmpty();
}

QString SQ_LibraryHandler::allSupportedForFilter() const
{
	QString ret = "";

	QMapConstIterator<QString, SQ_LIBRARY>   BEGIN = map.begin();
	QMapConstIterator<QString, SQ_LIBRARY>   END = map.end();

	for(QMapConstIterator<QString, SQ_LIBRARY> it = BEGIN;it != END;it++)
		ret = ret + (*it).filter;

	return ret;
}

void SQ_LibraryHandler::clear()
{
	map.clear();
}

void SQ_LibraryHandler::reInit(QStringList *foundLibraries)
{
	map.clear();
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
		QStringList formats;

		libtmp.lib = new QLibrary(*it);
		libtmp.libpath = *it;
		libtmp.lib->load();

		libtmp.fmt_init = (int (*)(fmt_info **, const char *))(libtmp.lib)->resolve("fmt_init");
		libtmp.fmt_read_info = (int (*)(fmt_info *))(libtmp.lib)->resolve("fmt_read_info");
		libtmp.fmt_read_scanline = (int (*)(fmt_info *, RGBA*))(libtmp.lib)->resolve("fmt_read_scanline");
		libtmp.fmt_version = (char* (*)())(libtmp.lib)->resolve("fmt_version");
		libtmp.fmt_quickinfo = (char* (*)())(libtmp.lib)->resolve("fmt_quickinfo");
		libtmp.fmt_extension = (char* (*)())(libtmp.lib)->resolve("fmt_extension");
		libtmp.fmt_readimage = (void (*)(fmt_info *, RGBA*))(libtmp.lib)->resolve("fmt_readimage");
		libtmp.fmt_close = (int* (*)(fmt_info*))(libtmp.lib)->resolve("fmt_close");

		if(libtmp.fmt_init == 0 ||
			libtmp.fmt_read_info == 0 ||
			libtmp.fmt_read_scanline == 0 ||
			libtmp.fmt_version == 0 ||
			libtmp.fmt_quickinfo == 0 ||
			libtmp.fmt_extension == 0 ||
			libtmp.fmt_readimage == 0 ||
			libtmp.fmt_close == 0)
		{
			libtmp.lib->unload();
			delete libtmp.lib;
		}
		else
        		{
			formats = QStringList::split(" ", QString(libtmp.fmt_extension()));
			libtmp.filter = "";

			QValueList<QString>::iterator   f1 = formats.begin();
			QValueList<QString>::iterator   f2 = formats.end();

			for(QValueList<QString>::iterator f3 = f1;f3 != f2;f3++)
				libtmp.filter = libtmp.filter + "*." + *f3 + " ";

			for(QValueList<QString>::iterator f3 = f1;f3 != f2;f3++)
			{
				libtmp.sinfo = *f3;
				libtmp.quickinfo = libtmp.fmt_quickinfo();
				libtmp.version = libtmp.fmt_version();
				map[*f3] = libtmp;
			}
		}
	}
}

void SQ_LibraryHandler::remove(QStringList *foundLibraries)
{

	QValueList<QString>::iterator   BEGIN = foundLibraries->begin();
	QValueList<QString>::iterator      END = foundLibraries->end();

	QMap<QString, SQ_LIBRARY>::iterator   vBEGIN = map.begin();
	QMap<QString, SQ_LIBRARY>::iterator      vEND = map.end();

	for(QValueList<QString>::iterator it = BEGIN;it != END;it++)
	{
		for(QMap<QString, SQ_LIBRARY>::iterator vit = vBEGIN;vit != vEND;vit++)
		{
			if(*it == (*vit).libpath)
			{
				map.erase(vit);
				break;
			}
		}
	}
}

int SQ_LibraryHandler::count() const
{
	return map.count();
}

QValueVector<SQ_LIBRARY> SQ_LibraryHandler::getLibs()
{
	QValueVector<SQ_LIBRARY> vect;

	QMap<QString, SQ_LIBRARY>::iterator   BEGIN = map.begin();
	QMap<QString, SQ_LIBRARY>::iterator      END = map.end();
	SQ_LIBRARY last;
	QString fix;

	for(QMap<QString, SQ_LIBRARY>::iterator it = BEGIN;it != END;it++)
	{
		if(fix.find((*it).quickinfo) != -1)
			continue;

		fix.append((*it).quickinfo);

		last = *it;

		vect.append(*it);
	}

	return vect;
}
