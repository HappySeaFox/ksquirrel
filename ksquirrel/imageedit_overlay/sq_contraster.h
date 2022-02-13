/***************************************************************************
                          sq_contraster.h  -  description
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

#ifndef SQ_CONTRASTER_H
#define SQ_CONTRASTER_H

#include "sq_editbase.h"

class SQ_ImageBCG;
class SQ_ImageOptions;
class SQ_ImageBCGOptions;

/**
  *@author Baryshev Dmitry
  */

class SQ_Contraster : public SQ_EditBase
{
	Q_OBJECT

	public: 
		SQ_Contraster();
		~SQ_Contraster();

		static SQ_Contraster* instance();

	protected:
		virtual void startEditPrivate();
		virtual void dialogReset();
		virtual int manipDecodedImage(fmt_image *im);
		virtual void setPreviewImage(const QImage &im);

	private slots:
		void slotStartContrast(SQ_ImageOptions*, SQ_ImageBCGOptions*);

	private:
		SQ_ImageBCG		*bcg;
		static SQ_Contraster	*sing;
		SQ_ImageBCGOptions bcgopt;
};

#endif
