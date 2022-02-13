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
#include "sq_config.h"
#include "sq_imageloader.h"

SQ_Filter * SQ_Filter::sing = NULL;

SQ_Filter::SQ_Filter() : SQ_EditBase()
{
	sing = this;

	special_action = i18n("Filtering");

	prefix = "Filtering of ";

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

        bool generate_preview = SQ_Config::instance()->readBoolEntry("Edit tools", "preview", false);

        if(generate_preview)
        {
           filter->setPreviewImage(generatePreview());
	    SQ_ImageLoader::instance()->cleanup();
        }

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
        fmt_filters::image img((unsigned char *)image, im->w, im->h);
        fmt_filters::rgba c = fmt_filters::white;

        switch(filtopt.type)
        {
            case 0: fmt_filters::blend(img, filtopt.rgb1, filtopt._float); break;
            case 1: fmt_filters::blur(img, filtopt._double1, filtopt._double2);break;
            case 2: fmt_filters::desaturate(img, filtopt._float); break;
            case 3: fmt_filters::despeckle(img); break;
            case 4: fmt_filters::edge(img, filtopt._double1); break;
            case 5: fmt_filters::emboss(img, filtopt._double1, filtopt._double2); break;
            case 6: fmt_filters::equalize(img); break;
            case 7: fmt_filters::fade(img, filtopt.rgb1, filtopt._float); break;
            case 8: fmt_filters::flatten(img, filtopt.rgb1, filtopt.rgb2); break;
            case 9: fmt_filters::implode(img, filtopt._double1, c); break;
            case 10: fmt_filters::negative(img); break;
            case 11: fmt_filters::noise(img, (fmt_filters::NoiseType)filtopt._uint); break;
            case 12: fmt_filters::oil(img, filtopt._double1); break;
            case 13: fmt_filters::shade(img, filtopt._bool, filtopt._double1, filtopt._double2); break;
            case 14: fmt_filters::sharpen(img, filtopt._double1, filtopt._double2); break;
            case 15: fmt_filters::solarize(img, filtopt._double1); break;
            case 16: fmt_filters::spread(img, filtopt._uint); break;
            case 17: fmt_filters::swapRGB(img, filtopt._uint); break;
            case 18: fmt_filters::swirl(img, filtopt._double1, c); break;
            case 19: fmt_filters::threshold(img, filtopt._uint); break;
            case 20: fmt_filters::gray(img); break;
            case 21:
            {
                fmt_filters::rgba *rr = NULL;
                int h;
                fmt_filters::wave(img, filtopt._double1, filtopt._double2, c, &h, &rr);

                if(rr)
                {
                    free(image);
                    image = (RGBA *)rr;
                    im->h = h;
                }
            }
            break;
        }

	return SQE_OK;
}

void SQ_Filter::dialogReset()
{
	filter->startFiltering(files.count());
}

void SQ_Filter::cycleDone()
{
}
