/***************************************************************************
                          sq_previewwidget.cpp  -  description
                             -------------------
    begin                : ??? Mar 13 2007
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

#include <qsize.h>
#include <qimage.h>
#include <qpainter.h>

#include <klocale.h>
#include <kpopupmenu.h>
#include <kcolordialog.h>

#include <ksquirrel-libs/fmt_defs.h>

#include "ksquirrel.h"
#include "sq_previewwidget.h"
#include "sq_imageloader.h"
#include "sq_config.h"
#include "libpixops/pixops.h"

SQ_PreviewWidget * SQ_PreviewWidget::m_inst = 0;

SQ_PreviewWidget::SQ_PreviewWidget(QWidget *parent, const char *name)
    : QWidget(parent, name, Qt::WNoAutoErase), all(0), small(0), m_ignore(true)
{
    m_inst = this;

    rereadColor();

    popup = new KPopupMenu;
    popup->insertItem(i18n("Background color..."), this, SLOT(slotBackground()));
}

SQ_PreviewWidget::~SQ_PreviewWidget()
{
    delete popup;
    delete [] small;
    delete [] all;
}

void SQ_PreviewWidget::load(const KURL &url)
{
    if(m_forceignore || m_ignore)
    {
//        m_ignore = true;
        pending = url;
//        printf("Remember %s\n", url.path().ascii());
        return;
    }
    else
    {
//        m_ignore = false;
        pending = KURL();
//        printf("Load remembered %s\n", url.path().ascii());
    }

    QString path = url.path();
    fmt_info *finfo;
    RGBA *bits;

    // load first page
    bool b = SQ_ImageLoader::instance()->loadImage(path, false);

    finfo = SQ_ImageLoader::instance()->info();
    bits = SQ_ImageLoader::instance()->bits();

    // memory allocation failed in SQ_ImageLoader::loadImage()
    if(!b || !bits || !finfo->image.size())
        return;

    delete [] small;
    delete [] all;
    all = small = 0;
    pixmap = QPixmap();

    w = finfo->image[0].w;
    h = finfo->image[0].h;

    all = bits;

    const int wh = w * h;
    unsigned char t;

    for(int i = 0;i < wh;i++)
    {
        t = (all+i)->r;
        (all+i)->r = (all+i)->b;
        (all+i)->b = t;
    }

    SQ_ImageLoader::instance()->cleanup(false);

    resizeEvent(0);
}

void SQ_PreviewWidget::resizeEvent(QResizeEvent *)
{
//    printf("resize %d %d\n", e, m_ignore);
    if(!m_ignore && fit())
    {
        QImage image;

        if(small)
            image = QImage((unsigned char *)small, smallw, smallh, 32, 0, 0, QImage::LittleEndian);
        else
            image = QImage((unsigned char *)all, w, h, 32, 0, 0, QImage::LittleEndian);

        image.setAlphaBuffer(true);

        if(pixmap.convertFromImage(image))
            update();
    }
}

void SQ_PreviewWidget::paintEvent(QPaintEvent *)
{
//    printf("paint\n");
    QPainter p(this);
    p.setBrush(color);
    p.setPen(colorGroup().highlight());
    p.drawRect(rect());

    if(!m_ignore && !pixmap.isNull())
    {
        p.drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);
    }
}

bool SQ_PreviewWidget::fit()
{
//    printf("FIT %d\n", all);
    if(!all)
        return false;

    // image is bigger that preview widget -
    // scale it down
//    printf("sz %d %d - %d %d\n", width(), height(), sizeHint().width(), sizeHint().height());

    if(width() < 2 || height() < 2)
        return false;

    if(w > width() || h > height())
    {
        QSize sz(w, h);
        sz.scale(width(), height(), QSize::ScaleMin);

        smallw = sz.width();
        smallh = sz.height();

        delete [] small;
        small = new RGBA[smallw * smallh];

        if(!small)
            return false;

//        printf("scale %d %d - %d %d\n", w, h, smallw, smallh);

        pixops_scale((unsigned char *)small, 0, 0, smallw, smallh, smallw * 4, 4, true,
                    (unsigned char *)all, w, h, w * 4, 4, true,
                    (double)smallw / w, (double)smallh / h,
                    PIXOPS_INTERP_BILINEAR);
    }
    else // image is less than preview widget
    {
        small = 0;
        smallw = smallh = 0;
    }

    return true;
}

void SQ_PreviewWidget::saveValues()
{
    SQ_Config::instance()->setGroup("Main");
    SQ_Config::instance()->writeEntry("preview_background", color.name());
}

void SQ_PreviewWidget::rereadColor()
{
    SQ_Config::instance()->setGroup("Sidebar");
    bool b = SQ_Config::instance()->readBoolEntry("preview", true);
    m_forceignore = !b;
    setShown(b);
    color.setNamedColor(SQ_Config::instance()->readEntry("preview_background", "#4e4e4e"));
    m_delay = SQ_Config::instance()->readNumEntry("preview_delay", 400);
    m_cancel = SQ_Config::instance()->readBoolEntry("preview_dont", true);

    if(m_delay < 50 || m_delay > 2000)
        m_delay = 400;
}

void SQ_PreviewWidget::slotBackground()
{
    KColorDialog dlg(KSquirrel::app(), 0, true);

    dlg.setColor(color);

    if(dlg.exec() == QDialog::Accepted)
    {
        color = dlg.color();
        update();
    }
}

void SQ_PreviewWidget::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    if(e->button() == Qt::RightButton)
        popup->exec(e->globalPos());
}

void SQ_PreviewWidget::loadPending()
{
//    printf("show\n");

    if(pending.isValid())
    {
        KURL tmp = pending;
//        printf("Show %s\n", pending.path().ascii());
        load(tmp);
    }
}

#include "sq_previewwidget.moc"
