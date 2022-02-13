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

    frame1->move(M,M);
    frame2->move(PANEL_W-frame2->width()-M,M);
    frame3->move(M,PANEL_H-frame2->height()-M);
    frame4->move(PANEL_W-frame2->width()-M,PANEL_H-frame2->height()-M);
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
