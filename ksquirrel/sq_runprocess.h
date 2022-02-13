/***************************************************************************
                          sq_runprocess.h  -  description
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

#ifndef MENU_RUN_PROCESSES
#define MENU_RUN_PROCESSES

#include <qprocess.h>
#include <qvaluelist.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qobject.h>

#include <stdio.h>

typedef struct
{
    QString name;
    QString program;
    QPixmap	px;
    int 	id;
}PROCESSITEM;


class SQ_RunProcess : public QObject
{
    Q_OBJECT
    
    public:
		SQ_RunProcess(const int = 0);
		~SQ_RunProcess();

		void AddItem(const QString&, const QString&);
	
		int  GetCount()               		const;
		QString GetName(unsigned int)		const;
		QString GetProgName(unsigned int)	const;
		QPixmap GetPixmap(unsigned int)		const;
		int 	GetElementId(const int)		const;

    public slots:
		void slotRunCommand(int menu_id);


    private:
		QValueList<PROCESSITEM> *plist;
		int baseid, curid;
};


#endif