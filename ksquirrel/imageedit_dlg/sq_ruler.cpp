/***************************************************************************
                          sq_ruler.cpp  -  description
                             -------------------
    begin                : Mon Oct 17 2005
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

#include "sq_ruler.h"

SQ_Ruler::SQ_Ruler(QWidget *parent, const char *name) : KRuler(Qt::Vertical, parent, name)
{}

SQ_Ruler::~SQ_Ruler()
{}
