/***************************************************************************
                          sq_externaltool.h  -  description
                             -------------------
    begin                : ??? ??? 12 2004
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

#ifndef SQ_EXTERNALTOOL_H
#define SQ_EXTERNALTOOL_H

#include <qvaluevector.h>
#include <qstring.h>

/**
  *@author CKulT
  */

typedef struct
{
	QString name;
	QString command;
}SQ_EXT_TOOL;

class SQ_ExternalTool : public QValueVector<SQ_EXT_TOOL>
{
	public: 
		SQ_ExternalTool();
		~SQ_ExternalTool();

		void addTool(const QString &name, const QString &command);
		void addTool(const SQ_EXT_TOOL &tool);
};

#endif
