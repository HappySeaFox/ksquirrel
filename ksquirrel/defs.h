/*  This file is part of SQuirrel (http://ksquirrel.sf.net) libraries

    Copyright (c) 2004 Dmitry Baryshev <ckult@yandex.ru>

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

#ifndef __SQUIRREL_LIBS_DEFS__
#define __SQUIRREL_LIBS_DEFS__

#include <stdio.h>


#define ATTR_ __attribute__ ((packed))


#if !defined TRUE
    #define TRUE  (1==1)
#endif

#if !defined FALSE
    #define FALSE (1==0)
#endif

#if !defined BOOL
    #define BOOL  char
#endif


typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

}ATTR_ RGBA;


typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;

}ATTR_ RGB;

typedef struct
{
    unsigned long	w;        /*  width  */
    unsigned long	h;        /*  height */
    unsigned char	bpp;
    
    BOOL		hasalpha; /* has alpha channel ?  */
    BOOL		needflip;
    
    ushort		images;   /*  how many images  (normally 1) */
    BOOL		animated;

    RGB			*pal;     /*  palette  */
    short		pal_entr; /*  how many elements contains *pal (usually 1<<bpp elements)  */

				  /* currently is not used  */
    char		*dump;	  /*  dump: "Width: 1024\nHeight: 768\nBPP: 24\n.... + some more interesting information"  */

    FILE		*fptr;

}ATTR_ fmt_info;


typedef unsigned char	byte;
typedef unsigned char	BYTE;
typedef unsigned char	uchar;

typedef unsigned short	word;
typedef unsigned short	WORD;

typedef long		LONG;
typedef unsigned long	ULONG;

typedef unsigned long	dword;
typedef unsigned long	DWORD;


#endif
