#include <qpixmap.h>
#include <qpainter.h>

#include "sq_printpanelbase.h"

SQ_PrintPanelBase::SQ_PrintPanelBase(QWidget *parent, const char *name) : QFrame(parent, name)
{
    setFixedWidth(PANEL_W_F);
    setFixedHeight(PANEL_H_F);

    QPixmap pix(PANEL_W_F, PANEL_H_F, -1, QPixmap::BestOptim);

    pix.fill(colorGroup().background());

    QPainter paint(&pix);

    paint.fillRect(K, K, PANEL_W, PANEL_H, QBrush(darkGray, Qt::Dense1Pattern));
    paint.fillRect(0, 0, PANEL_W, PANEL_H, QBrush(white));

    setPaletteBackgroundPixmap(pix);
}

SQ_PrintPanelBase::~SQ_PrintPanelBase()
{}
