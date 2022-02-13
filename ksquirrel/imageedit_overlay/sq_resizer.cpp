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
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_resizer.h"
#include "sq_library.h"
#include "sq_imageresize.h"

#include "fmt_filters.h"

SQ_Resizer * SQ_Resizer::sing = NULL;

SQ_Resizer::SQ_Resizer(QObject *parent) : SQ_EditBase(parent)
{
    sing = this;

    kdDebug() << "+SQ_Resizer" << endl;

    special_action = i18n("Resizing");

    prefix = "Resizing of ";

    ondisk = true;
}

SQ_Resizer::~SQ_Resizer()
{
    kdDebug() << "-SQ_Resizer" << endl;
}

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

void SQ_Resizer::dialogReset()
{
    res->startResizing(files.count());
}

int SQ_Resizer::manipDecodedImage(fmt_image *im)
{
    int w = im->w, h = im->h;

    if(resopt.percentage)
    {
        if(!resopt.adjust)
            w = (int)((double)im->w * resopt.pc / 100.0);
        else if(resopt.adjust == 1)
            h = (int)((double)im->h * resopt.pc / 100.0);
        else
        {
            w = (int)((double)im->w * resopt.pc / 100.0);
            h = (int)((double)im->h * resopt.pc / 100.0);
        }
    }
    else
    {
        if(resopt.preserve)
        {
            double aspect;

            w = resopt.w;
            h = resopt.h;

            if(!resopt.adjust)
            {
                aspect = (double)resopt.w / im->w;
                h = int((double)im->h * aspect);
            }
            else if(resopt.adjust == 1)
            {
                aspect = (double)resopt.h / im->h;
                h = int((double)im->w * aspect);
            }
            else
            {
                QSize s(im->w, im->h);
                s.scale(resopt.w, resopt.h, QSize::ScaleMin);

                w = s.width();
                h = s.height();
            }
        }
        else
        {
            w = resopt.w;
            h = resopt.h;
        }
    }

    RGBA *rgba;
/*
    pixops_scale((unsigned char *)rgba, 0, 0, w, h, w * 4, 4, true,
    (unsigned char *)image, im->w, im->h, im->w * 4, 4, true,
    (double)w / im->w, (double)h / im->h,
    (PixopsInterpType)resopt.method);
*/
    fmt_filters::image img((unsigned char *)image, im->w, im->h);

    if(fmt_filters::resize(img, w, h, resopt.method, (unsigned char **)&rgba))
    {
        free(image);

        image = rgba;

        im->w = w;
        im->h = h;
        return SQE_OK;
    }
    else
        return SQE_R_NOMEMORY;
}

void SQ_Resizer::cycleDone()
{
    delete res;
}
