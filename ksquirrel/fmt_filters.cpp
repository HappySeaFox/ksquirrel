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

#include <cmath>
#include <algorithm>

#define MaxRGB 255L
#define DegreesToRadians(x) ((x)*M_PI/180.0)
#define MagickSQ2PI 2.50662827463100024161235523934010416269302368164062
#define MagickEpsilon  1.0e-12
#define MagickPI  3.14159265358979323846264338327950288419716939937510

#define F_MAX(a, b)  ((b) < (a) ? (a) : (b))
#define F_MIN(a, b)  ((a) < (b) ? (a) : (b))

static void rgb2hsv(const fmt_filters::rgb &rgb, s32 *h, s32 *s, s32 *v);
static void hsv2rgb(s32 h, s32 s, s32 v, fmt_filters::rgb *rgb);
static fmt_filters::rgba interpolateColor(const fmt_filters::image &image, double x_offset, double y_offset, const fmt_filters::rgba &background);
static u32 generateNoise(u32 pixel, fmt_filters::NoiseType noise_type);
static u32 intensityValue(s32 r, s32 g, s32 b);
static u32 intensityValue(const fmt_filters::rgba &rr);
static s32 getBlurKernel(s32 width, double sigma, double **kernel);
static void blurScanLine(double *kernel, s32 width, fmt_filters::rgba *src, fmt_filters::rgba *dest, s32 columns);
static void hull(const s32 x_offset, const s32 y_offset, const s32 polarity, const s32 columns,
                        const s32 rows, u8 *f, u8 *g);
static bool convolveImage(fmt_filters::image *image, fmt_filters::rgba **dest, const unsigned int order, const double *kernel);
static int getOptimalKernelWidth(double radius, double sigma);
static u32 iluScaleAdvanced(fmt_filters::image *iluCurImage, u32 Width, u32 Height, u32 Filter, u8 **nn);

template<class T>
static void scaleDown(T &val, T min, T max);

struct double_packet
{
    double red;
    double green;
    double blue;
    double alpha;
};

struct short_packet
{
    unsigned short int red;
    unsigned short int green;
    unsigned short int blue;
    unsigned short int alpha;
};

bool fmt_filters::checkImage(const image &im)
{
    return (im.w && im.h && im.data);
}

