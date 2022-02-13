/***************************************************************************
                          sq_converter.cpp  -  description
                             -------------------
    begin                : ??? Feb 24 2005
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
#include "sq_libraryhandler.h"
#include "sq_converter.h"
#include "sq_imageconvert.h"

SQ_Converter * SQ_Converter::sing = NULL;

SQ_Converter::SQ_Converter() : SQ_EditBase()
{
    sing = this;

    special_action = i18n("Converting");

    ondisk = true;
}

SQ_Converter::~SQ_Converter()
{}

void SQ_Converter::startEditPrivate()
{
    convert = new SQ_ImageConvert(KSquirrel::app());
    convert->setCaption(i18n("Convert 1 file", "Convert %n files", files.count()));

    connect(convert, SIGNAL(convert(SQ_ImageOptions*, SQ_ImageConvertOptions*)), this, SLOT(slotStartConvert(SQ_ImageOptions*, SQ_ImageConvertOptions*)));
    connect(this, SIGNAL(convertText(const QString &, bool)), convert, SLOT(slotDebugText(const QString &, bool)));
    connect(this, SIGNAL(oneFileProcessed()), convert, SLOT(slotOneProcessed()));
    connect(this, SIGNAL(done(bool)), convert, SLOT(slotDone(bool)));

    convert->exec();
}

void SQ_Converter::slotStartConvert(SQ_ImageOptions *o, SQ_ImageConvertOptions *copt)
{
    imageopt = *o;
    convopt = *copt;

    decodingCycle();
}

SQ_Converter* SQ_Converter::instance()
{
    return sing;
}

void SQ_Converter::setWritingLibrary()
{
    lw = SQ_LibraryHandler::instance()->libraryByName(convopt.libname);
}

void SQ_Converter::dialogReset()
{
    convert->startConvertion(files.count());
}

void SQ_Converter::initWriteOptions()
{
    convert->fillWriteOptions(&opt, lw->opt);
}

int SQ_Converter::manipDecodedImage(fmt_image *)
{
    return SQE_OK;
}
