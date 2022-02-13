/***************************************************************************
                          sq_printer.cpp  -  description
                             -------------------
    begin                : ??? May 11 2005
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

#include <qpaintdevicemetrics.h>
#include <qpainter.h>

#include <klocale.h>
#include <kprinter.h>

#include "ksquirrel.h"
#include "sq_libraryhandler.h"
#include "sq_printer.h"
#include "sq_imageprint.h"
#include "sq_about.h"

#include "libpixops/pixops.h"

SQ_Printer * SQ_Printer::sing = NULL;

SQ_Printer::SQ_Printer() : SQ_EditBase()
{
	sing = this;

	special_action = i18n("Printing");

	ondisk = false;

	painter = new QPainter;
}

SQ_Printer::~SQ_Printer()
{}

void SQ_Printer::startEditPrivate()
{
	print = new SQ_ImagePrint(KSquirrel::app());
	print->setCaption(i18n("Print 1 file", "Print %n files", files.count()));

	connect(print, SIGNAL(print(SQ_ImageOptions*, SQ_ImagePrintOptions*)), this, SLOT(slotStartPrint(SQ_ImageOptions*, SQ_ImagePrintOptions*)));
	connect(this, SIGNAL(convertText(const QString &, bool)), print, SLOT(slotDebugText(const QString &, bool)));
	connect(this, SIGNAL(oneFileProcessed()), print, SLOT(slotOneProcessed()));
	connect(this, SIGNAL(done(bool)), print, SLOT(slotDone(bool)));

	print->exec();
}

void SQ_Printer::slotStartPrint(SQ_ImageOptions *o, SQ_ImagePrintOptions *popt)
{
	imageopt = *o;
	prnopt = *popt;

	printer = new KPrinter;
	printer->setCreator(QString::fromLatin1("KSquirrel %1").arg(SQ_VERSION));

	if(printer->setup(print))
	{
		currentPage = -1;
		painter->begin(printer);
		decodingCycle();
	}
	else
		print->close();
}

SQ_Printer* SQ_Printer::instance()
{
	return sing;
}

void SQ_Printer::setWritingLibrary()
{
	lw = lr;
}

int SQ_Printer::manipDecodedImage(fmt_image *)
{
	return SQE_OK;
}

void SQ_Printer::dialogReset()
{
	print->startPrinting(files.count());
}

int SQ_Printer::manipAndWriteDecodedImage(const QString &, fmt_image *im, const fmt_writeoptions &)
{
	QPaintDeviceMetrics mt(printer);
	RGBA *rgba;

	const int spacing = 6;

	currentPage++;

	if(prnopt.type == 1)
	{

	}
	else if(prnopt.type == 2)
	{

	}
	else
	{
		int x, y;

		if(im->w > mt.width() || im->h > mt.height())
		{
			double asp;

			if(im->w > mt.width() && im->h > mt.height())
			{
				double asp1 = (double)im->w / im->h;
				double asp2 = (double)mt.width() / mt.height();

				asp = (asp1 > asp2) ? asp1 : asp2;
			}
			else
				asp = (im->w > mt.width()) ? ((double)im->w / mt.width()) : ((double)im->h / mt.height());

			int w = int((double)im->w / asp);
			int h = int((double)im->h / asp);

			rgba = new RGBA [w * h];

			if(!rgba)
				return SQE_W_NOMEMORY;

			pixops_scale((unsigned char *)rgba, 0, 0, w, h, w * 4, 4, true,
						(unsigned char *)image, im->w, im->h, im->w * 4, 4, true,
						(double)w / im->w, (double)h / im->h,
						PIXOPS_INTERP_BILINEAR);

			free(image);

			image = rgba;

			im->w = w;
			im->h = h;
		}

		if(prnopt.align == "lefttop")
		{
			x = 0;
			y = 0;
		}
		else if(prnopt.align == "righttop")
		{
			x = mt.width() - im->w;
			y = 0;
		}
		else if(prnopt.align == "leftbottom")
		{
			x = 0;
			y = mt.height() - im->h;
		}
		else if(prnopt.align == "rightbottom")
		{
			x = mt.width() - im->w;
			y = mt.height() - im->h;
		}
		else
		{
			x = (mt.width() - im->w) / 2;
			y = (mt.height() - im->h) / 2;
		}

		QImage img((uchar *)image, im->w, im->h, 32, 0, 0, QImage::LittleEndian);
		img.setAlphaBuffer(true);
		img = img.swapRGB();

		int cps = printer->numCopies();

		for(int pages = 0;pages < cps;pages++)
		{
			if(currentPage || (!currentPage && pages))
  				if(!printer->newPage())
     				{
					qWarning("KPrinter::newPage() error");
					return SQE_NOTOK;
				}

			painter->drawImage(x, y, img);
		}
	}

	return SQE_OK;
}

void SQ_Printer::cycleDone()
{
	painter->end();

	delete printer;
}
