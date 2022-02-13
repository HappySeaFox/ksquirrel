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
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_contraster.h"
#include "sq_imagebcg.h"
#include "sq_library.h"
#include "fmt_filters.h"
#include "sq_imageloader.h"

SQ_Contraster * SQ_Contraster::sing = NULL;

SQ_Contraster::SQ_Contraster(QObject *parent) : SQ_EditBase(parent)
{
    sing = this;

    kdDebug() << "+SQ_Contraster" << endl;

    special_action = i18n("Colorizing");

    prefix = "Colorizing of ";

    ondisk = true;
}

SQ_Contraster::~SQ_Contraster()
{
    kdDebug() << "-SQ_Contraster" << endl;
}

void SQ_Contraster::startEditPrivate()
{
    bcg = new SQ_ImageBCG(KSquirrel::app());
    bcg->setCaption(i18n("Colorize 1 file", "Edit %n files", files.count()));

    connect(bcg, SIGNAL(bcg(SQ_ImageOptions*,SQ_ImageBCGOptions*)), this, SLOT(slotStartContrast(SQ_ImageOptions*,SQ_ImageBCGOptions*)));
    connect(this, SIGNAL(convertText(const QString &, bool)), bcg, SLOT(slotDebugText(const QString &, bool)));
    connect(this, SIGNAL(oneFileProcessed()), bcg, SLOT(slotOneProcessed()));
    connect(this, SIGNAL(done(bool)), bcg, SLOT(slotDone(bool)));

    bcg->setPreviewImage(generatePreview());
    SQ_ImageLoader::instance()->cleanup();

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

void SQ_Contraster::dialogReset()
{
    bcg->startBCG(files.count());
}

int SQ_Contraster::manipDecodedImage(fmt_image *im)
{
    if(bcgopt.b)
        fmt_filters::brightness(fmt_filters::image((unsigned char *)image, im->w, im->h), bcgopt.b);

    if(bcgopt.c)
        fmt_filters::contrast(fmt_filters::image((unsigned char *)image, im->w, im->h), bcgopt.c);

    if(bcgopt.g != 100)
        fmt_filters::gamma(fmt_filters::image((unsigned char *)image, im->w, im->h), (double)bcgopt.g / 100.0);

    if(bcgopt.red || bcgopt.green || bcgopt.blue)
        fmt_filters::colorize(fmt_filters::image((unsigned char *)image, im->w, im->h), bcgopt.red, bcgopt.green, bcgopt.blue);

    return SQE_OK;
}

void SQ_Contraster::cycleDone()
{
    delete bcg;
}

#include "sq_contraster.moc"
