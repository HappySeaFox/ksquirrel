/***************************************************************************
                          sq_label.h  -  description
                             -------------------
    begin                : June 10 2005
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

#include <qwidget.h>

class SQ_Label : public QWidget
{
	public:
		SQ_Label(QWidget *parent = 0, const char *name = 0);
		~SQ_Label();

		void setText(const QString &lt, const QString &rt);
		void setText(const QString &lt);
		void setSingle(bool s);

	protected:
		void paintEvent(QPaintEvent *);

	private:
		QString ltext, rtext;
		bool single;
};
