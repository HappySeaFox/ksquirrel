#include "ksquirrel.h"
#include "sq_tray.h"

SQ_SystemTray::SQ_SystemTray(QWidget *_parent, const char *name) : KSystemTray(_parent,name), parent(_parent)
{}	

void SQ_SystemTray::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
	      sqApp->show();
    }
    
    KSystemTray::mousePressEvent(ev);
}

void SQ_SystemTray::mouseReleaseEvent(QMouseEvent *ev)
{
    KSystemTray::mouseReleaseEvent(ev);
}
