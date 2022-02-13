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

class KTempFile;

class SQ_EditBase : public QObject
{
	Q_OBJECT

	public: 
		SQ_EditBase();
		virtual ~SQ_EditBase();

	protected:
		virtual int manipDecodedImage(fmt_image *im) = 0;
		virtual void startEditPrivate() = 0;
		virtual void dialogReset() = 0;

		virtual int manipAndWriteDecodedImage(const QString &name, fmt_image *im);
		virtual int determineNextScan(const fmt_image &im, RGBA *scan, int y);

		virtual void initWriteOptions();
		virtual void setWritingLibrary();
		virtual void cycleDone();

		void decodingCycle();

		QImage generatePreview() const;

        private:
		QString adjustFileName(const QString &globalprefix, const QString &name, int replace,
					QString putto, bool paged = false, int page = 0);
		int copyFile(const QString &src, const QString &dst) const;
		void errorjmp(jmp_buf jmp, const int code);

	signals:
		void convertText(const QString &, bool);
		void oneFileProcessed();
		void done(bool);

	protected slots:
		void slotStartEdit();

	protected:
		QStringList			files;
		QString				err_internal, err_failed;
		QString				currentFile;
		int				error_code, current_page;
		SQ_ImageOptions	                imageopt;
		fmt_writeoptions		opt;
		SQ_LIBRARY		        *lr, *lw;
		RGBA				*image;
		QString				special_action, prefix;
		bool				ondisk, multi, last, lastFrame;
		KTempFile			*tempfile;
		QString 			altlibrary;
                fmt_image                       *im;
};

#endif
