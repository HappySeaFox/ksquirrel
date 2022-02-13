/***************************************************************************
                          sq_filter.h  -  description
                             -------------------
    begin                : ??? Jul 25 2005
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

#ifndef SQ_FILTER_H
#define SQ_FILTER_H

#include "sq_editbase.h"

class SQ_ImageFilter;

class SQ_Filter : public SQ_EditBase
{
	Q_OBJECT

	public: 
		SQ_Filter();
		~SQ_Filter();

		static SQ_Filter* instance();

	protected:
		virtual void startEditPrivate();
		virtual void dialogReset();
		virtual int manipDecodedImage(fmt_image *im);
		virtual void setPreviewImage(const QImage &im);

	private slots:
		void slotStartFilter(SQ_ImageOptions*, SQ_ImageFilterOptions*);

	private:
		SQ_ImageFilter		*filter;
		static SQ_Filter		*sing;
		SQ_ImageFilterOptions filtopt;
};

#endif
