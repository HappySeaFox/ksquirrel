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

#include <kstandarddirs.h>

#include <qpainter.h>
#include <qpixmap.h>

#include "sq_progress.h"

#include <limits.h>

SQ_Progress::SQ_Progress(QWidget * parent, const char * name, WFlags f) : QLabel(parent, name, f)
{
	painter = NULL;

	setAutoResize(true);
	setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/progress.png")));
}

SQ_Progress::~SQ_Progress()
{}

void SQ_Progress::setTotalSteps(int totalSteps)
{
	total_steps = totalSteps;
	step = 0;
	percentage = 0;

	QColor c(104,120,247);

	painter = new QPainter(this);
	painter->setBrush(c);
	painter->setPen(c);
}

void SQ_Progress::setIndicator(int progress)
{
	int totalSteps = total_steps;

	if(totalSteps > INT_MAX/1000)
	{
		progress /= 1000;
		totalSteps /= 1000;
	}

	int np = progress * 192 / totalSteps;

	if(np != percentage)
	{
		percentage = np;
		painter->drawRect(4, 3, percentage, 14);
	}

	step++;
}

void SQ_Progress::advance(int step_new)
{
	setIndicator(step + step_new);
}

void SQ_Progress::flush()
{
	update();

	if(painter)
		delete painter;
}
