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

#include "fmt_filters.h"
#include "fmt_types.h"
#include "fmt_defs.h"

#include <cmath>

// colorize tool
void fmt_filters::colorize(RGBA *image, int w, int h, int red, int green, int blue)
{
    // check if all parameters are good
    if(!image || !w || !h)
	return;

    if(!red && !green && !blue)
	return;

    unsigned char *bits = (unsigned char *)image;
    int val;
    const int S = w * h;

    int V[3] = { red, green, blue };

    // add to RED component 'red' value, and check if the result is out of bounds.
    // do the same with GREEN and BLUE channels.
    for(int x = 0;x < S;++x)
    {
	for(int v = 0;v < 3;++v)
	{
	    val = (int)*(bits + v) + V[v];

	    if(val > 255)
		*(bits + v) = 255;
	    else if(val < 0)
		*(bits + v) = 0;
	    else
		*(bits + v) = val;
	}

	bits += 4;
    }
}

// brightness tool
void fmt_filters::brightness(RGBA *image, int w, int h, int bn)
{
    // check if all parameters are good
    if(!image || !w || !h)
	return;

    unsigned char *bits = (unsigned char *)image;
    int val;
    const int S = w * h * 4;

    // add to all color components 'bn' value, and check if the result is out of bounds.
    for(int x = 0;x < S;x++)
    {
	if(!((x+1) % 4))
	{
	    bits++;
	    continue;
	}

	val = bn + *bits;

	*bits = val < 0 ? 0 : (val > 255 ? 255 : val);

	bits++;
    }
}

// gamma tool
void fmt_filters::gamma(RGBA *image, int w, int h, double L)
{
    // check if all parameters are good
    if(!image || !w || !h)
	return;

    if(L == 0 || L < 0) L = 0.01;

    unsigned char *bits = (unsigned char *)image;
    unsigned char R, G, B;
    int X;
    unsigned char GT[256];

    GT[0] = 0;

    for (X = 1; X < 256; ++X)
	GT[X] = (unsigned char)round(255 * pow((double)X / 255.0, 1.0 / L));

    const int S = w * h;

    // change gamma
    for(X = 0; X < S; ++X)
    {
	R = *bits;
	G = *(bits+1);
	B = *(bits+2);

	*bits = GT[R];
	*(bits+1) = GT[G];
	*(bits+2) = GT[B];

	bits += 4;
    }
}

// contrast tool
void fmt_filters::contrast(RGBA *image, int w, int h, int contrast)
{
    if(!image || !w || !h || !contrast)

    if(contrast <= -256) contrast = -255;
    if(contrast >= 256) contrast = 255;

    unsigned char *bits = (unsigned char *)image, Ravg, Gavg, Bavg;
    int Ra = 0, Ga = 0, Ba = 0, Rn, Gn, Bn;
    const int S = w * h;

    // calculate the average values for RED, GREEN and BLUE
    // color components
    for(int x = 0;x < S;x++)
    {
	Ra += *bits;
	Ga += *(bits+1);
	Ba += *(bits+2);

	bits += 4;
    }

    Ravg = Ra / S;
    Gavg = Ga / S;
    Bavg = Ba / S;

    bits = (unsigned char *)image;

    // ok, now change contrast
    // with the terms of alghoritm:
    //
    // if contrast > 0: I = (I - Avg) * 256 / (256 - contrast) + Avg
    // if contrast < 0: I = (I - Avg) * (256 + contrast) / 256 + Avg
    //
    // where
    //   I - current color component value
    //   Avg - average value of this component (Ravg, Gavg or Bavg)
    //
    for(int x = 0;x < S;x++)
    {
	Rn = (contrast > 0) ? ((*bits - Ravg) * 256 / (256 - contrast) + Ravg) : ((*bits - Ravg) * (256 + contrast) / 256 + Ravg);
	Gn = (contrast > 0) ? ((*(bits+1) - Gavg) * 256 / (256 - contrast) + Gavg) : ((*(bits+1) - Gavg) * (256 + contrast) / 256 + Gavg);
	Bn = (contrast > 0) ? ((*(bits+2) - Bavg) * 256 / (256 - contrast) + Bavg) : ((*(bits+2) - Bavg) * (256 + contrast) / 256 + Bavg);

	*bits = Rn < 0 ? 0 : (Rn > 255 ? 255 : Rn);
	*(bits+1) = Gn < 0 ? 0 : (Gn > 255 ? 255 : Gn);
	*(bits+2) = Bn < 0 ? 0 : (Bn > 255 ? 255 : Bn);

	bits += 4;
    }
}
