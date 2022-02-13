/***************************************************************************
                          sq_archivehandler.cpp  -  description
                             -------------------
    begin                : ??? ??? 26 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
 ***************************************************************************/

/* Originally taken from ShowImg */

/***************************************************************************
                          extract.cpp  -  description
                             -------------------
    begin                : Sat Dec 1 2001
    copyright            : (C) 2001 by Richard Groult, 2003 OGINO Tomonori
    email                : rgroult@jalix.org ogino@nn.iij4u.or.jp
***************************************************************************

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcursor.h>
#include <qfile.h>

#include <kzip.h>
#include <kar.h>
#include <ktar.h>
#include <kmessagebox.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kprocess.h>
#include <kdebug.h>

#include "sq_archivehandler.h"
#include "sq_dir.h"

SQ_ArchiveHandler::SQ_ArchiveHandler(QObject * parent, const char *name) : QObject(parent, name)
{
	dir = new SQ_Dir;
	dir->setRoot(QString::fromLatin1("extracts"));

	protocols["application/x-bzip"] = 0;
	protocols["application/x-bzip2"] = 1;
	protocols["application/x-gzip"] = 2;
	protocols["application/x-tar"] = 3;
	protocols["application/x-tbz"] = 4;
	protocols["application/x-tgz"] = 5;
	protocols["application/x-zip"] = 6;
}

SQ_ArchiveHandler::~SQ_ArchiveHandler()
{}

int SQ_ArchiveHandler::findProtocolByName(const QString &prot)
{
	return (protocols.contains(prot))?protocols[prot]:(-1);
}

int SQ_ArchiveHandler::findProtocolByFile(KFileItem *item)
{
	QString m = item->mimetype();

	return (protocols.contains(m))?protocols[m]:(-1);
}

void SQ_ArchiveHandler::setFile(KFileItem *_item)
{
	item = _item;
	QString _path = item->url().path();
	QFileInfo f(_path);
	
	fullpath = _path;
	file = f.fileName();
}

QString SQ_ArchiveHandler::getPath() const
{
	return fullpath;
}

QString SQ_ArchiveHandler::getExtractedPath() const
{
	return fullextracteddir;
}

bool SQ_ArchiveHandler::unpack()
{
	QString ss;
	extracteddir = fullpath;
	fullextracteddir = dir->getAbsPath(extracteddir);

	if(dir->fileExists(extracteddir, ss))
		if(dir->updateNeeded(extracteddir))
			clean(ss);
		else
			return true;
	else;

	if(dir->mkdir(extracteddir) == false)
	{
		KMessageBox::error(0, QString::fromLatin1("<qt>") + i18n("Unable to create directory: %1").arg(extracteddir)+("</qt>"), i18n("Archive problem"));
		return false;
	}

	QFile::Offset big = 0x4000000;
	QFile qfile(fullpath);

	if(qfile.size() > big)
	{
		QString msg = QString(i18n("The size of selected archive seems to be too big;\ncontinue? (size: %1MB)")).arg((qfile.size()) >> 20);

		if(KMessageBox::warningContinueCancel(0, msg, i18n("Confirm")) == KMessageBox::Cancel)
			return false;
	}

	KArchive *arc = 0L;
	
	switch(findProtocolByFile(item))
	{
		case 6:
			arc = new KZip(fullpath);
		break;

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			arc = new KTar(fullpath);
		break;

		default:
			arc = 0L;
	}

	if(arc)
	{
		if(arc->open(IO_ReadOnly))
		{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        		const KArchiveDirectory *adir = arc->directory();

			if(!adir)
				return false;

			try{
				adir->copyTo(fullextracteddir);
			}
			catch(...)
			{}

			QApplication::restoreOverrideCursor();
		}
		else
                    return false;

		delete arc;
	}
	else
	{
		KMessageBox::error(0, QString::fromLatin1("<qt>") + i18n("Unable to open the archive '<b>%1</b>'.").arg(fullpath)+QString::fromLatin1("</qt>"), i18n("Archive problem"));
		return false;
	}

	return true;
}

bool SQ_ArchiveHandler::needClean()
{
	return dir->exists(dir->root());
}

void SQ_ArchiveHandler::clean(QString s)
{
	KShellProcess   del;

	del << "rm -rf " << KShellProcess::quote(s) << "/*";

	kdDebug() << "cleaning archive ... ";

	bool removed = del.start(KProcess::Block);

	kdDebug() << ((removed)?"OK":"error") << endl;
}
