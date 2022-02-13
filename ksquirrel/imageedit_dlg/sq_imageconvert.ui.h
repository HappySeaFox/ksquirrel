/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_ImageConvert::init()
{
    pixmap->setPixmap(QPixmap(locate("appdata", "images/imageedit/squirrels/squirrel_convert.png")));
    pixmap->setPaletteBackgroundColor(pixmap->colorGroup().background().dark(115));

    tableLib->header()->hide();   

    createExtension();
    
    initLibs();

    pushWriteOptions->setPixmap(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall));
    pushOptions->setPixmap(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall));

    SQ_Config::instance()->setGroup("Image edit options");

    imageopt.putto = SQ_Config::instance()->readEntry("convert_putto", QString::null);
    imageopt.where_to_put = SQ_Config::instance()->readNumEntry("convert_where_to_put", 0);
    imageopt.close = SQ_Config::instance()->readBoolEntry("convert_close", true);

    checkDontShow->setChecked(SQ_Config::instance()->readBoolEntry("convert_dontshowhelp", false));

    done = true;

    if(checkDontShow->isChecked())
        slotNext();
}

void SQ_ImageConvert::initLibs()
{
    QPixmap pix;
    QListViewItem *first;
    
    for(QValueVector<SQ_LIBRARY>::iterator it = SQ_LibraryHandler::instance()->begin();
            it != SQ_LibraryHandler::instance()->end();++it)
    {
        if((*it).writable)
        {
            QFileInfo libfileinfo((*it).libpath);
            QListViewItem *item = new QListViewItem(tableLib, QString::null, QString((*it).quickinfo));
    
            if(pix.convertFromImage((*it).mime))
                item->setPixmap(0, pix);

            tableLib->insertItem(item);
        }
    }

    first = tableLib->firstChild();

    if(first)
    {
        tableLib->setSelected(first, true);
        slotCurrentLibraryChanged(first);
    }
}

void SQ_ImageConvert::slotShowExtWrapper(bool show)
{
    showExtension(show);
}

void SQ_ImageConvert::createExtension()
{
    SQ_WriteOption *c = new SQ_WriteOption(this);
    c->slider->setRange(0, 255, 1, true);
    setExtension(c);
    setOrientation(Qt::Horizontal);
}

void SQ_ImageConvert::slotDebugText(const QString &text, bool bold)
{
    textResult->setBold(bold);
    textResult->insertAt(text, paragraph, textResult->paragraphLength(paragraph));
    textResult->setBold(false);
}

void SQ_ImageConvert::slotStartConvert()
{
    pushConvert->setDefault(false);
    pushConvert->setDisabled(true);
    pushCancel->setDefault(true);
    pushCancel->setFocus();
    widgetStack->raiseWidget(1);
    showExtension(false);

    qApp->processEvents();

    QListViewItem *i = tableLib->selectedItem();

    if(!i)
        return;

    SQ_ImageConvertOptions copt;
    copt.libname =  i->text(1);

    SQ_Config::instance()->writeEntry("convert_dontshowhelp", checkDontShow->isChecked());

    emit convert(&imageopt, &copt);
}

void SQ_ImageConvert::startConvertion(int count)
{
    done = false;
    paragraph = 0;
    progress->setProgress(0);
    progress->setTotalSteps(count);
}

void SQ_ImageConvert::slotOneProcessed()
{
    progress->advance(1);
    paragraph++;
}

void SQ_ImageConvert::slotCurrentLibraryChanged( QListViewItem * i)
{
    if(!i)
        return;

    QString name = i->text(1);

    SQ_LIBRARY *l = SQ_LibraryHandler::instance()->libraryByName(name);

    if(!l)
        return;

    SQ_WriteOption *e = (SQ_WriteOption*)extension();

    if(!e)
        return;

    e->widgetStack->raiseWidget(0);

    bool bslider = (l->opt.compression_scheme == CompressionInternal && (l->opt.compression_min || l->opt.compression_max));

    e->slider->setShown(bslider);
    e->checkRLE->setShown(!bslider);

    if(bslider)
    {
        e->slider->setRange(l->opt.compression_min, l->opt.compression_max, 1, true);
        e->slider->setValue(l->opt.compression_def);
    }

    bool rle = true;

    if(!(l->opt.compression_scheme & CompressionRLE))
    {
        e->checkRLE->hide();
        rle = false;
    }

    bool binter = l->opt.interlaced;

    e->checkInterlaced->setShown(binter);

    if(!binter && !rle && !bslider)
        e->widgetStack->raiseWidget(1);
}

void SQ_ImageConvert::fillWriteOptions(fmt_writeoptions *opt, const fmt_writeoptionsabs &opt2)
{
    SQ_WriteOption *e = (SQ_WriteOption*)extension();

    if(!e)
        return;

    opt->interlaced = (opt2.interlaced) ? e->checkInterlaced->isChecked() : false;

    if(opt2.compression_scheme == CompressionInternal)
        opt->compression_scheme = CompressionInternal;
    else if(opt2.compression_scheme == CompressionRLE)
        opt->compression_scheme = (e->checkRLE->isChecked()) ? CompressionRLE : CompressionNo;
    else
        opt->compression_scheme = CompressionNo;

    opt->compression_level = (opt->compression_scheme != CompressionNo) ? e->slider->value() : 0;
}

void SQ_ImageConvert::slotOptions()
{
    SQ_ImageEditOptions o(this);

    // SQ_ImageEditOptions will write needed KConfig entries, if
    // exec() will return QDialog::Accepted
    o.setConfigPrefix("convert");

    o.exec(&imageopt);
}

void SQ_ImageConvert::slotDone(bool close)
{
    done = true;

    if(close)
        reject();
}

void SQ_ImageConvert::slotReject()
{
    if(done)
        reject();
}

void SQ_ImageConvert::closeEvent(QCloseEvent *e)
{
    if(done)
        e->accept();
    else
    {
        e->ignore();
        QWhatsThis::display(SQ_ErrorString::instance()->string(SQE_NOTFINISHED));
    }
}

void SQ_ImageConvert::slotNext()
{
    pushNext->setDefault(false);
    pushConvert->setDefault(true);
    pushConvert->setFocus();

    widgetStackWizard->raiseWidget(1);
}

void SQ_ImageConvert::slotBack()
{
    pushNext->setDefault(true);
    pushConvert->setDefault(false);

    widgetStackWizard->raiseWidget(0);
}
