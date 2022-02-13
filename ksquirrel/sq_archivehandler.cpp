/***************************************************************************
                          sq_archivehandler.cpp  -  description
                             -------------------
    begin                : ??? ??? 26 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@uandex.ru
 ***************************************************************************/
/***************************************************************************
                          extract.cpp  -  description
                             -------------------
    begin                : Sat Dec 1 2001
    copyright            : (C) 2001 by Richard Groult, 2003 OGINO Tomonori
    email                : rgroult@jalix.org ogino@nn.iij4u.or.jp
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
#include <qcursor.h>

#include <kprocess.h>
#include <kzip.h>
#include <kar.h>
#include <ktar.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "sq_archivehandler.h"
#include "sq_dir.h"

SQ_ArchiveHandler::SQ_ArchiveHandler(QObject * parent, const char *name) : QObject(parent, name)
{
	dir = new SQ_Dir;
	dir->setRoot("extracts");
}

SQ_ArchiveHandler::~SQ_ArchiveHandler()
{}

bool SQ_ArchiveHandler::supported(const QString filename)
{
	return 	filename.lower().endsWith(".zip") ||
			filename.lower().endsWith(".tar") ||
			filename.lower().endsWith(".tar.gz") ||
			filename.lower().endsWith(".gz") ||
			filename.lower().endsWith(".bz2") ||
			filename.lower().endsWith(".tar.bz2") ||
			filename.lower().endsWith(".tgz") ||
			filename.lower().endsWith(".ar");
}

void SQ_ArchiveHandler::setPath(const QString _path)
{
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
		KMessageBox::error(0, "<qt>" + i18n("Unable to create directory: %1").arg(extracteddir)+"</qt>", i18n("Archive problem"));
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

	if (file.lower().endsWith("zip"))
		arc = new KZip(fullpath);
	else if(file.lower().endsWith("tar") || file.lower().endsWith("tar.gz") || file.lower().endsWith("tar.bz2") || file.lower().endsWith("tgz") || file.lower().endsWith("gz") || file.lower().endsWith("bz2"))
		arc = new KTar(fullpath);
	else if(file.lower().endsWith("ar"))
		arc = new KAr(fullpath);

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
			{
				fprintf(stderr, "SIGNAL WAS CAUGHT: \n");
			}

			QApplication::restoreOverrideCursor();
		}
		else
                    return false;

		delete arc;
	}
	else
	{
		KMessageBox::error(0, "<qt>" + i18n("Unable to open the archive '<b>%1</b>'.").arg(fullpath)+"</qt>", i18n("Archive problem"));
		return false;
	}

	return true;
}

bool SQ_ArchiveHandler::needClean()
{
	return dir->exists(dir->getRoot());
}

void SQ_ArchiveHandler::clean(QString s)
{
	QString sssssssss = QString("rm -rf %1").arg((s==QString::null)?dir->getRoot():s);
	KShellProcess p;

	printf("cleaning \"%s\" ...\n", sssssssss.ascii());

	p << sssssssss;
//	p.start(KProcess::Block);
}
