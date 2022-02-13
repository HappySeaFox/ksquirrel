/***************************************************************************
                          sq_glviewspec.h  -  description
                             -------------------
    begin                : ??? ??? 5 2004
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

#ifndef SQ_GLVIEW_H
#define SQ_GLVIEW_H

#include "qvbox.h"

class SQ_GLWidget;

class KStatusBar;

/**
  *@author CKulT
  */

class SQ_GLView : public QVBox
{
	public: 
		SQ_GLView(QWidget *parent = 0, const char *name = 0);
		~SQ_GLView();

		SQ_GLWidget *gl;
		bool isSeparate() const;
		KStatusBar* statusbar();
		void reparent(QWidget *parent, const QPoint &p, bool showIt = false);

		static SQ_GLView* window();

	protected:
		void createContent();
		void closeEvent(QCloseEvent *e);
		virtual bool eventFilter(QObject *watched, QEvent *e);

	private:
		bool separate;
		KStatusBar *sbar;

		static SQ_GLView *view;
};

#endif
