#include "sq_printpanelframe.h"
#include "sq_printpanel.h"

SQ_PrintPanelFrame::SQ_PrintPanelFrame(QWidget *parent, const char *name) : QFrame(parent, name)
{
    setFixedWidth(48);
    setFixedHeight(48);
    setFrameShape(QFrame::Box);
}

SQ_PrintPanelFrame::~SQ_PrintPanelFrame()
{}

void SQ_PrintPanelFrame::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    SQ_PrintPanel::instance()->toggleCurrentFrameColor(this);
}
