/***************************************************************************
                          sq_hloptions.h  -  description
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

#ifndef SQ_HLOPTIONS_H
#define SQ_HLOPTIONS_H

#include <kurl.h>

/*
 *  SQ_HLOptions stores command line options.
 */

struct SQ_HLOptions
{
    SQ_HLOptions();
    ~SQ_HLOptions();

    static SQ_HLOptions* instance() { return m_instance; }

    QString     param;
    KURL        file, dir;

    /*
     *  Just show found libraries and exit ?
     */
    bool         showLibsAndExit;

    bool         have_directorybasket, have_categories, have_imagebasket, have_mountview;

    static SQ_HLOptions *m_instance;
};

#endif
