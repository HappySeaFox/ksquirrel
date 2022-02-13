/*  This file is part of ksquirrel-libs (http://ksquirrel.sf.net)

    Copyright (c) 2005 Dmitry Baryshev <ksquirrel@tut.by>

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

#include <vector>
#include <string>

using namespace std;

#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif

/* Metainfo support */

struct fmt_metaentry
{
    std::string	group;
    std::string	data;

}PACKED;

/* RGBA and RGB pixels */

struct RGBA
{
    RGBA() : r(0), g(0), b(0), a(0)
    {}

    RGBA(s32 r1, s32 g1, s32 b1, s32 a1) : r(r1), g(g1), b(b1), a(a1)
    {}

    u8 r;
    u8 g;
    u8 b;
    u8 a;

}PACKED;

struct RGB
{
    RGB() : r(0), g(0), b(0)
    {}
    
    RGB(s32 r1, s32 g1, s32 b1) : r(r1), g(g1), b(b1)
    {}

    u8 r;
    u8 g;
    u8 b;

}PACKED;

/* Image decription */
struct fmt_image
{
    fmt_image() : w(0), h(0), bpp(0), hasalpha(false), needflip(false), delay(0), interlaced(false), passes(1)
    {}

    // width and height
    s32			w;
    s32			h;
    
    // bit depth
    s32			bpp;
    
    // has alpha channel ?
    bool		hasalpha;
    
    // need to be flipped ?
    bool		needflip;
    
    // some useful information
    std::string		dump;
    
    // if it's a frame from animated sequence,
    // 'delay' will define a delay time
    s32			delay;
    
    // interlaced ?
    bool		interlaced;
    
    // if interlaced, 'passes' stores the number
    // of passes. if no, passes should be 1
    s32			passes;

}PACKED;

/* General description */
struct fmt_info
{
    fmt_info() : images(0), animated(false)
    {}

    // array of images
    std::vector<fmt_image>	image;
    
    // array of metainfo entries
    std::vector<fmt_metaentry>	meta;

    // number of images
    s32				images;
    
    // animated or static
    bool			animated;

}PACKED;

/*                                                                Internal cmpression.
								  E.g. compression_level is
								  passed to internal routines,
                       No compression       RLE compression       for ex. in libjpeg, libpng.
*/
enum fmt_compression { CompressionNo = 1,   CompressionRLE = 2,   CompressionInternal = 4 };
/*                                                                Note: if the image can be compressed
                                                                  with RLE encoding and with only RLE
								  encoding, compression_scheme should be
								  CompressionInternal
*/

/* Write options for image format */
struct fmt_writeoptionsabs
{
    // can be interlaced ?
    bool interlaced;
    
    // with which compression it can be encoded ?
    //    for example: CompressionNo | CompressionRLE.
    //    it means, that image can be encoded with RLE
    //    method or can be saved uncompressed.
    s32  compression_scheme;

    // minimum compression level, maximum and default
    //    For example, JPEG library has minimum = 0, 
    //    maximum = 100 and default = 25.
    s32  compression_min, compression_max, compression_def;

}PACKED;

/* this information will be passed to writing function */
struct fmt_writeoptions
{
    // write interlaced image or normal ?
    bool interlaced;

    // with which compression encode the image ?
    fmt_compression compression_scheme;
    
    // compression level
    s32 compression_level;

    // has alpha channel ?
    // If no, A channel in RGBA image will be ignored
    bool alpha;

}PACKED;

#if defined SQ_NEED_OPERATOR_RGBA_RGBA
static s32 operator== (const RGBA &rgba1, const RGBA &rgba2)
{
    return (rgba1.r == rgba2.r && rgba1.g == rgba2.g && rgba1.b == rgba2.b && rgba1.a == rgba2.a);
}
#endif

#if defined SQ_NEED_OPERATOR_RGB_RGBA
static s32 operator== (const RGB &rgb, const RGBA &rgba)
{
    return (rgb.r == rgba.r && rgb.g == rgba.g && rgb.b == rgba.b);
}
#endif

#if defined SQ_NEED_OPERATOR_RGBA_RGB
static s32 operator== (const RGBA &rgba, const RGB &rgb)
{
    return (rgba.r == rgb.r && rgba.g == rgb.g && rgba.b == rgb.b);
}
#endif

#endif
