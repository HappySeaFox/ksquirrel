/***************************************************************************
                          sq_iconloader.cpp  -  description
                             -------------------
    begin                : ??? ??? 20 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#include "sq_iconloader.h"

SQ_IconLoader* SQ_IconLoader::sing = NULL;

SQ_IconLoader::SQ_IconLoader() : KIconLoader("ksquirrel")
{
	sing = this;
}

SQ_IconLoader::~SQ_IconLoader()
{}

SQ_IconLoader* SQ_IconLoader::instance()
{
	return sing;
}
