/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_ImageResize::init()
{
    pixmap->setPixmap(QPixmap(locate("appdata", "images/imageedit/squirrels/squirrel_resize.png")));
    pixmap->setPaletteBackgroundColor(pixmap->colorGroup().background().light(90));
    pushOptions->setPixmap(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall));
    groupBoxApsect->setEnabled(false);

    SQ_Config::instance()->setGroup("Image edit options");

    method = SQ_Config::instance()->readEntry("resize_method", "BOX");
    methods = new QPopupMenu;
    methods->insertItem("BOX");
    methods->insertItem("TRIANGLE");
    methods->insertItem("BELL");
    methods->insertItem("BSPLINE");
    methods->insertItem("LANCZOS3");
    methods->insertItem("MITCHELL");
    connect(methods, SIGNAL(activated(int)), this, SLOT(slotMethodActivated(int)));
    pushMethod->setPopup(methods);

//    widgetStackTypes->setPaletteBackgroundColor(widgetStackTypes->colorGroup().highlight().light(110));

    strings.append(tr2i18n("<b>resize&nbsp;in&nbsp;percents</b>"));
    strings.append(tr2i18n("<b>resize&nbsp;in&nbsp;pixels</b>"));

    id = SQ_Config::instance()->readNumEntry("resize_which", 1);
    widgetStackTypes->raiseWidget(id);
    text->setText(strings[id] + QString::fromLatin1("&nbsp;<b>[%1]</b>").arg(method));

    comboApplyTo->insertItem(QPixmap(locate("appdata", "images/imageedit/resize_w.png")), tr2i18n("Width"));
    comboApplyTo->insertItem(QPixmap(locate("appdata", "images/imageedit/resize_h.png")), tr2i18n("Height"));
    comboApplyTo->insertItem(QPixmap(locate("appdata", "images/imageedit/resize_wh.png")), tr2i18n("Both"));
    comboFit->insertItem(QPixmap(locate("appdata", "images/imageedit/resize_w.png")), tr2i18n("Width"));
    comboFit->insertItem(QPixmap(locate("appdata", "images/imageedit/resize_h.png")), tr2i18n("Height"));
    comboFit->insertItem(QPixmap(locate("appdata", "images/imageedit/resize_wh.png")), tr2i18n("Both"));

    QPixmap tool1 = QPixmap::fromMimeSource(locate("appdata", "images/imageedit/resize_toolbutton.png"));    
    QPixmap tool2 = QPixmap::fromMimeSource(locate("appdata", "images/imageedit/resize_toolbutton2.png"));
    push1->setPixmap(tool1);
    push2->setPixmap(tool2);

    bool show_sl = true;
    kIntPercent->setRange(1, 5000, 1, show_sl);
    kIntPixH->setRange(1, 10000, 1, show_sl);
    kIntPixW->setRange(1, 10000, 1, show_sl);
    kIntPercent->setValue(100);

    imageopt.putto = SQ_Config::instance()->readEntry("resize_putto", QString::null);
    imageopt.where_to_put = SQ_Config::instance()->readNumEntry("resize_where_to_put", 0);
    imageopt.close = SQ_Config::instance()->readBoolEntry("resize_close", true);

    checkPreserve->setChecked(SQ_Config::instance()->readBoolEntry("resize_preserve", true));
    comboFit->setCurrentItem(SQ_Config::instance()->readNumEntry("resize_fit", 2));
    comboApplyTo->setCurrentItem(SQ_Config::instance()->readNumEntry("resize_applyto", 2));
    kIntPercent->setValue(SQ_Config::instance()->readNumEntry("resize_percent", 100));
    kIntPixW->setValue(SQ_Config::instance()->readNumEntry("resize_w", 1));
    kIntPixH->setValue(SQ_Config::instance()->readNumEntry("resize_h", 1));
    checkDontShow->setChecked(SQ_Config::instance()->readBoolEntry("resize_dontshowhelp", false));

    done = true;

    if(checkDontShow->isChecked())
        slotNext();
}

void SQ_ImageResize::startResizing(int count)
{
    done = false;
    paragraph = 0;
    progress->setProgress(0);
    progress->setTotalSteps(count);
}

