/***************************************************************************
                          sq_hloptions.cpp  -  description
                             -------------------
    begin                : ??? ??? 30 2004
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

#include "sq_hloptions.h"

SQ_HLOptions * SQ_HLOptions::sing = NULL;

SQ_HLOptions::SQ_HLOptions() : QObject(), showLibsAndExit(false)
{
    sing = this;
}

SQ_HLOptions::~SQ_HLOptions()
{}

SQ_HLOptions* SQ_HLOptions::instance()
{
    return sing;
}