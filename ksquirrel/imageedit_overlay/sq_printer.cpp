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

#include <klocale.h>
#include <kprinter.h>

#include "ksquirrel.h"
#include "sq_libraryhandler.h"
#include "sq_printer.h"
#include "sq_imageprint.h"
#include "sq_about.h"

SQ_Printer * SQ_Printer::sing = 0;

SQ_Printer::SQ_Printer() : SQ_EditBase()
{
	sing = this;

	special_action = i18n("Printing");

	ondisk = true;

	printer = new KPrinter;
	printer->setCreator(QString::fromLatin1("KSquirrel %1").arg(SQ_VERSION));
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

	if(printer->setup(print))
		decodingCycle();
}

SQ_Printer* SQ_Printer::instance()
{
	return sing;
}

void SQ_Printer::setWritingLibrary()
{
	lw = lr;
}

int SQ_Printer::manipDecodedImage(SQ_LIBRARY *lw, const QString &name, RGBA *image,
												const fmt_image &im, const fmt_writeoptions &opt)
{
	return SQE_OK;
}

void SQ_Printer::dialogReset()
{
	//print->startConvertion(files.count());
	print->startPrinting(files.count());
}

void SQ_Printer::dialogAdditionalInit()
{
	
}
