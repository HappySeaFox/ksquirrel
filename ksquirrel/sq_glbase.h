/***************************************************************************
                          sq_glbase.h  -  description
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

#ifndef SQ_GLBASE_H
#define SQ_GLBASE_H

#include <qwidget.h>
#include <kurl.h>

class SQ_GLViewWidget;

/**
  *@author CKulT
  */

class SQ_GLBase : public QWidget
{
	Q_OBJECT

	public: 
		SQ_GLBase(QWidget *parent = 0, const char *name = 0);
		~SQ_GLBase();

		SQ_GLViewWidget* getGL() const { return gl; }

		virtual bool isSeparate() const { return separate; }

	protected:
		virtual void closeEvent(QCloseEvent *e);
		virtual void create() = 0;

	protected:
		SQ_GLViewWidget	*gl;
		bool separate;
};

#endif
