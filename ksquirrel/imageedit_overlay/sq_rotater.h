/***************************************************************************
                          sq_rotater.h  -  description
                             -------------------
    begin                : ??? Apr 29 2005
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

#ifndef SQ_ROTATER_H
#define SQ_ROTATER_H

#include "sq_editbase.h"

class SQ_ImageRotate;
class SQ_ImageOptions;
class SQ_ImageRotateOptions;

/**
  *@author Baryshev Dmitry
  */

class SQ_Rotater : public SQ_EditBase
{
	Q_OBJECT

	public: 
		SQ_Rotater();
		~SQ_Rotater();

		static SQ_Rotater* instance();

	protected:
		virtual void startEditPrivate();
		virtual void dialogReset();
		virtual int manipDecodedImage(fmt_image *im);
		virtual int determineNextScan(const fmt_image &im, RGBA *scan, int y);

	private slots:
		void slotStartRotate(SQ_ImageOptions*, SQ_ImageRotateOptions*);

	private:
		SQ_ImageRotate	*rotate;
		static SQ_Rotater	*sing;
		SQ_ImageRotateOptions rotopt;
};

#endif
