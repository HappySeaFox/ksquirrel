/***************************************************************************
                          sq_progress.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>

#include <qpainter.h>
#include <qbrush.h>
#include <qpen.h>

#include <kglobalsettings.h>

#include "sq_progress.h"

SQ_Progress::SQ_Progress(QWidget *parent, const char *name)
    : QLabel(parent, name, Qt::WNoAutoErase), m_text(true), painter(0)
{}

SQ_Progress::~SQ_Progress()
{
    delete painter;
}

void SQ_Progress::setTotalSteps(int totalSteps)
{
    total_steps = totalSteps;
    step = 0;
    percentage = 0;

    if(!color.isValid())
        color = KGlobalSettings::highlightColor();

    painter = new QPainter(this);
    painter->setBrush(color);
    painter->setPen(color);

    QFont f = font();
    f.setBold(true);
    painter->setFont(f);

    painter->setPen(color);
    painter->setBrush(KGlobalSettings::baseColor());
    painter->drawRect(rect());
}

/*
 *  Internal. Set current progress to 'progress'.
 */
void SQ_Progress::setIndicator(int progress)
{
    int totalSteps = total_steps;

    // scale down...
    if(totalSteps > INT_MAX/1000)
    {
        progress /= 1000;
        totalSteps /= 1000;
    }

    int np = progress * 100 / totalSteps;

    // draw progress if needed
    if(np != percentage)
    {
        percentage = np;
        drawProgress();
    }

    step = progress;
}

/*
 *  Add 'step_new' to current step.
 */
void SQ_Progress::advance(int step_new)
{
    setIndicator(step + step_new);
}

/*
 *  Clear drawing.
 */
void SQ_Progress::flush()
{
    delete painter;
    painter = 0;
}

void SQ_Progress::paintEvent(QPaintEvent *)
{
    if(painter)
    {
        drawProgress();
    }
}

void SQ_Progress::drawProgress()
{
        int np = width() * percentage / 100;

        // reset clip rectangle
        painter->setClipRect(0, 0, width(), height());
        painter->setPen(color);
        painter->setBrush(KGlobalSettings::baseColor());
        painter->drawRect(rect());

        // draw rectangle
        painter->fillRect(2, 2, np-4, height()-4, color);

        if(m_text)
        {
            // draw percentage value
            painter->setPen(KGlobalSettings::highlightedTextColor());
            painter->setClipRect(0, 0, np, height());
            painter->drawText(rect(), Qt::AlignCenter | Qt::SingleLine, QString::number(percentage) + "%");
            painter->setPen(KGlobalSettings::textColor());
            painter->setClipRect(np, 0, width()-np, height());
            painter->drawText(rect(), Qt::AlignCenter | Qt::SingleLine, QString::number(percentage) + "%");
        }
}
