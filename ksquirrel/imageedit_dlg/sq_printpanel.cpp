/***************************************************************************
                          sq_printpanel.cpp  -  description
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

#include "sq_printpanel.h"
#include "sq_printpanelframe.h"

SQ_PrintPanel * SQ_PrintPanel::sing = 0;

SQ_PrintPanel::SQ_PrintPanel(QWidget *parent, const char *name) : SQ_PrintPanelBase(parent, name)
{
    sing = this;

    frame1 = new SQ_PrintPanelFrame(this, "lefttop");
    frame2 = new SQ_PrintPanelFrame(this, "righttop");
    frame3 = new SQ_PrintPanelFrame(this, "leftbottom");
    frame4 = new SQ_PrintPanelFrame(this, "rightbottom");
    frame5 = new SQ_PrintPanelFrame(this, "center");

    frame1->move(SQ_M,SQ_M);
    frame2->move(PANEL_W-frame2->width()-SQ_M,SQ_M);
    frame3->move(SQ_M,PANEL_H-frame2->height()-SQ_M);
    frame4->move(PANEL_W-frame2->width()-SQ_M,PANEL_H-frame2->height()-SQ_M);
    frame5->move((PANEL_W-frame5->width()) / 2, (PANEL_H - frame5->height()) / 2);

    cur = 0;
}

SQ_PrintPanel::~SQ_PrintPanel()
{}

SQ_PrintPanel* SQ_PrintPanel::instance()
{
    return sing;
}

void SQ_PrintPanel::toggleCurrentFrameColor(SQ_PrintPanelFrame *newf)
{
    if(cur)
        cur->setPaletteBackgroundColor(colorGroup().base());

    cur = newf;

    cur->setPaletteBackgroundColor(colorGroup().highlight());
}

QString SQ_PrintPanel::currentFrame() const
{
    return (cur ? QString(cur->name()) : QString::null);
}
