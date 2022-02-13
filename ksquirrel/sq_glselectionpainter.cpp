/***************************************************************************
                          sq_glselectionpainter.cpp  -  description
                             -------------------
    begin                : Apr 4 2007
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

#include <qbitmap.h>
#include <qpainter.h>

#include "sq_glselectionpainter.h"

static const int len = 4;

/* ******************************************************************* */

SQ_GLSelection::SQ_GLSelection(QWidget *parent, const char *name, Type typ) : QWidget(parent, name), m_type(typ)
{
    setAutoMask(true);
    setPaletteBackgroundColor(Qt::magenta);
}

SQ_GLSelection::~SQ_GLSelection()
{}

void SQ_GLSelection::updateMask()
{
    QBitmap bm(size());

    bm.fill(Qt::color0);

    QPainter p(&bm);

    p.setPen(Qt::color1);

    if(m_type == Rectangle)
        p.drawRect(bm.rect());
    else
        p.drawEllipse(bm.rect());

    bool crossDrawn = !(width() < len*2 + 2 || height() < len*2 + 2);

    if(crossDrawn)
    {
        QPoint Cntr = rect().center();
        p.drawLine(Cntr.x(), Cntr.y() - len, Cntr.x(), Cntr.y() + len);
        p.drawLine(Cntr.x() - len, Cntr.y(), Cntr.x() + len, Cntr.y());
    }

    setMask(bm);
}

/* ******************************************************************* */

SQ_GLSelectionPainter::SQ_GLSelectionPainter(QWidget *widget) 
    : w(widget), selection(0), m_valid(false)
{}

SQ_GLSelectionPainter::~SQ_GLSelectionPainter()
{}

void SQ_GLSelectionPainter::begin(SQ_GLSelection::Type tp, int x, int y)
{
    // end previous drawing if any
    end();

    selection = new SQ_GLSelection(w);
    selection->setType(tp);
    selection->resize(1, 1);
    selection->move(x, y);
    selection->show();

    // new painter
    xmoveold = x;
    ymoveold = y;

    m_valid = true;
}

void SQ_GLSelectionPainter::move(int x, int y)
{
    int X, Y, Xmin, Ymin;

    X = QMAX(x, xmoveold);
    Y = QMAX(y, ymoveold);
    Xmin = QMIN(x, xmoveold);
    Ymin = QMIN(y, ymoveold);

    selection->move(Xmin, Ymin);
    selection->resize(X - Xmin, Y - Ymin);
}

void SQ_GLSelectionPainter::end()
{
    delete selection;
    selection = 0;

    m_valid = false;
}

void SQ_GLSelectionPainter::setVisible(bool vis)
{
    if(m_valid)
        selection->setShown(vis);
}
