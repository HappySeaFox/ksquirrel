/***************************************************************************
                          sq_previewwidget.cpp  -  description
                             -------------------
    begin                : ??? Mar 13 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qsize.h>
#include <qimage.h>
#include <qpainter.h>

#include <klocale.h>
#include <kfileitem.h>
#include <kpopupmenu.h>
#include <kcolordialog.h>
#include <kio/global.h>

#include <ksquirrel-libs/fmt_defs.h>

#include "ksquirrel.h"
#include "sq_previewwidget.h"
#include "sq_iconloader.h"
#include "sq_imageloader.h"
#include "sq_libraryhandler.h"
#include "sq_config.h"
#include "sq_downloader.h"
#include "sq_utils.h"

#ifdef SQ_HAVE_KEXIF
#include <libkexif/kexifdata.h>
#include <algorithm>
#include "sq_utils.h"
#endif

SQ_PreviewWidget * SQ_PreviewWidget::m_inst = 0;

SQ_PreviewWidget::SQ_PreviewWidget(QWidget *parent, const char *name)
    : QWidget(parent, name, Qt::WNoAutoErase), all(0), small(0), m_ignore(true)
{
    m_inst = this;

    rereadColor();

    down = new SQ_Downloader(this);
    connect(down, SIGNAL(result(const KURL &)), this, SLOT(slotDownloadResult(const KURL &)));
    connect(down, SIGNAL(percents(int)), this, SLOT(slotDownloadPercents(int)));

    popup = new KPopupMenu;
    popup->insertItem(i18n("Background color..."), this, SLOT(slotBackground()));
    popup->insertItem(i18n("Text color..."), this, SLOT(slotText()));
    popup->insertSeparator();
    popup->insertItem(i18n("Go to first image")+"\tHome", this, SIGNAL(first()));
    popup->insertItem(i18n("Next image")+"\tSpace", this, SIGNAL(next()));
    popup->insertItem(i18n("Previous image")+"\tBackSpace", this, SIGNAL(previous()));
    popup->insertItem(i18n("Go to last image")+"\tEnd", this, SIGNAL(last()));
    popup->insertSeparator();
    popup->insertItem(i18n("Execute")+"\tEnter", this, SIGNAL(execute()));

    multi_pix = SQ_IconLoader::instance()->loadIcon("kmultiple", KIcon::Desktop, KIcon::SizeSmall);

    setMinimumHeight(20);
    setFocusPolicy(QWidget::WheelFocus);
}

SQ_PreviewWidget::~SQ_PreviewWidget()
{
    delete popup;
    delete small;
    delete all;
}

void SQ_PreviewWidget::load(const KURL &_url)
{
    if(SQ_LibraryHandler::instance()->maybeSupported(_url) == SQ_LibraryHandler::No)
        return;

    if(!percentString.isEmpty())
    {
        percentString = QString::null;
        update();
    }

    down->kill();

    if(m_forceignore || m_ignore)
    {
        pending = m_url = _url;
        return;
    }
    else
        pending = KURL();

    m_url = _url;

    if(m_url.isLocalFile())
        slotDownloadResult(m_url);
    else
    {
        KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, m_url);
        down->start(&fi);
    }
}

void SQ_PreviewWidget::fitAndConvert()
{
    if(!m_ignore && fit())
        pixmap.convertFromImage(small?*small:*all);
}

void SQ_PreviewWidget::resizeEvent(QResizeEvent *)
{
    fitAndConvert();
}

void SQ_PreviewWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.fillRect(rect(), color);

    int x = 4;

    if(!percentString.isEmpty())
    {
        QFont fnt = p.font();
        fnt.setBold(true);
        p.setFont(fnt);
        p.setPen(colorText);
        p.drawText(x, 4, width(), height(), Qt::AlignLeft, percentString);
    }

    if(!m_ignore && !pixmap.isNull())
    {
        p.drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);

        if(multi)
        {
            x = x + multi_pix.width() + 4;
            p.drawPixmap(4, 4, multi_pix);
        }

        if(dim)
        {
            QFont fnt = p.font();
            fnt.setBold(true);
            p.setFont(fnt);
            p.setPen(colorText);
            p.drawText(x, 4, width(), height(), Qt::AlignLeft, dimstring);
        }
    }
}

bool SQ_PreviewWidget::fit()
{
    if(!all)
        return false;

    // image is bigger than preview widget -
    // scale it down
    if(width() < 2 || height() < 2)
        return false;

    delete small;
    small = 0;

    if(all->width() > width() || all->height() > height())
    {
        small = new QImage();

        *small = SQ_Utils::scale(*all, width(), height(), SQ_Utils::SMOOTH_FAST, QImage::ScaleMin);
    }

    return true;
}

void SQ_PreviewWidget::saveValues()
{
    SQ_Config::instance()->setGroup("Sidebar");
    SQ_Config::instance()->writeEntry("preview_background", color.name());
    SQ_Config::instance()->writeEntry("preview_text", colorText.name());
}

void SQ_PreviewWidget::rereadColor()
{
    SQ_Config::instance()->setGroup("Sidebar");
    bool b = SQ_Config::instance()->readBoolEntry("preview", true);
    m_forceignore = !b;
    setShown(b);
    color.setNamedColor(SQ_Config::instance()->readEntry("preview_background", "#4e4e4e"));
    dim = SQ_Config::instance()->readBoolEntry("preview_text_enable", true);
    colorText.setNamedColor(SQ_Config::instance()->readEntry("preview_text", "#ffffff"));
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
        saveValues();
        update();
    }
}

void SQ_PreviewWidget::slotText()
{
    KColorDialog dlg(KSquirrel::app(), 0, true);

    dlg.setColor(colorText);

    if(dlg.exec() == QDialog::Accepted)
    {
        colorText = dlg.color();
        saveValues();
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
    if(pending.isValid())
    {
        KURL tmp = pending;
        load(tmp);
    }
}

void SQ_PreviewWidget::slotDownloadResult(const KURL &url)
{
    percentString = QString::null;
    QString path = url.path();
    fmt_info *finfo;
    RGBA *bits;

    // load first page
    bool b = SQ_ImageLoader::instance()->loadImage(path, SQ_CodecSettings::ImageViewer, true, 2);

    finfo = SQ_ImageLoader::instance()->info();
    bits = SQ_ImageLoader::instance()->bits();

    // memory allocation failed in SQ_ImageLoader::loadImage()
    if(!b || !bits || !finfo->image.size())
        return;

    delete small;
    delete all;
    all = small = 0;
    pixmap = QPixmap();

    int w = finfo->image[0].w;
    int h = finfo->image[0].h;
    dimstring = QString::fromLatin1("%1x%2").arg(w).arg(h);

    const int wh = w * h;
    unsigned char t;

    for(int i = 0;i < wh;i++)
    {
        t = (bits+i)->r;
        (bits+i)->r = (bits+i)->b;
        (bits+i)->b = t;
    }

    all = new QImage((uchar *)bits, w, h, 32, 0, 0, QImage::LittleEndian);
    all->setAlphaBuffer(true);

#ifdef SQ_HAVE_KEXIF
    KExifData data;
    data.readFromFile(path);
    int O = data.getImageOrientation();

    if(O != KExifData::UNSPECIFIED && O != KExifData::NORMAL)
    {
        // copy original image
        QImage img = *all;

        // rotate image
        SQ_Utils::exifRotate(QString::null, img, O);

        // transfer back
        *all = img;
    }
    else
#endif
    *all = all->copy();

    multi = finfo->image.size() > 1;

    SQ_ImageLoader::instance()->cleanup();

    fitAndConvert();
    update();
}

void SQ_PreviewWidget::keyPressEvent(QKeyEvent *e)
{
    e->accept();

    int key = e->key();

    if(key == Qt::Key_PageDown || key == Qt::Key_Space)
        emit next();
    else if(key == Qt::Key_PageUp || key == Qt::Key_BackSpace)
        emit previous();
    else if(key == Qt::Key_Return || key == Qt::Key_Enter)
        emit execute();
    else if(key == Qt::Key_Home)
        emit first();
    else if(key == Qt::Key_End)
        emit last();
}

void SQ_PreviewWidget::wheelEvent(QWheelEvent *e)
{
    if(e->delta() < 0)
        emit next();
    else
        emit previous();
}

void SQ_PreviewWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    e->accept();
    emit execute();
}

void SQ_PreviewWidget::slotDownloadPercents(int p)
{
    percentString = i18n("Downloading...") + ' ' + KIO::convertSize(p);
    update();
}

#include "sq_previewwidget.moc"
