/***************************************************************************
                          sq_urllabel.cpp  -  description
                             -------------------
    begin                : Jan 2 2006
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

#ifndef SQ_URLLABEL
#define SQ_URLLABEL

#include <qlabel.h>

/*
 *  Simple replacement for KURLLabel.
 */

class SQ_URLLabel : public QLabel
{
    Q_OBJECT

    public:
        SQ_URLLabel(QWidget *parent = 0, const char *name = 0);
        ~SQ_URLLabel();

    protected:

        /*
         *  Reimplement enterEvent() anf leaveEvent()
         *  to highlight our url.
         */
        void enterEvent(QEvent *);
        void leaveEvent(QEvent *);

        /*
         *  just "emit clicked()"
         */
        void mousePressEvent(QMouseEvent *);

    signals:
        void clicked();
};

#endif
