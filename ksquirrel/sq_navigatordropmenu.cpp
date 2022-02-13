/***************************************************************************
                          sq_navigatordropmenu.cpp  -  description
                             -------------------
    begin                : ??? Feb 23 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <klocale.h>
#include <kaction.h>
#include <kio/job.h>
#include <kstringhandler.h>

#include "ksquirrel.h"
#include "sq_popupmenu.h"
#include "sq_navigatordropmenu.h"

SQ_NavigatorDropMenu * SQ_NavigatorDropMenu::m_instance= 0;

SQ_NavigatorDropMenu::SQ_NavigatorDropMenu(QObject *parent) : QObject(parent)
{
    m_instance = this;

    KActionCollection *ac = new KActionCollection(0, this, "Actions for drop menu");

    dropmenu = new SQ_PopupMenu(0);

    // "copy" action
    KAction *pACopy = new KAction(i18n("Copy here"), "editpaste", 0, this, SLOT(slotCopy()), ac, "sq_copy");

    // "move" action
    KAction *pAMove = new KAction(i18n("Move here"), 0, 0, this, SLOT(slotMove()), ac, "sq_move");

    // "link" action
    KAction *pALink = new KAction(i18n("Link here"), "www", 0, this, SLOT(slotLink()), ac, "sq_link");

    // "cancel" action, this action will do nothing - just close popup menu
    KAction *pACancel = new KAction(i18n("Cancel"), 0, 0, 0, 0, ac, "sq_cancel");

    // plug all actions to popup menu
    dropmenu->insertTitle(QString::null);
    pACopy->plug(dropmenu);
    pAMove->plug(dropmenu);
    pALink->plug(dropmenu);
    dropmenu->insertSeparator();
    pACancel->plug(dropmenu);
}

SQ_NavigatorDropMenu::~SQ_NavigatorDropMenu()
{
    delete dropmenu;
}

void SQ_NavigatorDropMenu::slotCopy()
{
    if(also) emit done(url, SQ_NavigatorDropMenu::Copy);

    KIO::Job *job = KIO::copy(list, url);
    connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotJobResult(KIO::Job *)));
}

void SQ_NavigatorDropMenu::slotMove()
{
    if(also) emit done(url, SQ_NavigatorDropMenu::Move);

    KIO::Job *job = KIO::move(list, url);
    connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotJobResult(KIO::Job *)));
}

void SQ_NavigatorDropMenu::slotLink()
{
    if(also) emit done(url, SQ_NavigatorDropMenu::Link);

    KIO::Job *job = KIO::link(list, url);
    connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotJobResult(KIO::Job *)));
}

void SQ_NavigatorDropMenu::slotJobResult(KIO::Job *job)
{
    if(job && job->error())
        job->showErrorDialog(KSquirrel::app());
}

/*
 *  Save destination url and urls of dropped files.
 */
void SQ_NavigatorDropMenu::setupFiles(const KURL::List &l, const KURL &u)
{
    list = l;
    url = u;

    dropmenu->changeTitle(KStringHandler::lsqueeze(u.isLocalFile() ? u.path() : u.prettyURL(), 20));
}

/*
 *  Show popup menu with available actions.
 */
void SQ_NavigatorDropMenu::exec(const QPoint &pos, bool _also)
{
    also = _also;
    dropmenu->exec(pos);
}

#include "sq_navigatordropmenu.moc"
