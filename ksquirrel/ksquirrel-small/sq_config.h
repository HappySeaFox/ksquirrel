/***************************************************************************
                          sq_config.h  -  description
                             -------------------
    begin                : ??? ??? 14 2004
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

#ifndef SQ_CONFIG_H
#define SQ_CONFIG_H

#include <kconfig.h>
#include <qstring.h>

/*
 *  Class for reading/writing config file
 */

class SQ_Config : public KConfig
{
	public: 
		SQ_Config(const QString& fileName = QString::null);
		~SQ_Config();

/*
 *  All methods are same to KConfig's methods, except additional parameter -
 *  group name.
 */

		QString readEntry(const QString &sgroup, const QString &key, const QString& aDefault = QString::null);

		int readNumEntry(const QString &sgroup, const QString &key, int nDefault = 0);

		double readDoubleNumEntry(const QString &sgroup, const QString &key, double nDefault = 0.0);

		bool readBoolEntry(const QString &sgroup, const QString &pKey, const bool bDefault = false);

		QStringList readListEntry(const QString &sgroup, const QString &key, char sep = ',');

		QValueList<int> readIntListEntry(const QString &sgroup, const QString &key);

		QRect readRectEntry(const QString &sgroup, const QString &key, const QRect *def = 0L);

		QFont readFontEntry(const QString &sgroup, const QString &key, const QFont *pd = 0);

		static SQ_Config* instance();

	private:
		static SQ_Config *sing;
};

#endif
