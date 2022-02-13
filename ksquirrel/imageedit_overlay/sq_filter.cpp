/***************************************************************************
                          sq_filter.cpp  -  description
                             -------------------
    begin                : ??? ??? 25 2005
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
#include "sq_filter.h"
#include "sq_imagefilter.h"
#include "fmt_filters.h"

SQ_Filter * SQ_Filter::sing = NULL;

SQ_Filter::SQ_Filter() : SQ_EditBase()
{
	sing = this;

	special_action = i18n("Filtering");

	ondisk = true;
}

SQ_Filter::~SQ_Filter()
{}

void SQ_Filter::startEditPrivate()
{
	filter = new SQ_ImageFilter(KSquirrel::app());
	filter->setCaption(i18n("Filter 1 file", "Filter %n files", files.count()));

	connect(filter, SIGNAL(filter(SQ_ImageOptions*, SQ_ImageFilterOptions*)), this, SLOT(slotStartFilter(SQ_ImageOptions*, SQ_ImageFilterOptions*)));
	connect(this, SIGNAL(convertText(const QString &, bool)), filter, SLOT(slotDebugText(const QString &, bool)));
	connect(this, SIGNAL(oneFileProcessed()), filter, SLOT(slotOneProcessed()));
	connect(this, SIGNAL(done(bool)), filter, SLOT(slotDone(bool)));

	preview = true;

	filter->exec();
}

void SQ_Filter::slotStartFilter(SQ_ImageOptions *o, SQ_ImageFilterOptions *fopt)
{
	imageopt = *o;
	filtopt = *fopt;

	decodingCycle();
}

SQ_Filter* SQ_Filter::instance()
{
	return sing;
}

int SQ_Filter::manipDecodedImage(fmt_image *im)
{
	switch(filtopt.type)
	{
		case 0:
		{
			if(filtopt.subtype)
				fmt_filters::swapRGB((unsigned char *)image, im->w, im->h, filtopt.sb.swaprgb.type);
			else
				fmt_filters::negative((unsigned char *)image, im->w, im->h);
		}
		break;
	}

	return SQE_OK;
}

void SQ_Filter::dialogReset()
{
	filter->startFiltering(files.count());
}

void SQ_Filter::setPreviewImage(const QImage &)
{

}
