/***************************************************************************
                          sq_glu.h  -  description
                             -------------------
    begin                : ??? ??? 29 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
 ***************************************************************************/
/*
 * Mesa 3-D graphics library
 * Version:  3.5
 * Copyright (C) 1995-2001  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef SQ_GLU_H
#define SQ_GLU_H

#include <GL/gl.h>

/*
 *  SQ_GLU is a replacement for libGLU. SQ_GLWidget uses only one method
 *  in original GLU library - gluLookAt(). It would be better to place it
 *  in standalone namespace, and remove -lGLU dependency at all.
 *
 *  Note: Now KSquirrel doesn't use GLU, but Qt's QGLWidget still
 *  depends on <GL/glu.h>
 */

namespace SQ_GLU
{
        /*
         *  Replacement for gluLookAt().
         */
        void gluLookAt(GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble);
};

#endif