void SQ_ImageResize::slotOneProcessed()
{
    progress->advance(1);
    paragraph++;
}

void SQ_ImageResize::slotDebugText(const QString &text, bool bold)
{
    textResult->setBold(bold);
    textResult->insertAt(text, paragraph, textResult->paragraphLength(paragraph));
    textResult->setBold(false);
}

void SQ_ImageResize::slotStartResize()
{
    pushCancel->setFocus();
    pushNext->setDefault(false);
    pushNext->setDisabled(true);
    pushCancel->setDefault(true);
    pushCancel->setFocus();
    widgetStack->raiseWidget(1);

    qApp->processEvents();

    SQ_ImageResizeOptions ropt;

    ropt.percentage = (id == 0);
    ropt.pc = kIntPercent->value();
    ropt.w = kIntPixW->value();
    ropt.h = kIntPixH->value();
    ropt.preserve = checkPreserve->isChecked();
    ropt.adjust = (ropt.percentage) ? comboApplyTo->currentItem() : comboFit->currentItem();

    if(method == "BOX") ropt.method = fmt_filters::ILU_SCALE_BOX;
    else if(method == "TRIANGLE") ropt.method = fmt_filters::ILU_SCALE_TRIANGLE;
    else if(method == "BELL") ropt.method = fmt_filters::ILU_SCALE_BELL;
    else if(method == "BSPLINE") ropt.method = fmt_filters::ILU_SCALE_BSPLINE;
    else if(method == "MITCHELL") ropt.method = fmt_filters::ILU_SCALE_MITCHELL;
    else ropt.method = fmt_filters::ILU_SCALE_LANCZOS3;

    SQ_Config::instance()->setGroup("Image edit options");
    SQ_Config::instance()->writeEntry("resize_w", kIntPixW->value());
    SQ_Config::instance()->writeEntry("resize_h", kIntPixH->value());
    SQ_Config::instance()->writeEntry("resize_percent", kIntPercent->value());
    SQ_Config::instance()->writeEntry("resize_applyto", comboApplyTo->currentItem());
    SQ_Config::instance()->writeEntry("resize_fit", comboFit->currentItem());
    SQ_Config::instance()->writeEntry("resize_preserve", checkPreserve->isChecked());
    SQ_Config::instance()->writeEntry("resize_which", id);
    SQ_Config::instance()->writeEntry("resize_method", method);
    SQ_Config::instance()->writeEntry("resize_dontshowhelp", checkDontShow->isChecked());

    emit _resize(&imageopt, &ropt);
}

void SQ_ImageResize::slotOptions()
{
    SQ_ImageEditOptions *o = new SQ_ImageEditOptions(this);

    // SQ_ImageEditOptions will write needed KConfig entries, if
    // exec() will return QDialog::Accepted
    o->setConfigPrefix("resize");

    o->exec(&imageopt);
}

void SQ_ImageResize::slotDone(bool close)
{
    done = true;
    
    if(close)
        reject();
}

void SQ_ImageResize::slotReject()
{
    if(done)
        reject();
}

void SQ_ImageResize::slotPush()
{
    if(!id)    id = 1;
    else     id = 0;

    widgetStackTypes->raiseWidget(id);
    text->setText(strings[id] + QString::fromLatin1("&nbsp;<b>[%1]</b>").arg(method));
}

void SQ_ImageResize::slotMethodActivated(int idd)
{
    method = methods->text(idd);
    text->setText(strings[id] + QString::fromLatin1("&nbsp;<b>[%1]</b>").arg(method));
}

void SQ_ImageResize::closeEvent(QCloseEvent *e)
{
    if(done)
        e->accept();
    else
    {
        e->ignore();
        QWhatsThis::display(SQ_ErrorString::instance()->string(SQE_NOTFINISHED));
    }
}

void SQ_ImageResize::slotNext()
{
    pushNext->setDefault(false);
    pushResize->setDefault(true);
    pushResize->setFocus();

    widgetStackWizard->raiseWidget(1);
}

void SQ_ImageResize::slotBack()
{
    pushNext->setDefault(true);
    pushResize->setDefault(false);

    widgetStackWizard->raiseWidget(0);
}
