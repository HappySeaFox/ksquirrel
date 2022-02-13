/*  This file is part of ksquirrel-libs (http://ksquirrel.sf.net)

    Copyright (c) 2004 Dmitry Baryshev <ksquirrel@tut.by>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    either version 2 of the License, or (at your option) any later
    version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef KSQUIRREL_LIBS_DEFS_H
#define KSQUIRREL_LIBS_DEFS_H

#include <stdio.h>
#include <linux/types.h>

#define ATTR_ __attribute__ ((packed))

/* Metainfo support */

struct fmt_meta_entry
{
    char group[80];
    int datalen;
    char *data;
}ATTR_;

struct fmt_metainfo
{
    int entries;
    fmt_meta_entry *m;
}ATTR_;

struct RGBA
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
}ATTR_;

struct RGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}ATTR_;

struct fmt_image
{
    int			w;
    int			h;
    int			bpp;
    bool		hasalpha;
    bool		needflip;
    char		*dump;
    fmt_metainfo	*meta;
    int			delay;
}ATTR_;

struct fmt_info
{
    fmt_image		*image;
    unsigned short	images;
    bool		animated;
    bool		interlaced;
    int			passes;
}ATTR_;

#define trace(a) printf("%s\n", a)

#if defined SQ_NEED_OPERATOR_RGBA_RGBA
static int operator== (const RGBA &rgba1, const RGBA &rgba2)
{
    return (rgba1.r == rgba2.r && rgba1.g == rgba2.g && rgba1.b == rgba2.b && rgba1.a == rgba2.a);
}
#endif

#if defined SQ_NEED_OPERATOR_RGB_RGBA
static int operator== (const RGB &rgb, const RGBA &rgba)
{
    return (rgb.r == rgba.r && rgb.g == rgba.g && rgb.b == rgba.b);
}
#endif

#if defined SQ_NEED_OPERATOR_RGBA_RGB
static int operator== (const RGBA &rgba, const RGB &rgb)
{
    return (rgba.r == rgb.r && rgba.g == rgb.g && rgba.b == rgb.b);
}
#endif

#endif
