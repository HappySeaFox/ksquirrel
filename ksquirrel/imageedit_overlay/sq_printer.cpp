/***************************************************************************
                          sq_printer.cpp  -  description
                             -------------------
    begin                : ??? May 11 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#include <klocale.h>
#include <kprinter.h>
#include <kdebug.h>

#include "ksquirrel.h"
#include "sq_libraryhandler.h"
#include "sq_printer.h"
#include "sq_imageprint.h"
#include "sq_about.h"

#include "libpixops/pixops.h"

#define IMAGE_ALPHA_CHECK1            0x009C999C
#define IMAGE_ALPHA_CHECK2            0x00626562
#define IMAGE_ALPHA_CHECK_SIZE        8

SQ_Printer * SQ_Printer::sing = NULL;

SQ_Printer::SQ_Printer(QObject *parent) : SQ_EditBase(parent)
{
    sing = this;

    kdDebug() << "+SQ_Printer" << endl;

    special_action = i18n("Printing");

    ondisk = false;

    painter = new QPainter;
}

SQ_Printer::~SQ_Printer()
{
    kdDebug() << "-SQ_Printer" << endl;
}

void SQ_Printer::startEditPrivate()
{
    print = new SQ_ImagePrint(KSquirrel::app());
    print->setCaption(i18n("Print 1 file", "Print %n files", files.count()));

    connect(print, SIGNAL(print(SQ_ImageOptions*, SQ_ImagePrintOptions*)), this, SLOT(slotStartPrint(SQ_ImageOptions*, SQ_ImagePrintOptions*)));
    connect(this, SIGNAL(convertText(const QString &, bool)), print, SLOT(slotDebugText(const QString &, bool)));
    connect(this, SIGNAL(oneFileProcessed()), print, SLOT(slotOneProcessed()));
    connect(this, SIGNAL(done(bool)), print, SLOT(slotDone(bool)));

    print->exec();
}

void SQ_Printer::slotStartPrint(SQ_ImageOptions *o, SQ_ImagePrintOptions *popt)
{
    imageopt = *o;
    prnopt = *popt;

    printer = new KPrinter;
    printer->setCreator(QString::fromLatin1("KSquirrel %1").arg(SQ_VERSION));

    if(printer->setup(print))
    {
        currentPage = -1;

        mt = new QPaintDeviceMetrics(printer);

        pixmap = prnopt.type ? new unsigned char [mt->width() * mt->height() * sizeof(RGBA)] : NULL;

        if(prnopt.type && !pixmap)
            return;

        painter->begin(printer);

        x = y = 0;

        if(prnopt.type == 2)
        {
            memset(pixmap, 255, mt->width() * mt->height() * sizeof(RGBA));
            w = mt->width() / prnopt.in_x;
            h = mt->height() / prnopt.in_y;
        }

        decodingCycle();
    }
    else
        print->close();
}

SQ_Printer* SQ_Printer::instance()
{
    return sing;
}

int SQ_Printer::manipDecodedImage(fmt_image *)
{
    return SQE_OK;
}

void SQ_Printer::dialogReset()
{
    print->startPrinting(files.count());
}

int SQ_Printer::manipAndWriteDecodedImage(const QString &, fmt_image *im)
{
    unsigned char *rgba;

    const int spacing = 3;

    if(prnopt.type == 1)
    {

    }
    else if(prnopt.type == 2)
    {
        QSize s(im->w, im->h);
        s.scale(w-spacing, h-spacing, QSize::ScaleMin);

        rgba = new unsigned char [s.width() * s.height() * sizeof(RGBA)];

        if(!rgba)
            return SQE_W_NOMEMORY;

        if(im->hasalpha)
        {
            if(!prnopt.transp)
                pixops_composite_color(rgba, 0, 0, s.width(), s.height(), s.width() * 4, 4, true,
                        (unsigned char *)image, im->w, im->h, im->w * 4, 4, true,
                        (double)s.width() / im->w, (double)s.height() / im->h,
                        PIXOPS_INTERP_BILINEAR, 255, 0, 0,
                        IMAGE_ALPHA_CHECK_SIZE,
                        IMAGE_ALPHA_CHECK1,
                        IMAGE_ALPHA_CHECK2);
            else
            {
                int qrgba = qRgba(prnopt.transp_color.red(), prnopt.transp_color.green(), prnopt.transp_color.blue(), 255);

                pixops_composite_color(rgba, 0, 0, s.width(), s.height(), s.width() * 4, 4, true,
                                (unsigned char *)image, im->w, im->h, im->w * 4, 4, true,
                                (double)s.width() / im->w, (double)s.height() / im->h,
                                PIXOPS_INTERP_BILINEAR, 255, 0, 0,
                                IMAGE_ALPHA_CHECK_SIZE, qrgba, qrgba);
            }
        }
        else
            pixops_scale(rgba, 0, 0, s.width(), s.height(), s.width() * 4, 4, true,
                        (unsigned char *)image, im->w, im->h, im->w * 4, 4, true,
                        (double)s.width() / im->w, (double)s.height() / im->h,
                        PIXOPS_INTERP_BILINEAR);

        free(image);

        image = (RGBA *)rgba;

        im->w = s.width();
        im->h = s.height();

        for(int Y = 0;Y < im->h;Y++)
        {
            memcpy(pixmap + mt->width() * sizeof(RGBA) * (y * h + Y) + x * w * sizeof(RGBA),
            rgba + im->w * sizeof(RGBA) * Y, im->w * sizeof(RGBA));
        }

        x++;

        if(x == prnopt.in_x || lastFrame)
        {
            if(!lastFrame)
            {
                x = 0;
                y++;
            }

            if(y == prnopt.in_y || lastFrame)
            {
                currentPage++;

                if(!lastFrame) y = 0;

                QImage img(pixmap, mt->width(), mt->height(), 32, 0, 0, QImage::LittleEndian);
                img.setAlphaBuffer(true);
                img = img.swapRGB();

                int cps = printer->numCopies();

                for(int pages = 0;pages < cps;pages++)
                {
                    if(currentPage || (!currentPage && pages))
                        if(!printer->newPage())
                        {
                            qWarning(i18n("KPrinter::newPage() error"));
                            continue;
                        }

                    painter->drawImage(0, 0, img);
                }

                memset(pixmap, 255, mt->width() * mt->height() * sizeof(RGBA));
            }
        }
    }
    else
    {
        currentPage++;

        if(im->w > mt->width() || im->h > mt->height())
        {
            double asp;

            if(im->w > mt->width() && im->h > mt->height())
            {
                double asp1 = (double)im->w / im->h;
                double asp2 = (double)mt->width() / mt->height();

                asp = (asp1 > asp2) ? asp1 : asp2;
            }
            else
                asp = (im->w > mt->width()) ? ((double)im->w / mt->width()) : ((double)im->h / mt->height());

            w = int((double)im->w / asp);
            h = int((double)im->h / asp);

            rgba = new unsigned char [w * h * sizeof(RGBA)];

            if(!rgba)
                return SQE_W_NOMEMORY;

            pixops_scale(rgba, 0, 0, w, h, w * 4, 4, true,
                (unsigned char *)image, im->w, im->h, im->w * 4, 4, true,
                (double)w / im->w, (double)h / im->h,
                PIXOPS_INTERP_BILINEAR);

            free(image);

            image = (RGBA *)rgba;

            im->w = w;
            im->h = h;
        }

        if(prnopt.align == "lefttop")     { x = 0;     y = 0;     }
        else if(prnopt.align == "righttop")     { x = mt->width() - im->w;    y = 0;     }
        else if(prnopt.align == "leftbottom")    { x = 0;    y = mt->height() - im->h;    }
        else if(prnopt.align == "rightbottom"){ x = mt->width() - im->w;    y = mt->height() - im->h;    }
        else    { x = (mt->width() - im->w) / 2; y = (mt->height() - im->h) / 2; }

        QImage img((uchar *)image, im->w, im->h, 32, 0, 0, QImage::LittleEndian);
        img.setAlphaBuffer(true);
        img = img.swapRGB();

        int cps = printer->numCopies();

        for(int pages = 0;pages < cps;pages++)
        {
            if(currentPage || (!currentPage && pages))
                if(!printer->newPage())
                {
                    qWarning(i18n("KPrinter::newPage() error"));
                    continue;
                }

                painter->drawImage(x, y, img);
        }
    }

    return SQE_OK;
}

void SQ_Printer::cycleDone()
{
    painter->end();

    if(pixmap) delete pixmap;

    delete mt;
    delete printer;
    delete print;
}
