/***************************************************************************
                          sq_glwidget_helpers.h  -  description
                             -------------------
    begin                : May 31 2005
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

#ifndef SQ_GLWIDGET_HELPERS
#define SQ_GLWIDGET_HELPERS

#include <ktoolbar.h>
#include <ktoolbarbutton.h>

class SQ_ToolButtonPopup : public KToolBarButton
{
	public:
		SQ_ToolButtonPopup(const QPixmap &pix, const QString &textLabel, QWidget *parent);
		~SQ_ToolButtonPopup();

		static int id;
};

class SQ_ToolButton : public QToolButton
{
	public:
		SQ_ToolButton(const QIconSet &iconSet, const QString &textLabel, QObject *receiver,
						const char *slot, QToolBar *parent, const char *name = 0);

		~SQ_ToolButton();

		static int fixedWidth();
};

class SQ_ToolBar : public KToolBar
{
	public:
		SQ_ToolBar(QWidget *parent, const int members);
		~SQ_ToolBar();

	protected:
		void mouseReleaseEvent(QMouseEvent *);
};

#endif
