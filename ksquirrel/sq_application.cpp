/***************************************************************************
                          sq_application.cpp  -  description
                             -------------------
    begin                : Mon Mar 1 2004
    copyright            : (C) 2004 by CKulT
    email                : squirrel-sf@yandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sq_application.h"
#include "ksquirrel.h"

SQ_Application::SQ_Application(bool allowS, bool GUIenabled) : KApplication(allowS, GUIenabled)
{
}

SQ_Application::~SQ_Application()
{
}
