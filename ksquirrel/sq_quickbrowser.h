/***************************************************************************
                          sq_quickbrowser.h  -  description
                             -------------------
    begin                : ??? ??? 4 2004
    copyright            : (C) 2004 by CKulT
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

#ifndef SQ_QUICKBROWSER_H
#define SQ_QUICKBROWSER_H

#include <qvbox.h>

class SQ_DirOperator;

/**
  *@author CKulT
  */

class SQ_QuickBrowser : public QVBox
{
	Q_OBJECT

	public: 
		SQ_QuickBrowser(QWidget *parent = 0, const char *name = 0);
		~SQ_QuickBrowser();

		SQ_DirOperator 	*quick;

	public slots:
		void slotClose();

	protected:
		void closeEvent(QCloseEvent *);
};

#endif
