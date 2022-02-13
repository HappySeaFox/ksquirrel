/***************************************************************************
                          sq_fileviewfilter.h  -  description
                             -------------------
    begin                : ??? ??? 6 2004
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

#ifndef SQ_FILEVIEWFILTER_H
#define SQ_FILEVIEWFILTER_H

#include <qvaluevector.h>
#include <qstringlist.h>
#include <qstring.h>

/**
  *@author CKulT
  */
typedef struct
{
	QString name;
	QString filter;

}FILTER;

class SQ_FileviewFilter : public QValueVector<FILTER>
{
	private:
		bool both;

	public: 
		SQ_FileviewFilter();
		~SQ_FileviewFilter();

		void addFilter(const FILTER &filter);
		void addFilter(const QString &name, const QString &ext);

		QString getFilterName(const int i);
		QString getFilterExt(const int i);

		void writeEntries();
		void setShowBoth(bool newboth);
		bool getShowBoth();
};

#endif
