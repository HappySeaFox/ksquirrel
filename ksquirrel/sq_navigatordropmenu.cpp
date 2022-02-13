/***************************************************************************
                          sq_navigatordropmenu.cpp  -  description
                             -------------------
    begin                : ??? Feb 23 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <klocale.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kio/job.h>

#include "sq_navigatordropmenu.h"

SQ_NavigatorDropMenu * SQ_NavigatorDropMenu::m_instance= 0;

SQ_NavigatorDropMenu::SQ_NavigatorDropMenu(QObject *parent) : QObject(parent)
{
    m_instance = this;

    KActionCollection *ac = new KActionCollection(0, this, "Actions for drop menu");

    dropmenu = new KPopupMenu(0);

    // "copy" action
    KAction *pACopy = new KAction(i18n("Copy here"), "editpaste", 0, this, SLOT(slotCopy()), ac, "sq_copy");

    // "move" action
    KAction *pAMove = new KAction(i18n("Move here"), 0, 0, this, SLOT(slotMove()), ac, "sq_move");

    // "link" action
    KAction *pALink = new KAction(i18n("Link here"), "www", 0, this, SLOT(slotLink()), ac, "sq_link");

    // "cancel" action, this action will do nothing - just close popup menu
    KAction *pACancel = new KAction(i18n("Cancel"), 0, 0, 0, 0, ac, "sq_cancel");

    // plug all actions to popup menu
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
    // use KIO to copy
    KIO::copy(list, url);
}

void SQ_NavigatorDropMenu::slotMove()
{
    KIO::move(list, url);
}

void SQ_NavigatorDropMenu::slotLink()
{
    KIO::link(list, url);
}

/*
 *  Save destination url and urls of dropped files.
 */
void SQ_NavigatorDropMenu::setupFiles(const KURL::List &l, const KURL &u)
{
    list = l;
    url = u;
}

/*
 *  Show popup menu with available actions.
 */
void SQ_NavigatorDropMenu::exec(const QPoint &pos)
{
    dropmenu->exec(pos);
}

#include "sq_navigatordropmenu.moc"
