/***************************************************************************
                          sq_externaltool.cpp  -  description
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

#include "sq_externaltool.h"

SQ_ExternalTool::SQ_ExternalTool() : QValueVector<SQ_EXT_TOOL>()
{}

SQ_ExternalTool::~SQ_ExternalTool()
{}

void SQ_ExternalTool::addTool(const QString &name, const QString &command)
{
	 SQ_EXT_TOOL tool = {name, command};

	 append(tool);
}

void SQ_ExternalTool::addTool(const SQ_EXT_TOOL &tool)
{
	append(tool);
}
