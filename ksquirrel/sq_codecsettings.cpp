/***************************************************************************
                          sq_codecsettings.cpp  -  description
                             -------------------
    begin                : Mon Mar 5 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#include <ksquirrel-libs/fmt_codec_base.h>

#include "sq_codecsettings.h"
#include "sq_library.h"
#include "sq_config.h"

enum settings { ThumbnailLoader = 0, ImageViewer, Both, RunTume };

void SQ_CodecSettings::applySettings(SQ_LIBRARY *lib, SQ_CodecSettings::settings fromwhere)
{
    SQ_Config::instance()->setGroup("Main");

    int set = SQ_Config::instance()->readNumEntry("applyto", SQ_CodecSettings::Both);

    // thumbnail loader and image viewer will use default settings
    if((fromwhere == SQ_CodecSettings::ThumbnailLoader && (set == SQ_CodecSettings::ThumbnailLoader || set == SQ_CodecSettings::Both))
                || (fromwhere == SQ_CodecSettings::ImageViewer && (set == SQ_CodecSettings::ImageViewer || set == SQ_CodecSettings::Both)))
        lib->codec->set_settings(lib->settings);
    else
        lib->codec->fill_default_settings();
}
