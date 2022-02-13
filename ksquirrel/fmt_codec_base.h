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

#ifndef KSQUIRREL_LIBS_CLASS_DEFINITION_H
#define KSQUIRREL_LIBS_CLASS_DEFINITION_H

#include "fio.h"

#include "fmt_defs.h"


//////////////////////////////////
//                              //
//  Base class for all          //
//  codecs                      //
//                              //
//////////////////////////////////


class fmt_codec_base
{
    public:
	fmt_codec_base()
	{}

	virtual ~fmt_codec_base()
	{}

	// version of the library, e.g. "1.2.2", etc.
	virtual std::string	fmt_version() = 0;

	// quick information about library
	// for example "Photoshop PSD"
	virtual std::string	fmt_quickinfo() = 0;

	// filter for filemanager, e.g. "*.aaa " or "*.aaa *.bbb *.ccc "
	// don't forget to insert a blank after each extension
	virtual std::string	fmt_filter() = 0;

	// header/identifier of file
	virtual std::string	fmt_mime() = 0;

	// 16x16 PNG pixmap in ASCII mode
	virtual std::string	fmt_pixmap() = 0;

	/*
	 *             read methods
	 */
	
	// fmt_init: do what you need before decoding
	virtual s32	fmt_init(std::string file) = 0;

	// fmt_next: seek to correct file offset, do other initialization stuff.
	// this method should be (and will be) called before image is about to
	// be decoded.
	virtual s32	fmt_next() = 0;

	// fmt_next_pass: do somethimg important before the next pass
	// will be decoded (usually do nothing, if the image has only 1 pass (like BMP, PCX ...),
	// or adjust variables if the image is interlaced, with passes > 1 (like GIF, PNG))
	virtual s32	fmt_next_pass() = 0;

	// fmt_readscanline: read one scanline from file
	virtual s32	fmt_read_scanline(RGBA *scan) = 0;

	// fmt_readimage: read entire image. usually thumbnail generator or image converter calls this method
	virtual s32	fmt_readimage(std::string file, RGBA **image, std::string &dump) = 0;

	// fmt_close: close all handles, free memory, etc.
	virtual void	fmt_close() = 0;

	/*
	 *             write methods
	 */

	// writable ?
	virtual bool fmt_writable() const = 0;

	// fmt_getwriteoptions: return write options for this image format
	virtual void    fmt_getwriteoptions(fmt_writeoptionsabs *) = 0;

	// fmt_writeimage: write image on disk
	virtual s32	fmt_writeimage(std::string file, RGBA *image, s32 w, s32 h, const fmt_writeoptions &opt) = 0;

	fmt_info information() const
	{
	    return finfo;
	}

    protected:
	s32 	 	currentImage;
	fmt_info 	finfo;
	ifstreamK	frs;
	bool		globalerror;
	s32		line;
};

#endif
