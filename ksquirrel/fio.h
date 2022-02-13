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

#ifndef KSQUIRREL_LIBS_FIO_H
#define KSQUIRREL_LIBS_FIO_H

#include <fstream>

using namespace std;


////////////////////////////////
//                            //
// Binary-oriented file i/o   //
//                            //
////////////////////////////////


// read binary data
class ifstreamK : public ifstream
{
    public:
	ifstreamK();

	bool readK(void *data, int size);
	bool getS(char *, const int);
	bool readCHex(u32 &hex);

	// big-endian-oriented reading
	bool be_getchar(unsigned char *c);
	bool be_getshort(unsigned short *s);
	bool be_getlong(unsigned int *l);
	
	void close();
};

// write binary data
class ofstreamK : public ofstream
{
    public:
	ofstreamK();

	bool writeK(void *data, int size);
};

// ksquirrel-libs won't define this value, so it will be
// successfully compiled. But KSquirrel (or other viewers) should
// define this value to prevent ld error '... already defined ...'.
//
// For example:
// 
// #define SQ_FIO_NO_IMPLEMENT
// #include "fmt_codec_base.h"
//
#ifndef SQ_FIO_NO_IMPLEMENT

// implement read methods
ifstreamK::ifstreamK() : ifstream()
{}

bool ifstreamK::readK(void *data, int size)
{
    read((char *)data, size);

    return good();
}

bool ifstreamK::getS(char *s, const int sz)
{
    getline(s, sz);

    return good();
}

bool ifstreamK::be_getchar(unsigned char *c)
{
    return readK(c, 1);
}

bool ifstreamK::be_getshort(unsigned short *s)
{
    unsigned char buf[2];

    if(!readK(buf, 2))
	return false;

    *s = (buf[0] << 8) + buf[1];
    
    return good();
}

bool ifstreamK::be_getlong(unsigned int *l)
{
    unsigned char buf[4];

    if(!readK(buf,4))
	return false;

    *l = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3]);

    return good();
}

void ifstreamK::close()
{
    ifstream::close();
    
    clear();
}

bool ifstreamK::readCHex(u32 &hex)
{
    s8 prefix1, prefix2;
    s8 h[9], c;
    u32 i = 0;

    if(!readK(&prefix1, sizeof(s8)))
	return false;

    if(!readK(&prefix2, sizeof(s8)))
	return false;

    if(prefix1 != '0' || prefix2 != 'x')
	return false;

    while(true)
    {
	if(!readK(&c, sizeof(s8)))
	    return false;

	if(c < '0' || c > '9')
	{
	    if(c != 'A' && c != 'B' && c != 'C' && c != 'D' && c != 'E' && c != 'F')
	    {
		seekg(-1, ios::cur);
		break;
	    }
	}

	h[i++] = c;
    }

    h[i] = '\0';

    hex = strtol(h, NULL, 16);

    return good();
}


// implement write methods
ofstreamK::ofstreamK() : ofstream()
{}
	
bool ofstreamK::writeK(void *data, int size)
{
    write((char *)data, size);

    return good();
}

#endif

#endif
