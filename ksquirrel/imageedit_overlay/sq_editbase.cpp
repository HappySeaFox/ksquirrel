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

#include <qapplication.h>
#include <qfile.h>

#include <kmessagebox.h>
#include <kstringhandler.h>
#include <klocale.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_libraryhandler.h"
#include "sq_editbase.h"
#include "sq_errorstring.h"

SQ_EditBase::SQ_EditBase() : QObject()
{
	err_internal = i18n("internal error") + "\n";
	err_failed = i18n("failed") + "\n";

	special_action = i18n("Editing");
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

	if(special_action != i18n("Converting"))
		KMessageBox::information(KSquirrel::app(), "This tool is working in preview mode. It won't write images on disk.");

	startEditPrivate();
}

QString SQ_EditBase::adjustFileName(const QString &globalprefix, const QString &name, QString putto, bool paged, int page)
{
	QFileInfo ff(name);
	QString result, inner, filter = lw->filter;
	QString ext = ff.extension(false);
	QString prefix, suffix, name2 = name;
	QString spage = QString::fromLatin1("page_%1.").arg((QString::fromLatin1("%1").arg(page)).rightJustify(3, '0'));

	if(!putto.isEmpty())
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

void SQ_EditBase::errorjmp(jmp_buf jmp, const int code)
{
	error_code = code;
	longjmp(jmp, 1);
}

void SQ_EditBase::decodingCycle()
{
	int 				i, j;
	fmt_info			finfo;
	QString			name;
	jmp_buf			jmp;
	static RGBA		*image = NULL;
	RGBA			*scan;
	int				errors, gerrors = 0, current;
	QString 			putto;
	bool			replace = (imageopt.where_to_put == 1);
	fmt_image 		im;

	QValueList<QString>::iterator   BEGIN = files.begin();
	QValueList<QString>::iterator   END = files.end();

	dialogReset();
	putto = imageopt.putto;

	for(QValueList<QString>::iterator it = BEGIN;it != END;++it)
	{
		QFileInfo ff(*it);

		emit convertText(special_action + " " + KStringHandler::rsqueeze(ff.fileName()) + QString(" ... "), false);

		if(SQ_LibraryHandler::instance()->supports(*it))
		{
			lr = SQ_LibraryHandler::instance()->latestLibrary();
			setWritingLibrary();

			if(!lr || !lw)
			{
				gerrors++;
				emit convertText(err_internal, true);
				emit oneFileProcessed();
				continue;
			}

			name = QFile::encodeName(*it);
			const char *s = name.ascii();

			i = lr->codec->fmt_read_init(s);

			if(setjmp(jmp))
			{
				gerrors++;

				lr->codec->fmt_read_close();

				emit convertText(SQ_ErrorString::instance()->stringSN(error_code), true);
				emit oneFileProcessed();

				continue;
			}

			if(i != SQE_OK)
				errorjmp(jmp, i);

			errors = 0;
			current = 0;

			while(true)
			{
//				finfo->image = (fmt_image *)realloc(finfo->image, sizeof(fmt_image) * (finfo->images+1));
//				memset(&finfo->image[current], 0, sizeof(fmt_image));

//				name = adjustFileName(*it, lw->filter);
//				printf("name: %s\n", name.ascii());

				i = lr->codec->fmt_read_next();

				qApp->processEvents();

				finfo = lr->codec->information();

				im.w = finfo.image[current-1].w;
				im.h = finfo.image[current-1].h;

				if(i != SQE_OK)
				{
					if(i == SQE_NOTOK)
					{
						if(ondisk)
						{
							if(current == 1)
								name = adjustFileName(imageopt.prefix, *it, putto);
							else
								name = adjustFileName(imageopt.prefix, *it, putto, true, current);
						}
						else
							name = QString::null;

						i = manipDecodedImage(lw, name, image, im, opt);

						emit convertText((errors) ? (i18n("1 error", "%n errors", errors)+"\n") : SQ_ErrorString::instance()->stringSN(SQE_OK), true);
						emit oneFileProcessed();

						qApp->processEvents();

						if(replace && ondisk)
						{
							emit convertText(i18n("Removing ") + KStringHandler::rsqueeze(ff.fileName()) + QString(" ... "), false);

							bool b = QFile::remove(*it);

							emit convertText((b) ? SQ_ErrorString::instance()->stringSN(SQE_OK) : err_failed, true);
							emit oneFileProcessed();

							qApp->processEvents();
						}

						break;
					}
					else
						errorjmp(jmp, i);
				}

				if(current)
				{
					if(ondisk)
						name = adjustFileName(imageopt.prefix, *it, putto, true, current);
					else
						name = QString::null;

//					i = lw->codec->fmt_writeimage(name.ascii(), image, finfo.image[current-1].w, finfo.image[current-1].h, opt);
					i = manipDecodedImage(lw, name, image, im, opt);

//					emit convertText((errors) ? (i18n("1 error", "%n errors", errors)+"\n") : messages[i], true);
//					emit oneFileProcessed();
//					qApp->processEvents();
				}

//				printf("%dx%d@%d ...\n", finfo.image[current].w, finfo.image[current].h, finfo.image[current].bpp);
				image = (RGBA *)realloc(image, finfo.image[current].w * finfo.image[current].h * sizeof(RGBA));

				if(!image)
				{
					i = SQE_R_NOMEMORY;
					errorjmp(jmp, i);
				}

				memset(image, 255, finfo.image[current].w * finfo.image[current].h * sizeof(RGBA));

//				printf("FOR: %dx%d@%d\n", finfo.image[current].w, finfo.image[current].h, finfo.image[current].bpp);
//				printf("passes: %d\n", finfo.image[0].passes);

				for(int pass = 0;pass < finfo.image[current].passes;pass++)
				{
					lr->codec->fmt_read_next_pass();

					for(j = 0;j < finfo.image[current].h;j++)
					{
						scan = image + j * finfo.image[current].w;
						i = lr->codec->fmt_read_scanline(scan);
						errors += (int)(i != SQE_OK);
					}
				}

				if(finfo.image[current].needflip)
					fmt_utils::flipv((char *)image, finfo.image[current].w * sizeof(RGBA), finfo.image[current].h);

				dialogAdditionalInit();

				opt.alpha = finfo.image[current].hasalpha;

				current++;
			}

			lr->codec->fmt_read_close();
		}
		else
		{
			emit convertText(SQ_ErrorString::instance()->stringSN(SQE_R_NOTSUPPORTED), true);
			emit oneFileProcessed();
		}

		finfo.image.clear();
		finfo.meta.clear();
	}

	if(image)
	{
		free(image);
		image = NULL;
	}

	if(imageopt.close && !gerrors)
		emit done(true);
	else
		emit done(false);
}
