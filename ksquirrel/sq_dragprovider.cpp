/***************************************************************************
                          sq_dragprovider.cpp  -  description
                             -------------------
    begin                : ??? Sep 17 2007
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

#include <qwidget.h>
#include <qdragobject.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qfontmetrics.h>
#include <qtooltip.h>
#include <qvaluevector.h>
#include <qsize.h>

#include <kstringhandler.h>
#include <kurldrag.h>
#include <klocale.h>
#include <kurl.h>

#include <algorithm>

#include "sq_dragprovider.h"
#include "sq_iconloader.h"
#include "sq_filethumbviewitem.h"
#include "sq_thumbnailsize.h"

#define SQ_THUMB_SIZE 96

SQ_DragProvider * SQ_DragProvider::m_inst = 0;

SQ_DragProvider::SQ_DragProvider(QObject *parent) : QObject(parent), source(0)
{
    m_inst = this;
}

SQ_DragProvider::~SQ_DragProvider()
{}

void SQ_DragProvider::setParams(QWidget *_source, const KFileItemList &_files, SourceType tp)
{
    source = _source;
    files = _files;
    type = tp;
}

void SQ_DragProvider::start()
{
    if(!source)
        return;

    int count = files.count();

    if(!count)
        return;

    KURL::List urls;
    KFileItem *fi;
    SQ_FileThumbViewItem *kfi;
    QStringList names;
    int capas = type == Thumbnails ? 2 : 10; // 2 thumbnails or 10 file names
    int _capas = capas;
    QValueVector<QPixmap> pixmaps;
    QPixmap *pix;
    int pixw = 0, pixh = 0;
    const int margin = 2;

    KFileItem *first = files.first();

    KFileItemListIterator it(files);

    while((fi = it.current()))
    {
        urls.append(fi->url());

        if(_capas)
        {
            if(type == Thumbnails)
            {
                kfi = reinterpret_cast<SQ_FileThumbViewItem *>(fi->extraData(source));

                if(kfi && (pix = kfi->pixmap()))
                {
                    pixmaps.append(*pix);

                    if(!pixw || !pixh)
                    {
                        pixw = pix->width();
                        pixh = pix->height();

                        if(pixw > SQ_THUMB_SIZE)
                        {
                            QSize sz(pixw, pixh);
                            sz.scale(SQ_THUMB_SIZE, SQ_THUMB_SIZE, QSize::ScaleMin);
                            pixw = sz.width();
                            pixh = sz.height();
                        }
                    }
                }
            }
            else
                names.append(KStringHandler::csqueeze(fi->name()));

            --_capas;
        }

        ++it;
    }

    QDragObject *drag = new KURLDrag(urls, source);
    QPixmap dragIcon;

    if(urls.count() > 1)
        dragIcon = SQ_IconLoader::instance()->loadIcon("kmultiple", KIcon::Desktop, KIcon::SizeSmall);
    else
        dragIcon = first->pixmap(KIcon::SizeSmall);

    const int flags = Qt::AlignAuto|Qt::AlignVCenter|Qt::ShowPrefix;
    QPixmap dragPixmap;
    QString text;

    // construct text
    if(type == Icons)
    {
        QStringList::iterator sEnd = names.end();
        for(QStringList::iterator sit = names.begin();sit != sEnd;++sit)
            text = text + *sit + '\n';
    }

    text += QString::fromLatin1("%1 %2").arg(i18n("Total")).arg(i18n("1 file", "%n files", count));

    QFontMetrics ms(source->font());
    QRect r = ms.boundingRect(0, 0, 1, 1, flags, text);

    // resize pixmap
    if(type == Icons)
        dragPixmap.resize(r.width(), r.height());
    else
        dragPixmap.resize(std::max(r.width(), pixw)+margin*2, r.height()+margin+(pixh+margin)*pixmaps.count());

    QPalette pal = QToolTip::palette();
    QPainter p;
    p.begin(&dragPixmap);

    // draw frame
    p.setPen(QPen(pal.color(QPalette::Active, QColorGroup::Text)));
    p.setBrush(QBrush(pal.color(QPalette::Active, QColorGroup::Background)));
    p.drawRect(dragPixmap.rect());

    // draw file names
    if(type == Icons)
        p.drawText(dragPixmap.rect(), flags, text);
    else // or thumbnails
    {
        int y = margin;
        QValueVector<QPixmap>::iterator itEnd = pixmaps.end();

        for(QValueVector<QPixmap>::iterator it = pixmaps.begin();it != itEnd;++it)
        {
            p.drawPixmap(QRect((dragPixmap.width()-pixw)/2, y, pixw, pixh), *it);
            y = y + pixh + margin;
        }

        p.drawText(0, y, dragPixmap.width(), dragPixmap.height()-y, flags, text);
    }

    p.end();

    // finally, setup drag object
    drag->setPixmap(dragPixmap, QPoint(16, -16));
    drag->dragCopy();
}
