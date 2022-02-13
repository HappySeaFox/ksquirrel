/***************************************************************************
                          sq_config.cpp  -  description
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

#include <kdebug.h>

#include "sq_config.h"

SQ_Config * SQ_Config::m_instance = NULL;

SQ_Config::SQ_Config(const QString& fileName) : KConfig(fileName)
{
    m_instance = this;

    kdDebug() << "+SQ_Config" << endl;
}

SQ_Config::~SQ_Config()
{
    kdDebug() << "-SQ_Config" << endl;
}
