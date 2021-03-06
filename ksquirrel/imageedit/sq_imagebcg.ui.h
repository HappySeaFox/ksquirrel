/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

SQ_ImageBCG * SQ_ImageBCG::m_inst = 0;

void SQ_ImageBCG::init()
{
    m_inst = this;

    QPixmap p = QPixmap::fromMimeSource(locate("data", "images/imageedit/reset_value.png"));

    sQ_LabelB->setSingle(true);
    sQ_LabelC->setSingle(true);
    sQ_LabelG->setSingle(true);
    sQ_LabelB->setText(tr2i18n("Brightness"));
    sQ_LabelC->setText(tr2i18n("Contrast"));
    sQ_LabelG->setText(tr2i18n("Gamma"));
    sQ_LabelRed->setText(tr2i18n("Red"), tr2i18n("Cyan"));
    sQ_LabelGreen->setText(tr2i18n("Green"), tr2i18n("Magenta"));
    sQ_LabelBlue->setText(tr2i18n("Blue"), tr2i18n("Yellow"));

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

    QPixmap tool1 = QPixmap::fromMimeSource(locate("data", "images/imageedit/resize_toolbutton.png"));    
    QPixmap tool2 = QPixmap::fromMimeSource(locate("data", "images/imageedit/resize_toolbutton2.png"));
    push1->setPixmap(tool1);
    push2->setPixmap(tool2);

    spinG->setRange(0, 6.0, 0.01, 2);
    spinG->setValue(1.0);
    connect(spinG, SIGNAL(valueChanged(int)), spinG, SLOT(slotValueChanged(int)));
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
    SQ_ImageBCGOptions opt;
    opt.b = sliderB->value();
    opt.c = sliderC->value();
    opt.g = sliderG->value();
    opt.red = sliderRed->value();
    opt.green = sliderGreen->value();
    opt.blue = sliderBlue->value();

    emit bcg(&opt);
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
    fmt_filters::brightness(fmt_filters::image(sample.bits(), sample.width(), sample.height()), b);

    //change contrast
    if(c)
        fmt_filters::contrast(fmt_filters::image(sample.bits(), sample.width(), sample.height()), c);

    // change gamma
    if(g1 != 100)
        fmt_filters::gamma(fmt_filters::image(sample.bits(), sample.width(), sample.height()), g);

    if(red || green || blue)
        fmt_filters::colorize(fmt_filters::image(sample.bits(), sample.width(), sample.height()), blue, green, red);

    assignNewImage(sample);
}

void SQ_ImageBCG::slotPush()
{
    if(!id)    id = 1;
    else     id = 0;

    widgetStackParams->raiseWidget(id);
    text->setText(strings[id]);
}

void SQ_ImageBCG::setPreviewImage(const QImage &im)
{    
    if(im.isNull()) return;

    sample = im.copy();
    sample_saved = sample.copy();

    QPixmap p;

    p.convertFromImage(sample_saved);
    pixmap->setPixmap(p);
    pixmap1->setPixmap(p);

    changeImage(sliderB->value(), sliderC->value(), sliderG->value(), sliderRed->value(), sliderGreen->value(), sliderBlue->value());
}

SQ_ImageBCG* SQ_ImageBCG::instance()
{
    return m_inst;
}
