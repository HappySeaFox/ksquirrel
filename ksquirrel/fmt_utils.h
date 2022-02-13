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

#ifndef KSQUIRREL_LIBS_UTILS_H
#define KSQUIRREL_LIBS_UTILS_H

#include <cstring>

using namespace std;


////////////////////
//                //
//  Helper class  //
//                //
////////////////////


class fmt_utils
{
    public:

	// flip the image vertically
	// usually BMP, SGI (or some other formats) need to be flipped
	static void flip(s8 *image, int bytes_w, int h);

	// Big endian to Little endian conversion
	static u16 konvertWord(u16 a);
	static u32 konvertLong(u32 a);
	
	// return temporary file name for a given file name.
	// for example adjustTempName("/home/ckult/images/1/2/car.png", ".png")
	// will return something like "/home/ckult/tmp/car.png.1122341234.433421231.png"
	static std::string adjustTempName(const std::string &n, const std::string &suffix = std::string());

	// return color system name by bpp.
	// for example 32 bpp means RGBA image, 1 bpp - monochrome
	static std::string colorSystemByBpp(const s32 bpp);
	
	static void expandMono1Byte(const u32 byte, u8 *array);
	static void expandMono2Byte(const u32 byte, u8 *array);
	static void expandMono4Byte(const u32 byte, u8 *array);
};

#if defined SQ_HAVE_FMT_UTILS

void fmt_utils::flip(s8 *image, int bytes_w, int h)
{
    int i;
    s8 *hptr = new s8 [bytes_w];

    if(!hptr)
	return;

    for(i = 0; i < h/2; i++)
    {
        memcpy(hptr, image + i * bytes_w, bytes_w);
        memcpy(image + i * bytes_w, image + (h - i - 1) * bytes_w, bytes_w);
        memcpy(image + (h - i - 1) * bytes_w, hptr, bytes_w);
    }

    delete hptr;
}

u16 fmt_utils::konvertWord(u16 a)
{
    u16 i = a, b;

    b = i & 255;
    b = b << 8;
    b = b|0;
    i = i >> 8;
    i = i|0;

    return i|b;
}

u32 fmt_utils::konvertLong(u32 a)
{
    u32 i = a, b, c;
    u16 m, n;

    b = i >> 16; // high word
    c = i << 16;
    c = c >> 16; // low word

    m = (u16)b;
    n = (u16)c;

    m = fmt_utils::konvertWord(m);
    n = fmt_utils::konvertWord(n);

    b = m;
    c = n;
    c = c << 16;

    b = b|0;
    c = c|0;

    return b|c;
}

std::string fmt_utils::adjustTempName(const std::string &n, const std::string &suffix)
{
    const char *tmp_global = "/tmp";

    std::string tmp, res;
    char *t;

    t = getenv("TMP");

    if(!t)
        t = getenv("TMPDIR");

    tmp  = (t) ? t : tmp_global;
    tmp += "/";

    std::string::size_type pos = 0;

    pos = n.find_last_of("/");

    res = tmp + n.substr(pos);

    stringstream s;

    s << time(NULL) << "." << random() << suffix;

    return res + "." + s.str();
}

std::string fmt_utils::colorSystemByBpp(const int bpp)
{
    switch(bpp)
    {
	case 1:
	    return std::string("Monochrome");

	case 4:
	case 8:
	case 15:
	case 16:
	    return std::string("Color indexed");

	case 24:
	    return std::string("RGB");

	case 32:
	    return std::string("RGBA");
	    
	default:
	    return std::string("Unknown");
    }
}

void fmt_utils::expandMono1Byte(const u32 byte, u8 *array)
{
    u8 mask = 0x80;
    u8 *p = array;
    u8 b = byte & 0xff; 

    for(s32 i = 0;i < 8;i++)
    {
	*p = (b & mask) ? 1 : 0;

	p++;
	mask >>= 1;
    }
}

void fmt_utils::expandMono2Byte(const u32 byte, u8 *array)
{
    u16 mask = 0x8000;
    u8 *p = array;
    u16 b = byte & 0xffff; 

    for(s32 i = 0;i < 16;i++)
    {
	*p = (b & mask) ? 1 : 0;

	p++;
	mask >>= 1;
    }
}

void fmt_utils::expandMono4Byte(const u32 byte, u8 *array)
{
    u32 mask = 0x80000000;
    u8 *p = array;
    u32 b = byte & 0xffffffff; 

    for(s32 i = 0;i < 32;i++)
    {
	*p = (b & mask) ? 1 : 0;

	p++;
	mask >>= 1;
    }
}

#endif

#endif
