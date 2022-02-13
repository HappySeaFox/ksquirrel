/***************************************************************************
                          sq_errorstring.cpp  -  description
                             -------------------
    begin                : ??? ??? 26 2005
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sq_errorstring.h"

#include <ksquirrel-libs/error.h>

SQ_ErrorString * SQ_ErrorString::m_instance = 0;

SQ_ErrorString::SQ_ErrorString(QObject *parent) : QObject(parent)
{
    m_instance = this;

    // fill map with translated messages
    messages.insert(SQE_OK,             QString::fromLatin1("OK"));
    messages.insert(SQE_R_NOFILE,       i18n("can't open file for reading"));
    messages.insert(SQE_R_BADFILE,      i18n("file corrupted"));
    messages.insert(SQE_R_NOMEMORY,     i18n("no memory"));
    messages.insert(SQE_R_NOTSUPPORTED, i18n("file type not supported"));
    messages.insert(SQE_R_WRONGDIM,     i18n("wrong image dimensions"));
    messages.insert(SQE_W_NOFILE,       i18n("can't open file for writing"));
    messages.insert(SQE_W_NOTSUPPORTED, i18n("write feature is not supported"));
    messages.insert(SQE_W_ERROR,        i18n("write error (check free space)"));
    messages.insert(SQE_W_WRONGPARAMS,  i18n("wrong parameters"));
    messages.insert(SQE_NOTFINISHED,    i18n("Editing process is not finished yet"));
}

SQ_ErrorString::~SQ_ErrorString()
{}

// Get string by error code.
QString SQ_ErrorString::string(const int code)
{
    return messages[code];
}

QString SQ_ErrorString::stringSN(const int code)
{
    return messages[code] + "\n";
}
