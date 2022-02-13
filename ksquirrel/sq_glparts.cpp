/***************************************************************************
                          sq_glparts.cpp  -  description
                             -------------------
    begin                : ??? ??? 13 2007
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

#include "sq_glparts.h"
#include "sq_library.h"

#include <ksquirrel-libs/fmt_codec_base.h>

/* ***************************************************************************************** */

Parts::Parts() : w(0), h(0), realw(0), realh(0), m_parts(0), buffer(0)
{}

Part::Part() : x1(0), y1(0), x2(0), y2(0), tex(0), list(0)
{}

/* ***************************************************************************************** */

memoryPart::memoryPart(const int sz) : m_size(sz), m_data(0)
{}

memoryPart::~memoryPart()
{
    del();
}

void memoryPart::create()
{
    m_data = new RGBA [m_size];

//    if(m_data)
//        memset(m_data, 0, m_size * sizeof(RGBA));
}

// Delete textures and display lists.
void Parts::removeParts()
{
    if(!m_parts.empty())
    {
        int toy = tilesy.size();
        int toxy = tilesx.size() * toy;

        for(int z = 0;z < toxy;z++)
            glDeleteTextures(1, &m_parts[z].tex);

        glDeleteLists(m_parts[0].list, toy);

        m_parts.clear();
    }
}

// Create parts: generate textures and display lists.
bool Parts::makeParts()
{
    int z;
    int toy = tilesy.size();

    GLuint base = glGenLists(toy);

    if(!base)
        return false;

    Part pt;
    int tox = tilesx.size();
    int toxy = tox * toy;

    for(z = 0;z < toxy;z++)
    {
        glGenTextures(1, &pt.tex);
        m_parts.push_back(pt);
    }

    // calculate display list's id
    for(z = 0;z < toy;z++)
        m_parts[z * tox].list = base + z;

    return true;
}

// Calculate coordinates for textures
void Parts::computeCoords()
{
    Part *p;
    int index = 0;
    float X, Y;

    Y = (float)h / 2.0;

    int tlsy = tilesy.size();
    int tlsx = tilesx.size();

    for(int y = 0;y < tlsy;y++)
    {
        X = -(float)w / 2.0;

        for(int x = 0;x < tlsx;x++)
        {
            p = &m_parts[index];

            p->x1 = X;
            p->y1 = Y;
            p->x2 = X + tilesx[x];
            p->y2 = Y - tilesy[y];

            p->tx1 = 0.0;
            p->tx2 = 1.0;
            p->ty1 = 0.0;
            p->ty2 = 1.0;

            index++;
            X += tilesx[x];
        }

        Y -= tilesy[y];
    }
}

/* ***************************************************************************************** */

Tab::Tab()
{
    empty();
}

Tab::~Tab()
{}

void Tab::clearParts()
{
    if(broken) return;

    std::vector<Parts>::iterator itEnd = parts.end();

    for(std::vector<Parts>::iterator it = parts.begin();it != itEnd;++it)
    {
        // delete textures and memory buffers
        (*it).removeParts();
        (*it).deleteBuffer();
    }

    parts.clear();

    finfo.image.clear();
    finfo.meta.clear();
}

void Tab::removeParts()
{
    if(broken) return;

    std::vector<Parts>::iterator itEnd = parts.end();

    for(std::vector<Parts>::iterator it = parts.begin();it != itEnd;++it)
        (*it).removeParts();
}

void Tab::remakeParts()
{
    if(broken) return;

    std::vector<Parts>::iterator itEnd = parts.end();

    for(std::vector<Parts>::iterator it = parts.begin();it != itEnd;++it)
    {
        (*it).makeParts();
        (*it).computeCoords();
    }
}

void Tab::empty()
{
    nullMatrix();

    orient = -1;
    rotate = 0;
    fmt_size = 0;
    lib = 0;
    codeK = 0;
    current = 0;
    curangle = 0;
    total = 0;
    sx = sy = sw = sh = 0;

    glselection = -1;
    srect = QRect();

    manualBlocked = false;
    isflippedV = isflippedH = false;
    broken = false;

    m_original = KURL();
    File = QString::null;
    m_File = QString::null;
    quickImageInfo = QString::null;
    fmt_ext = QString::null;
}

void Tab::nullMatrix()
{
    for(int i = 0;i < 12;i++)
        matrix[i] = (GLfloat)(i % 5 == 0);
}
