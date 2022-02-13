/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_ImageBCG::init()
{
    pushOptions->setPixmap(KSquirrel::loader()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall));
    pixmapA->setPixmap(QPixmap(locate("appdata", "images/imageedit/squirrels/squirrel_colorize.png")));
    pixmapA->setPaletteBackgroundColor(pixmapA->colorGroup().background().light(90));

    QPixmap p = QPixmap::fromMimeSource(locate("appdata", "images/imageedit/reset_value.png"));
/*
    pushResetB->setFixedWidth(pushResetB->height());
    pushResetC->setFixedWidth(pushResetC->height());
    pushResetG->setFixedWidth(pushResetG->height());
    pushResetRed->setFixedWidth(pushResetRed->height());
    pushResetGreen->setFixedWidth(pushResetGreen->height());
    pushResetBlue->setFixedWidth(pushResetBlue->height());
*/
    sQ_LabelB->setText(tr2i18n("Brightness"));
    sQ_LabelC->setText(tr2i18n("Contrast"));
    sQ_LabelG->setText(tr2i18n("Gamma"));
    sQ_LabelRed->setText(tr2i18n("Red"));
    sQ_LabelGreen->setText(tr2i18n("Green"));
    sQ_LabelBlue->setText(tr2i18n("Blue"));

    pushResetB->setPixmap(p);
    pushResetC->setPixmap(p);
    pushResetG->setPixmap(p);
    pushResetRed->setPixmap(p);
    pushResetGreen->setPixmap(p);
    pushResetBlue->setPixmap(p);

    strings.append(QString("<b>") + tr2i18n("Brightness") + ",&nbsp;" + tr2i18n("Contrast") + ",&nbsp;" + tr2i18n("Gamma") + "</b>");
    strings.append(QString("<b>") + tr2i18n("Red") + ",&nbsp;" + tr2i18n("Green") + ",&nbsp;" + tr2i18n("Blue") + "</b>");

    id = 0;
    widgetStackParams->raiseWidget(id);
    text->setText(strings[id]);

    QPixmap tool1 = QPixmap::fromMimeSource(locate("appdata", "images/imageedit/resize_toolbutton.png"));    
    QPixmap tool2 = QPixmap::fromMimeSource(locate("appdata", "images/imageedit/resize_toolbutton2.png"));
    push1->setPixmap(tool1);
    push2->setPixmap(tool2);

    widgetStackParams->setPaletteBackgroundColor(widgetStackParams->colorGroup().highlight().light(110));

    imageopt.putto = SQ_Config::instance()->readEntry("Image edit options", "bcg_putto", QString::null);
    imageopt.prefix = SQ_Config::instance()->readEntry("Image edit options", "bcg_prefix", QString::null);
    imageopt.where_to_put = SQ_Config::instance()->readNumEntry("Image edit options", "bcg_where_to_put", 0);
    imageopt.close = SQ_Config::instance()->readBoolEntry("Image edit options", "bcg_close", true);

    spinG->setRange(0, 6.0, 0.01, 2);
    spinG->setValue(1.0);
    connect(spinG, SIGNAL(valueChanged(int)), spinG, SLOT(slotValueChanged(int)));

    sample.load(locate("appdata", "images/imageedit/edit_sample.png"));

    if(sample.isNull())
	return;

    sample = sample.convertDepth(32);
    sample.setAlphaBuffer(false);
    sample_saved = sample.copy();

    if(sample.width() != sample.height())
    {
	qWarning("SQ_ImageRotate::init: sample's w & h are not equal");
	sample = sample_saved = QImage();
    }

    p.convertFromImage(sample_saved);
    pixmap->setPixmap(p);
    pixmap1->setPixmap(p);
    
    done = true;
}

void SQ_ImageBCG::slotDebugText(const QString &text, bool bold)
{
    textResult->setBold(bold);
    textResult->insertAt(text, paragraph, textResult->paragraphLength(paragraph));
    textResult->setBold(false);
}

void SQ_ImageBCG::slotOptions()
{
    SQ_ImageEditOptions *o = new SQ_ImageEditOptions(this);

    if(o->exec(&imageopt) == QDialog::Accepted)
    {
	SQ_Config::instance()->setGroup("Image edit options");
	SQ_Config::instance()->writeEntry("bcg_putto", imageopt.putto);
	SQ_Config::instance()->writeEntry("bcg_prefix", imageopt.prefix);
	SQ_Config::instance()->writeEntry("bcg_where_to_put", imageopt.where_to_put);
	SQ_Config::instance()->writeEntry("bcg_close", imageopt.close);
    }
}

void SQ_ImageBCG::slotResetG()
{
    sliderG->setValue(100);
}

void SQ_ImageBCG::slotResetC()
{
    sliderC->setValue(0);
}

void SQ_ImageBCG::slotResetB()
{
    sliderB->setValue(0);
}

void SQ_ImageBCG::slotResetRed()
{
    sliderRed->setValue(0);
}

void SQ_ImageBCG::slotResetGreen()
{
    sliderGreen->setValue(0);
}

void SQ_ImageBCG::slotResetBlue()
{
    sliderBlue->setValue(0);
}

