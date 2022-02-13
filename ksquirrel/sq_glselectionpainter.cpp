/***************************************************************************
                          sq_glselectionpainter.cpp  -  description
                             -------------------
    begin                : Apr 4 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qbitmap.h>
#include <qpainter.h>

#include <cmath>

#include "sq_glwidget.h"
#include "sq_glselectionpainter.h"

static const int len  = 4;
static const int len2 = 10;
static const int lenc = 2;
static const int lenc2 = lenc / 2;
static const double rad_const = 3.14159265358979323846 / 180.0;

/* ******************************************************************* */

SQ_GLSelectionPainter::SQ_GLSelectionPainter(SQ_GLWidget *widget) 
    : w(widget), sourcew(1), sourceh(1), sw(0), sh(0), sx(0), sy(0), 
      angle(0), m_valid(false), m_shown(false)
{}

SQ_GLSelectionPainter::~SQ_GLSelectionPainter()
{}

void SQ_GLSelectionPainter::begin(Type tp, int x, int y, bool U)
{
    // end previous drawing if any
    end();

    m_type = tp;

    hackXY(x, y);

    sx = xmoveold = x;
    sy = ymoveold = y;
    sw = 0;
    sh = 0;

    m_valid = true;
    m_shown = true;

    if(U) w->updateGLA();
}

void SQ_GLSelectionPainter::move(int x, int y)
{
    hackXY(x, y);

    int X = QMAX(x, xmoveold);
    int Y = QMIN(y, ymoveold);
    int Xmin = QMIN(x, xmoveold);
    int Ymin = QMAX(y, ymoveold);

    sx = Xmin;
    sy = Ymin;
    sw = X - Xmin;
    sh = Ymin - Y;

    angle += 3;

    if(angle > 360)
        angle = 0;

    // SQ_GLWidget::paintGL() will call draw()
    w->updateGLA();
}

void SQ_GLSelectionPainter::end()
{
    m_valid = false;
    m_shown = false;

    w->updateGLA();
}

void SQ_GLSelectionPainter::drawEllipse(float xradius, float yradius)
{
    w->makeCurrent();

    double degInRad;

    glBegin(GL_LINE_LOOP);
    glColor4f(1,0,1,1);

    for(int i = 0; i < 360; i++)
    {
        degInRad = rad_const * i;
        glVertex2f(cos(degInRad) * xradius, sin(degInRad) * yradius);
    }

    glColor4f(1,1,1,1);
    glEnd();
}

void SQ_GLSelectionPainter::drawRect()
{
    w->makeCurrent();

    glBegin(GL_LINE_LOOP);
    glColor4f(1,0,1,1);

    glVertex2f(-sw/2, sh/2);
    glVertex2f(sw/2,  sh/2);
    glVertex2f(sw/2,  -sh/2);
    glVertex2f(-sw/2, -sh/2);

    glColor4f(1,1,1,1);
    glEnd();
}

void SQ_GLSelectionPainter::draw()
{
    if(!sw || !sh)
        return;

    if(m_type == Ellipse)
        drawEllipse(sw/2, sh/2);
    else
        drawRect();

    // center rectangle
    if(sw > lenc && sh > lenc)
    {
        glColor4f(1,0,1,1);
        glRectf(-lenc2, lenc2, lenc2, -lenc2);
        glColor4f(1,1,1,1);
    }
}
