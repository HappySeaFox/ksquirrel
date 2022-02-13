/***************************************************************************
                          sq_contraster.cpp  -  description
                             -------------------
    begin                : ??? Apr 29 2005
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
#include "sq_contraster.h"
#include "sq_imagebcg.h"

SQ_Contraster * SQ_Contraster::sing = NULL;

SQ_Contraster::SQ_Contraster() : SQ_EditBase()
{
	sing = this;

	special_action = i18n("Colorizing");

	ondisk = true;
}

SQ_Contraster::~SQ_Contraster()
{}

void SQ_Contraster::startEditPrivate()
{
	bcg = new SQ_ImageBCG(KSquirrel::app());
	bcg->setCaption(i18n("Colorize 1 file", "Edit %n files", files.count()));

	connect(bcg, SIGNAL(bcg(SQ_ImageOptions*,SQ_ImageBCGOptions*)), this, SLOT(slotStartContrast(SQ_ImageOptions*,SQ_ImageBCGOptions*)));
	connect(this, SIGNAL(convertText(const QString &, bool)), bcg, SLOT(slotDebugText(const QString &, bool)));
	connect(this, SIGNAL(oneFileProcessed()), bcg, SLOT(slotOneProcessed()));
	connect(this, SIGNAL(done(bool)), bcg, SLOT(slotDone(bool)));

	bcg->exec();
}

void SQ_Contraster::slotStartContrast(SQ_ImageOptions *o, SQ_ImageBCGOptions *bopt)
{
	imageopt = *o;
	bcgopt = *bopt;

	decodingCycle();
}

SQ_Contraster* SQ_Contraster::instance()
{
	return sing;
}

void SQ_Contraster::setWritingLibrary()
{
	lw = lr;
}

void SQ_Contraster::dialogReset()
{
	bcg->startBCG(files.count());
}

void SQ_Contraster::dialogAdditionalInit()
{

}

int SQ_Contraster::manipDecodedImage(SQ_LIBRARY *lw, const QString &name, RGBA *image,
												const fmt_image &im, const fmt_writeoptions &opt)
{
	return SQE_OK;
}
