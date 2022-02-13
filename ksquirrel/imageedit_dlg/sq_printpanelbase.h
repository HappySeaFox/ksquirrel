#ifndef SQ_PRINTPANELBASE_H
#define SQ_PRINTPANELBASE_H

#include <qframe.h>

#define K 6
#define M 2

#define PANEL_W_F 132
#define PANEL_H_F 152
#define PANEL_W PANEL_W_F-K
#define PANEL_H PANEL_H_F-K

class SQ_PrintPanelBase : public QFrame
{
    public:
        SQ_PrintPanelBase(QWidget *parent = 0, const char *name = 0);
        ~SQ_PrintPanelBase();
};

#endif
