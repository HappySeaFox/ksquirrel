/***************************************************************************
                          sq_config.h  -  description
                             -------------------
    begin                : ??? ??? 14 2004
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

#ifndef SQ_CONFIG_H
#define SQ_CONFIG_H

#include <kconfig.h>
#include <qstring.h>

/*
 *  Class for reading/writing config file
 */

class SQ_Config : public KConfig
{
    public: 
        SQ_Config(const QString& fileName = QString::null);
        ~SQ_Config();

        static SQ_Config* instance() { return m_instance; }

    private:
        static SQ_Config *m_instance;
};

#endif
