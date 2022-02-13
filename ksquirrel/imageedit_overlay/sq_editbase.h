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

#include <sstream>
#include <csetjmp>

#include "fmt_types.h"
#include "fmt_utils.h"
#include "fileio.h"
#include "fmt_codec_base.h"
#include "error.h"

#include "sq_imageedit.h"

struct SQ_LIBRARY;

class SQ_EditBase : public QObject
{
	Q_OBJECT

	public: 
		SQ_EditBase();
		virtual ~SQ_EditBase();

	protected:
		virtual void startEditPrivate() = 0;
		virtual void setWritingLibrary() = 0;
		virtual void dialogReset() = 0;
		virtual void dialogAdditionalInit() = 0;
		virtual int manipDecodedImage(SQ_LIBRARY *lw, const QString &name, RGBA *image,
											const fmt_image &im, const fmt_writeoptions &opt) = 0;

		void decodingCycle();

		void errorjmp(jmp_buf jmp, const int code);

		QString adjustFileName(const QString &globalprefix, const QString &name,
									QString putto, bool paged = false, int page = 0);

	signals:
		void convertText(const QString &, bool);
		void oneFileProcessed();
		void done(bool);

	protected slots:
		void slotStartEdit();

	protected:
		QStringList			files;
		QString				err_internal, err_failed;
		int					error_code;
		SQ_ImageOptions	imageopt;
		fmt_writeoptions		opt;
		SQ_LIBRARY		*lr, *lw;
		QString				special_action;
		bool				ondisk;
};

#endif
