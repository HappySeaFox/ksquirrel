/***************************************************************************
                          sq_progressbox.h  -  description
                             -------------------
    begin                : ??? Jan 21 2007
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

#ifndef SQ_PROGRESSBOX_H
#define SQ_PROGRESSBOX_H

#include <qhbox.h>

#include "sq_progress.h"

class QPushButton;

class SQ_ProgressBox : public QHBox
{
    public:
        SQ_ProgressBox(QWidget * parent = 0, const char * name = 0);
        ~SQ_ProgressBox();

        SQ_Progress* progressBar() const;

        QPushButton* button() const;

        void toggleButtonPixmap();

        // redirect to SQ_Progress
        void flush();
        void advance(int step = 1);
        void setTotalSteps(int steps);
        void addSteps(int steps);

    private:
        SQ_Progress *p;
        QPushButton  *buttonStop;
        bool stopped;
};

inline
QPushButton* SQ_ProgressBox::button() const
{
    return buttonStop;
}

inline
SQ_Progress* SQ_ProgressBox::progressBar() const
{
    return p;
}

inline
void SQ_ProgressBox::flush()
{
    p->flush();
}

inline
void SQ_ProgressBox::advance(int step)
{
    p->advance(step);
}

inline
void SQ_ProgressBox::setTotalSteps(int steps)
{
    p->setTotalSteps(steps);
}

inline
void SQ_ProgressBox::addSteps(int steps)
{
    p->addSteps(steps);
}

#endif
