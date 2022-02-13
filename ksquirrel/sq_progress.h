/***************************************************************************
                          sq_progress.h  -  description
                             -------------------
    begin                : ??? ??? 29 2004
    copyright            : (C) 2004 by Baryshev Dmitry
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

#ifndef SQ_PROGRESS_H
#define SQ_PROGRESS_H

#include <qlabel.h>
#include <qcolor.h>

class QPainter;

/*
 *  SQ_Progress is a simple replacement for QProgressBar
 */

class SQ_Progress : public QLabel
{
    public:
        SQ_Progress(QWidget *parent = 0, const char *name = 0);
        ~SQ_Progress();

        void setTotalSteps(int totalSteps);
        void addSteps(int steps);

        void setColor(const QColor &c); // alternative progress color

        /*
         *  Add 'step_new' to current step.
         */
        void advance(int step_new = 1);

        /*
         *  Clear drawing.
         */
        void flush();

        bool showText() const;

        void setShowText(bool sh);

    protected:
            virtual void paintEvent(QPaintEvent *);

    private:
        /*
         *  Internal. Set current progress to 'progress'.
         */
        void setIndicator(int progress);
        void drawProgress();

    private:
        int percentage, total_steps, step;
        bool m_text;
        QColor color;

        QPainter *painter;
};

inline
bool SQ_Progress::showText() const
{
    return m_text;
}

inline
void SQ_Progress::setShowText(bool sh)
{
    m_text = sh;
}

inline
void SQ_Progress::setColor(const QColor &c)
{
    color = c;
}

inline
void SQ_Progress::addSteps(int steps)
{
    total_steps += steps;
    setIndicator(step);
}

#endif
