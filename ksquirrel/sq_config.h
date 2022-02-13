/***************************************************************************
                          sq_config.h  -  description
                             -------------------
    begin                : ??? ??? 14 2004
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

#ifndef SQ_CONFIG_H
#define SQ_CONFIG_H

#include <kconfig.h>
#include <qstring.h>

/**
  *@author CKulT
  */

class SQ_Config : public KConfig
{
	public: 
		SQ_Config(const QString& fileName = QString::null);
		~SQ_Config();

		// Our specific 'read' methods
		QString readEntry(const QString &sgroup, const QString &key, const QString& aDefault = QString::null);
		QString readEntry(const char *sgroup, const QString &key, const QString& aDefault = QString::null);

		int readNumEntry(const QString &sgroup, const QString &key, int nDefault = 0);
		int readNumEntry(const char *sgroup, const QString &key, int nDefault = 0);

		bool readBoolEntry(const QString &sgroup, const QString &pKey, const bool bDefault = false);
		bool readBoolEntry(const char *sgroup, const QString &pKey, const bool bDefault = false);
};

#endif
