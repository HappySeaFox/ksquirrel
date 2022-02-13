/***************************************************************************
                          sq_printer.h  -  description
                             -------------------
    begin                : ??? May 11 2005
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

#ifndef SQ_PRINTER_H
#define SQ_PRINTER_H

#include "sq_editbase.h"

class KPrinter;

class SQ_ImagePrint;

class SQ_Printer : public SQ_EditBase
{
	Q_OBJECT

	public: 
		SQ_Printer();
		~SQ_Printer();

		static SQ_Printer* instance();

	protected:
		virtual void startEditPrivate();
		virtual void setWritingLibrary();
		virtual void dialogReset();
		virtual void dialogAdditionalInit();
		virtual int   manipDecodedImage(SQ_LIBRARY *lw, const QString &name, RGBA *image,
												const fmt_image &im, const fmt_writeoptions &opt);

	private slots:
		void slotStartPrint(SQ_ImageOptions*, SQ_ImagePrintOptions*);

	private:
		SQ_ImagePrint		*print;
		static SQ_Printer		*sing;
		SQ_ImagePrintOptions prnopt;
		KPrinter				*printer;
};

#endif
