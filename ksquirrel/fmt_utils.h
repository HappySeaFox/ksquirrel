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

#include <string>

using namespace std;

#include "fmt_types.h"

////////////////////////
//                    //
//  Helper namespace  //
//                    //
////////////////////////


namespace fmt_utils
{
	// flip the image vertically
	// usually BMP, SGI (or some other formats) need to be flipped
	void flipv(s8 *image, s32 bytes_w, s32 h);

	// flip the image horizontally
	void fliph(s8 *image, s32 w, s32 h, s32 bpp);

	// Big endian to Little endian conversion
	u16 konvertWord(u16 a);
	u32 konvertLong(u32 a);

	// return temporary file name for a given file name.
	//
	// for example adjustTempName("/home/krasu/images/1/2/car.png", ".png")
	// will return something like "/home/krasu/tmp/car.png.1122341234.433421231.png"
	std::string adjustTempName(const std::string &n, const std::string &suffix = std::string());

	// return color system name by bpp.
	// for example 32 bpp means RGBA image, 1 bpp - monochrome
	std::string colorSpaceByBpp(const s32 bpp);
	
	void expandMono1Byte(const u32 byte, u8 *array);
	void expandMono2Byte(const u32 byte, u8 *array);
	void expandMono4Byte(const u32 byte, u8 *array);
}

#endif
