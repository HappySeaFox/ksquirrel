/***************************************************************************
                          sq_glselectionpainter.h  -  description
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

#ifndef SQ_GLSELECTIONPAINTER_H
#define SQ_GLSELECTIONPAINTER_H

#include <qrect.h>
#include <qpoint.h>

class SQ_GLWidget;

/*
 *  This is a selection painter for SQ_GLWidget.
 *
 *  Selection can be shown as rectangle and ellipse.
 *
 *  Ellipsis selection mainly used in redeye filter.
 */

class SQ_GLSelectionPainter
{
    public:
        enum Type { Rectangle, Ellipse };

        SQ_GLSelectionPainter(SQ_GLWidget *widget);
        ~SQ_GLSelectionPainter();

        void setSourceSize(int, int);

        QPoint center() const;

        void draw();

        /*
         *  Set selection type - rectangle or ellipse
         */
        int type() const;

        /*
         *  when selection is drawn and visible, it's valid.
         *  After end() it becomes invalid.
         */
        bool valid() const;

        void setVisible(bool vis);

        void begin(Type tp, int x, int y, bool U = true);
        void move(int x, int y);
        void setGeometry(const QRect &rc);
        void end();

        /*
         *  Selected rectangle geometry
         */
        QPoint pos() const;
        QSize size() const;

    private:
        void drawEllipse(float xradius, float yradius);
        void drawRect();
        void hackXY(int &x, int &y);

    private:
        SQ_GLWidget *w;
        int       sourcew, sourceh;
        int       sw, sh, sx, sy;

        int       angle;
        int       xmoveold, ymoveold;
        bool      m_valid, m_shown;
        Type      m_type;
};

inline
QPoint SQ_GLSelectionPainter::pos() const
{
    return valid() ? QPoint(sourcew/2 + sx, sourceh/2 - sy) : QPoint();
}

inline
QSize SQ_GLSelectionPainter::size() const
{
    return valid() ? QSize(sw, sh) : QSize();
}

inline
int SQ_GLSelectionPainter::type() const
{
    return m_type;
}

inline
void SQ_GLSelectionPainter::setGeometry(const QRect &rc)
{
    int X = rc.x(), Y = rc.y();

    hackXY(X, Y);

    sx = X;
    sy = Y;
    sw = rc.width();
    sh = rc.height();
}

inline
void SQ_GLSelectionPainter::setSourceSize(int w, int h)
{
    sourcew = w;
    sourceh = h;
}

inline
void SQ_GLSelectionPainter::setVisible(bool vis)
{
    if(m_valid) m_shown = vis;
}

inline
bool SQ_GLSelectionPainter::valid() const
{
    return m_valid && m_shown;
}

inline
QPoint SQ_GLSelectionPainter::center() const
{
    return QPoint(sx + sw/2, sy - sh/2);
}

inline
void SQ_GLSelectionPainter::hackXY(int &x, int &y)
{
    x -= sourcew / 2;
    y =  sourceh / 2 - y;
}

#endif
