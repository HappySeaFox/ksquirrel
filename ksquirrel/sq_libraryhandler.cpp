/***************************************************************************
                          SQ_LibraryHandler.cpp  -  description
                             -------------------
    begin                :  Mar 5 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
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
#include <qlibrary.h>
#include <qfileinfo.h>

#include <kstringhandler.h>
#include <kdebug.h>

#include "sq_libraryhandler.h"
#include "ksquirrel.h"
#include "sq_config.h"

#define	SQ_HAVE_MIMESTRING
#include "sq_thumbnailjob.h"
#undef		SQ_HAVE_MIMESTRING

SQ_LibraryHandler::SQ_LibraryHandler(QStringList *foundLibraries) :
											QValueVector<SQ_LIBRARY>()
{
	if(foundLibraries)
		reInit(foundLibraries);
}

SQ_LibraryHandler::~SQ_LibraryHandler()
{}

SQ_LIBRARY* SQ_LibraryHandler::libraryForFile(const QString &full_path) const
{
	if(full_path.isEmpty())
		return 0;

	QValueVector<SQ_LIBRARY>::const_iterator	BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator	END = end();

	SQ_LIBRARY *l, *found = 0;

	QFile file(full_path);
	char buffer[buffer_size];

	if(!file.open(IO_ReadOnly))
		return 0;

	int rr = file.readBlock(buffer, buffer_size);

	if(rr != buffer_size || file.status() != IO_Ok)
	{
		file.close();
		return 0;
	}
	else
		file.close();

	for(int j = 0;j < buffer_size;j++)
		if(buffer[j] == '\0')
			buffer[j] = '\001';

	buffer[buffer_size - 1] = '\0';

	QString read = QString::fromLatin1(buffer);

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
	{
		l = (SQ_LIBRARY *)&(*it);

		if(!l->regexp.pattern().isEmpty() && !l->regexp.pattern().isNull())
			if(read.find(l->regexp) == 0)
			{
				found = l;
				break;
			}
	}

	if(found)
		return found;

	QFileInfo f(full_path);
	QString ext = "*.";
	ext += f.extension(false);
	ext += " ";

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
	{
		l = (SQ_LIBRARY *)&(*it);

		if(l->filter.find(ext, 0, false) != -1)
			return l;
	}

	return 0;
}

bool SQ_LibraryHandler::supports(const QString &f) const
{
	return libraryForFile(f) != 0;
}

QString SQ_LibraryHandler::allFiltersString() const
{
     QString ret;

	QValueVector<SQ_LIBRARY>::const_iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator   END = end();

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
		ret = ret + (*it).filter + " ";

	return ret;
}

QStringList SQ_LibraryHandler::allFilters() const
{
	QStringList ret;

	QValueVector<SQ_LIBRARY>::const_iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator   END = end();

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
		ret.append((*it).filter);

	return ret;
}

void SQ_LibraryHandler::clear()
{
	QValueVector<SQ_LIBRARY>::iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::iterator   END = end();

	// unload libraries on clear()
	for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;++it)
		(*it).lib->unload();

	QValueVector<SQ_LIBRARY>::clear();
}

void SQ_LibraryHandler::reInit(QStringList *foundLibraries)
{
	clear();
	add(foundLibraries);
}

void SQ_LibraryHandler::add(QStringList *foundLibraries)
{
	QValueList<QString>::iterator   BEGIN = foundLibraries->begin();
	QValueList<QString>::iterator      END = foundLibraries->end();
	QString mime_str;

	if(BEGIN == END) return;

	for(QValueList<QString>::iterator it = BEGIN;it != END;++it)
	{
		QFileInfo ff(*it);

		if(!ff.isExecutable() || !ff.isReadable() || ff.isDir())
			continue;

		SQ_LIBRARY libtmp;
		QStringList formats;

		libtmp.lib = new QLibrary(*it);
		libtmp.libpath = *it;
		libtmp.lib->load();

		libtmp.fmt_init = (int (*)(fmt_info *, const char *))(libtmp.lib)->resolve(QString::fromLatin1("fmt_init"));
		libtmp.fmt_read_scanline = (int (*)(fmt_info *, RGBA*))(libtmp.lib)->resolve(QString::fromLatin1("fmt_read_scanline"));
		libtmp.fmt_next = (int (*)(fmt_info *))(libtmp.lib)->resolve(QString::fromLatin1("fmt_next"));
		libtmp.fmt_next_pass = (int (*)(fmt_info *))(libtmp.lib)->resolve(QString::fromLatin1("fmt_next_pass"));
		libtmp.fmt_version = (const char* (*)())(libtmp.lib)->resolve(QString::fromLatin1("fmt_version"));
		libtmp.fmt_pixmap = (const char* (*)())(libtmp.lib)->resolve(QString::fromLatin1("fmt_pixmap"));
		libtmp.fmt_quickinfo = (const char* (*)())(libtmp.lib)->resolve(QString::fromLatin1("fmt_quickinfo"));
		libtmp.fmt_filter = (const char* (*)())(libtmp.lib)->resolve(QString::fromLatin1("fmt_filter"));
		libtmp.fmt_mime = (const char* (*)())(libtmp.lib)->resolve(QString::fromLatin1("fmt_mime"));
		libtmp.fmt_readimage = (int (*)(const char*, RGBA**, char **))(libtmp.lib)->resolve(QString::fromLatin1("fmt_readimage"));
		libtmp.fmt_close = (int (*)())(libtmp.lib)->resolve(QString::fromLatin1("fmt_close"));

		if(libtmp.fmt_init == 0 ||	libtmp.fmt_next == 0 || libtmp.fmt_next_pass == 0 ||
			libtmp.fmt_read_scanline == 0 ||
			libtmp.fmt_version == 0 || libtmp.fmt_pixmap == 0 ||
			libtmp.fmt_quickinfo == 0 || libtmp.fmt_filter == 0 ||
			libtmp.fmt_mime == 0 ||	libtmp.fmt_readimage == 0 ||
			libtmp.fmt_close == 0)
		{
			libtmp.lib->unload();
			delete libtmp.lib;
		}
		else
		{
			QString q = QString::fromLatin1(libtmp.fmt_quickinfo());

			if(!alreadyInMap(q))
			{
				libtmp.mime_len = convertMimeFromBits(QString::fromLatin1(libtmp.fmt_pixmap()), mime_str);
				libtmp.mime.loadFromData((unsigned char*)mime_str.ascii(), libtmp.mime_len, "PNG");
				libtmp.filter = QString::fromLatin1(libtmp.fmt_filter());
				libtmp.quickinfo = q;
				libtmp.version = QString::fromLatin1(libtmp.fmt_version());
				libtmp.regexp_str = QString::fromLatin1(libtmp.fmt_mime());
				libtmp.regexp.setPattern(libtmp.regexp_str);
				libtmp.regexp.setCaseSensitive(true);

				append(libtmp);
			}
			else
			{
				kdDebug() << "Library \"" << q << "\" is already handled" << endl;
			}
		}
	}

	dump();
}

bool SQ_LibraryHandler::alreadyInMap(const QString &quick) const
{
	QValueVector<SQ_LIBRARY>::const_iterator  BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator   END = end();

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
		if((*it).quickinfo == quick)
			return true;

	return false;
}

void SQ_LibraryHandler::remove(QStringList *foundLibraries)
{

	QValueList<QString>::iterator   BEGIN = foundLibraries->begin();
	QValueList<QString>::iterator      END = foundLibraries->end();

	QValueVector<SQ_LIBRARY>::iterator   vBEGIN = begin();
	QValueVector<SQ_LIBRARY>::iterator      vEND = end();

	for(QValueList<QString>::iterator it = BEGIN;it != END;++it)
	{
		for(QValueVector<SQ_LIBRARY>::iterator vit = vBEGIN;vit != vEND;++vit)
		{
			if(*it == (*vit).libpath)
			{
				erase(vit);
				break;
			}
		}
	}

	dump();
}

void SQ_LibraryHandler::dump() const
{
	QValueVector<SQ_LIBRARY>::const_iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator      END = end();
	SQ_LIBRARY *l;
	QString regexp;

	printf("\n\nSQ_LibraryHandler: memory dump (total %d)\n********************************************\n\n\
%-30s%-15s%-20s%-20s%-10s\n", count(), "Lib path", "RegExp", "Filter", "Quick info", "Version");

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
	{
		l = (SQ_LIBRARY *)&(*it);

		regexp = "";

		for(unsigned int i = 0;i < l->regexp_str.length();i++)
		{
			QChar c = l->regexp_str[i];
			if(c.isPrint())
				regexp += QString::fromLatin1("%1").arg(c);
			else
				regexp += QString::fromLatin1("\\%1").arg(l->regexp_str[i].unicode());
		}

		printf("%-30s%-15s%-20s%-20s%-10s\n",
				KStringHandler::csqueeze(l->libpath, 27).ascii(),
				KStringHandler::rsqueeze(regexp, 13).ascii(),
				KStringHandler::rsqueeze(l->filter, 18).ascii(),
				KStringHandler::rsqueeze(l->quickinfo, 18).ascii(),
				l->version.ascii());
	}

	printf("\n\n");
}
