/***************************************************************************
                          sq_printpanel.h  -  description
                             -------------------
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

#ifndef SQ_PRINTPANEL_H
#define SQ_PRINTPANEL_H

#include "sq_printpanelbase.h"

class SQ_PrintPanelFrame;

class SQ_PrintPanel : public SQ_PrintPanelBase
{
    public:
        SQ_PrintPanel(QWidget *parent = 0, const char *name = 0);
        ~SQ_PrintPanel();

        void toggleCurrentFrameColor(SQ_PrintPanelFrame *);
        QString currentFrame() const;

    static SQ_PrintPanel* instance();

    private:
        SQ_PrintPanelFrame *frame1, *frame2, *frame3, *frame4, *frame5, *cur;
        static SQ_PrintPanel *sing;
};

#endif
