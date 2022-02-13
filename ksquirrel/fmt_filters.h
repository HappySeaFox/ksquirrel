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

#ifndef KSQUIRREL_LIBS_FILTERS_H
#define KSQUIRREL_LIBS_FILTERS_H

//////////////////////////////////////////
//                                      //
//  Pass the image through some filter  //
//                                      //
//////////////////////////////////////////

namespace fmt_filters
{
	// colorize the image, which is w x h, left alpha channel unchanged.
	//
	// it just adds to each pixel in the image
	// aproproriate value.
	void colorize(unsigned char *image, int w, int h, int red, int green, int blue);

	// change brightness of the image
	void brightness(unsigned char *image, int w, int h, int bn);

	// change gamma
	// gamma should be  0.0 <= L <= 6.0
	//
	// it is no problem to set L to 8.0 or 9.0, but the resulting
	// image won't have much difference from 6.0
	void gamma(unsigned char *image, int w, int h, double L);

	// change contrast with Photoshop-like method
	// contrast should be  -255 <= contrast <= 255
	void contrast(unsigned char *image, int w, int h, int contrast);

	enum swapRGBtype { GBR = 0, BRG = 1 };

	// negative
	void negative(unsigned char *image, int w, int h);

	// swap RGB values
	void swapRGB(unsigned char *image, int w, int h, int type);
}

#endif
