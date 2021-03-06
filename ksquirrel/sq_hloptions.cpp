/***************************************************************************
                          sq_hloptions.cpp  -  description
                             -------------------
    begin                : ??? ??? 30 2004
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

#include "sq_hloptions.h"

SQ_HLOptions * SQ_HLOptions::m_instance = 0;

SQ_HLOptions::SQ_HLOptions() 
    : showLibsAndExit(false),
    have_directorybasket(true),
    have_categories(true),
    have_imagebasket(true),
    have_mountview(true)
{
    m_instance = this;
}

SQ_HLOptions::~SQ_HLOptions()
{}
