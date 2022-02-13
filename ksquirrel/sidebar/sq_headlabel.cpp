#include <kcursor.h>
#include <kglobalsettings.h>

#include "sq_headlabel.h"

SQ_HeadLabel::SQ_HeadLabel(QWidget *parent, const char *name) : QLabel(parent, name)
{
    setPaletteBackgroundColor(KGlobalSettings::highlightColor());
    setPaletteForegroundColor(KGlobalSettings::highlightedTextColor());
    setCursor(KCursor::handCursor());

    QFont f = font();
    f.setBold(true);
    setFont(f);

    setMargin(4);
}

SQ_HeadLabel::~SQ_HeadLabel()
{}
