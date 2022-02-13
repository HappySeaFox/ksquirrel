/***************************************************************************
                          sq_diroperator.cpp  -  description
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

#include <kurl.h>
#include <konq_operations.h>

#include "sq_diroperator.h"
#include "sq_libraryhandler.h"
#include "sq_downloader.h"
#include "sq_glwidget.h"

SQ_DirOperator * SQ_DirOperator::m_inst = 0;

SQ_DirOperator::SQ_DirOperator(QObject *parent) : QObject(parent)
{
    m_inst = this;

    down = new SQ_Downloader(this, "SQ_Downloader [dirop]");

    connect(down, SIGNAL(result(const KURL &)), this, SLOT(slotDownloaderResult(const KURL &)));
    connect(down, SIGNAL(percents(int)), this, SLOT(slotDownloadPercents(int)));
}

SQ_DirOperator::~SQ_DirOperator()
{}

void SQ_DirOperator::execute(KFileItem *fi)
{
    down->kill();

    SQ_GLWidget::window()->setOriginalURL(fi->url());

    if(fi->url().isLocalFile())
        executePrivate(fi);
    else if(SQ_LibraryHandler::instance()->maybeSupported(fi->url(), fi->mimetype()) != SQ_LibraryHandler::No)
        down->start(fi);
}

void SQ_DirOperator::slotDownloaderResult(const KURL &url)
{
    SQ_GLWidget::window()->setDownloadPercents(-1);

    if(url.isEmpty())
        return;    

    KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, url);

    executePrivate(&fi);
}

void SQ_DirOperator::executePrivate(KFileItem *fi)
{
    QString fullpath = fi->url().path();

    if(SQ_LibraryHandler::instance()->libraryForFile(fullpath))
        SQ_GLWidget::window()->startDecoding(fullpath);
}

void SQ_DirOperator::slotDownloadPercents(int p)
{
    SQ_GLWidget::window()->setDownloadPercents(p);
}

void SQ_DirOperator::del(const KURL &u, QWidget *parent)
{
    KURL::List list;
    list.append(u);

    KonqOperations::del(parent, KonqOperations::DEL, list);
}

#include "sq_diroperator.moc"
