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

#include "fmt_types.h"

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

	// read 'size' bytes of binary data and store it into 'data'.
	// return true if reading was successful, and false otherwise
	bool readK(void *data, int size);
	
	// read string from file
	bool getS(char *, const int);
	
	// read ascii hex value from file
	bool readCHex(u32 &hex);

	// big-endian-oriented reading
	bool be_getchar(u8 *c);
	bool be_getshort(u16 *s);
	bool be_getlong(u32 *l);

	void close();
};

// write binary data
class ofstreamK : public ofstream
{
    public:
	ofstreamK();

	bool writeK(void *data, int size);

	void close();
};

#endif
