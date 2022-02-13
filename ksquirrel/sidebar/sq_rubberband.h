/***************************************************************************
                          sq_rubberband.h  -  description
                             -------------------
    begin                : June 2 2006
    copyright            : (C) 2006 by Baryshev Dmitry
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

#ifndef SQ_RUBBERBAND_H
#define SQ_RUBBERBAND_H

#include <qwidget.h>

class SQ_RubberBand : public QWidget
{
    Q_OBJECT

    public:
        SQ_RubberBand(QWidget *parent = 0, const char *name = 0);
        ~SQ_RubberBand();

    protected:
        void mousePressEvent(QMouseEvent *e);
        void mouseMoveEvent(QMouseEvent *e);
        void mouseReleaseEvent(QMouseEvent *);
        void paintEvent(QPaintEvent *);

    signals:
        void newx(const int x);

    private:
        bool pressed;
        int virt_x, mapped_x;
};

#endif
