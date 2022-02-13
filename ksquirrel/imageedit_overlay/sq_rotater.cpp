/***************************************************************************
                          sq_rotater.cpp  -  description
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
#include "sq_rotater.h"
#include "sq_imagerotate.h"

SQ_Rotater * SQ_Rotater::sing = NULL;

SQ_Rotater::SQ_Rotater() : SQ_EditBase()
{
	sing = this;

	special_action = i18n("Rotating");

	ondisk = true;
}

SQ_Rotater::~SQ_Rotater()
{}

void SQ_Rotater::slotStartRotate(SQ_ImageOptions *o, SQ_ImageRotateOptions *ropt)
{
	imageopt = *o;
	rotopt = *ropt;

	decodingCycle();
}

void SQ_Rotater::startEditPrivate()
{
	rotate = new SQ_ImageRotate(KSquirrel::app());
	rotate->setCaption(i18n("Rotate or flip 1 file", "Rotate or flip %n files", files.count()));

	connect(rotate, SIGNAL(rotate(SQ_ImageOptions*, SQ_ImageRotateOptions*)), this, SLOT(slotStartRotate(SQ_ImageOptions*, SQ_ImageRotateOptions*)));
	connect(this, SIGNAL(convertText(const QString &, bool)), rotate, SLOT(slotDebugText(const QString &, bool)));
	connect(this, SIGNAL(oneFileProcessed()), rotate, SLOT(slotOneProcessed()));
	connect(this, SIGNAL(done(bool)), rotate, SLOT(slotDone(bool)));

	rotate->exec();
}

SQ_Rotater* SQ_Rotater::instance()
{
	return sing;
}

void SQ_Rotater::setWritingLibrary()
{
	lw = lr;
}

void SQ_Rotater::dialogReset()
{
	rotate->startRotation(files.count());
}

void SQ_Rotater::dialogAdditionalInit()
{

}

int SQ_Rotater::manipDecodedImage(SQ_LIBRARY *lw, const QString &name, RGBA *image,
												const fmt_image &im, const fmt_writeoptions &opt)
{
	return SQE_OK;
}
