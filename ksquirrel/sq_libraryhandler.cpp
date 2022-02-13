/***************************************************************************
                          sq_libraryhandler.cpp  -  description
                             -------------------
    begin                : Mar 5 2004
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
#include <qfile.h>

#include <kstringhandler.h>
#include <kdebug.h>

#include "sq_libraryhandler.h"
#include "sq_config.h"

#include <csetjmp>

#include "fileio.h"
#include "fmt_codec_base.h"

#define SQ_HAVE_MIMESTRING
#include "sq_thumbnailinfo.h"

static const int buffer_size = 10;

SQ_LibraryHandler * SQ_LibraryHandler::hand = NULL;

SQ_LibraryHandler::SQ_LibraryHandler(QStringList *foundLibraries) : QValueVector<SQ_LIBRARY>(), latest(NULL)
{
	hand = this;

	if(foundLibraries)
		reInit(foundLibraries);
}

SQ_LibraryHandler::~SQ_LibraryHandler()
{}

SQ_LIBRARY* SQ_LibraryHandler::libraryForFile(const QString &full_path)
{
	if(full_path.isEmpty() || !QFile::exists(full_path))
		return NULL;

	QValueVector<SQ_LIBRARY>::const_iterator	BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator	END = end();

	SQ_LIBRARY *l, *found = NULL;

	QFile file(full_path);
	char buffer[buffer_size+1];

	if(!file.open(IO_ReadOnly))
		return NULL;

	int rr = file.readBlock(buffer, buffer_size);

	if(rr != buffer_size || file.status() != IO_Ok)
	{
		file.close();
		return NULL;
	}
	else
		file.close();

	for(int j = 0;j < buffer_size;j++)
		if(buffer[j] == '\0')
			buffer[j] = '\001';

	buffer[buffer_size] = '\0';

	QString read = QString::fromLatin1(buffer);

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
	{
		l = (SQ_LIBRARY *)&(*it);

		if(!l->regexp_str.isEmpty())
			if(read.find(l->regexp) == 0)
			{
				found = l;
				latest = found;
				break;
			}
	}

	if(found)
	{
		if(found->codec->fmt_readable())
		{
			printf("1. SQ_LibraryHandler::libraryForFile: \"%s\" => \"%s\"\n", 
				KStringHandler::csqueeze(full_path, 22).ascii(),
				found->quickinfo.ascii());

			return found;
		}

		return NULL;
	}

	QFileInfo f(full_path);
	QString ext = "*.";
	ext += f.extension(false);
	ext += " ";

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
	{
		l = (SQ_LIBRARY *)&(*it);

		if(l->filter.find(ext, 0, false) != -1 && l->regexp_str.isEmpty())
		{
			printf("2. SQ_LibraryHandler::libraryForFile: \"%s\" => \"%s\"\n",
			    KStringHandler::csqueeze(full_path, 22).ascii(),
			    l->quickinfo.ascii());

			latest = l;

			return ((l && l->codec->fmt_readable()) ? l : NULL);
		}
	}

	return NULL;
}

bool SQ_LibraryHandler::supports(const QString &f)
{
	return libraryForFile(f) != NULL;
}

QString SQ_LibraryHandler::allFiltersString() const
{
     QString ret;

	QValueVector<SQ_LIBRARY>::const_iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator   END = end();

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
	{
		if(!(*it).filter.isEmpty())
			ret = ret + (*it).filter + " ";
	}

	return ret;
}

void SQ_LibraryHandler::allFilters(QStringList &filters, QStringList &quick) const
{
	filters.clear();
	quick.clear();

	if(empty())
		return;

	QValueVector<SQ_LIBRARY>::const_iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator   END = end();

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
		if(!(*it).filter.isEmpty())
		{
			filters.append((*it).filter);
			quick.append((*it).quickinfo);
		}
}

void SQ_LibraryHandler::clear()
{
	QValueVector<SQ_LIBRARY>::iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::iterator   END = end();

	// unload libraries on clear()
	for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;++it)
	{
		(*it).fmt_codec_destroy((*it).codec);
		(*it).lib->unload();
	}

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

		libtmp.fmt_codec_create = (fmt_codec_base*(*)())(libtmp.lib)->resolve("fmt_codec_create");
		libtmp.fmt_codec_destroy = (void (*)(fmt_codec_base*))(libtmp.lib)->resolve(QString::fromLatin1("fmt_codec_destroy"));

		if(!libtmp.fmt_codec_create || !libtmp.fmt_codec_destroy)
		{
			libtmp.lib->unload();
			delete libtmp.lib;
		}
		else
		{
			fmt_codec_base *codeK = libtmp.fmt_codec_create();

#ifndef QT_NO_STL
			QString q = codeK->fmt_quickinfo();
#else
			QString q = codeK->fmt_quickinfo().c_str();
#endif

			if(!alreadyInMap(q))
			{
#ifndef QT_NO_STL
				libtmp.mime_len = convertMimeFromBits(codeK->fmt_pixmap(), mime_str);
#else
				libtmp.mime_len = convertMimeFromBits(codeK->fmt_pixmap().c_str(), mime_str);
#endif
				libtmp.mime.loadFromData((unsigned char*)mime_str.ascii(), libtmp.mime_len, "PNG");
				libtmp.quickinfo = q;
#ifndef QT_NO_STL
				libtmp.filter = codeK->fmt_filter();
				libtmp.version = codeK->fmt_version();
				libtmp.regexp_str = codeK->fmt_mime();
#else
				libtmp.filter = codeK->fmt_filter().c_str();
				libtmp.version = codeK->fmt_version().c_str();
				libtmp.regexp_str = codeK->fmt_mime().c_str();
#endif
				libtmp.regexp.setPattern(libtmp.regexp_str);
				libtmp.regexp.setCaseSensitive(true);
//				libtmp.regexp.setWildcard(true);
//				libtmp.regexp.setMinimal(true);

				libtmp.writable = codeK->fmt_writable();
				libtmp.readable = codeK->fmt_readable();

				if(libtmp.writable)
					codeK->fmt_getwriteoptions(&libtmp.opt);

				libtmp.codec = codeK;

				append(libtmp);
			}
			else
			{
				kdDebug() << "Library \"" << q << "\" is already in map" << endl;
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
				(*vit).fmt_codec_destroy((*vit).codec);
				(*vit).lib->unload();
				delete (*vit).lib;

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
%-25s%-15s%-20s%-25s%-10s\n", count(), "Library", "RegExp", "Filter", "Quick info", "Version");

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

		printf("%-25s%-15s%-20s%-25s%-10s\n",
				KStringHandler::csqueeze(QFileInfo(l->libpath).fileName(), 23).ascii(),
				KStringHandler::rsqueeze(regexp, 13).ascii(),
				KStringHandler::rsqueeze(l->filter, 18).ascii(),
				KStringHandler::rsqueeze(l->quickinfo, 23).ascii(),
				l->version.ascii());
	}

	printf("\n\n");
}

bool SQ_LibraryHandler::knownExtension(const QString &ext)
{
	QValueVector<SQ_LIBRARY>::const_iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator      END = end();
	SQ_LIBRARY *l;

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
	{
		l = (SQ_LIBRARY *)&(*it);

		if(l->filter.contains(ext, false))
			return true;
	}

	return false;
}

SQ_LIBRARY* SQ_LibraryHandler::libraryByName(const QString &name)
{
	QValueVector<SQ_LIBRARY>::const_iterator   BEGIN = begin();
	QValueVector<SQ_LIBRARY>::const_iterator      END = end();
	SQ_LIBRARY *l;

	for(QValueVector<SQ_LIBRARY>::const_iterator it = BEGIN;it != END;++it)
	{
		l = (SQ_LIBRARY *)&(*it);

		if(l->quickinfo == name)
			return l;
	}

	return NULL;
}

fmt_codec_base* SQ_LibraryHandler::codecForFile(const QString &file)
{
	SQ_LIBRARY *lib = libraryForFile(file);

	if(lib && lib->codec)
		return lib->codec;

	return NULL;
}

fmt_codec_base* SQ_LibraryHandler::codecByName(const QString &name)
{
	SQ_LIBRARY *lib = libraryByName(name);

	if(lib && lib->codec)
		return lib->codec;

	return NULL;
}

fmt_codec_base *SQ_LibraryHandler::latestCodec()
{
	if(latest && latest->codec)
		return latest->codec;

	return NULL;
}

SQ_LibraryHandler* SQ_LibraryHandler::instance()
{
	return hand;
}
