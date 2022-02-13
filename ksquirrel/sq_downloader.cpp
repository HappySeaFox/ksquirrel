/***************************************************************************
                          sq_downloader.cpp  -  description
                             -------------------
    begin                : Fri Jun 07 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#include <qfile.h>
#include <qfileinfo.h>

#include <kio/job.h>
#include <kfileitem.h>
#include <kstandarddirs.h>

#include "sq_libraryhandler.h"
#include "sq_archivehandler.h"
#include "sq_downloader.h"

#define SQ_PREDOWNLOAD_SIZE 50

SQ_Downloader::SQ_Downloader(QObject *parent, const char *name) : QObject(parent, name), job(0), m_error(false)
{}

SQ_Downloader::~SQ_Downloader()
{
    clean();
}

void SQ_Downloader::start(KFileItem *fi)
{
    m_error = false;
    mURL = fi->url();

    totalSize = fi->size();
    mime = fi->mimetype();

    job = KIO::get(mURL, false, false);

    QFileInfo fm(mURL.path());

    clean();

    tmp = locateLocal("tmp", fm.fileName());

    continueDownload = false;

    connect(job, SIGNAL(data(KIO::Job *, const QByteArray &)),
            this, SLOT(slotData(KIO::Job *, const QByteArray &)));
    connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotDataResult(KIO::Job *)));
}

void SQ_Downloader::slotData(KIO::Job *job, const QByteArray &data)
{
    size += data.size();

    QFile f(tmp);

    if(f.open(IO_WriteOnly | IO_Append))
    {
        f.writeBlock(data);
        f.close();
    }

    // 50 bytes are enough to determine file type
    if(size >= SQ_PREDOWNLOAD_SIZE && !continueDownload && totalSize != size)
    {
        // cancel download (file type is not supported)
        SQ_LIBRARY *lib = SQ_LibraryHandler::instance()->libraryForFile(tmp);

        if(SQ_ArchiveHandler::instance()->findProtocolByMime(mime).isEmpty()
            && !lib)
        {
            job->kill(false); // kill job & emit result
        }
        else
        {
            // nice, we can open this image/archive - continue download
            continueDownload = true;
        }
    }
}

void SQ_Downloader::slotDataResult(KIO::Job *cpjob)
{
    job = 0;

    // job error
    if(cpjob->error() && cpjob->error() != KIO::ERR_USER_CANCELED)
    {
        m_error = true;
        emit result(mEmptyURL);
    }
    else if(cpjob->error() == KIO::ERR_USER_CANCELED) // not supported image/archive type - 
                                                      // emit empty url without errors
    {
        emit result(mEmptyURL);
    }
    else // supported image type/archive type (no errors or job killed)
    {
        emit result(KURL::fromPathOrURL(tmp));
    }
}

void SQ_Downloader::clean()
{
    if(!tmp.isEmpty())
        QFile::remove(tmp);
}

void SQ_Downloader::kill()
{
    if(job) job->kill();
}

#include "sq_downloader.moc"
