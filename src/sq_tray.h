#ifndef SQ_TRAY_H
#define SQ_TRAY_H

#include <ksystemtray.h>

class SQ_SystemTray : public KSystemTray
{
    private:
	QWidget *parent;
	
    public:
	SQ_SystemTray(QWidget *_parent = 0, const char *name = "");
	
    protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);	
};


#endif
