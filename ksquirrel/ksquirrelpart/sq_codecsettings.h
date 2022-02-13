/***************************************************************************
                          sq_codecsettings.h  -  description
                             -------------------
    begin                : Mon Mar 5 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#ifndef SQ_CODECSETTINGS_H
#define SQ_CODECSETTINGS_H

/**
  *@author Baryshev Dmitry
  */

struct SQ_LIBRARY;

struct SQ_CodecSettings
{
    enum settings { ThumbnailLoader = 0, ImageViewer, Both, RunTume, None };

    static void applySettings(SQ_LIBRARY *l, SQ_CodecSettings::settings fromwhere);
};

#endif
