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
#include "sq_library.h"
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

	preview = true;

	rotate->exec();
}

SQ_Rotater* SQ_Rotater::instance()
{
	return sing;
}

void SQ_Rotater::dialogReset()
{
	rotate->startRotation(files.count());
}

int SQ_Rotater::manipDecodedImage(fmt_image *im)
{
	if(rotopt.flipv)
		fmt_utils::flipv((char *)image, im->w * sizeof(RGBA), im->h);

	if(rotopt.fliph)
		fmt_utils::fliph((char *)image, im->w, im->h, sizeof(RGBA));

	if(rotopt.angle == 90 || rotopt.angle == 270)
	{
		int t;
		t = im->w;
		im->w = im->h;
		im->h = t;
	}

	return SQE_OK;
}

int SQ_Rotater::determineNextScan(const fmt_image &im, RGBA *scan, int y)
{
	if(rotopt.angle == 90)
	{
		for(int i = im.w-1, j = 0;i >= 0;i--,j++)
			scan[j] = *(image + i*im.h + y);
	}
	else if(rotopt.angle == 180)
	{
		for(int i = 0, j = im.w-1;i < im.w;i++,j--)
			scan[j] = *(image + y*im.w + i);
	}
	else if(rotopt.angle == 270)
	{
		for(int i = 0, j = im.w-1;i < im.w;i++,j--)
			scan[i] = *(image + i*im.h + y);
	}
	else
		memcpy(scan, image + y * im.w, im.w * sizeof(RGBA));

	return SQE_OK;
}

void SQ_Rotater::setPreviewImage(const QImage &)
{

}
