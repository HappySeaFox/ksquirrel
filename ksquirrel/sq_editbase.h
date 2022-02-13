/***************************************************************************
                          sq_editbase.h  -  description
                             -------------------
    begin                : ??? Mar 3 2005
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

#ifndef SQ_EDITBASE_H
#define SQ_EDITBASE_H

#include <qobject.h>
#include <qstringlist.h>
#include <qmap.h>

#include <sstream>

#include "fmt_types.h"
#define SQ_FIO_NO_IMPLEMENT
#include "fmt_codec_base.h"
#include "fmt_utils.h"
#include "error.h"

/**
  *@author Baryshev Dmitry
  */

class SQ_EditBase : public QObject
{
	Q_OBJECT

	public: 
		SQ_EditBase();
		~SQ_EditBase();

	protected:
		QString adjustFileName(const QString &name, const QString &filter, QString putto, bool paged = false, int page = 0);

		virtual void startEditPrivate() = 0;

	signals:
		void convertText(const QString &, bool);
		void oneFileProcessed();

	protected slots:
		void slotStartEdit();

	protected:
		QStringList			files;
		QMap<int, QString> messages;
		QString				err_notsupported, err_internal, err_failed;
};

#endif
