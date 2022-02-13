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
    pushOptions->setPixmap(KSquirrel::loader()->loadIcon("configure", KIcon::Desktop, 16));
    groupBoxApsect->setEnabled(false);

    method = "NEAREST";
    methods = new QPopupMenu;
    methods->insertItem(tr2i18n("NEAREST"));
    methods->insertItem(tr2i18n("BILINEAR"));
    methods->insertItem(tr2i18n("TILES"));
    methods->insertItem(tr2i18n("HYPER"));
    connect(methods, SIGNAL(activated(int)), this, SLOT(slotMethodActivated(int)));
    pushMethod->setPopup(methods);

    widgetStackTypes->setPaletteBackgroundColor(widgetStackTypes->colorGroup().highlight().light(110));

    strings.append(tr2i18n("<b>resize&nbsp;in&nbsp;percents</b>"));
    strings.append(tr2i18n("<b>resize&nbsp;in&nbsp;pixels</b>"));

    id = 1;
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

    imageopt.putto = SQ_Config::instance()->readEntry("Image edit options", "resize_putto", QString::null);
    imageopt.prefix = SQ_Config::instance()->readEntry("Image edit options", "resize_prefix", QString::null);
    imageopt.where_to_put = SQ_Config::instance()->readNumEntry("Image edit options", "resize_where_to_put", 0);
    imageopt.close = SQ_Config::instance()->readBoolEntry("Image edit options", "resize_close", true);

    done = true;
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
    pushResize->setDefault(false);
    pushResize->setDisabled(true);
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

    emit _resize(&imageopt, &ropt);
}

void SQ_ImageResize::slotOptions()
{
    SQ_ImageEditOptions *o = new SQ_ImageEditOptions(this);

    if(o->exec(&imageopt) == QDialog::Accepted)
    {
	SQ_Config::instance()->setGroup("Image edit options");
	SQ_Config::instance()->writeEntry("resize_putto", imageopt.putto);
	SQ_Config::instance()->writeEntry("resize_prefix", imageopt.prefix);
	SQ_Config::instance()->writeEntry("resize_where_to_put", imageopt.where_to_put);
	SQ_Config::instance()->writeEntry("resize_close", imageopt.close);
    }
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
    if(!id)	id = 1;
    else 	id = 0;

    widgetStackTypes->raiseWidget(id);
    text->setText(strings[id] + QString::fromLatin1("&nbsp;<b>[%1]</b>").arg(method));
}

void SQ_ImageResize::slotMethodActivated(int idd)
{
    method = methods->text(idd);
    text->setText(strings[id] + QString::fromLatin1("&nbsp;<b>[%1]</b>").arg(method));
}
