/*  This file is part of ksquirrel-libs (http://ksquirrel.sf.net) libraries

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

#ifndef KSQUIRREL_LIBS_ERROR_CODES_H
#define KSQUIRREL_LIBS_ERROR_CODES_H


///////////////////////////////////
//                               //
//  Error codes for libraries    //
//                               //
///////////////////////////////////


#define		SQE_NOTOK	0
#define		SQE_OK		1

#define		SQE_R_NOFILE		1024
#define		SQE_R_BADFILE		1025
#define		SQE_R_NOMEMORY		1026
#define		SQE_R_NOTSUPPORTED	1027
#define		SQE_R_WRONGDIM		1028

#define		SQE_W_NOFILE		1029
#define		SQE_W_NOMEMORY		SQE_R_NOMEMORY
#define		SQE_W_NOTSUPPORTED	1032
#define		SQE_W_ERROR		1033
#define		SQE_W_WRONGPARAMS	1034
#define		SQE_W_WRONGDIM		SQE_R_WRONGDIM

#endif
