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

#include <klocale.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kio/job.h>

#include "sq_navigatordropmenu.h"

SQ_NavigatorDropMenu * SQ_NavigatorDropMenu::app = 0;

SQ_NavigatorDropMenu::SQ_NavigatorDropMenu() : QObject()
{
	app = this;

	KActionCollection *ac = new KActionCollection(NULL, this, "SQ_NavigatorDropMenu");

	dropmenu = new KPopupMenu(NULL);

	KAction *pACopy = new KAction(i18n("Copy here"), "editpaste", 0, this, SLOT(slotCopy()), ac, "sq_copy");
	KAction *pAMove = new KAction(i18n("Move here"), "editpaste", 0, this, SLOT(slotMove()), ac, "sq_move");
	KAction *pALink = new KAction(i18n("Link here"), "www", 0, this, SLOT(slotLink()), ac, "sq_link");
	KAction *pACancel = new KAction(i18n("Cancel"), "cancel", 0, 0, 0, ac, "sq_cancel");

	pACopy->plug(dropmenu);
	pAMove->plug(dropmenu);
	pALink->plug(dropmenu);
	dropmenu->insertSeparator();
	pACancel->plug(dropmenu);
}

SQ_NavigatorDropMenu::~SQ_NavigatorDropMenu()
{}

void SQ_NavigatorDropMenu::slotCopy()
{
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

SQ_NavigatorDropMenu* SQ_NavigatorDropMenu::instance()
{
	return app;
}

void SQ_NavigatorDropMenu::setupFiles(const KURL::List &l, const KURL &u)
{
	list = l;
	url = u;
}

void SQ_NavigatorDropMenu::exec(const QPoint &pos)
{
	dropmenu->exec(pos);
}
