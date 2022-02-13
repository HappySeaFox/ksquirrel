/***************************************************************************
                          sq_resizer.h  -  description
                             -------------------
    begin                : ??? Mar 3 2005
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

#ifndef SQ_RESIZER_H
#define SQ_RESIZER_H

#include "sq_editbase.h"

/**
  *@author Baryshev Dmitry
  */

class SQ_ImageResize;
class SQ_ImageOptions;
class SQ_ImageResizeOptions;

class SQ_Resizer : public SQ_EditBase
{
    Q_OBJECT

    public:
        SQ_Resizer();
        ~SQ_Resizer();

        static SQ_Resizer* instance();

    protected:
        virtual void startEditPrivate();
        virtual void dialogReset();
        virtual int manipDecodedImage(fmt_image *im);

    private slots:
        void slotStartResize(SQ_ImageOptions*, SQ_ImageResizeOptions*);

    private:
        SQ_ImageResize *res;
        static SQ_Resizer *sing;
        SQ_ImageResizeOptions resopt;
};

#endif
