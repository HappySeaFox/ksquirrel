/***************************************************************************
                          sq_slideshowwidget.cpp  -  description
                             -------------------
    begin                : ??? Mar 30 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel@tut.by
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qimage.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qhbox.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <kwin.h>
#include <kcursor.h>
#include <klocale.h>
#include <kio/global.h>

#include <ksquirrel-libs/fmt_defs.h>

#include "sq_slideshowwidget.h"
#include "sq_imageloader.h"
#include "sq_iconloader.h"
#include "sq_config.h"

#include "libpixops/pixops.h"

SQ_SlideShowWidget * SQ_SlideShowWidget::m_inst = 0;

SQ_SlideShowWidget::SQ_SlideShowWidget(QWidget *parent, const char *name)
    : QWidget(parent,name, Qt::WNoAutoErase)
{
    m_inst = this;

    timerHide = new QTimer(this);

    message = new QLabel(this);

    QFont f = message->font();
    f.setPointSize(12);
    f.setBold(true);
    message->setFont(f);
    message->setMargin(4);

    connect(timerHide, SIGNAL(timeout()), this, SLOT(slotHide()));

    options = new QHBox(this, "kde toolbar widget");
    options->setSpacing(0);
    options->setMargin(1);
    options->move(0, 0);
    options->hide();
    message->hide();

    int is = KIcon::SizeMedium;

    QToolButton *b = new QToolButton(options);
    b->setIconSet(SQ_IconLoader::instance()->loadIcon("previous", KIcon::Desktop, is));
    connect(b, SIGNAL(clicked()), this, SIGNAL(previous()));

    b = new QToolButton(options);
    b->setIconSet(SQ_IconLoader::instance()->loadIcon("next", KIcon::Desktop, is));
    connect(b, SIGNAL(clicked()), this, SIGNAL(next()));

    b = new QToolButton(options);
    b->setIconSet(SQ_IconLoader::instance()->loadIcon("player_pause", KIcon::Desktop, is));
    connect(b, SIGNAL(clicked()), this, SIGNAL(pause()));

    b = new QToolButton(options);
    b->setIconSet(SQ_IconLoader::instance()->loadIcon("help", KIcon::Desktop, is));
    connect(b, SIGNAL(clicked()), this, SLOT(slotShowHelp()));

    b = new QToolButton(options);
    b->setIconSet(SQ_IconLoader::instance()->loadIcon("cancel", KIcon::Desktop, is));
    connect(b, SIGNAL(clicked()), this, SIGNAL(stopSlideShow()));

    options->adjustSize();
    message->setFixedHeight(options->height());

    options->installEventFilter(this);

    KCursor::setAutoHideCursor(this, true);
    KCursor::setHideCursorDelay(2500);
}

SQ_SlideShowWidget::~SQ_SlideShowWidget()
{}

void SQ_SlideShowWidget::beginSlideShow(int totl)
{
    total = totl;

    SQ_Config::instance()->setGroup("Slideshow");

    bgcolor.setNamedColor(SQ_Config::instance()->readEntry("background", "#4e4e4e"));

    messages = SQ_Config::instance()->readBoolEntry("messages", true);
    mes_pos = SQ_Config::instance()->readBoolEntry("messages_pos", true);
    mes_name = SQ_Config::instance()->readBoolEntry("messages_name", true);
    mes_size = SQ_Config::instance()->readBoolEntry("messages_size", false);

    show();
    KWin::setState(winId(), NET::FullScreen);

    message->hide();
    options->hide();

    if(messages)
    {
        message->move(0, 0);
        message->show();
    }

    path = "";
    current = 0;

    setFocus();
}

void SQ_SlideShowWidget::endSlideShow()
{
    options->hide();
    message->hide();
    hide();

    KWin::clearState(winId(), NET::FullScreen);

    pixmap = QPixmap();
}

void SQ_SlideShowWidget::loadImage(const QString &_path, int _current)
{
    fmt_info *finfo;
    RGBA *bits, *small;

    // load first page
    bool b = SQ_ImageLoader::instance()->loadImage(_path, false);

    finfo = SQ_ImageLoader::instance()->info();
    bits = SQ_ImageLoader::instance()->bits();

    // memory allocation failed in SQ_ImageLoader::loadImage()
    if(!b || !bits || !finfo->image.size())
        return;

    path = _path;
    current = _current;

    constructMessage();

    pixmap = QPixmap();

    int w = finfo->image[0].w;
    int h = finfo->image[0].h;
    int smallw, smallh;
    bool deleteSmall = false;

    // scale down to fit into window
    if(w > width() || h > height())
    {
        QSize sz(w, h);
        sz.scale(width(), height(), QSize::ScaleMin);

        smallw = sz.width();
        smallh = sz.height();

        small = new RGBA[smallw * smallh];

        if(!small)
        {
            SQ_ImageLoader::instance()->cleanup();
            return;
        }

//        printf("scale %d %d - %d %d\n", w, h, smallw, smallh);

        // fast MMX scaling...
        pixops_scale((unsigned char *)small, 0, 0, smallw, smallh, smallw * 4, 4, true,
                    (unsigned char *)bits, w, h, w * 4, 4, true,
                    (double)smallw / w, (double)smallh / h,
                    PIXOPS_INTERP_BILINEAR);

        deleteSmall = true;
    }
    else
    {
        small = bits;
        smallw = w;
        smallh = h;
    }

    const int wh = smallw * smallh;
    unsigned char t;

    // swap R and B components - QPixmap need it
    for(int i = 0;i < wh;i++)
    {
        t = (small+i)->r;
        (small+i)->r = (small+i)->b;
        (small+i)->b = t;
    }

    QImage image((unsigned char *)small, smallw, smallh, 32, 0, 0, QImage::LittleEndian);

    image.setAlphaBuffer(true);

    pixmap.convertFromImage(image);

    // finally clean all memory buffers
    SQ_ImageLoader::instance()->cleanup();

    if(deleteSmall)
        delete [] small;

    // show loaded image
    update();
}

void SQ_SlideShowWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    p.setClipRect(e->rect());
    p.fillRect(e->rect(), bgcolor);

    if(!pixmap.isNull())
        p.drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);
}

void SQ_SlideShowWidget::closeEvent(QCloseEvent *e)
{
    emit stopSlideShow();

    e->ignore();
}

void SQ_SlideShowWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Escape || e->key() == Qt::Key_Return)
        close();
    else if(e->key() == Qt::Key_Pause || e->key() == Qt::Key_Space)
        emit pause();
    else if(e->key() == Qt::Key_PageUp)
        emit previous();
    else if(e->key() == Qt::Key_PageDown)
        emit next();
    else if(e->key() == Qt::Key_M)
    {
        SQ_Config::instance()->setGroup("Slideshow");

        messages = SQ_Config::instance()->readBoolEntry("messages", true);
        messages = !messages;
        SQ_Config::instance()->writeEntry("messages", messages);

        if(options->isVisible() && messages)
            message->move(options->width()+1, 0);

        constructMessage();

        message->setShown(messages);
    }

    e->accept();
}

void SQ_SlideShowWidget::mouseMoveEvent(QMouseEvent *)
{
    if(!options->isVisible())
    {
        options->show();
        if(messages) message->move(options->width()+1, 0);
    }

    timerHide->start(1500, true);
}

void SQ_SlideShowWidget::slotHide()
{
    options->hide();

    if(messages) message->move(0, 0);
}

bool SQ_SlideShowWidget::eventFilter(QObject *, QEvent *e)
{
    if(e->type() == QEvent::Enter)
    {
        timerHide->stop();
        return true;
    }

    return false;
}

void SQ_SlideShowWidget::slotShowHelp()
{
    QWhatsThis::display(i18n(
    "<table cellspacing=0>"
    "<tr><td><b>Enter, Escape, Return</b></td><td>stop slideshow</td></tr>"
    "<tr><td><b>Pause, Space</b></td><td>pause/unpause slideshow</td></tr>"
    "<tr><td><b>Page Up</b></td><td>previous image</td></tr>"
    "<tr><td><b>Page Down</b></td><td>next image</td></tr>"
    "<tr><td><b>M</b></td><td>show/hide onscreen messages</td></tr>"
    "</table>"
    ));
}

void SQ_SlideShowWidget::constructMessage()
{
    // construct onscreen message
    if(messages)
    {
        QFileInfo fm(path);
        QString s_message;

        if(mes_pos) s_message = QString::fromLatin1("[%1/%2] ").arg(current+1).arg(total);

        if(mes_name) s_message += fm.fileName();

        if(mes_size)
            s_message = (mes_name || mes_pos) ? (s_message + " (" + KIO::convertSize(fm.size()) + ")") : KIO::convertSize(fm.size());

        message->setText(s_message);
        message->adjustSize();
    }
}

#include "sq_slideshowwidget.moc"
