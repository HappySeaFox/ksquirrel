/***************************************************************************
                          sq_converter.cpp  -  description
                             -------------------
    begin                : ??? Feb 24 2005
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

#include <qapplication.h>
#include <qfile.h>
#include <qcheckbox.h>

#include <kmessagebox.h>
#include <kprogress.h>
#include <kstringhandler.h>
#include <klocale.h>

#include <setjmp.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_libraryhandler.h"
#include "sq_convertfiles.h"
#include "sq_converter.h"

SQ_Converter * SQ_Converter::conv = 0L;

SQ_Converter::SQ_Converter() : SQ_EditBase()
{
	conv = this;
}

SQ_Converter::~SQ_Converter()
{}

void SQ_Converter::startEditPrivate()
{
	convert = new SQ_ConvertFiles(KSquirrel::app());
	convert->setCaption(i18n("Convert 1 file", "Convert %n files", files.count()));

	connect(convert, SIGNAL(convert(const QString &)), this, SLOT(slotStartConvert(const QString &)));
	connect(this, SIGNAL(convertText(const QString &, bool)), convert, SLOT(slotDebugText(const QString &, bool)));
	connect(this, SIGNAL(oneFileProcessed()), convert, SLOT(slotOneProcessed()));

	convert->progress->setProgress(0);
	convert->progress->setTotalSteps(files.count());

	convert->exec();
}

void SQ_Converter::slotStartConvert(const QString &libname)
{
	SQ_LIBRARY 		*lr, *lw;
	int 					i, j;
	fmt_info			finfo;
	QString			name;
	jmp_buf			jmp;
	static RGBA		*image = 0L;
	RGBA				*scan;
	int					errors, current;
	fmt_writeoptions	opt;
	QString 			putto;
	bool				replace = convert->checkReplace->isChecked();

	QValueList<QString>::iterator   BEGIN = files.begin();
	QValueList<QString>::iterator   END = files.end();

	convert->startConvertion();
	putto = convert->putTo();

	for(QValueList<QString>::iterator it = BEGIN;it != END;++it)
	{
		QFileInfo ff(*it);

		emit convertText(i18n("Converting ") + KStringHandler::rsqueeze(ff.fileName()) + QString(" ... "), false);
//	convert->textResult->append(i18n("Converting ") + KStringHandler::rsqueeze(ff.fileName()) + QString(" ... "));

		if(SQ_LibraryHandler::instance()->supports(*it))
		{
			lr = SQ_LibraryHandler::instance()->latestLibrary();
			lw = SQ_LibraryHandler::instance()->libraryByName(libname);

			if(!lr || !lw)
			{
				emit convertText(err_internal, true);
				emit oneFileProcessed();
				continue;
			}

			name = QFile::encodeName(*it);
			const char *s = name.ascii();

			i = lr->codec->fmt_init(s);

			if(setjmp(jmp))
			{
				lr->codec->fmt_close();

				emit convertText(messages[i], true);
				emit oneFileProcessed();

				continue;
			}

			if(i != SQERR_OK)
				longjmp(jmp, 1);

			errors = 0;
			current = 0;

			while(true)
			{
//				finfo->image = (fmt_image *)realloc(finfo->image, sizeof(fmt_image) * (finfo->images+1));
//				memset(&finfo->image[current], 0, sizeof(fmt_image));

//				name = adjustFileName(*it, lw->filter);
//				printf("name: %s\n", name.ascii());

				i = lr->codec->fmt_next();

				qApp->processEvents();

				finfo = lr->codec->information();

				if(i != SQERR_OK)
				{
					if(i == SQERR_NOTOK)
					{
						if(current == 1)
							name = adjustFileName(*it, lw->filter, putto);
						else
							name = adjustFileName(*it, lw->filter, putto, true, current);

						i = lw->codec->fmt_writeimage(name.ascii(), image, finfo.image[current-1].w, finfo.image[current-1].h, opt);

						emit convertText((errors) ? (i18n("1 error", "%n errors", errors)+"\n") : messages[i], true);
						emit oneFileProcessed();

						qApp->processEvents();

						if(replace)
						{
							emit convertText(i18n("Removing ") + KStringHandler::rsqueeze(ff.fileName()) + QString(" ... "), false);

							bool b = QFile::remove(*it);

							emit convertText((b) ? messages[SQERR_OK] : err_failed, true);
							emit oneFileProcessed();

							qApp->processEvents();
						}

						break;
					}
					else
						longjmp(jmp, 1);
				}

				if(current)
				{
					name = adjustFileName(*it, lw->filter, putto, true, current);
					i = lw->codec->fmt_writeimage(name.ascii(), image, finfo.image[current-1].w, finfo.image[current-1].h, opt);
//					emit convertText((errors) ? (i18n("1 error", "%n errors", errors)+"\n") : messages[i], true);
//					emit oneFileProcessed();
//					qApp->processEvents();
				}

//				printf("%dx%d@%d ...\n", finfo.image[current].w, finfo.image[current].h, finfo.image[current].bpp);
				image = (RGBA*)realloc(image, finfo.image[current].w * finfo.image[current].h * sizeof(RGBA));

				if(!image)
				{
					i = SQERR_NOMEMORY;
					longjmp(jmp, 1);
				}

				memset(image, 255, finfo.image[current].w * finfo.image[current].h * sizeof(RGBA));

//				printf("FOR: %dx%d@%d\n", finfo.image[0].w, finfo.image[0].h, finfo.image[0].bpp);
//				printf("passes: %d\n", finfo.image[0].passes);

				for(int pass = 0;pass < finfo.image[current].passes;pass++)
				{
					lr->codec->fmt_next_pass();

					for(j = 0;j < finfo.image[current].h;j++)
					{
						scan = image + j * finfo.image[current].w;
						i = lr->codec->fmt_read_scanline(scan);
						errors += (int)(i != SQERR_OK);
					}
				}

				if(finfo.image[current].needflip)
					fmt_utils::flip((char*)image, finfo.image[current].w * sizeof(RGBA), finfo.image[current].h);

				convert->fillWriteOptions(&opt, lw->opt);

				opt.alpha = finfo.image[current].hasalpha;

				current++;
			}

			lr->codec->fmt_close();

		}
		else
		{
			emit convertText(messages[SQERR_NOTSUPPORTED], true);
			emit oneFileProcessed();
		}

		finfo.image.clear();
		finfo.meta.clear();
	}

	if(image)
	{
		free(image);
		image = 0L;
	}
}

SQ_Converter* SQ_Converter::instance()
{
	return conv;
}
