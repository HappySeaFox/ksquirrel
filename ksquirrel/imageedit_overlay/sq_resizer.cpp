/***************************************************************************
                          sq_resizer.cpp  -  description
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

#include <klocale.h>

#include "ksquirrel.h"
#include "sq_resizer.h"
#include "sq_imageresize.h"

SQ_Resizer * SQ_Resizer::sing = NULL;

SQ_Resizer::SQ_Resizer() : SQ_EditBase()
{
	sing = this;

	special_action = i18n("Resizing");

	ondisk = true;
}

SQ_Resizer::~SQ_Resizer()
{}

void SQ_Resizer::startEditPrivate()
{
	res = new SQ_ImageResize(KSquirrel::app());
	res->setCaption(i18n("Resize 1 file", "Resize %n files", files.count()));

	connect(res, SIGNAL(_resize(SQ_ImageOptions*, SQ_ImageResizeOptions*)), this, SLOT(slotStartResize(SQ_ImageOptions*, SQ_ImageResizeOptions*)));
	connect(this, SIGNAL(convertText(const QString &, bool)), res, SLOT(slotDebugText(const QString &, bool)));
	connect(this, SIGNAL(oneFileProcessed()), res, SLOT(slotOneProcessed()));
	connect(this, SIGNAL(done(bool)), res, SLOT(slotDone(bool)));

	res->exec();
}

void SQ_Resizer::slotStartResize(SQ_ImageOptions *o, SQ_ImageResizeOptions *ropt)
{
	imageopt = *o;
	resopt = *ropt;

	decodingCycle();
}

SQ_Resizer* SQ_Resizer::instance()
{
	return sing;
}

void SQ_Resizer::setWritingLibrary()
{
	lw = lr;
}

void SQ_Resizer::dialogReset()
{
	res->startResizing(files.count());
}

void SQ_Resizer::dialogAdditionalInit()
{

}

int SQ_Resizer::manipDecodedImage(SQ_LIBRARY *lw, const QString &name, RGBA *image,
												const fmt_image &im, const fmt_writeoptions &opt)
{
	return SQE_OK;
}
