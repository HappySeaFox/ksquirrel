/***************************************************************************
                          sq_config.cpp  -  description
                             -------------------
    begin                : ??? ??? 14 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sq_config.h"

SQ_Config * SQ_Config::m_instance = 0;

SQ_Config::SQ_Config(QObject *parent) : QObject(parent)
{
    m_instance = this;

    kconf = new KConfig("ksquirrelrc");
}

SQ_Config::~SQ_Config()
{
    delete kconf;
}
