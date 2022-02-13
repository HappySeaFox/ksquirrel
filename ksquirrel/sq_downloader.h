/***************************************************************************
                          sq_downloader.h  -  description
                             -------------------
    begin                : Fri Jun 07 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_DOWNLOADER_H
#define SQ_DOWNLOADER_H

#include <qobject.h>
#include <qdatetime.h>

#include <kurl.h>

class KFileItem;
class KTempFile;

namespace KIO { class Job; }

class SQ_Downloader : public QObject
{
    Q_OBJECT

    public:
        SQ_Downloader(QObject *parent = 0, const char *name = 0);
        ~SQ_Downloader();

        void start(KFileItem *fi);

        bool error() const;

        void clean();

        void kill();

    signals:
        void result(const KURL &);
        void percents(int);

    private slots:
        void slotData(KIO::Job *job, const QByteArray &data);
        void slotDataResult(KIO::Job *);

    private:
        KIO::Job *job;
        KURL mEmptyURL, mURL;
        KTempFile *tmp;
        KIO::filesize_t totalSize, size;
        bool continueDownload;
        bool nomime;
        bool m_error, m_lightmode;
        QTime startTime;
        bool emitPercents;
};

inline
bool SQ_Downloader::error() const
{
    return m_error;
}

#endif
