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

/**
  *@author CKulT
  */

class SQ_Progress : public QLabel
{
	public:
		SQ_Progress(QWidget * parent = 0, const char * name = 0, WFlags f = 0);
		~SQ_Progress();

		void setTotalSteps(int totalSteps);
		void setIndicator(int progress);
		void advance(int step_new);
		void flush();

	private:
		int percentage;
		int total_steps;
		int step;

		QPainter *painter;
};

#endif
