/***************************************************************************
                          sq_diroperator.h  -  description
                             -------------------
    begin                : Thu Nov 29 2007
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

#ifndef SQ_DIROPERATOR_H
#define SQ_DIROPERATOR_H

#include <kfileitem.h>

#include <qobject.h>

class SQ_Downloader;

class KURL;

class SQ_DirOperator : public QObject
{
    Q_OBJECT

    public:
        SQ_DirOperator(QObject *parent = 0);
        ~SQ_DirOperator();

        static SQ_DirOperator* instance() { return m_inst; }

        void execute(KFileItem *item);

        void del(const KURL &u, QWidget *parent = 0);

    private:
        void executePrivate(KFileItem *);

    private slots:

        void slotDownloadPercents(int);
        void slotDownloaderResult(const KURL &);

    private:
        SQ_Downloader *down;

        static SQ_DirOperator *m_inst;
};

#endif
