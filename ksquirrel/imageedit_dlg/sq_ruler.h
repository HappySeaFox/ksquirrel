/***************************************************************************
                          sq_ruler.h  -  description
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

/*
 *  Simple wrapper for KRuler. Why the hell KRuler in QT Designer cann't
 *  be vertical ??
 */

#ifndef SQ_RULER_H
#define SQ_RULER_H

#include <kruler.h>

class SQ_Ruler : public KRuler
{
    public:
        SQ_Ruler(QWidget *parent = 0, const char *name = 0);
        ~SQ_Ruler();
};

#endif
