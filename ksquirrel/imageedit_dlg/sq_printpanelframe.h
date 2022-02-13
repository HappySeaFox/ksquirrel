#ifndef SQ_PRINTPANELFRAME_H
#define SQ_PRINTPANELFRAME_H

#include <qframe.h>

class SQ_PrintPanelFrame : public QFrame
{
    public:
        SQ_PrintPanelFrame(QWidget *parent = 0, const char *name = 0);
        ~SQ_PrintPanelFrame();

    protected:
	virtual void mousePressEvent(QMouseEvent *e);

    private:
};

#endif
