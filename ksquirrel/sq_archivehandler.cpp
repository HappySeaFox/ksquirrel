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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qapplication.h>
#include <qcursor.h>
#include <qfile.h>

#include <kzip.h>
#include <kar.h>
#include <ktar.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kprocess.h>
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_archivehandler.h"
#include "sq_dir.h"

SQ_ArchiveHandler * SQ_ArchiveHandler::m_instance = 0;

SQ_ArchiveHandler::SQ_ArchiveHandler(QObject * parent, const char *name) : QObject(parent, name)
{
    m_instance = this;
    dir = new SQ_Dir(SQ_Dir::Extracts);

    // fill the map with protocols
    protocols.insert("application/x-bzip", 0);
    protocols.insert("application/x-bzip2", 1);
    protocols.insert("application/x-gzip", 2);
    protocols.insert("application/x-tar", 3);
    protocols.insert("application/x-tbz", 4);
    protocols.insert("application/x-tgz", 5);
    protocols.insert("application/x-zip", 6);

    // find "unrar" executable to handle RAR archives.
    // we will execute "unrar x <archive.rar> <extract_path>"
    // via KShellProcess
    QString unrar = KStandardDirs::findExe("unrar");

    if(!unrar.isEmpty())
        protocols.insert("application/x-rar", 7);
}

SQ_ArchiveHandler::~SQ_ArchiveHandler()
{
    delete dir;
}

/*
 *  Find protocol number by its name.
 * 
 *  For example findProtocolByName(""application/x-bzip"") will
 *  return 0.
 */
int SQ_ArchiveHandler::findProtocolByName(const QString &prot)
{
    // find protocol
    QMap<QString, int>::iterator it = protocols.find(prot);

    // return protocol number, if found, and -1 otherwise
    return (it == protocols.end()) ? (-1) : it.data();
}

/*
 *  Find protocol number by KFileItem's mimetype.
 */
int SQ_ArchiveHandler::findProtocolByFile(KFileItem *item)
{
    // determine mimetype
    QString m = item->mimetype();

    // use findProtocolByName() to find
    // protocol number
    return findProtocolByName(m);
}

/*
 *  Store fileitem, which we should be unpacked.
 *  Also store full path to it.
 */
void SQ_ArchiveHandler::setFile(KFileItem *_item)
{
    item = _item;
    QString _path = item->url().path();

    // store full path
    fullpath = _path;
}

/*
 *  Return fileitem's full path.
 * 
 *  For example, "/home/krasu/arc.zip"
 */
QString SQ_ArchiveHandler::itemPath() const
{
    return fullpath;
}

/*
 *  Get full path to extracted directory.
 *
 *  For example, after unpacking /home/krasu/arc.zip this method will return
 *  "/home/krasu/.ksquirrel/extracts/home/krasu/arc.zip".
 */
QString SQ_ArchiveHandler::itemExtractedPath() const
{
    return fullextracteddir;
}

/*
 *  Unpack an archive.
 *
 *  Use 'fullpath' as a path to archive.
 */
bool SQ_ArchiveHandler::unpack()
{
    QString ss;
    extracteddir = fullpath;
    fullextracteddir = dir->absPath(extracteddir);

    // Check if directory already exists and needs update
    if(dir->fileExists(extracteddir, ss))
    {
        if(dir->updateNeeded(extracteddir))
            clean(ss); // clean it!
        else
            return true; // Archive already been unpacked,
    }                    // and was not changed. 

    // try to create new directory in temporary directory
    if(!dir->mkdir(extracteddir))
    {
        KMessageBox::error(KSquirrel::app(), QString::fromLatin1("<qt>") + i18n("Unable to create directory: %1").arg(extracteddir)+("</qt>"), i18n("Archive problem"));
        return false;
    }

    QFile::Offset big = 0x4000000;
    QFile qfile(fullpath);

    // ooh, archive file is too big!
    if(qfile.size() > big)
    {
        QString msg = i18n("The size of selected archive seems to be too big;\ncontinue? (size: %1MB)").arg((qfile.size()) >> 20);

        if(KMessageBox::warningContinueCancel(KSquirrel::app(), msg, i18n("Confirm")) == KMessageBox::Cancel)
            return false;
    }

    KArchive *arc = 0;

    // find protocol number
    switch(findProtocolByFile(item))
    {
        // RAR
        case 7:
        {
            KShellProcess   unrar;

            unrar << "unrar" << "x" << KShellProcess::quote(fullpath) << KShellProcess::quote(fullextracteddir);

            // show 'busy' cursor
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            bool ok = unrar.start(KProcess::Block);

            // restore cursor
            QApplication::restoreOverrideCursor();

            return ok;
        }
        break;

        // archive is ZIP
        case 6:
            arc = new KZip(fullpath);
        break;

        // archive is TAR (.tar.gz, .tgz, ...)
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            arc = new KTar(fullpath);
        break;

        // protocol not found!
        default:
            arc = 0;
    }

    if(arc)
    {
        // try open
        if(arc->open(IO_ReadOnly))
        {
            // show 'busy' cursor
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            const KArchiveDirectory *adir = arc->directory();

            if(!adir)
                return false;

            // unpack it!
            adir->copyTo(fullextracteddir);

            // restore cursor
            QApplication::restoreOverrideCursor();
        }
        else // can't open archive!
            return false;

        delete arc;
    }
    else
    {
        KMessageBox::error(KSquirrel::app(), QString::fromLatin1("<qt>") 
                                            + i18n("Unable to open the archive '<b>%1</b>'.").arg(fullpath)
                                            + QString::fromLatin1("</qt>")

                                            ,i18n("Archive problem"));
        return false;
    }

    return true;
}

/*
 *  Does temporary directory need to be cleaned ?
 *
 *  Temporary directory is ususally ~/.ksquirrel/extracts/.
 */
bool SQ_ArchiveHandler::needClean()
{
    // TODO: find items in ~/.ksquirrel/extracts !
    return dir->exists(dir->root());
}

/*
 *  Remove specified directory from temporary directory.
 *
 *  For example, clean("/home/krasu/arc.zip") will remove
 *  /home/krasu/.ksquirrel/extracts/home/krasu/arc.zip.
 */
void SQ_ArchiveHandler::clean(QString s)
{
    // We will use KShellProcess to delete directory recursively
    KShellProcess   del;

    // hehe, rm -rf
    del << "rm -rf " << KShellProcess::quote(s) << "/*";

    kdDebug() << "SQ_ArchiveHandler: cleaning archive " << s << "..." << endl;

    // start process!
    bool removed = del.start(KProcess::Block);

    kdDebug() << "SQ_ArchiveHandler: " << (removed ? "OK" : "error") << endl;
}
