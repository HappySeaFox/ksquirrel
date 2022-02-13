/***************************************************************************
                          sq_squirreloptions.h  -  description
                             -------------------
    begin                : Wed Mar 24 2004
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

#ifndef SQ_SQUIRRELOPTIONS_H
#define SQ_SQUIRRELOPTIONS_H

#include <qobject.h>

/**
  *@author ckult
  */

class KConfig;
template <class T> class QValueVector;

typedef struct
{
	QString	group;
	QString	strvalue;
	int		intvalue;
	bool		boolvalue;
	int		name;
}SquirrelOption;

class SQ_SquirrelOptions : public QObject
{
	public:
		enum BoolOptions
		{
			checkSome=0
		};

		enum StringOptions
		{
			strSome=0
		};

		enum IntOptions
		{
                	intSome=0
		};

		enum Groups { Main = 0, Interface, FileView, GLView, Libraries, Caching };

	private:
		KConfig *config;
		short	num;

		QValueVector<SquirrelOption>	**l;

	public: 
		SQ_SquirrelOptions(QObject * parent = 0, const char *name = 0);
		~SQ_SquirrelOptions();

		void init(KConfig *);
		void reinit(KConfig *);

		QString readStringEntry(const int group, const int value, QString &def);
		bool readBoolEntry(const int group, const int value, bool def);
		int readNumEntry(const int group, const int value, int &def);
};

#endif
