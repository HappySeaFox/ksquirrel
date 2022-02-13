/***************************************************************************
                          sq_converter.h  -  description
                             -------------------
    begin                : ??? Feb 24 2005
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

#ifndef SQ_CONVERTER_H
#define SQ_CONVERTER_H

#include "sq_editbase.h"

class SQ_ImageConvert;
class SQ_ImageOptions;
class SQ_ImageConvertOptions;

/**
  *@author Baryshev Dmitry
  */

class SQ_Converter : public SQ_EditBase
{
    Q_OBJECT

    public: 
        SQ_Converter();
        ~SQ_Converter();

        static SQ_Converter* instance();

    protected:
        virtual void startEditPrivate();
        virtual void setWritingLibrary();
        virtual void dialogReset();
        virtual void initWriteOptions();
        virtual int manipDecodedImage(fmt_image *im);

    private slots:
        void slotStartConvert(SQ_ImageOptions*, SQ_ImageConvertOptions*);

    private:
        SQ_ImageConvert    *convert;
        static SQ_Converter    *sing;
        SQ_ImageConvertOptions convopt;
};

#endif
