#include <qapplication.h>
#include <qpixmap.h>
#include <qfontmetrics.h>
#include <qpainter.h>

#include "sq_splash.h"


SQ_Splash::SQ_Splash(QWidget *parent, const char *name)
    :QVBox(parent, name, WType_Modal | WStyle_Customize | WStyle_NoBorder | WDestructiveClose) 
{
    setMargin(0);
    setSpacing(0);

    QLabel *picLabel = new QLabel(this);
    QPixmap pixmap = QPixmap::fromMimeSource("images/splash.png");

    picLabel->setPixmap(pixmap);

    m_infoBox = new QLabel(this);
    m_infoBox->setMargin(2);
    m_infoBox->setPaletteBackgroundColor(black);
    m_infoBox->setPaletteForegroundColor(white);
}


SQ_Splash::~SQ_Splash() 
{}


void SQ_Splash::mousePressEvent(QMouseEvent *) 
{
  close();
}


void SQ_Splash::addInfo(const QString &s) 
{
    m_infoBox->setText(s);
    qApp->processEvents();
}
