/***************************************************************************
                          sq_glparts.h  -  description
                             -------------------
    begin                : ??? Mar 13 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel@tut.by
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_GLPARTS_H
#define SQ_GLPARTS_H

#include <qgl.h>

#include <vector>

#include <ksquirrel-libs/fmt_defs.h>

struct RGBA;

class memoryPart
{
    public:
        memoryPart(const int sz);
        ~memoryPart();

        // create/delete memory buffer
        void create();
        void del();

        bool valid() const;
        RGBA *data();

    private:
        int m_size;
        RGBA *m_data;
};

inline
RGBA* memoryPart::data()
{
    return m_data;
}

inline
void memoryPart::del()
{
    delete [] m_data;
    m_data = 0;
}

inline
bool memoryPart::valid() const
{
    return m_data != 0;
}

/* *************************************************************** */

struct Part
{
    Part();

    float           x1, y1, x2, y2, tx1, tx2, ty1, ty2;
    unsigned int    tex;
    GLuint          list;
};

/* *************************************************************** */

// one image page. All pages are stored in SQ_GLWidget::parts
struct Parts
{
    Parts();

    int w, h, realw, realh;

    std::vector<Part> m_parts;
    std::vector<int> tilesx, tilesy;
    memoryPart *buffer;

    bool makeParts();
    void removeParts();
    void computeCoords();
    void deleteBuffer();
};

inline
void Parts::deleteBuffer()
{
    delete buffer;
    buffer = 0;
}


#endif
