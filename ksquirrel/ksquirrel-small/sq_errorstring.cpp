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

#include "sq_errorstring.h"

#include "error.h"

SQ_ErrorString * SQ_ErrorString::sing = 0;

SQ_ErrorString::SQ_ErrorString()
{
	sing = this;

	// fill map with translated messages
	messages[SQE_OK] = QString::fromLatin1("OK");
	messages[SQE_R_NOFILE] = i18n("can't open file for reading");
	messages[SQE_R_BADFILE] = i18n("file corrupted");
	messages[SQE_R_NOMEMORY] = i18n("no memory");
	messages[SQE_R_NOTSUPPORTED] = i18n("file type not supported");
	messages[SQE_R_WRONGDIM] = i18n("wrong image dimensions");
	messages[SQE_W_NOFILE] = i18n("can't open file for writing");
	messages[SQE_W_NOTSUPPORTED] = i18n("write feature is not supported");
	messages[SQE_W_ERROR] = i18n("write error (check free space)");
	messages[SQE_W_WRONGPARAMS] = i18n("wrong parameters");
	messages[SQE_NOTFINISHED] = i18n("Editing process is not finished yet");
}

SQ_ErrorString::~SQ_ErrorString()
{}

SQ_ErrorString* SQ_ErrorString::instance()
{
	return sing;
}

// Get string by error code.
QString SQ_ErrorString::string(const int code)
{
	return messages[code];
}

QString SQ_ErrorString::stringSN(const int code)
{
	return messages[code] + "\n";
}
