/***************************************************************************
                          sq_bookmarkowner.h  -  description
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

#ifndef SQ_BOOKMARKOWNER_H
#define SQ_BOOKMARKOWNER_H

#include <kbookmarkmanager.h>
#include <kurl.h>

#include <qobject.h>
#include <qwidget.h>

/**
  *@author CKulT
  */

class SQ_BookmarkOwner : public QObject, public KBookmarkOwner
{
	Q_OBJECT

	public: 
		SQ_BookmarkOwner(QWidget *parent = 0);
		~SQ_BookmarkOwner();

		virtual void openBookmarkURL(const QString &);
		virtual QString currentURL() const;

	public slots:
		void setURL(const KURL &);

	signals:
		void openURL(const KURL &);

	private:
		KURL URL;
};

#endif