// colorize tool
void fmt_filters::colorize(const image &im, s32 red, s32 green, s32 blue)
{
    // check if all parameters are good
    if(!checkImage(im))
	return;

    if(!red && !green && !blue)
	return;

    u8 *bits = im.data;
    s32 val;
    const s32 S = im.w * im.h;

    s32 V[3] = { red, green, blue };

    // add to RED component 'red' value, and check if the result is out of bounds.
    // do the same with GREEN and BLUE channels.
    for(s32 x = 0;x < S;++x)
    {
	for(s32 v = 0;v < 3;++v)
	{
	    val = (s32)*(bits + v) + V[v];

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
void fmt_filters::brightness(const image &im, s32 bn)
{
    // check if all parameters are good
    if(!checkImage(im))
	return;

    u8 *bits = im.data;
    s32 val;
    const s32 S = im.w * im.h * 4;

    // add to all color components 'bn' value, and check if the result is out of bounds.
    for(s32 x = 0;x < S;x++)
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
void fmt_filters::gamma(const image &im, double L)
{
    // check if all parameters are good
    if(!checkImage(im))
	return;

    if(L == 0 || L < 0) L = 0.01;

    rgba *_rgba = (rgba *)im.data;
    u8 R, G, B;
    s32 X;
    u8 GT[256];

    GT[0] = 0;

    // fill the array with gamma koefficients
    for (X = 1; X < 256; ++X)
	GT[X] = (u8)round(255 * pow((double)X / 255.0, 1.0 / L));

    const s32 S = im.w * im.h;

    // now change gamma
    for(X = 0; X < S; ++X)
    {
	R = _rgba[X].r;
	G = _rgba[X].g;
	B = _rgba[X].b;

	_rgba[X].r = GT[R];
	_rgba[X].g = GT[G];
	_rgba[X].b = GT[B];
    }
}

// contrast tool
void fmt_filters::contrast(const image &im, s32 contrast)
{
    if(!checkImage(im) || !contrast)

    if(contrast <= -256) contrast = -255;
    if(contrast >= 256) contrast = 255;

    u8 *bits = im.data, Ravg, Gavg, Bavg;
    s32 Ra = 0, Ga = 0, Ba = 0, Rn, Gn, Bn;
    const s32 S = im.w * im.h;

    // calculate the average values for RED, GREEN and BLUE
    // color components
    for(s32 x = 0;x < S;x++)
    {
	Ra += *bits;
	Ga += *(bits+1);
	Ba += *(bits+2);

	bits += 4;
    }

    Ravg = Ra / S;
    Gavg = Ga / S;
    Bavg = Ba / S;

    bits = im.data;

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
    for(s32 x = 0;x < S;x++)
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

// negative
void fmt_filters::negative(const image &im)
{
    // check if all parameters are good
    if(!checkImage(im))
	return;

    u8 *bits = im.data, R, G, B;
    const s32 S = im.w * im.h;

    for(s32 X = 0; X < S; ++X)
    {
	R = *bits;
	G = *(bits+1);
	B = *(bits+2);

	*bits     = 255 - R;
	*(bits+1) = 255 - G;
	*(bits+2) = 255 - B;

	bits += 4;
    }
}

// swap RGB values
void fmt_filters::swapRGB(const image &im, s32 type)
{
    // check if all parameters are good
    if(!checkImage(im) || (type != fmt_filters::GBR && type != fmt_filters::BRG))
	return;

    rgba *_rgba = (rgba *)im.data;
    u8 R, G, B;
    const s32 S = im.w * im.h;

    for(s32 X = 0; X < S; ++X)
    {
	R = _rgba[X].r;
	G = _rgba[X].g;
	B = _rgba[X].b;

	_rgba[X].r = (type == fmt_filters::GBR) ? G : B;
	_rgba[X].g = (type == fmt_filters::GBR) ? B : R;
	_rgba[X].b = (type == fmt_filters::GBR) ? R : G;
    }
}

// blend
void fmt_filters::blend(const image &im, const rgb &rgb, float opacity)
{
    // check parameters
    if(!checkImage(im))
	return;

    scaleDown(opacity, 0.0f, 1.0f);

    rgba *_rgba = (rgba *)im.data;
    s32 S = im.w * im.h;
    s32 r = rgb.r, g = rgb.g, b = rgb.b;

    // blend!
    for(s32 i = 0; i < S;i++)
    {
        _rgba[i].r = _rgba[i].r + (u8)((b - _rgba[i].r) * opacity);
        _rgba[i].g = _rgba[i].g + (u8)((g - _rgba[i].g) * opacity);
        _rgba[i].b = _rgba[i].b + (u8)((r - _rgba[i].b) * opacity);
    }
}

void fmt_filters::flatten(const image &im, const rgb &ca, const rgb &cb)
{
    if(!checkImage(im))
      return;

    s32 r1 = ca.r; s32 r2 = cb.r;
    s32 g1 = ca.g; s32 g2 = cb.g;
    s32 b1 = ca.b; s32 b2 = cb.b;
    s32 min = 0, max = 255;
    s32 mean;

    rgba *_rgba = (rgba *)im.data, *data;
    rgb _rgb;

    for(s32 y = 0;y < im.h;++y)
    {
        data = _rgba + im.w*y;

        for(s32 x = 0;x < im.w;++x)
        {
            mean = ((data+x)->r + (data+x)->g + (data+x)->b) / 3;
            min = F_MIN(min, mean);
            max = F_MAX(max, mean);
        }
    }

    // Conversion factors
    float sr = ((float) r2 - r1) / (max - min);
    float sg = ((float) g2 - g1) / (max - min);
    float sb = ((float) b2 - b1) / (max - min);


    // Repaint the image
    for(s32 y = 0;y < im.h;++y)
    {
        data = _rgba + im.w*y;

        for(s32 x = 0;x < im.w;++x)
        {
            mean = ((data+x)->r + (data+x)->g + (data+x)->b) / 3;

            (data+x)->r = (s32)(sr * (mean - min) + r1 + 0.5);
            (data+x)->g = (s32)(sg * (mean - min) + g1 + 0.5);
            (data+x)->b = (s32)(sb * (mean - min) + b1 + 0.5);
        }
    }
}

void fmt_filters::fade(const image &im, const rgb &rgb, float val)
{
    if(!checkImage(im))
        return;

    u8 tbl[256];

    for (s32 i = 0;i < 256;i++)
        tbl[i] = (s32)(val * i + 0.5);

    s32 r, g, b, cr, cg, cb;

    rgba *_rgba = (rgba *)im.data;

    const s32 S = im.w * im.h;

    for (s32 i = 0;i < S;i++)
    {
        cr = _rgba[i].r;
        cg = _rgba[i].g;
        cb = _rgba[i].b;

        r = (cr > rgb.r) ? (cr - tbl[cr - rgb.r]) : (cr + tbl[rgb.r - cr]);
        g = (cg > rgb.g) ? (cg - tbl[cg - rgb.g]) : (cg + tbl[rgb.g - cg]);
        b = (cb > rgb.b) ? (cb - tbl[cb - rgb.b]) : (cb + tbl[rgb.b - cb]);

        _rgba[i].r = r;
        _rgba[i].g = g;
        _rgba[i].b = b;
    }
}

void fmt_filters::gray(const image &im)
{
    if(!checkImage(im))
        return;

    rgba *_rgba = (rgba *)im.data;
    const s32 S = im.w * im.h;
    s32 g;

    for(s32 i = 0;i < S;++i)
    {
        g = (_rgba[i].r * 11 + _rgba[i].g * 16 + _rgba[i].b * 5)/32;

        _rgba[i].r = g;
        _rgba[i].g = g;
        _rgba[i].b = g;
    }
}

void fmt_filters::desaturate(const image &im, float desat)
{
    if(!checkImage(im))
      return;

    scaleDown(desat, 0.0f, 1.0f);

    rgba *_rgba = (rgba *)im.data;
    const s32 S = im.w * im.h;
    s32 h, s, v;

    for(s32 i = 0;i < S;++i)
    {
        rgb _rgb(_rgba[i].r, _rgba[i].g, _rgba[i].b);
        rgb2hsv(_rgb, &h, &s, &v);
        hsv2rgb(h, (s32)(s * (1.0 - desat)), v, &_rgb);

        _rgba[i].r = _rgb.r;
        _rgba[i].g = _rgb.g;
        _rgba[i].b = _rgb.b;
    }
}

void fmt_filters::threshold(const image &im, u32 trh)
{
    if(!checkImage(im))
        return;

    scaleDown(trh, (u32)0, (u32)255);

    const s32 S = im.w * im.h;
    rgba *_rgba = (rgba *)im.data;

    for(s32 i = 0;i < S;++i)
    {
        if(intensityValue(_rgba[i].r, _rgba[i].g, _rgba[i].b) < trh)
            _rgba[i].r = _rgba[i].g = _rgba[i].b = 0;
        else
            _rgba[i].r = _rgba[i].g = _rgba[i].b = 255;
    }
}

void fmt_filters::solarize(const image &im, double factor)
{
    if(!checkImage(im))
        return;

    s32 threshold;
    rgba *_rgba = (rgba *)im.data;
    const s32 S = im.w * im.h;

    threshold = (s32)(factor * (MaxRGB+1)/100.0);

    for(s32 i = 0;i < S;++i)
    {
        _rgba[i].r = _rgba[i].r > threshold ? MaxRGB-_rgba[i].r : _rgba[i].r;
        _rgba[i].g = _rgba[i].g > threshold ? MaxRGB-_rgba[i].g : _rgba[i].g;
        _rgba[i].b = _rgba[i].b > threshold ? MaxRGB-_rgba[i].b : _rgba[i].b;
    }
}

void fmt_filters::spread(const image &im, u32 amount)
{
    if(!checkImage(im) || im.w < 3 || im.h < 3)
        return;

    rgba *n = new rgba [im.w * im.h];

    if(!n)
        return;

    s32 quantum;
    s32 x_distance, y_distance;

    rgba *_rgba = (rgba *)im.data;

    memcpy(n, im.data, im.w * im.h * sizeof(rgba));

    quantum = (amount+1) >> 1;

    rgba *p, *q;

    for(s32 y = 0;y < im.h;y++)
    {
        q = n + im.w*y;

        for(s32 x = 0;x < im.w;x++)
        {
            x_distance = x + ((rand() & (amount+1))-quantum);
            y_distance = y + ((rand() & (amount+1))-quantum);
            x_distance = F_MIN(x_distance, im.w-1);
            y_distance = F_MIN(y_distance, im.h-1);

            if(x_distance < 0)
                x_distance = 0;

            if(y_distance < 0)
                y_distance = 0;

            p = _rgba + y_distance*im.w;
            p += x_distance;

            *q++ = (*p);
        }
    }

    memcpy(im.data, n, im.w * im.h * sizeof(rgba));

    delete [] n;
}

void fmt_filters::swirl(const image &im, double degrees, const rgba &background)
{
    if(!checkImage(im))
        return;

    double cosine, distance, factor, radius, sine, x_center, x_distance,
        x_scale, y_center, y_distance, y_scale;
    s32 x, y;

    rgba *q, *p;
    rgba *_rgba = (rgba *)im.data;
    rgba *dest = new rgba [im.w * im.h];

    if(!dest)
        return;

    memcpy(dest, im.data, im.w * im.h * sizeof(rgba));

    // compute scaling factor
    x_center = im.w / 2.0;
    y_center = im.h / 2.0;

    radius = F_MAX(x_center, y_center);
    x_scale=1.0;
    y_scale=1.0;

    if(im.w > im.h)
        y_scale=(double)im.w / im.h;
    else if(im.w < im.h)
        x_scale=(double)im.h / im.w;

    degrees = DegreesToRadians(degrees);

    // swirl each row

    for(y = 0;y < im.h;y++)
    {
        p = _rgba + im.w * y;
        q = dest + im.w * y;
        y_distance = y_scale*(y-y_center);

        for(x = 0;x < im.w;x++)
        {
            // determine if the pixel is within an ellipse
            *q = (*p);
            x_distance = x_scale*(x-x_center);
            distance = x_distance*x_distance+y_distance*y_distance;

            if (distance < (radius*radius))
            {
                // swirl
                factor = 1.0 - sqrt(distance)/radius;
                sine = sin(degrees*factor*factor);
                cosine = cos(degrees*factor*factor);

                *q = interpolateColor(im,
                                      (cosine*x_distance-sine*y_distance)/x_scale+x_center,
                                      (sine*x_distance+cosine*y_distance)/y_scale+y_center,
                                      background);
            }

            p++;
            q++;
        }
    }

    memcpy(im.data, dest, im.w * im.h * sizeof(rgba));

    delete [] dest;
}

void fmt_filters::noise(const image &im, NoiseType noise_type)
{
    if(!checkImage(im))
        return;

    s32 x, y;
    rgba *dest = new rgba [im.w * im.h];

    if(!dest)
        return;

    rgba *_rgba = (rgba *)im.data;

    rgba *srcData;
    rgba *destData;

    for(y = 0;y < im.h;++y)
    {
        srcData = _rgba + im.w * y;
        destData = dest + im.w * y;

        for(x = 0;x < im.w;++x)
        {
            destData[x].r = generateNoise(srcData[x].r, noise_type);
            destData[x].g = generateNoise(srcData[x].g, noise_type);
            destData[x].b = generateNoise(srcData[x].b, noise_type);
        }
    }

    memcpy(im.data, dest, im.w * im.h * sizeof(rgba));

    delete [] dest;
}

void fmt_filters::implode(const image &im, double factor, const rgba &background)
{
    if(!checkImage(im))
        return;

    double amount, distance, radius;
    double x_center, x_distance, x_scale;
    double y_center, y_distance, y_scale;
    rgba *dest, *src;
    s32 x, y;

    rgba *n = new rgba [im.w * im.h];

    if(!n)
        return;

    rgba *_rgba = (rgba *)im.data;

    // compute scaling factor
    x_scale = 1.0;
    y_scale = 1.0;
    x_center = (double)0.5*im.w;
    y_center = (double)0.5*im.h;
    radius=x_center;

    if(im.w > im.h)
        y_scale = (double)im.w/im.h;
    else if(im.w < im.h)
    {
        x_scale = (double)im.h/im.w;
        radius = y_center;
    }

    amount=factor/10.0;

    if(amount >= 0)
        amount/=10.0;

    for(y = 0;y < im.h;++y)
    {
        src  = _rgba + im.w*y;
        dest =  n + im.w*y;
        y_distance=y_scale*(y-y_center);
        double factor;

        for(x = 0;x < im.w;++x)
        {
            x_distance = x_scale*(x-x_center);
            distance= x_distance*x_distance+y_distance*y_distance;

            if(distance < (radius*radius))
            {
                // Implode the pixel.
                factor = 1.0;

                if(distance > 0.0)
                    factor = pow(sin(0.5000000000000001*M_PI*sqrt(distance)/radius),-amount);

                dest[x] = interpolateColor(im, factor*x_distance/x_scale+x_center,
                                               factor*y_distance/y_scale+y_center,
                                               background);
            }
            else
                dest[x] = _rgba[x];
        }
    }

    memcpy(im.data, n, im.w * im.h * sizeof(rgba));

    delete [] n;
}

void fmt_filters::despeckle(const image &im)
{
    if(!checkImage(im))
        return;

    s32 i, j, x, y;
    u8 *blue_channel, *red_channel, *green_channel, *buffer, *alpha_channel;
    s32 packets;

    static const s32
    X[4]= {0, 1, 1,-1},
    Y[4]= {1, 0, 1, 1};

    rgba *dest;
    rgba *n;

    n = new rgba [im.w * im.h];

    if(!n)
        return;

    packets = (im.w+2) * (im.h+2);

    red_channel = new u8 [packets];
    green_channel = new u8 [packets];
    blue_channel = new u8 [packets];
    alpha_channel = new u8 [packets];
    buffer = new u8 [packets];

    if(!red_channel || ! green_channel || ! blue_channel || ! alpha_channel || !buffer)
    {
        if(!red_channel)   delete [] red_channel;
        if(!green_channel) delete [] green_channel;
        if(!blue_channel)  delete [] blue_channel;
        if(!alpha_channel) delete [] alpha_channel;
        if(!buffer)        delete [] buffer;

        delete [] n;

        return;
    }

    // copy image pixels to color component buffers
    j = im.w+2;

    rgba *src;
    rgba *_rgba = (rgba *)im.data;

    for(y = 0;y < im.h;++y)
    {
        src = _rgba + im.w*y;
        ++j;

        for(x = 0;x < im.w;++x)
        {
            red_channel[j] = src[x].r;
            green_channel[j] = src[x].g;
            blue_channel[j] = src[x].b;
            alpha_channel[j] = src[x].a;

            ++j;
        }

        ++j;
    }

    // reduce speckle in red channel
    for(i = 0;i < 4;i++)
    {
        hull(X[i],Y[i],1,im.w,im.h,red_channel,buffer);
        hull(-X[i],-Y[i],1,im.w,im.h,red_channel,buffer);
        hull(-X[i],-Y[i],-1,im.w,im.h,red_channel,buffer);
        hull(X[i],Y[i],-1,im.w,im.h,red_channel,buffer);
    }

    // reduce speckle in green channel
    for(i = 0;i < packets;i++)
        buffer[i] = 0;

    for(i = 0;i < 4;i++)
    {
        hull(X[i],Y[i],1,im.w,im.h,green_channel,buffer);
        hull(-X[i],-Y[i],1,im.w,im.h,green_channel,buffer);
        hull(-X[i],-Y[i],-1,im.w,im.h,green_channel,buffer);
        hull(X[i],Y[i],-1,im.w,im.h,green_channel,buffer);
    }

    // reduce speckle in blue channel
    for(i = 0;i < packets;i++)
        buffer[i] = 0;

    for(i = 0;i < 4;i++)
    {
        hull(X[i],Y[i],1,im.w,im.h,blue_channel,buffer);
        hull(-X[i],-Y[i],1,im.w,im.h,blue_channel,buffer);
        hull(-X[i],-Y[i],-1,im.w,im.h,blue_channel,buffer);
        hull(X[i],Y[i],-1,im.w,im.h,blue_channel,buffer);
    }

    // copy color component buffers to despeckled image
    j = im.w+2;

    for(y = 0;y < im.h;++y)
    {
        dest = n + im.w*y;
        ++j;

        for(x = 0;x < im.w;++x)
        {
            dest[x] = rgba(red_channel[j], green_channel[j],
                                blue_channel[j], alpha_channel[j]);

            ++j;
        }

        ++j;
    }

    delete [] buffer;
    delete [] red_channel;
    delete [] green_channel;
    delete [] blue_channel;
    delete [] alpha_channel;

    memcpy(im.data, n, im.w * im.h * sizeof(rgba));

    delete [] n;
}

void fmt_filters::blur(const image &im, double radius, double sigma)
{
    if(!checkImage(im))
        return;

    double *kernel;
    rgba *dest;
    s32 width;
    s32 x, y;
    rgba *scanline, *temp;
    rgba *p, *q;

    if(sigma == 0.0)
        return;

    kernel = NULL;

    if(radius > 0)
        width = getBlurKernel((s32)(2*ceil(radius)+1), sigma, &kernel);
    else
    {
        double *last_kernel = NULL;

        width = getBlurKernel(3, sigma, &kernel);

        while((long)(MaxRGB * kernel[0]) > 0)
        {
            if(last_kernel)
                delete [] last_kernel;

            last_kernel = kernel;
            kernel = NULL;

            width = getBlurKernel(width+2, sigma, &kernel);
        }

        if(last_kernel)
        {
            delete [] kernel;
            width -= 2;
            kernel = last_kernel;
        }
    }

    if(width < 3)
    {
        delete [] kernel;
        return;
    }

    dest = new rgba [im.w * im.h];

    if(!dest)
    {
        delete [] kernel;
        return;
    }

    scanline = new rgba [im.h];
    temp = new rgba [im.h];

    if(!scanline || !temp)
    {
        if(!scanline) delete [] scanline;
        if(!temp) delete [] temp;

        delete [] kernel;
        return;
    }

    rgba *_rgba = (rgba *)im.data;

    for(y = 0;y < im.h;++y)
    {
        p = _rgba + im.w*y;
        q = dest + im.w*y;

        blurScanLine(kernel, width, p, q, im.w);
    }

    for(x = 0;x < im.w;++x)
    {
        for(y = 0;y < im.h;++y)
            scanline[y] = *(_rgba + im.w*y + x);

        blurScanLine(kernel, width, scanline, temp, im.h);

        for(y = 0;y < im.h;++y)
            *(dest + im.w*y + x) = temp[y];
        
    }

    delete [] scanline;
    delete [] temp;
    delete [] kernel;

    memcpy(im.data, dest, im.w * im.h * sizeof(rgba));

    delete [] dest;
}

void fmt_filters::equalize(const image &im)
{
    if(!checkImage(im))
        return;

    double_packet high, low, intensity, *map, *histogram;
    short_packet *equalize_map;
    s32 x, y;
    rgba *p, *q;
    long i;
    u8 r, g, b, a;

    histogram = new double_packet [256];
    map = new double_packet [256];
    equalize_map = new short_packet [256];

    if(!histogram || !map || !equalize_map)
    {
        if(histogram)
            delete [] histogram;

        if(map)
            delete [] map;

        if(equalize_map)
            delete [] equalize_map;

        return;
    }

    rgba *_rgba = (rgba *)im.data;

    /*
     *  Form histogram.
     */
    memset(histogram, 0, 256*sizeof(double_packet));

    for(y = 0;y < im.h;++y)
    {
        p = _rgba + im.w * y;

        for(x = 0;x < im.w;++x)
        {
            histogram[p->r].red++;
            histogram[p->g].green++;
            histogram[p->b].blue++;
            histogram[p->a].alpha++;

            p++;
        }
    }
    /*
     Integrate the histogram to get the equalization map.
     */
    memset(&intensity, 0 ,sizeof(double_packet));

    for(i = 0;i <= 255;++i)
    {
        intensity.red += histogram[i].red;
        intensity.green += histogram[i].green;
        intensity.blue += histogram[i].blue;
        intensity.alpha += histogram[i].alpha;
        map[i]=intensity;
    }

    low=map[0];
    high=map[255];
    memset(equalize_map, 0, 256*sizeof(short_packet));

    for(i = 0;i <= 255;++i)
    {
        if(high.red != low.red)
            equalize_map[i].red=(unsigned short)
                ((65535*(map[i].red-low.red))/(high.red-low.red));
        if(high.green != low.green)
            equalize_map[i].green=(unsigned short)
                ((65535*(map[i].green-low.green))/(high.green-low.green));
        if(high.blue != low.blue)
            equalize_map[i].blue=(unsigned short)
                ((65535*(map[i].blue-low.blue))/(high.blue-low.blue));
        if(high.alpha != low.alpha)
            equalize_map[i].alpha=(unsigned short)
                ((65535*(map[i].alpha-low.alpha))/(high.alpha-low.alpha));
    }

    delete [] histogram;
    delete [] map;

    /*
     Stretch the histogram.
     */
    for(y = 0;y < im.h;++y)
    {
        q = _rgba + im.w*y;

        for(x = 0;x < im.w;++x)
        {
            if(low.red != high.red)
                r = (equalize_map[(unsigned short)(q[x].r)].red/257);
            else
                r = q[x].r;
            if(low.green != high.green)
                g = (equalize_map[(unsigned short)(q[x].g)].green/257);
            else
                g = q[x].g;
            if(low.blue != high.blue)
                b = (equalize_map[(unsigned short)(q[x].b)].blue/257);
            else
                b = q[x].b;
            if(low.alpha != high.alpha)
                a = (equalize_map[(unsigned short)(q[x].a)].alpha/257);
            else
                a = q[x].a;

            q[x] = rgba(r, g, b, a);
        }
    }

    delete [] equalize_map;
}

void fmt_filters::wave(const image &im, double amplitude, double wavelength,
                          const rgba &background, s32 *H, rgba **data)
{
    if(!checkImage(im))
        return;

    double *sine_map;
    rgba *q;

    s32 new_h = im.h + (s32)(2*fabs(amplitude));

    rgba *n = new rgba [im.w * new_h];

    if(!n)
        return;

    sine_map = new double [im.w];

    if(!sine_map)
    {
        delete [] n;
        return;
    }

    for(s32 x = 0;x < im.w;++x)
        sine_map[x] = fabs(amplitude) + amplitude*sin((2*M_PI*x)/wavelength);

    for(s32 y = 0;y < new_h;++y)
    {
        q = n + im.w*y;

        for(s32 x = 0;x < im.w;x++)
        {
            *q = interpolateColor(im, x, (s32)(y-sine_map[x]), background);

            ++q;
        }
    }

    delete [] sine_map;

    *H = new_h;
    *data = n;
}

void fmt_filters::shade(const image &im, bool color_shading, double azimuth,
             double elevation)
{
    if(!checkImage(im))
        return;

    rgba *n = new rgba [im.w * im.h];

    if(!n)
        return;

    struct PointInfo{
        double x, y, z;
    };

    double distance, normal_distance, shade;
    s32 x, y;

    struct PointInfo light, normal;

    rgba *_rgba = (rgba *)im.data;
    rgba *q;

    azimuth = DegreesToRadians(azimuth);
    elevation = DegreesToRadians(elevation);
    light.x = MaxRGB*cos(azimuth)*cos(elevation);
    light.y = MaxRGB*sin(azimuth)*cos(elevation);
    light.z = MaxRGB*sin(elevation);
    normal.z= 2*MaxRGB;  // constant Z of surface normal

    rgba *p, *s0, *s1, *s2;

    for(y = 0;y < im.h;++y)
    {
        p = _rgba + im.w * (F_MIN(F_MAX(y-1,0),im.h-3));
        q = n + im.w * y;

        // shade this row of pixels.
        *q++=(*(p+im.w));
        p++;
        s0 = p;
        s1 = p + im.w;
        s2 = p + 2*im.w;

        for(x = 1;x < im.w-1;++x)
        {
            // determine the surface normal and compute shading.
            normal.x=intensityValue(*(s0-1))+intensityValue(*(s1-1))+intensityValue(*(s2-1))-
                (double) intensityValue(*(s0+1))-(double) intensityValue(*(s1+1))-
                (double) intensityValue(*(s2+1));
            normal.y=intensityValue(*(s2-1))+intensityValue(*s2)+intensityValue(*(s2+1))-
                (double) intensityValue(*(s0-1))-(double) intensityValue(*s0)-
                (double) intensityValue(*(s0+1));
            if((normal.x == 0) && (normal.y == 0))
                shade=light.z;
            else
            {
                shade = 0.0;
                distance = normal.x*light.x+normal.y*light.y+normal.z*light.z;

                if(distance > 0.0)
                {
                    normal_distance=
                        normal.x*normal.x+normal.y*normal.y+normal.z*normal.z;
                    if(fabs(normal_distance) > 0.0000001)
                        shade=distance/sqrt(normal_distance);
                }
            }
          
            if(!color_shading)
            {
                *q = rgba((u8)(shade),
                           (u8)(shade),
                           (u8)(shade),
                           s1->a);
            }
            else
            {
                *q = rgba((u8)((shade * s1->r)/(MaxRGB+1)),
                           (u8)((shade * s1->g)/(MaxRGB+1)),
                           (u8)((shade * s1->b)/(MaxRGB+1)),
                           s1->a);
            }

            ++s0;
            ++s1;
            ++s2;
            q++;
        }

        *q++=(*s1);
    }

    memcpy(im.data, n, im.w * im.h * sizeof(rgba));

    delete [] n;
}

void fmt_filters::edge(image &im, double radius)
{
    if(!checkImage(im))
        return;

    double *kernel;
    int width;
    register long i;
    rgba *dest = NULL;
/*
    if(radius == 50.0)
    {
        radius = 0.0;
    }
*/
    width = getOptimalKernelWidth(radius, 0.5);

    const int W = width*width;

    if(im.w < width || im.h < width)
        return;

    kernel = new double [W];

    if(!kernel)
        return;

    for(i = 0;i < W;i++)
        kernel[i] = -1.0;

    kernel[i/2] = W-1.0;

    if(!convolveImage(&im, &dest, width, kernel))
    {
        delete [] kernel;

        if(dest)
            delete [] dest;

        return;
    }

    delete [] kernel;

    memcpy(im.data, dest, im.w * im.h * sizeof(rgba));

    delete [] dest;
}

void fmt_filters::emboss(image &im, double radius, double sigma)
{
    if(!checkImage(im))
        return;

    double alpha, *kernel;
    int j, width;
    register long i, u, v;
    rgba *dest = NULL;

    if(sigma == 0.0)
        return;

    width = getOptimalKernelWidth(radius, sigma);

    if(im.w < width || im.h < width)
        return;
    
    kernel = new double [width*width];

    if(!kernel)
        return;

    i = 0;
    j = width/2;

    const double S = sigma * sigma;

    for(v = (-width/2);v <= (width/2);v++)
    {
        for(u=(-width/2); u <= (width/2); u++)
        {
            alpha = exp(-((double) u*u+v*v)/(2.0*S));

            kernel[i] = ((u < 0) || (v < 0) ? -8.0 : 8.0)*alpha/(2.0*MagickPI*S);

            if (u == j)
                kernel[i]=0.0;

            i++;
        }

        j--;
    }

    if(!convolveImage(&im, &dest, width, kernel))
    {
        delete [] kernel;
        return;
    }

    delete [] kernel;

    fmt_filters::image mm((u8 *)dest, im.w, im.h);

    fmt_filters::equalize(mm);

    memcpy(im.data, dest, im.w * im.h * sizeof(rgba));

    delete [] dest;
}

void fmt_filters::sharpen(image &im, double radius, double sigma)
{
    if(!checkImage(im))
        return;

    double alpha, normalize, *kernel;
    int width;
    register long i, u, v;
    rgba *dest = NULL;

    if(sigma == 0.0)
        sigma = 0.01;

    width = getOptimalKernelWidth(radius, sigma);

    if(im.w < width)
        return;

    kernel = new double [width*width];

    if(!kernel)
        return;

    i = 0;
    normalize = 0.0;
    const double S = sigma * sigma;
    const int w2 = width / 2;

    for(v = -w2; v <= w2; v++)
    {
        for(u = -w2; u <= w2; u++)
        {
            alpha = exp(-((double) u*u+v*v)/(2.0*S));
            kernel[i] = alpha/(2.0*MagickPI*S);
            normalize += kernel[i];

            i++;
        }
    }

    kernel[i/2] = (-2.0)*normalize;

    if(!convolveImage(&im, &dest, width, kernel))
    {
        delete [] kernel;

        if(dest)
            delete [] dest;

        return;
    }

    delete [] kernel;

    memcpy(im.data, dest, im.w * im.h * sizeof(rgba));

    delete [] dest;
}

void fmt_filters::oil(const image &im, double radius)
{
    if(!checkImage(im))
        return;

    unsigned long count;
    unsigned long histogram[256];
    unsigned int k;
    int width;
    int x, y, mx, my, sx, sy;
    int mcx, mcy;
    rgba *s = NULL, *q;

    scaleDown(radius, 1.0, 5.0);

    rgba *n = new rgba [im.w * im.h];

    if(!n)
        return;

    memcpy(n, im.data, im.w * im.h * sizeof(rgba));

    width = getOptimalKernelWidth(radius, 0.5);

    if(im.w < width)
    {
        delete [] n;
        return;
    }

    rgba *_rgba = (rgba *)im.data;

    for(y = 0;y < im.h;++y)
    {
        sy = y-(width/2);
        q = n + im.w*y;

        for(x = 0;x < im.w;++x)
        {
            count = 0;
            memset(histogram, 0, 256 * sizeof(unsigned long));
            sy = y-(width/2);

            for(mcy = 0;mcy < width;++mcy,++sy)
            {
                my = sy < 0 ? 0 : sy > im.h-1 ? im.h-1 : sy;
                sx = x+(-width/2);

                for(mcx = 0; mcx < width;++mcx,++sx)
                {
                    mx = sx < 0 ? 0 : sx > im.w-1 ? im.w-1 : sx;

                    k = intensityValue(*(_rgba + my*im.w + mx));

                    if(k > 255)
                        k = 255;

                    histogram[k]++;

                    if(histogram[k] > count)
                    {
                        count = histogram[k];
                        s = _rgba + my*im.w + mx;
                    }
                }
            }

            *q++ = (*s);
        }
    }

    memcpy(im.data, n, im.w * im.h * sizeof(rgba));

    delete [] n;
}

bool fmt_filters::resize(image &im, const int new_w, const int new_h, int method, u8 **result)
{
    if(!checkImage(im))
        return false;

    rgba *n = new rgba [new_w * new_h];

    if(!n)
        return false;

    s32 res = iluScaleAdvanced(&im, new_w, new_h, method, (u8 **)&n);

    if(res)
    {
        *result = (u8 *)n;
        return true;
    }
    else    
    {
        delete [] n;
        return false;
    }
}

/*************************************************************************/

/*
 *
 *  Helper functions
 *
 */

/*************************************************************************/

static bool convolveImage(fmt_filters::image *image, fmt_filters::rgba **dest, const unsigned int order,
                                 const double *kernel)
{
    long width;
    double red, green, blue, alpha;
    double normalize, *normal_kernel;
    register const double *k;
    register fmt_filters::rgba *q;
    int x, y, mx, my, sx, sy;
    long i;
    int mcx, mcy;

    width = order;

    if((width % 2) == 0)
        return false;

    const int W = width*width;

    normal_kernel = new double [W];

    if(!normal_kernel)
        return false;

    *dest = new fmt_filters::rgba [image->w * image->h];

    if(!*dest)
    {
        delete [] normal_kernel;
        return false;
    }

    normalize = 0.0;

    for(i = 0;i < W;i++)
        normalize += kernel[i];

    if(fabs(normalize) <= MagickEpsilon)
        normalize = 1.0;

    normalize=1.0/normalize;

    for(i = 0;i < W;i++)
        normal_kernel[i] = normalize*kernel[i];

    fmt_filters::rgba *_rgba = (fmt_filters::rgba *)image->data;

    for(y = 0;y < image->h;++y)
    {
        sy = y-(width/2);
        q = *dest + image->w * y;

        for(x = 0;x < image->w;++x)
        {
            k = normal_kernel;
            red = green = blue = alpha = 0;
            sy = y-(width/2);

            for(mcy=0; mcy < width; ++mcy, ++sy)
            {
                my = sy < 0 ? 0 : sy > image->h-1 ? image->h-1 : sy;
                sx = x+(-width/2);

                for(mcx=0; mcx < width; ++mcx, ++sx)
                {
                    mx = sx < 0 ? 0 : sx > image->w-1 ? image->w-1 : sx;
                    red +=   (*k) * ((_rgba + image->w*my+mx)->r*257);
                    green += (*k) * ((_rgba + image->w*my+mx)->g*257);
                    blue +=  (*k) * ((_rgba + image->w*my+mx)->b*257);
                    alpha += (*k) * ((_rgba + image->w*my+mx)->a*257);
                    ++k;
                }
            }

            red = red < 0 ? 0 : red > 65535 ? 65535 : red+0.5;
            green = green < 0 ? 0 : green > 65535 ? 65535 : green+0.5;
            blue = blue < 0 ? 0 : blue > 65535 ? 65535 : blue+0.5;
            alpha = alpha < 0 ? 0 : alpha > 65535 ? 65535 : alpha+0.5;

            *q++ = fmt_filters::rgba((unsigned char)(red/257UL),
                         (unsigned char)(green/257UL),
                         (unsigned char)(blue/257UL),
                         (unsigned char)(alpha/257UL));
        }
    }

    delete [] normal_kernel;

    return true;
}

static void rgb2hsv(const fmt_filters::rgb &rgb, s32 *h, s32 *s, s32 *v)
{
    if(!h || !s || !v)
        return;

    s32 r = rgb.r;
    s32 g = rgb.g;
    s32 b = rgb.b;

    u32 max = r;
    s32 whatmax = 0;                // r=>0, g=>1, b=>2

    if((u32)g > max)
    {
        max = g;
        whatmax = 1;
    }

    if((u32)b > max)
    {
        max = b;
        whatmax = 2;
    }

    u32 min = r;               // find minimum value
    if((u32)g < min) min = g;
    if((u32)b < min) min = b;

    s32 delta = max-min;
    *v = max;                   // calc value
    *s = max ? (510*delta+max)/(2*max) : 0;

    if(*s == 0)
    {
        *h = -1;                // undefined hue
    }
    else
    {
        switch(whatmax)
        {
            case 0:             // red is max component
                if(g >= b)
                    *h = (120*(g-b)+delta)/(2*delta);
                else
                    *h = (120*(g-b+delta)+delta)/(2*delta) + 300;
            break;

            case 1:             // green is max component
                if(b > r)
                    *h = 120 + (120*(b-r)+delta)/(2*delta);
                else
                    *h = 60 + (120*(b-r+delta)+delta)/(2*delta);
            break;

            case 2:             // blue is max component
                if(r > g)
                    *h = 240 + (120*(r-g)+delta)/(2*delta);
                else
                    *h = 180 + (120*(r-g+delta)+delta)/(2*delta);
            break;
        }
    }
}

static void hsv2rgb(s32 h, s32 s, s32 v, fmt_filters::rgb *rgb)
{
    if(h < -1 || (u32)s > 255 || (u32)v > 255 || !rgb)
        return;

    s32 r = v, g = v, b = v;

    if(s == 0 || h == -1)
    {
        // Ignore
    }
    else
    {                    // chromatic case
        if((u32)h >= 360)
            h %= 360;

        u32 f = h%60;
        h /= 60;
        u32 p = (u32)(2*v*(255-s)+255)/510;
        u32 q, t;

        if(h&1)
        {
            q = (u32)(2*v*(15300-s*f)+15300)/30600;

            switch(h)
            {
                case 1: r=(s32)q; g=(s32)v, b=(s32)p; break;
                case 3: r=(s32)p; g=(s32)q, b=(s32)v; break;
                case 5: r=(s32)v; g=(s32)p, b=(s32)q; break;
            }
        }
        else
        {
            t = (u32)(2*v*(15300-(s*(60-f)))+15300)/30600;

            switch(h)
            {
                case 0: r=(s32)v; g=(s32)t, b=(s32)p; break;
                case 2: r=(s32)p; g=(s32)v, b=(s32)t; break;
                case 4: r=(s32)t; g=(s32)p, b=(s32)v; break;
            }
        }
    }

    rgb->r = r;
    rgb->g = g;
    rgb->b = b;
}

static fmt_filters::rgba interpolateColor(const fmt_filters::image &im, double x_offset, double y_offset, const fmt_filters::rgba &background)
{
    double alpha, beta;
    fmt_filters::rgba p, q, r, s;
    s32 x, y;
    fmt_filters::rgba *_rgba = (fmt_filters::rgba *)im.data;

    x = (s32)x_offset;
    y = (s32)y_offset;

    if((x < -1) || (x >= im.w) || (y < -1) || (y >= im.h))
        return background;

    if((x >= 0) && (y >= 0) && (x < (im.w-1)) && (y < (im.h-1)))
    {
        fmt_filters::rgba *t = _rgba + y * im.w;

        p = t[x];
        q = t[x+1];
        r = t[x+im.w];
        s = t[x+im.w+1];
    }
    else
    {
        fmt_filters::rgba *t = _rgba + y * im.w;

        p = background;

        if((x >= 0) && (y >= 0))
        {
            p = t[x];
        }

        q = background;

        if(((x+1) < im.w) && (y >= 0))
        {
            q = t[x+1];
        }

        r = background;

        if((x >= 0) && ((y+1) < im.h))
        {
            t = _rgba + (y+1) * im.w;
            r = t[x+im.w];
        }

        s = background;

        if(((x+1) < im.w) && ((y+1) < im.h))
        {
            t = _rgba + (y+1) * im.w;
            s = t[x+im.w+1];
        }
    }

    x_offset -= floor(x_offset);
    y_offset -= floor(y_offset);
    alpha = 1.0-x_offset;
    beta = 1.0-y_offset;

    fmt_filters::rgba _r;

    _r.r = (u8)(beta*(alpha*p.r+x_offset*q.r)+y_offset*(alpha*r.r+x_offset*s.r));
    _r.g = (u8)(beta*(alpha*p.g+x_offset*q.g)+y_offset*(alpha*r.g+x_offset*s.g));
    _r.b = (u8)(beta*(alpha*p.b+x_offset*q.b)+y_offset*(alpha*r.b+x_offset*s.b));
    _r.a = (u8)(beta*(alpha*p.a+x_offset*q.a)+y_offset*(alpha*r.a+x_offset*s.a));

    return _r;
}

static u32 generateNoise(u32 pixel, fmt_filters::NoiseType noise_type)
{
#define NoiseEpsilon  1.0e-5
#define NoiseMask  0x7fff
#define SigmaUniform  4.0
#define SigmaGaussian  4.0
#define SigmaImpulse  0.10
#define SigmaLaplacian 10.0
#define SigmaMultiplicativeGaussian  0.5
#define SigmaPoisson  0.05
#define TauGaussian  20.0

    double alpha, beta, sigma, value;
    alpha=(double) (rand() & NoiseMask)/NoiseMask;
    if (alpha == 0.0)
        alpha=1.0;
    switch(noise_type){
    case fmt_filters::UniformNoise:
    default:
        {
            value=(double) pixel+SigmaUniform*(alpha-0.5);
            break;
        }
    case fmt_filters::GaussianNoise:
        {
            double tau;

            beta=(double) (rand() & NoiseMask)/NoiseMask;
            sigma=sqrt(-2.0*log(alpha))*cos(2.0*M_PI*beta);
            tau=sqrt(-2.0*log(alpha))*sin(2.0*M_PI*beta);
            value=(double) pixel+
                (sqrt((double) pixel)*SigmaGaussian*sigma)+(TauGaussian*tau);
            break;
        }
    case fmt_filters::MultiplicativeGaussianNoise:
        {
            if (alpha <= NoiseEpsilon)
                sigma=MaxRGB;
            else
                sigma=sqrt(-2.0*log(alpha));
            beta=(rand() & NoiseMask)/NoiseMask;
            value=(double) pixel+
                pixel*SigmaMultiplicativeGaussian*sigma*cos(2.0*M_PI*beta);
            break;
        }
    case fmt_filters::ImpulseNoise:
        {
            if (alpha < (SigmaImpulse/2.0))
                value=0;
            else
                if (alpha >= (1.0-(SigmaImpulse/2.0)))
                    value=MaxRGB;
                else
                    value=pixel;
            break;
        }
    case fmt_filters::LaplacianNoise:
        {
            if (alpha <= 0.5)
            {
                if (alpha <= NoiseEpsilon)
                    value=(double) pixel-MaxRGB;
                else
                    value=(double) pixel+SigmaLaplacian*log(2.0*alpha);
                break;
            }
            beta=1.0-alpha;
            if (beta <= (0.5*NoiseEpsilon))
                value=(double) pixel+MaxRGB;
            else
                value=(double) pixel-SigmaLaplacian*log(2.0*beta);
            break;
        }
    case fmt_filters::PoissonNoise:
        {
            register s32
                i;

            for (i=0; alpha > exp(-SigmaPoisson*pixel); i++)
            {
                beta=(double) (rand() & NoiseMask)/NoiseMask;
                alpha=alpha*beta;
            }
            value=i/SigmaPoisson;
            break;
        }
    }

    if(value < 0.0)
        return 0;
    else if(value > MaxRGB)
        return MaxRGB;
    else
        return ((u32) (value+0.5));
}

static inline u32 intensityValue(s32 r, s32 g, s32 b)
{
    return ((u32)((0.299*r + 0.587*g + 0.1140000000000001*b)));
}

static inline u32 intensityValue(const fmt_filters::rgba &rr)
{
    return ((u32)((0.299*rr.r + 0.587*rr.g + 0.1140000000000001*rr.b)));
}

template<class T>
static inline void scaleDown(T &val, T min, T max)
{
    if(val < min)
        val = min;
    else if(val > max)
        val = max;
}

static void blurScanLine(double *kernel, s32 width, fmt_filters::rgba *src, fmt_filters::rgba *dest, s32 columns)
{
    register double *p;
    fmt_filters::rgba *q;
    register s32 x;
    register long i;
    double red, green, blue, alpha;
    double scale = 0.0;

    if(width > columns)
    {
        for(x = 0;x < columns;++x)
        {
            scale = 0.0;
            red = blue = green = alpha = 0.0;
            p = kernel;
            q = src;

            for(i = 0;i < columns;++i)
            {
                if((i >= (x-width/2)) && (i <= (x+width/2)))
                {
                    red += (*p)*(q->r * 257);
                    green += (*p)*(q->g*257);
                    blue += (*p)*(q->b*257);
                    alpha += (*p)*(q->a*257);
                }

                if(((i+width/2-x) >= 0) && ((i+width/2-x) < width))
                    scale += kernel[i+width/2-x];

                p++;
                q++;
            }

            scale = 1.0/scale;
            red = scale*(red+0.5);
            green = scale*(green+0.5);
            blue = scale*(blue+0.5);
            alpha = scale*(alpha+0.5);

            red = red < 0 ? 0 : red > 65535 ? 65535 : red;
            green = green < 0 ? 0 : green > 65535 ? 65535 : green;
            blue = blue < 0 ? 0 : blue > 65535 ? 65535 : blue;
            alpha = alpha < 0 ? 0 : alpha > 65535 ? 65535 : alpha;

            dest[x] = fmt_filters::rgba((u8)(red/257UL),
                            (u8)(green/257UL),
                            (u8)(blue/257UL),
                            (u8)(alpha/257UL));
        }

        return;
    }

    for(x = 0;x < width/2;++x)
    {
        scale = 0.0;
        red = blue = green = alpha = 0.0;
        p = kernel+width/2-x;
        q = src;

        for(i = width/2-x;i < width;++i)
        {
            red += (*p)*(q->r*257);
            green += (*p)*(q->g*257);
            blue += (*p)*(q->b*257);
            alpha += (*p)*(q->a*257);
            scale += (*p);
            p++;
            q++;
        }

        scale=1.0/scale;

        red = scale*(red+0.5);
        green = scale*(green+0.5);
        blue = scale*(blue+0.5);
        alpha = scale*(alpha+0.5);

        red = red < 0 ? 0 : red > 65535 ? 65535 : red;
        green = green < 0 ? 0 : green > 65535 ? 65535 : green;
        blue = blue < 0 ? 0 : blue > 65535 ? 65535 : blue;
        alpha = alpha < 0 ? 0 : alpha > 65535 ? 65535 : alpha;

        dest[x] = fmt_filters::rgba((u8)(red/257UL),
                        (u8)(green/257UL),
                        (u8)(blue/257UL),
                        (u8)(alpha/257UL));
    }

    for(;x < columns-width/2;++x)
    {
        red = blue = green = alpha = 0.0;
        p = kernel;
        q = src+(x-width/2);

        for(i = 0;i < (long)width;++i)
        {
            red += (*p)*(q->r*257);
            green += (*p)*(q->g*257);
            blue += (*p)*(q->b*257);
            alpha += (*p)*(q->a*257);
            p++;
            q++;
        }

        red = scale*(red+0.5);
        green = scale*(green+0.5);
        blue = scale*(blue+0.5);
        alpha = scale*(alpha+0.5);

        red = red < 0 ? 0 : red > 65535 ? 65535 : red;
        green = green < 0 ? 0 : green > 65535 ? 65535 : green;
        blue = blue < 0 ? 0 : blue > 65535 ? 65535 : blue;
        alpha = alpha < 0 ? 0 : alpha > 65535 ? 65535 : alpha;

        dest[x] = fmt_filters::rgba((u8)(red/257UL),
                        (u8)(green/257UL),
                        (u8)(blue/257UL),
                        (u8)(alpha/257UL));
    }

    for(;x < columns;++x)
    {
        red = blue = green = alpha = 0.0;
        scale=0;
        p = kernel;
        q = src+(x-width/2);

        for(i = 0;i < columns-x+width/2;++i)
        {
            red += (*p)*(q->r*257);
            green += (*p)*(q->g*257);
            blue += (*p)*(q->b*257);
            alpha += (*p)*(q->a*257);
            scale += (*p);
            p++;
            q++;
        }

        scale=1.0/scale;
        red = scale*(red+0.5);
        green = scale*(green+0.5);
        blue = scale*(blue+0.5);
        alpha = scale*(alpha+0.5);

        red = red < 0 ? 0 : red > 65535 ? 65535 : red;
        green = green < 0 ? 0 : green > 65535 ? 65535 : green;
        blue = blue < 0 ? 0 : blue > 65535 ? 65535 : blue;
        alpha = alpha < 0 ? 0 : alpha > 65535 ? 65535 : alpha;

        dest[x] = fmt_filters::rgba((u8)(red/257UL),
                        (u8)(green/257UL),
                        (u8)(blue/257UL),
                        (u8)(alpha/257UL));
    }
}

static s32 getBlurKernel(s32 width, double sigma, double **kernel)
{

#define KernelRank  3
#define KernelRankQ 18.0

    double alpha, normalize;
    register long i;
    s32 bias;

    if(sigma == 0.0)
        return 0;

    if(width == 0)
        width = 3;

    *kernel = new double [width];

    if(!*kernel)
        return 0;

    memset(*kernel, 0, width * sizeof(double));
    bias = KernelRank * width/2;

    for(i = (-bias);i <= bias; i++)
    {
        alpha = exp(-((double) i*i)/(KernelRankQ*sigma*sigma));
        (*kernel)[(i+bias)/KernelRank] += alpha/(MagickSQ2PI*sigma);
    }

    normalize = 0;
 
    for(i = 0;i < width;i++)
        normalize += (*kernel)[i];

    for(i = 0;i < width;i++)
        (*kernel)[i] /= normalize;

    return width;

#undef KernelRankQ
#undef KernelRank

}

static void hull(const s32 x_offset, const s32 y_offset, const s32 polarity, const s32 columns,
                        const s32 rows, u8 *f, u8 *g)
{
    s32 x, y;

    u8 *p, *q, *r, *s;
    u32 v;

    if(f == NULL || g == NULL)
        return;

    p = f+(columns+2);
    q = g+(columns+2);
    r = p+(y_offset*(columns+2)+x_offset);

    for(y = 0;y < rows;y++)
    {
        p++;
        q++;
        r++;
        if(polarity > 0)
            for(x = 0;x < columns;x++)
            {
                v=(*p);
                if (*r > v)
                    v++;
                *q=v > 255 ? 255 : v;
                p++;
                q++;
                r++;
            }
        else
            for(x = 0;x < columns;x++)
            {
                v=(*p);
                if (v > (u32) (*r+1))
                    v--;
                *q=v;
                p++;
                q++;
                r++;
            }
        p++;
        q++;
        r++;
    }

    p = f+(columns+2);
    q = g+(columns+2);
    r = q+(y_offset*(columns+2)+x_offset);
    s = q-(y_offset*(columns+2)+x_offset);

    for(y = 0;y < rows;y++)
    {
        p++;
        q++;
        r++;
        s++;

        if(polarity > 0)
            for(x=0; x < (s32) columns; x++)
            {
                v=(*q);
                if (((u32) (*s+1) > v) && (*r > v))
                    v++;
                *p=v > 255 ? 255 : v;
                p++;
                q++;
                r++;
                s++;
            }
        else
            for (x=0; x < columns; x++)
            {
                v=(*q);
                if (((u32) (*s+1) < v) && (*r < v))
                    v--;
                *p=v;
                p++;
                q++;
                r++;
                s++;
            }

        p++;
        q++;
        r++;
        s++;
    }
}

static int getOptimalKernelWidth(double radius, double sigma)
{
    double normalize, value;
    long width;
    register long u;

    if(sigma == 0.0)
        sigma = 0.01;

    if(radius > 0.0)
        return((int)(2.0*ceil(radius)+1.0));

    const double S = sigma * sigma;

    for(width = 5;;)
    {
        normalize = 0.0;

        for(u = (-width/2);u <= (width/2);u++)
            normalize+=exp(-((double) u*u)/(2.0*S))/(MagickSQ2PI*sigma);

        u = width/2;
        value = exp(-((double) u*u)/(2.0*S))/(MagickSQ2PI*sigma)/normalize;

        if((long)(65535*value) <= 0)
            break;

        width+=2;
    }

    return ((int)width-2);
}

//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 10/12/2001 <--Y2K Compliant! =]
//
// Filename: src-ILU/src/ilu_filter_rcg.c
//
// Description: Scales an image.  Based on the Graphic Gems III source.
//
//-----------------------------------------------------------------------------


/*
 *		Filtered Image Rescaling
 *
 *		  by Dale Schumacher
 *
 */

/*
	Additional changes by Ray Gardener, Daylon Graphics Ltd.
	December 4, 1999

	Summary:

		- Horizontal filter contributions are calculated on the fly,
		  as each column is mapped from src to dst image. This lets 
		  us omit having to allocate a temporary full horizontal stretch 
		  of the src image.

		- If none of the src pixels within a sampling region differ, 
		  then the output pixel is forced to equal (any of) the source pixel.
		  This ensures that filters do not corrupt areas of constant color.

		- Filter weight contribution results, after summing, are 
		  rounded to the nearest pixel color value instead of 
		  being casted to u8 (usually an s32 or char). Otherwise, 
		  artifacting occurs. 

		- All memory allocations checked for failure; zoom() returns 
		  error code. new_image() returns NULL if unable to allocate 
		  pixel storage, even if Image struct can be allocated.
		  Some assertions added.

		- load_image(), save_image() take filenames, not file handles.

		- TGA bitmap format available. If you want to add a filetype, 
		  extend the gImageHandlers array, and factor in your load_image_xxx()
		  and save_image_xxx() functions. Search for string 'add your' 
		  to find appropriate code locations.

		- The 'input' and 'output' command-line arguments do not have 
		  to specify .bm files; any supported filetype is okay.

		- Added implementation of getopt() if compiling under Windows.
*/

static const double IL_PI      = 3.1415926535897932384626;
static const double IL_DEGCONV = 0.0174532925199432957692;

#define	WHITE_PIXEL	(255)
#define	BLACK_PIXEL	(0)

#define CLAMP(v,l,h)    ((v)<(l) ? (l) : (v) > (h) ? (h) : v)

static u32 c;  // Current colour plane offset

/*
 *	filter function definitions
 */

#define	filter_support		(1.0)

static double filter(double t)
{
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if(t < 0.0) t = -t;
	if(t < 1.0) return((2.0 * t - 3.0) * t * t + 1.0);
	return(0.0);
}

#define	box_support		(0.5)

static double box_filter(double t)
{
	if((t > -0.5) && (t <= 0.5)) return(1.0);
	return(0.0);
}

#define	triangle_support	(1.0)

static double triangle_filter(double t)
{
	if(t < 0.0) t = -t;
	if(t < 1.0) return(1.0 - t);
	return(0.0);
}

#define	bell_support		(1.5)

static double bell_filter(double t)		/* box (*) box (*) box */
{
	if(t < 0) t = -t;
	if(t < .5) return(.75 - (t * t));
	if(t < 1.5) {
		t = (t - 1.5);
		return(.5 * (t * t));
	}
	return(0.0);
}

#define	B_spline_support	(2.0)

#define FRAC_2_3 0.6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666667
#define FRAC_1_6 0.1666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666667

static double B_spline_filter(double t)	/* box (*) box (*) box (*) box */
{
	double tt;

	if(t < 0) t = -t;
	if(t < 1) {
		tt = t * t;
		return((.5 * tt * t) - tt + (2.0 / 3.0));
	} else if(t < 2) {
		t = 2 - t;
		return((1.0 / 6.0) * (t * t * t));
	}
	return(0.0);
}

static double sinc(double x)
{
	x *= IL_PI;
	if(x != 0) return(sin(x) / x);
	return(1.0);
}

#define	Lanczos3_support	(3.0)

static double Lanczos3_filter(double t)
{
	if(t < 0) t = -t;
	if(t < 3.0) return(sinc(t) * sinc(t/3.0));
	return(0.0);
}

#define	Mitchell_support	(2.0)

#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

static double Mitchell_filter(double t)
{
	double tt;

	tt = t * t;
	if(t < 0) t = -t;
	if(t < 1.0) {
		t = (((12.0 - 9.0 * B - 6.0 * C) * (t * tt))
		   + ((-18.0 + 12.0 * B + 6.0 * C) * tt)
		   + (6.0 - 2 * B));
		return(t / 6.0);
	} else if(t < 2.0) {
		t = (((-1.0 * B - 6.0 * C) * (t * tt))
		   + ((6.0 * B + 30.0 * C) * tt)
		   + ((-12.0 * B - 48.0 * C) * t)
		   + (8.0 * B + 24 * C));
		return(t / 6.0);
	}
	return(0.0);
}

/*
 *	image rescaling routine
 */

typedef struct {
	s32	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	s32	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
} CLIST;

CLIST	*contrib;		/* array of contribution lists */

/*
	roundcloser()

	Round an FP value to its closest s32 representation.
	General routine; ideally belongs in general math lib file.
*/	
static s32 roundcloser(double d)
{
	/* Untested potential one-liner, but smacks of call overhead */
	/* return fabs(ceil(d)-d) <= 0.5 ? ceil(d) : floor(d); */

	/* Untested potential optimized ceil() usage */
/*	double cd = ceil(d);
	s32 ncd = (s32)cd;
	if(fabs(cd - d) > 0.5)
		ncd--;
	return ncd;
*/

	/* Version that uses no function calls at all. */
	s32 n = (s32) d;
	double diff = d - (double)n;
	if(diff < 0)
		diff = -diff;
	if(diff >= 0.5)
	{
		if(d < 0)
			n--;
		else
			n++;
	}
	return n;
} /* roundcloser */


/* 
	calc_x_contrib()
	
	Calculates the filter weights for a single target column.
	contribX->p must be freed afterwards.

	Returns -1 if error, 0 otherwise.
*/
static s32 calc_x_contrib(CLIST *contribX, double xscale, double fwidth, s32 /*dstwidth*/, s32 srcwidth, double (*filterf)(double), s32 i)
{
	double width;
	double fscale;
	double center, left, right;
	double weight;
	s32 j, k, n;

	if(xscale < 1.0)
	{
		/* Shrinking image */
		width = fwidth / xscale;
		fscale = 1.0 / xscale;

		contribX->n = 0;
		contribX->p = (CONTRIB *)calloc((s32) (width * 2 + 1),
				sizeof(CONTRIB));
		if (contribX->p == NULL) {
			return -1;
		}

		center = (double) i / xscale;
		left = ceil(center - width);
		right = floor(center + width);
		for(j = (s32)left; j <= right; ++j)
		{
			weight = center - (double) j;
			weight = (*filterf)(weight / fscale) / fscale;
			if(j < 0)
				n = -j;
			else if(j >= srcwidth)
				n = (srcwidth - j) + srcwidth - 1;
			else
				n = j;
			
			k = contribX->n++;
			contribX->p[k].pixel = n;
			contribX->p[k].weight = weight;
		}
	
	}
	else
	{
		/* Expanding image */
		contribX->n = 0;
		contribX->p = (CONTRIB*)calloc((s32) (fwidth * 2 + 1),
				sizeof(CONTRIB));
		if (contribX->p == NULL) {
			return -1;
		}
		center = (double) i / xscale;
		left = ceil(center - fwidth);
		right = floor(center + fwidth);

		for(j = (s32)left; j <= right; ++j)
		{
			weight = center - (double) j;
			weight = (*filterf)(weight);
			if(j < 0) {
				n = -j;
			} else if(j >= srcwidth) {
				n = (srcwidth - j) + srcwidth - 1;
			} else {
				n = j;
			}
			k = contribX->n++;
			contribX->p[k].pixel = n;
			contribX->p[k].weight = weight;
		}
	}
	return 0;
} /* calc_x_contrib */


/*
	zoom()

	Resizes bitmaps while resampling them.
	Returns -1 if error, 0 if success.
*/
static s32 zoom(fmt_filters::image *dst, fmt_filters::image *src, double (*filterf)(double), double fwidth)
{
	u8* tmp;
	double xscale, yscale;		/* zoom scale factors */
	s32 xx;
	s32 i, j, k;			/* loop variables */
	s32 n;				/* pixel number */
	double center, left, right;	/* filter calculation variables */
	double width, fscale, weight;	/* filter calculation variables */
	u8 pel, pel2;
	s32 bPelDelta;
	CLIST	*contribY;		/* array of contribution lists */
	CLIST	contribX;
	s32		nRet = -1;

	/* create intermediate column to hold horizontal dst column zoom */
	tmp = (u8 *)malloc(src->h * sizeof(u8));
	if (tmp == NULL) {
		return 0;
	}

	xscale = (double) dst->w / (double) src->w;

	/* Build y weights */
	/* pre-calculate filter contributions for a column */
	contribY = (CLIST*)calloc(dst->h, sizeof(CLIST));
	if (contribY == NULL) {
		free(tmp);
		return -1;
	}

	yscale = (double) dst->h / (double) src->h;

	if(yscale < 1.0)
	{
		width = fwidth / yscale;
		fscale = 1.0 / yscale;
		for(i = 0; i < (s32)dst->h; ++i)
		{
			contribY[i].n = 0;
			contribY[i].p = (CONTRIB*)calloc((s32) (width * 2 + 1),
					sizeof(CONTRIB));
			if(contribY[i].p == NULL) {
				free(tmp);
				free(contribY);
				return -1;
			}
			center = (double) i / yscale;
			left = ceil(center - width);
			right = floor(center + width);
			for(j = (s32)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= (s32)src->h) {
					n = (src->h - j) + src->h - 1;
				} else {
					n = j;
				}
				k = contribY[i].n++;
				contribY[i].p[k].pixel = n;
				contribY[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < (s32)dst->h; ++i) {
			contribY[i].n = 0;
			contribY[i].p = (CONTRIB*)calloc((s32) (fwidth * 2 + 1),
					sizeof(CONTRIB));
			if (contribY[i].p == NULL) {
				free(tmp);
				free(contribY);
				return -1;
			}
			center = (double) i / yscale;
			left = ceil(center - fwidth);
			right = floor(center + fwidth);
			for(j = (s32)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= (s32)src->h) {
					n = (src->h - j) + src->h - 1;
				} else {
					n = j;
				}
				k = contribY[i].n++;
				contribY[i].p[k].pixel = n;
				contribY[i].p[k].weight = weight;
			}
		}
	}


	for(xx = 0; xx < (s32)dst->w; xx++)
	{
		if(0 != calc_x_contrib(&contribX, xscale, fwidth, 
								dst->w, src->w, filterf, xx))
		{
			goto __zoom_cleanup;
		}
		/* Apply horz filter to make dst column in tmp. */
		for(k = 0; k < (s32)src->h; ++k)
		{
			weight = 0.0;
			bPelDelta = false;
			// Denton:  Put get_pixel source here
			//pel = get_pixel(src, contribX.p[0].pixel, k);
			pel = src->data[k * src->w * 4 + contribX.p[0].pixel * 4 + c];
			for(j = 0; j < contribX.n; ++j)
			{
				// Denton:  Put get_pixel source here
				//pel2 = get_pixel(src, contribX.p[j].pixel, k);
				pel2 = src->data[k * src->w * 4 + contribX.p[j].pixel * 4 + c];
				if(pel2 != pel)
					bPelDelta = true;
				weight += pel2 * contribX.p[j].weight;
			}
			weight = bPelDelta ? roundcloser(weight) : pel;

			tmp[k] = (u8)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL);
		} /* next row in temp column */

		free(contribX.p);

		/* The temp column has been built. Now stretch it 
		 vertically into dst column. */
		for(i = 0; i < (s32)dst->h; ++i)
		{
			weight = 0.0;
			bPelDelta = false;
			pel = tmp[contribY[i].p[0].pixel];

			for(j = 0; j < contribY[i].n; ++j)
			{
				pel2 = tmp[contribY[i].p[j].pixel];
				if(pel2 != pel)
					bPelDelta = true;
				weight += pel2 * contribY[i].p[j].weight;
			}
			weight = bPelDelta ? roundcloser(weight) : pel;
			// Denton:  Put set_pixel source here
			//put_pixel(dst, xx, i, (u8)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL));
			dst->data[i * dst->w * 4 + xx * 4 + c] =
				(u8)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL);
		} /* next dst row */
	} /* next dst column */
	nRet = 0; /* success */

__zoom_cleanup:
	free(tmp);

	// Free the memory allocated for vertical filter weights
	for (i = 0; i < (s32)dst->h; ++i)
		free(contribY[i].p);
	free(contribY);

	return nRet;
} /* zoom */

static u32 iluScaleAdvanced(fmt_filters::image *iluCurImage, u32 Width, u32 Height, u32 Filter, u8 **nn)
{
	double (*f)(double) = filter;
	double s = filter_support;
	fmt_filters::image Dest;

	switch(Filter)
	{
		case fmt_filters::ILU_SCALE_BOX: f=box_filter; s=box_support; break;
		case fmt_filters::ILU_SCALE_TRIANGLE: f=triangle_filter; s=triangle_support; break;
		case fmt_filters::ILU_SCALE_BELL: f=bell_filter; s=bell_support; break;
		case fmt_filters::ILU_SCALE_BSPLINE: f=B_spline_filter; s=B_spline_support; break;
		case fmt_filters::ILU_SCALE_LANCZOS3: f=Lanczos3_filter; s=Lanczos3_support; break;
		case fmt_filters::ILU_SCALE_MITCHELL: f=Mitchell_filter; s=Mitchell_support; break;

                default:
		    f = Lanczos3_filter; s=Lanczos3_support;
    	}

        Dest.data = *nn;
        Dest.w = Width;
        Dest.h = Height;

	for (c = 0; c < (u32)4; c++)
        {
		if (zoom(&Dest, iluCurImage, f, s) != 0)
                {
			return 0;
		}
	}

	return 1;
}
