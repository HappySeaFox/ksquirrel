/***************************************************************************
                          sq_converter.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qapplication.h>
#include <qfile.h>

#include <kmessagebox.h>
#include <kstringhandler.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <klocale.h>

#include "ksquirrel.h"
#include "sq_converter.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_libraryhandler.h"
#include "sq_config.h"
#include "sq_errorstring.h"
#include "sq_imageloader.h"
#include "sq_imageconvert.h"

SQ_Converter * SQ_Converter::m_sing = 0;

SQ_Converter::SQ_Converter(QObject *parent) : QObject(parent)
{
    m_sing = this;

    err_internal = i18n("internal error") + "\n";
    err_failed = i18n("failed") + "\n";

    special_action = i18n("Converting");

    image = 0;
}

SQ_Converter::~SQ_Converter()
{
    if(image) free(image);
}

void SQ_Converter::slotStartEdit()
{
    files.clear();

    KFileItemList *items = (KFileItemList *)SQ_WidgetStack::instance()->selectedItems();

    if(!items || !items->count())
    {
        KMessageBox::information(KSquirrel::app(), i18n("Select files to edit"));
        return;
    }
    else if(!items->first()->url().isLocalFile())
    {
        KMessageBox::information(KSquirrel::app(), i18n("Converter cannot work with non-local files.\nSorry"));
        return;
    }

    KFileItem *i = items->first();

    for(;i;i = items->next())
    {
        if(i->isFile())
            files.append(i->url().path());
    }

    startEditPrivate();
}

QString SQ_Converter::adjustFileName(const QString &globalprefix, const QString &name1, int replace, QString putto, bool paged, int page)
{
    QFileInfo ff(name1);
    QString name = ff.dirPath() + "/" + (replace == 0 ? globalprefix : (replace == 2 ? QString::null : globalprefix)) + ff.fileName();
    ff = QFileInfo(name);

    QString result, inner, filter = lw->filter;
    QString ext = ff.extension(false);
    QString prefix, suffix, name2 = name;
    QString spage = QString::fromLatin1("page_%1.").arg((QString::fromLatin1("%1").arg(page)).rightJustify(3, '0'));

    if(!putto.isEmpty())
    {
        if(QFile::exists(putto))
            name2 = putto + "/" + ff.fileName();
    }

    prefix = name2;

    prefix.truncate(name2.length() - ext.length());

    suffix = (SQ_LibraryHandler::instance()->knownExtension(QString::fromLatin1("*.") + ext))
                ? QString(lw->codec->extension(32)) : ext;

    if(replace == 0 || replace == 2)
        result = (!paged) ? (prefix + inner + suffix) : (prefix + spage + inner + suffix);
    else
    {
        result = (!paged) ? (prefix + inner + suffix) : (prefix + spage + inner + suffix);

        if(QFile::exists(result))
        {
            inner = QString::fromLatin1("1.");
            result = (!paged) ? (prefix + inner + suffix) : (prefix + spage + inner + suffix);
        }
    }

    return result;
}

void SQ_Converter::errorjmp(jmp_buf jmp, const int code)
{
    error_code = code;
    longjmp(jmp, 1);
}

void SQ_Converter::decodingCycle()
{
    int      i, j;
    QString  name;
    jmp_buf  jmp;
    RGBA     *scan;
    int      errors, gerrors = 0, current;
    QString  putto;
    int      replace = imageopt.where_to_put;
    bool    brk;

    SQ_Config::instance()->setGroup("Edit tools");

    int allpages = SQ_Config::instance()->readNumEntry("load_pages", 0);
    int pages_num = SQ_Config::instance()->readNumEntry("load_pages_number", 1);

    if(pages_num < 1) pages_num = 1;

    altw = SQ_LibraryHandler::instance()->libraryByName(SQ_Config::instance()->readEntry("altlibrary", "Portable Network Graphics"));
    multi = SQ_Config::instance()->readBoolEntry("multi", true);

    tempfile = new KTempFile;
    tempfile->setAutoDelete(true);

    if(tempfile->status())
    {
      KMessageBox::error(KSquirrel::app(), i18n("Temporary file creation failed"));
      return;
    }

    tempfile->close();

    QStringList::iterator last_it = files.fromLast();
    QStringList::iterator itEnd = files.end();
    convert->startConvertion(files.count());

    putto = imageopt.putto;

    for(QStringList::iterator it = files.begin();it != itEnd;++it)
    {
        currentFile = *it;
        last = (it == last_it);

        QFileInfo ff(*it);

        emit convertText(special_action + " " + KStringHandler::rsqueeze(ff.fileName()) + "... ", false);

        if((lr = SQ_LibraryHandler::instance()->libraryForFile(*it)))
        {
            lw = SQ_LibraryHandler::instance()->libraryByName(convopt.libname);

            if(!lr || !lw)
            {
                gerrors++;
                emit convertText(err_internal, true);
                emit oneFileProcessed();
                continue;
            }

            name = QFile::encodeName(*it);

            i = lr->codec->read_init(name.ascii());

            if(setjmp(jmp))
            {
                gerrors++;

                lr->codec->read_close();

                emit convertText(SQ_ErrorString::instance()->stringSN(error_code), true);
                emit oneFileProcessed();

                continue;
            }

            if(i != SQE_OK)
                errorjmp(jmp, i);

            errors = 0;
            current = 0;

            while(true)
            {
                brk = (allpages == 1 && current) || (allpages == 2 && current == pages_num);

                i = lr->codec->read_next();

                im = lr->codec->image(current-1);

                if(i != SQE_OK || brk)
                {
                    if(i == SQE_NOTOK || brk)
                    {
                        if(current == 1)
                            name = adjustFileName(prefix, *it, replace, putto);
                        else
                            name = adjustFileName(prefix, *it, replace, putto, true, current);

                        lastFrame = last;

                        i = manipAndWriteDecodedImage(tempfile->name(), im);

                        emit convertText(errors ? (i18n("1 error", "%n errors", errors)+"\n") : SQ_ErrorString::instance()->stringSN(SQE_OK), true);
                        emit oneFileProcessed();

                        i = SQE_OK;

                        if(replace == 2)
                        {
                            emit convertText(i18n("Removing") + KStringHandler::rsqueeze(ff.fileName()) + QString("... "), false);

                            bool b = QFile::remove(*it);

                            emit convertText(b ? SQ_ErrorString::instance()->stringSN(SQE_OK) : err_failed, true);
                            emit oneFileProcessed();
                        }

                        i = copyFile(tempfile->name(), name);

                        break;
                    }
                    else
                        errorjmp(jmp, i);
                }

                if(current)
                {
                    name = adjustFileName(prefix, *it, replace, putto, true, current);

                    lastFrame = false;

                    manipAndWriteDecodedImage(tempfile->name(), im);
                    i = copyFile(tempfile->name(), name);
                }

                im = lr->codec->image(current);

                image = (RGBA *)realloc(image, im->w * im->h * sizeof(RGBA));

                if(!image)
                {
                    i = SQE_R_NOMEMORY;
                    errorjmp(jmp, i);
                }

                for(int pass = 0;pass < im->passes;pass++)
                {
                    lr->codec->read_next_pass();

                    for(j = 0;j < im->h;j++)
                    {
                        scan = image + j * im->w;
                        i = lr->codec->read_scanline(scan);
                        errors += (int)(i != SQE_OK);
                    }
                }

                if(im->needflip)
                    fmt_utils::flipv((char *)image, im->w * sizeof(RGBA), im->h);

                convert->fillWriteOptions(&opt, lw->opt);

                opt.alpha = im->hasalpha;

                current++;
            }

            lr->codec->read_close();
        }
        else
        {
            emit convertText(SQ_ErrorString::instance()->stringSN(SQE_R_NOTSUPPORTED), true);
            emit oneFileProcessed();
        }
    }

    if(image)
    {
        free(image);
        image = 0;
    }

    delete convert;
    delete tempfile;

    if(imageopt.close && !gerrors)
        emit done(true);
    else
        emit done(false);
}

int SQ_Converter::manipAndWriteDecodedImage(const QString &name, fmt_image *im)
{
    int     passes = opt.interlaced ?  lw->opt.passes : 1;
    int     s, j, err;
    RGBA     *scan = 0;

    scan = new RGBA [im->w];

    if(!scan)
        return SQE_W_NOMEMORY;

    err = lw->codec->write_init(name, *im, opt);

    if(err != SQE_OK)
        goto error_exit;

    err = lw->codec->write_next();

    if(err != SQE_OK)
        goto error_exit;

    for(s = 0;s < passes;s++)
    {
        err = lw->codec->write_next_pass();

        if(err != SQE_OK)
            goto error_exit;

        for(j = 0;j < im->h;j++)
        {
            if(lw->opt.needflip)
                determineNextScan(*im, scan, im->h-j-1);
            else
                determineNextScan(*im, scan, j);

            err = lw->codec->write_scanline(scan);

            if(err != SQE_OK)
                goto error_exit;
        }
    }

    err = SQE_OK;

    error_exit:

    lw->codec->write_close();

    delete scan;

    return err;
}

int SQ_Converter::copyFile(const QString &src, const QString &dst) const
{
    QFile f_src(src), f_dst(dst);
    Q_LONG read;
    char data[4096];

    if(!f_src.open(IO_ReadOnly))
        return SQE_R_NOFILE;

    if(!f_dst.open(IO_WriteOnly))
    {
        f_src.close();
        return SQE_W_NOFILE;
    }

    while(!f_src.atEnd())
    {
        read = f_src.readBlock(data, sizeof(data));

        f_dst.writeBlock(data, read);

        if(f_dst.status() != IO_Ok || f_src.status() != IO_Ok)
        {
            f_src.close();
            f_dst.close();

            return SQE_W_ERROR;
        }
    }

    f_src.close();
    f_dst.close();

    return SQE_OK;
}

void SQ_Converter::determineNextScan(const fmt_image &im, RGBA *scan, int y)
{
    memcpy(scan, image + y * im.w, im.w * sizeof(RGBA));
}

void SQ_Converter::startEditPrivate()
{
    convert = new SQ_ImageConvert(KSquirrel::app());
    convert->setCaption(i18n("Convert 1 file", "Convert %n files", files.count()));

    connect(convert, SIGNAL(convert(SQ_ImageOptions*, SQ_ImageConvertOptions*)), this, SLOT(slotStartConvert(SQ_ImageOptions*, SQ_ImageConvertOptions*)));
    connect(this, SIGNAL(convertText(const QString &, bool)), convert, SLOT(slotDebugText(const QString &, bool)));
    connect(this, SIGNAL(oneFileProcessed()), convert, SLOT(slotOneProcessed()));
    connect(this, SIGNAL(done(bool)), convert, SLOT(slotDone(bool)));

    convert->exec();
}

void SQ_Converter::slotStartConvert(SQ_ImageOptions *o, SQ_ImageConvertOptions *copt)
{
    imageopt = *o;
    convopt = *copt;

    decodingCycle();
}

#include "sq_converter.moc"
