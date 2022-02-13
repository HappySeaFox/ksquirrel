/***************************************************************************
                          sq_editbase.cpp  -  description
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

#include <qfile.h>

#include <kmessagebox.h>
#include <klocale.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_libraryhandler.h"

#define SQ_HAVE_FMT_UTILS
#include "sq_editbase.h"

SQ_EditBase::SQ_EditBase() : QObject()
{
	messages[SQERR_OK] = QString("OK\n");
	messages[SQERR_NOMEMORY] = i18n("no memory\n");
	messages[SQERR_BADFILE] = i18n("file corrupted\n");
	messages[SQERR_NOFILE] = i18n("can't open file for writing\n");
	messages[SQERR_NOTOK] = i18n("file corrupted\n");
	err_notsupported = i18n("not supported\n");
	err_internal = i18n("internal error\n");
	err_failed = i18n("failed\n");
}

SQ_EditBase::~SQ_EditBase()
{}

void SQ_EditBase::slotStartEdit()
{
	files.clear();

	KFileItemList *items = (KFileItemList *)SQ_WidgetStack::instance()->selectedItems();

	if(!items) return;
	if(!items->count())
	{
		KMessageBox::information(KSquirrel::app(), i18n("Select files to edit"));
		return;
	}

	KFileItem *i = items->first();

	for(;i;i = items->next())
	{
		if(i->isFile())
			files.append(i->url().path());
	}

	if(!files.count())
	{
		KMessageBox::information(KSquirrel::app(), i18n("Select files to edit"));
		return;
	}

	startEditPrivate();
}

QString SQ_EditBase::adjustFileName(const QString &name, const QString &filter, QString putto, bool paged, int page)
{
	QFileInfo ff(name);
	QString result, inner;
	QString ext = ff.extension(false);
	QString prefix, suffix, name2 = name;
	QString spage = QString::fromLatin1("page_%1.").arg((QString::fromLatin1("%1").arg(page)).rightJustify(3, '0'));

	if(!putto.isNull() && !putto.isEmpty())
	{
		if(QFile::exists(putto))
			name2 = putto + "/" + ff.fileName();
	}

	prefix = name2;

	prefix.truncate(name2.length() - ext.length());

	if(SQ_LibraryHandler::instance()->knownExtension(QString::fromLatin1("*.")+ext))
		suffix = filter.section("*.", 0, 0, QString::SectionSkipEmpty).stripWhiteSpace();
	else
		suffix = ext;

	int f = 1;
	while(true)
	{
		result = (!paged) ? (prefix + inner + suffix) : (prefix + spage + inner + suffix);

		if(QFile::exists(result))
		{
			inner = QString::fromLatin1("%1.").arg(f);
			f++;
		}
		else
			break;
	}

	return QFile::encodeName(result);
}