void SQ_ImageBCG::slotStartBCG()
{
    pushGO->setDefault(false);
    pushGO->setDisabled(true);
    pushCancel->setDefault(true);
    pushCancel->setFocus();
    widgetStack->raiseWidget(1);

    qApp->processEvents();

    SQ_ImageBCGOptions opt;
    opt.b = sliderB->value();
    opt.c = sliderC->value();
    opt.g = sliderG->value();
    opt.red = sliderRed->value();
    opt.green = sliderGreen->value();
    opt.blue = sliderBlue->value();

    emit bcg(&imageopt, &opt);
}

void SQ_ImageBCG::startBCG(int count)
{
    done = false;
    paragraph = 0;
    progress->setProgress(0);
    progress->setTotalSteps(count);
}

void SQ_ImageBCG::slotOneProcessed()
{
    progress->advance(1);
    paragraph++;
}

int SQ_ImageBCG::RGB2Y(int r, int g, int b)
{
    return (int)((double)r * 0.299 + (double)g * 0.587 + (double)b * 0.114);
}

void SQ_ImageBCG::slotBChanged(int v)
{
    spinB->blockSignals(true);
    sliderB->blockSignals(true);
    spinB->setValue(v);
    sliderB->setValue(v);
    spinB->blockSignals(false);
    sliderB->blockSignals(false);

    changeImage(v, sliderC->value(), sliderG->value(), sliderRed->value(), sliderGreen->value(), sliderBlue->value());
}

void SQ_ImageBCG::slotCChanged(int v)
{
    spinC->blockSignals(true);
    sliderC->blockSignals(true);
    spinC->setValue(v);
    sliderC->setValue(v);
    spinC->blockSignals(false);
    sliderC->blockSignals(false);

    changeImage(sliderB->value(), v, sliderG->value(), sliderRed->value(), sliderGreen->value(), sliderBlue->value());
}

void SQ_ImageBCG::slotRedChanged(int v)
{
    spinRed->blockSignals(true);
    sliderRed->blockSignals(true);
    spinRed->setValue(v);
    sliderRed->setValue(v);
    spinRed->blockSignals(false);
    sliderRed->blockSignals(false);

    changeImage(sliderB->value(), sliderC->value(), sliderG->value(), v, sliderGreen->value(), sliderBlue->value());
}

void SQ_ImageBCG::slotGreenChanged(int v)
{
    spinGreen->blockSignals(true);
    sliderGreen->blockSignals(true);
    spinGreen->setValue(v);
    sliderGreen->setValue(v);
    spinGreen->blockSignals(false);
    sliderGreen->blockSignals(false);

    changeImage(sliderB->value(), sliderC->value(), sliderG->value(), sliderRed->value(), v, sliderBlue->value());
}

void SQ_ImageBCG::slotBlueChanged(int v)
{
    spinBlue->blockSignals(true);
    sliderBlue->blockSignals(true);
    spinBlue->setValue(v);
    sliderBlue->setValue(v);
    spinBlue->blockSignals(false);
    sliderBlue->blockSignals(false);

    changeImage(sliderB->value(), sliderC->value(), sliderG->value(), sliderRed->value(), sliderGreen->value(), v);
}

void SQ_ImageBCG::slotGChanged(int v)
{
    double g = (double)v / 100.0;
    spinG->blockSignals(true);
    spinG->setValue(g);
    spinG->blockSignals(false);

    changeImage(sliderB->value(), sliderC->value(), v, sliderRed->value(), sliderGreen->value(), sliderBlue->value());
}

void SQ_ImageBCG::slotGSpinChanged(double v)
{
    int val = (int)(v * 100.0);
    sliderG->blockSignals(true);
    sliderG->setValue(val);
    sliderG->blockSignals(false);

    changeImage(sliderB->value(), sliderC->value(), val, sliderRed->value(), sliderGreen->value(), sliderBlue->value());
}

void SQ_ImageBCG::assignNewImage(const QImage &im)
{
    QPixmap p;
    p.convertFromImage(im);
    pixmap1->setPixmap(p);
}

void SQ_ImageBCG::changeImage(int b, int c, int g1, int red, int green, int blue)
{
    if(sample.isNull() || sample_saved.isNull())
	return;

    double g = (double)g1 / 100.0;

    sample = sample_saved.copy();

    // change brightness
    fmt_filters::brightness((RGBA *)sample.bits(), sample.width(), sample.height(), b);

    //change contrast
    if(c)
	fmt_filters::contrast((RGBA *)sample.bits(), sample.width(), sample.height(), c);

    // change gamma
    if(g1 != 100)
	fmt_filters::gamma((RGBA *)sample.bits(), sample.width(), sample.height(), g);

    if(red || green || blue)
	fmt_filters::colorize((RGBA *)sample.bits(), sample.width(), sample.height(), blue, green, red);

    assignNewImage(sample);
}

void SQ_ImageBCG::slotDone(bool close)
{
    done = true;
    
    if(close)
	reject();
}

void SQ_ImageBCG::slotReject()
{
    if(done)
	reject();
}

void SQ_ImageBCG::slotPush()
{
    if(!id)	id = 1;
    else 	id = 0;

    widgetStackParams->raiseWidget(id);
    text->setText(strings[id]);
}

void SQ_ImageBCG::closeEvent(QCloseEvent *e)
{
    if(done)
	e->accept();
    else
    {
	e->ignore();
	QWhatsThis::display(tr2i18n("Editing process is not finished yet"));
    }
}
