/***************************************************************************
                          sq_splitter.h  -  description
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

#ifndef SQ_SPLITTER_H
#define SQ_SPLITTER_H

#include <qhbox.h>

class SQ_RubberBand;

class SQ_Splitter : public QHBox
{
    Q_OBJECT

    public:
        SQ_Splitter(QWidget *parent = 0, const char *name = 0);
        ~SQ_Splitter();

        void addWidget(QWidget *w);

    private slots:
        void slotNewX(const int);

    private:
        QWidget *first, *second;
        SQ_RubberBand *rb;
};

#endif
