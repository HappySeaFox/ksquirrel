/***************************************************************************
                          sq_converter.h  -  description
                             -------------------
    begin                : ??? Mar 3 2005
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

#ifndef SQ_CONVERTER_H
#define SQ_CONVERTER_H

#include <qobject.h>
#include <qstringlist.h>

#include <sstream>
#include <csetjmp>

#include <ksquirrel-libs/fmt_utils.h>
#include <ksquirrel-libs/fmt_codec_base.h>
#include <ksquirrel-libs/error.h>

#include "sq_imageedit.h"

struct SQ_LIBRARY;

class KTempFile;

class SQ_ImageConvert;
class SQ_ImageConvertOptions;

/*
 *  Base class for all edit tools. It contains all methods needed by
 *  converter, resizer, printer etc. It decodes an image, writes it back to
 *  disk. All specific actions (like colorizing, resizing etc.) are done by
 *  appropriate edit tool.
 */

class SQ_Converter : public QObject
{
    Q_OBJECT

    public:
        SQ_Converter(QObject *parent);
        ~SQ_Converter();

    public:
         static SQ_Converter* instance() { return m_sing; }

    private:
        /*
         *  Create edit tool-specific dialog, and wait for user
         *  input
         */
        void startEditPrivate();

        /*
         *  Do something with decoded image(manipDecodedImage()) and write it
         *  on disk.
         */
        int manipAndWriteDecodedImage(const QString &name, fmt_image *im);

        /*
         *  Main decoding loop.
         */
        void decodingCycle();

        /*
         *  Determine file name. In this file SQ_Converter will save new image.
         */
        QString adjustFileName(const QString &globalprefix, const QString &name, int replace,
                    QString putto, bool paged = false, int page = 0);

        void determineNextScan(const fmt_image &im, RGBA *scan, int y);

        /*
         *  Copy file :-)
         */
        int copyFile(const QString &src, const QString &dst) const;

        void errorjmp(jmp_buf jmp, const int code);

    signals:
        void convertText(const QString &, bool);
        void oneFileProcessed();
        void done(bool allok);

    private slots:
        void slotStartEdit();
        void slotStartConvert(SQ_ImageOptions*, SQ_ImageConvertOptions*);

    protected:
        QStringList         files;
        QString             err_internal, err_failed;
        QString             currentFile;
        int                 error_code, current_page;
        SQ_ImageOptions     imageopt;
        fmt_writeoptions    opt;
        SQ_LIBRARY          *lr, *lw, *altw;
        RGBA                *image;
        QString             special_action, prefix;
        bool                multi, last, lastFrame;
        KTempFile           *tempfile;
        fmt_image           *im;
        static SQ_Converter    *m_sing;

        SQ_ImageConvert    *convert;
        SQ_ImageConvertOptions convopt;
};

#endif
