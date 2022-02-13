/***************************************************************************
                          sq_decoder.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
    email                : squirrel-sf@yandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sq_decoder.h"
#include <qfileinfo.h>

#include "ksquirrel.h"
#include "sq_libraryhandler.h"

SQ_Decoder::SQ_Decoder(QObject *parent, const char *name) : QObject(parent, name)
{
	// prevent deleting empty array in decode()
	pic = (PICTURE*)calloc(1, sizeof(PICTURE));
	pic->A = (RGBA*)calloc(1, sizeof(RGBA));
	pic->pal = (RGB*)calloc(1, sizeof(RGBA));
}

SQ_Decoder::~SQ_Decoder()
{}

RGBA* SQ_Decoder::decode(const QString &file)
{
	QFileInfo finfo(file);

	free(pic->pal);
	free(pic->A);
	free(pic);
	sqLibHandler->setCurrentLibrary(finfo.extension(false));
	sqLibHandler->callReadFormat(file, &pic);

	return (pic->A);
}

const int SQ_Decoder::width()
{
	return pic->w;
}

const int SQ_Decoder::height()
{
	return pic->h;
}

const int SQ_Decoder::bpp()
{
	return pic->bpp;
}
