#ifndef __SQUIRREL_LIBS_DEFS__
#define __SQUIRREL_LIBS_DEFS__

#include <stdio.h>


#define ATTR_ __attribute__ ((packed))


#define TRUE  (1==1)
#define FALSE (1==0)
#define BOOL  char


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
    long w;
    long h;
    long bpp;
    long imagesize;

    RGBA *A;
    RGB  *pal;
    
    /*  @todo for all formats: write 'dump'  */
    char **dump;
    
}ATTR_ PICTURE;

typedef struct
{
    unsigned long	w;        /*  width  */
    unsigned long	h;        /*  height */
    unsigned char	bpp;
    BOOL		hasalpha; /* has alpha channel ?  */
    RGB			*pal;     /*  palette  */
    unsigned char	pal_entr; /*  how many elements contains *pal (usually 1<<bpp elements)  */

/*  use F1 and F2 as you want. for exmaple in libSQ_serve_bmp.so F1==width of scanline  */
    unsigned long	F1;
    unsigned long	F2;

    FILE		*fptr;

    char		*dump;  /*  dump: "Width: 1024\nHeight: 768\nBPP: 24\n...."  */

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
