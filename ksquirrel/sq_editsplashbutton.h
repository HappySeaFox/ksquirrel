/***************************************************************************
                          sq_editsplashbutton.h  -  description
                             -------------------
    begin                : ??? May 5 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_EDITSPLASHBUTTON_H
#define SQ_EDITSPLASHBUTTON_H

#include <qwidget.h>
#include <qpixmap.h>

/**
  *@author Baryshev Dmitry
  */

class SQ_EditSplashButton : public QWidget
{
	Q_OBJECT

	public: 
		SQ_EditSplashButton(QWidget *parent=0, const char *name=0);
		~SQ_EditSplashButton();

		void setText(const QString &_text);
		void setPixmap(const QPixmap &_pix);

	protected:
		virtual void mouseReleaseEvent(QMouseEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void enterEvent(QEvent *);
		virtual void leaveEvent(QEvent *);
		virtual void paintEvent(QPaintEvent *);

	signals:
		void clicked();

	private:
		bool on, pressed;
		QPixmap pix;
		QString text;
};

inline void SQ_EditSplashButton::setText(const QString &_text)
{
	text = _text;
}

#endif
