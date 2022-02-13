/***************************************************************************
                          sq_bookmarkowner.cpp  -  description
                             -------------------
    begin                : ??? ??? 27 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@uandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sq_bookmarkowner.h"

SQ_BookmarkOwner::SQ_BookmarkOwner(QWidget *parent) : QObject(parent)
{}

SQ_BookmarkOwner::~SQ_BookmarkOwner()
{}

void SQ_BookmarkOwner::openBookmarkURL(const QString &url)
{
	emit openURL(KURL(url));
}

QString SQ_BookmarkOwner::currentURL() const
{
	return URL.path();
}

void SQ_BookmarkOwner::setURL(const KURL &new_url)
{
	URL = new_url;
}