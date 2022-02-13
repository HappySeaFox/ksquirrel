/***************************************************************************
                          sq_glhelpwidget.h  -  description
                             -------------------
    begin                : ??? ??? 14 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@uandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_GLHELPWIDGET_H
#define SQ_GLHELPWIDGET_H

#include <qlabel.h>

/**
  *@author CKulT
  */

class SQ_GLHelpWidget : public QLabel
{
	public: 
		SQ_GLHelpWidget(const QString & text, QWidget * parent, const char * name = 0);
		~SQ_GLHelpWidget();

	protected:
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void focusOutEvent(QFocusEvent *e);
};

#endif
