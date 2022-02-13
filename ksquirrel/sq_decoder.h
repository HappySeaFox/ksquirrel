/***************************************************************************
                          sq_decoder.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
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

#ifndef SQ_DECODER_H
#define SQ_DECODER_H

#include <qobject.h>

/**
@author ckult
*/

#include "defs.h"

class SQ_Decoder : public QObject
{
	Q_OBJECT

	private:
		PICTURE			*pic;

	public:
		SQ_Decoder(QObject *parent = 0, const char *name = "");
		~SQ_Decoder();

		RGBA*	decode(const QString &file);
		const int	width();
		const int	height();
		const int	bpp();
};

#endif
