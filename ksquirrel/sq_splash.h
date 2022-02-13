/***************************************************************************
                          sq_splash.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
    email                : squirrel-sf@yandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QP_SPLASH_H
#define QP_SPLASH_H

#include <qvbox.h>

class QLabel;
class QMouseEvent;
class QString;

class SQ_Splash : public QVBox
{
	Q_OBJECT

	public:
		SQ_Splash(QWidget *parent = 0, const char *name = 0);
		~SQ_Splash();

	protected:
		void mousePressEvent(QMouseEvent *);

	private:
		QLabel *picLabel;
};

#endif
