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

typedef char            s8;
typedef unsigned char   u8;

typedef short           s16;
typedef unsigned short  u16;

typedef int             s32;
typedef unsigned int    u32;

#define PACKED __attribute__ ((packed))

namespace fmt_filters
{

	struct image
	{
	    image() : data(0), w(0), h(0)
	    {}

	    image(unsigned char *d, int _w, int _h) : data(d), w(_w), h(_h)
	    {}

	    unsigned char *data;
	    int w;
	    int h;
	};
	
	struct rgb
	{
	    rgb() : r(0), g(0), b(0)
	    {}

	    rgb(int _r, int _g, int _b) : r(_r), g(_g), b(_b)
	    {}

	    unsigned char r;
	    unsigned char g;
	    unsigned char b;
	};

        struct rgba
        {
            rgba(int r1, int g1, int b1, int a1) : r(r1), g(g1), b(b1), a(a1)
            {}

            rgba() : r(0), g(0), b(0), a(0)
            {}

            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };

        enum NoiseType
        {
                UniformNoise = 0,            // Uniform distribution
                GaussianNoise,               // Gaussian distribution
                MultiplicativeGaussianNoise, // Multiplicative Gaussian distribution
                ImpulseNoise,                // Impulse distribution
                LaplacianNoise,              // Laplacian distribution
                PoissonNoise                 // Poisson distribution
        };


	bool checkImage(const image &im);

	// colorize the image, which is w x h, left alpha channel unchanged.
	//
	// it just adds to each pixel in the image
	// aproproriate value.
	void colorize(const image &im, int red, int green, int blue);

	// change brightness of the image
	void brightness(const image &im, int bn);

	// change gamma
	// gamma should be  0.0 <= L <= 6.0
	//
	// it is no problem to set L to 8.0 or 9.0, but the resulting
	// image won't have much difference from 6.0
	void gamma(const image &im, double L);

	// change contrast with Photoshop-like method
	// contrast should be  -255 <= contrast <= 255
	void contrast(const image &im, int contrast);

	enum swapRGBtype { GBR = 0, BRG = 1 };

	// negative
	void negative(const image &im);

	// swap RGB values
	void swapRGB(const image &im, int type);

        //
        // All the following filters are ported from KDE's
        // KImageEffect. See kdelibs/kdefx/kimageeffect.cpp
        // for more.
        //

	// blend
        // opacity = [0.0; 1.0]
	void blend(const image &im, const rgb &rgb, float opacity);

        // val = [0.0; 1.0]
        void fade(const image &im, const rgb &rgb, float val);

        void gray(const image &im);

        // desat = [0.0; 1.0]
        void desaturate(const image &im, float desat);

        // threshold = [0; 255]
        void threshold(const image &im, unsigned int threshold);

        // factor = [0.0; 50.0]
        void solarize(const image &im, double factor);

        // amount = [1; 10]
        void spread(const image &im, unsigned int amount);

        // degrees = [-720.0; 720.0]
        void swirl(const image &im, double degrees, const rgba &background);

        void noise(const image &im, NoiseType noise_type);

        void flatten(const image &im, const rgb &ca, const rgb &cb);

        // amplitude = [0.01; 50.0], wavelength = [0.01; 360.0]
        void wave(const image &im, double amplitude, double wavelength, const rgba &background, s32 *, rgba **);

        // azimuth = [0.0; 90.0], elevation = [0.0; 90.0]
        void shade(const image &im, bool color, double azimuth, double elevation);

        void equalize(const image &im);

        // radius = [0.01; 90.0], sigma = [0.01; 50.0]
        void blur(const image &im, double radius, double sigma);

        void despeckle(const image &im);

        // factor = [0; 100]
        void implode(const image &im, double factor, const rgba &background);

        // radius = [0.01; 30.0]
        void edge(image &im, double radius);

        // radius = [0.01; 99.9], sigma = [0.01; 99.9]
        void emboss(image &im, double radius, double sigma);

        // radius = [0.01; 99.9], sigma = [0.01; 30.0]
        void sharpen(image &im, double radius, double sigma);

        // radius = [1.0; 5.0]
        void oil(const image &im, double radius);

        /*
         *  Resize methods ported from DevIL library, which is licensed under LGPL
         */
        enum ILUResizeMethod
        {

            ILU_SCALE_BOX = 0,
            ILU_SCALE_TRIANGLE,
            ILU_SCALE_BELL,
            ILU_SCALE_BSPLINE,
            ILU_SCALE_LANCZOS3,
            ILU_SCALE_MITCHELL
                            
        };

        // Resize image and store new one in 'result',
        // return true on success
        bool resize(image &im, const int new_w, const int new_h, int method, u8 **result);

        // Do we need some color definitions ?
        static const rgba white       = rgba(255, 255, 255, 255);
        static const rgba black       = rgba(0,   0,   0,   255);
        static const rgba red         = rgba(255, 0,   0,   255);
        static const rgba green       = rgba(0,   255, 0,   255);
        static const rgba blue        = rgba(0,   0,   255, 255);
        static const rgba cyan        = rgba(0,   255, 255, 255);
        static const rgba magenta     = rgba(255, 0,   255, 255);
        static const rgba yellow      = rgba(255, 255, 0,   255);
        static const rgba mediumgray  = rgba(128, 128, 128, 255);
        static const rgba lightgray   = rgba(160, 160, 164, 255);
        static const rgba normalgray  = rgba(192, 192, 192, 255);
        static const rgba darkred     = rgba(128, 0,   0,   255);
        static const rgba darkgreen   = rgba(0,   128, 0,   255);
        static const rgba darkblue    = rgba(0,   0,   128, 255);
        static const rgba darkcyan    = rgba(0,   128, 128, 255);
        static const rgba darkmagenta = rgba(128, 0,   128, 255);
        static const rgba darkyellow  = rgba(128, 128, 0,   255);
}

#endif
