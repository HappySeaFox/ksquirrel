/***************************************************************************
                          sq_glview.h  -  description
                             -------------------
    begin                : ??? ??? 5 2004
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

#ifndef SQ_GLVIEW_H
#define SQ_GLVIEW_H

#include <qvbox.h>
#include <qmap.h>

class SQ_GLWidget;

#ifdef SQ_SMALL

class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_Config;

#endif

class KStatusBar;

class QLabel;

/*
 *  SQ_GLView represents a widget containing SQ_GLWidget and statusbar.
 *
 *  If SQ_SMALL defined, it becomes the meain class in 'small' version
 *  of KSquirrel.
 */

class SQ_GLView : public QVBox
{
	Q_OBJECT

	public: 
		SQ_GLView(QWidget *parent = 0, const char *name = 0);
		~SQ_GLView();

#ifndef SQ_SMALL

		/*
		 *  Is this widget separate ?
		 *
		 *  Note: it can be separate, or built-in.
		 */
		bool isSeparate() const;

		/*
		 *  Make widget built-in with reparenting it.
		 */
		void reparent(QWidget *parent, const QPoint &p, bool showIt = false);

#endif

		/*
		 *  Get a pointer to statusbar.
		 */
		KStatusBar* statusbar();

		/*
		 *  Get a pointer to a widget in statusbar by name.
		 */
		QLabel* sbarWidget(const QString &name) const;

		static SQ_GLView* window();

	protected:
		void closeEvent(QCloseEvent *e);

	private:
		/*
		 *  Internal.
		 */
		void createContent();

#ifndef SQ_SMALL

		virtual bool eventFilter(QObject *watched, QEvent *e);

#endif

	private slots:
		/*
		 *  All libraries now loaded.
		 */
		void slotContinueLoading();

		/*
		 *  Goto fullscreen. If current version is NOT
		 *  'small', fullscreen state will be managed by KSquirrel.
		 */
		void slotFullScreen(bool full);

#ifdef SQ_SMALL

	private:
		SQ_LibraryListener *libl;
		SQ_LibraryHandler  *libh;
		SQ_Config			*kconf;

#endif

	private:

#ifndef SQ_SMALL

		bool separate;

#endif
		SQ_GLWidget *gl;
		KStatusBar *sbar;
		QMap<QString, QLabel*> names;

		static SQ_GLView *sing;
};

#endif
