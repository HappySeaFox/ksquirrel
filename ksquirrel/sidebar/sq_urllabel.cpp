#include <kcursor.h>
#include <kglobalsettings.h>

#include "sq_urllabel.h"

SQ_URLLabel::SQ_URLLabel(QWidget *parent, const char *name) : QLabel(parent, name)
{
    setPaletteForegroundColor(KGlobalSettings::textColor());
    setPaletteBackgroundColor(KGlobalSettings::baseColor());
    setCursor(KCursor::handCursor());
}

SQ_URLLabel::~SQ_URLLabel()
{}

void SQ_URLLabel::enterEvent(QEvent *)
{
    QFont f = font();
    f.setUnderline(true);
    setFont(f);
}

void SQ_URLLabel::leaveEvent(QEvent *)
{
    QFont f = font();
    f.setUnderline(false);
    setFont(f);
}

void SQ_URLLabel::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        emit clicked();
}
