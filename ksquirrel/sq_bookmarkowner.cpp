/***************************************************************************
                          sq_bookmarkowner.cpp  -  description
                             -------------------
    begin                : ??? ??? 27 2004
    copyright            : (C) 2004 by Baryshev Dmitry
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

#include "sq_bookmarkowner.h"

SQ_BookmarkOwner * SQ_BookmarkOwner::sing = NULL;

SQ_BookmarkOwner::SQ_BookmarkOwner(QWidget *parent) : QObject(parent)
{
    sing = this;
}

SQ_BookmarkOwner::~SQ_BookmarkOwner()
{}

/*
 *  User selected some bookmark. It will emit 'openURL' signal,
 *  and SQ_WidgetStack will catch it.
 */
void SQ_BookmarkOwner::openBookmarkURL(const QString &url)
{
    emit openURL(KURL(url));
}

/*
 *  Current url. If user selected "Add bookmark", this
 *  url will be added to bookmarks. See also KBookmarkOwner::setURL()
 */
QString SQ_BookmarkOwner::currentURL() const
{
    return URL.path();
}

/*
 *  Set current url
 */
void SQ_BookmarkOwner::setURL(const KURL &new_url)
{
    URL = new_url;
}

SQ_BookmarkOwner* SQ_BookmarkOwner::instance()
{
    return sing;
}
