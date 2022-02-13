/***************************************************************************
                          sq_imageedit.h  -  description
                             -------------------
    begin                : ‘Œ ·–“ 29 2005
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

#ifndef SQ_IMAGEEDIT_H
#define SQ_IMAGEEDIT_H

struct SQ_ImageOptions
{
	QString putto;
	QString prefix;
	int where_to_put;
	bool close;
};

struct SQ_ImageResizeOptions
{
	bool percentage;
	int pc;

	int w, h;
	bool preserve;
	int adjust;
};

struct SQ_ImageRotateOptions
{
	bool flipv, fliph;
	int angle;
};

struct SQ_ImageBCGOptions
{
	int b, c, g;
	int red, green, blue;
};

struct SQ_ImageConvertOptions
{
	QString libname;
};

struct SQ_ImagePrintOptions
{
	int fix;
};

#endif
