/***************************************************************************
                          sq_quickbrowser.h  -  description
                             -------------------
    begin                : ??? ??? 4 2004
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

#ifndef SQ_QUICKBROWSER_H
#define SQ_QUICKBROWSER_H

#include <qvbox.h>
#include <qsizegrip.h>

#include <ktoolbar.h>

class SQ_DirOperatorBase;

class SQ_Header : public KToolBar
{
	public:
		SQ_Header(QWidget *parent = 0);
		~SQ_Header();

	protected:
		void mousePressEvent(QMouseEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);

	private:
		int oldX, oldY, oldParentX, oldParentY;
		bool inMouse;
		QWidget *p;
};

class SQ_SizeGrip : public QSizeGrip
{
	public:
		SQ_SizeGrip(QWidget *parent = 0, const char *name = 0);
		~SQ_SizeGrip();

	protected:
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseMoveEvent(QMouseEvent *e);

	private:
		QWidget *p;
		QPoint mother;
};

class SQ_QuickBrowser : public QVBox
{
	Q_OBJECT

	public: 
		SQ_QuickBrowser(QWidget *parent = 0, const char *name = 0);
		~SQ_QuickBrowser();

		SQ_DirOperatorBase *quick;

	public slots:
		void slotClose();

	protected:
		void closeEvent(QCloseEvent *);
		void showEvent(QShowEvent *);
};

#endif
