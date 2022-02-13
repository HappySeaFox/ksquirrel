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
    RGB			*pal;     /*  palette  */
    short		pal_entr; /*  how many elements contains *pal (usually 1<<bpp elements)  */
    char		*dump;	  /*  dump: "Width: 1024\nHeight: 768\nBPP: 24\n...."  */

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
