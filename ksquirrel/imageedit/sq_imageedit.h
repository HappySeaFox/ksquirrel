/***************************************************************************
                          sq_imageedit.h  -  description
                             -------------------
    begin                : ??? ??? 29 2005
    copyright            : (C) 2005 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
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

#include <qfont.h>
#include <qcolor.h>

#include "fmt_filters.h"

struct SQ_ImageOptions
{
    QString putto;
    int where_to_put;
    bool close;
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

struct SQ_ImageFilterOptions
{
    int type;

    bool _bool;
    fmt_filters::rgb rgb1, rgb2;
    float _float;
    unsigned int _uint;
    double _double1, _double2;
};

namespace F
{
    enum ftype
    {
        fblend = 0,
        fblur,
        fdesaturate,
        fdespeckle,
        fedge,
        femboss,
        fequalize,
        ffade,
        fflatten,
        fimplode,
        fnegative,
        fnoise,
        foil,
        fshade,
        fsharpen,
        fsolarize,
        fspread,
        fswapRGB,
        fswirl,
        fthreshold,
        fgray,
        fredeye
    };
}

#endif
