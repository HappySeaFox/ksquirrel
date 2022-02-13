/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

SQ_ImageFilter * SQ_ImageFilter::m_inst = 0;

void SQ_ImageFilter::init()
{
    m_inst = this;

    // restore NumInputs...
    const double sigmaMax = 99.9, sigmaMin = 0.01;
    const double radiusMax = 99.9, radiusMin = 0.01;

    blendOpacity->setRange(0.0f, 1.0f, 0.01, true);
    fadeValue->setRange(0.0f, 1.0f, 0.01f, true);
    desaturateValue->setRange(0.0f, 1.0f, 0.01, true);
    thresholdValue->setRange(0, 255, 1, true);
    thresholdRE->setRange(0, 255, 1, true);
    solarizeValue->setRange(0.0f, 50.0f, 0.01f, true);
    spreadValue->setRange(1, 10, 1, true);
    swirlAngle->setRange(-720.0f, 720.0f, 0.1f, true);
    shadeAzim->setRange(0.0f, 90.0f, 0.01f, true);
    shadeElev->setRange(0.0f, 90.0f, 0.01f, true);
    blurRadius->setRange(radiusMin, radiusMax, 0.01f, true);
    blurSigma->setRange(sigmaMin, 50.0, 0.01f, true);
    implodeFactor->setRange(0, 100, 1, true);
    edgeRadius->setRange((int)radiusMin, 30, 1, true);
    embossRadius->setRange(radiusMin, radiusMax, 0.01f, true);
    embossSigma->setRange(sigmaMin, sigmaMax, 0.01f, true);
    sharpenRadius->setRange(radiusMin, radiusMax, 0.01f, true);
    sharpenSigma->setRange(sigmaMin, 30.0, 0.01f, true);
    oilRadius->setRange(1, 5, 1, true);

    SQ_Config::instance()->setGroup("Image edit options");

    buttonGroupSwapRGB->setButton(SQ_Config::instance()->readNumEntry("filter_swapRGB", 0));
    // blend
    QColor c;
    c.setNamedColor(SQ_Config::instance()->readEntry("filter_blend_color", "#00ff00"));
    pushBlendColor->setColor(c);
    blendOpacity->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_blend_opacity", 0.5));

    // fade
    c.setNamedColor(SQ_Config::instance()->readEntry("filter_flend_color", "#00ff00"));
    fadeColor->setColor(c);
    fadeValue->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_fade_value", 0.5));

    // desaturate
    desaturateValue->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_desaturate_value", 0.5));

    // threshold
    thresholdValue->setValue(SQ_Config::instance()->readNumEntry("filter_threshold_value", 1));

    // threshold for redeye
    thresholdRE->setValue(SQ_Config::instance()->readNumEntry("filter_threshold_redeye", 10));

    // solarize
    solarizeValue->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_solarize_value", 0.5));

    // spread
    spreadValue->setValue(SQ_Config::instance()->readNumEntry("filter_spread_value", 1));

    // swirl
    swirlAngle->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_swirl_value", 0.0));

    // noise
    buttonGroupNoise->setButton(SQ_Config::instance()->readNumEntry("filter_noise", 0));

    // flatten
    c.setNamedColor(SQ_Config::instance()->readEntry("filter_flatten_color1", "#00ff00"));
    flattenColor1->setColor(c);
    c.setNamedColor(SQ_Config::instance()->readEntry("filter_flatten_color2", "#00ff00"));
    flattenColor2->setColor(c);

    // shade
    shadeAzim->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_shade_azim", 0.1));
    shadeElev->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_shade_elev", 0.1));

    if(SQ_Config::instance()->readBoolEntry("filter_shade_color", false))
        shadeColor->toggle();

    // blur
    blurRadius->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_blur_radius", 0.1));
    blurSigma->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_blur_sigma", 0.1));
    
    // implode
    implodeFactor->setValue(SQ_Config::instance()->readNumEntry("filter_implode_factor", 1));

    // edge
    edgeRadius->setValue(SQ_Config::instance()->readNumEntry("filter_egde_radius", 1));

    // emboss
    embossRadius->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_emboss_radius", 0.1));
    embossSigma->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_emboss_sigma", 0.1));

    // sharpen
    sharpenRadius->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_sharpen_radius", 0.1));
    sharpenSigma->setValue(SQ_Config::instance()->readDoubleNumEntry("filter_sharpen_sigma", 0.1));

    // oil
    oilRadius->setValue(SQ_Config::instance()->readNumEntry("filter_oil_radius", 1));

    listMain->setCurrentItem(SQ_Config::instance()->readNumEntry("filter_filter", 0));

    hackConnect();
}

void SQ_ImageFilter::slotStartFiltering()
{
    SQ_Config *kconf = SQ_Config::instance();

    kconf->setGroup("Image edit options");
    kconf->writeEntry("filter_swapRGB", buttonGroupSwapRGB->selectedId());
    kconf->writeEntry("filter_blend_color", pushBlendColor->color().name());
    kconf->writeEntry("filter_blend_opacity", blendOpacity->value());
    kconf->writeEntry("filter_fade_color", fadeColor->color().name());
    kconf->writeEntry("filter_fade_value", fadeValue->value());
    kconf->writeEntry("filter_desaturate_value", desaturateValue->value());
    kconf->writeEntry("filter_threshold_value", thresholdValue->value());
    kconf->writeEntry("filter_threshold_redeye", thresholdRE->value());
    kconf->writeEntry("filter_solarize_value", solarizeValue->value());
    kconf->writeEntry("filter_spread_value", spreadValue->value());
    kconf->writeEntry("filter_swirl_value", swirlAngle->value());
    kconf->writeEntry("filter_noise", buttonGroupNoise->selectedId());
    kconf->writeEntry("filter_flatten_color1", flattenColor1->color().name());
    kconf->writeEntry("filter_flatten_color2", flattenColor2->color().name());
    kconf->writeEntry("filter_shade_azim", shadeAzim->value());
    kconf->writeEntry("filter_shade_elev", shadeElev->value());
    kconf->writeEntry("filter_shade_color", shadeColor->isChecked());
    kconf->writeEntry("filter_blur_radius", blurRadius->value());
    kconf->writeEntry("filter_blur_sigma", blurSigma->value());
    kconf->writeEntry("filter_implode_factor", implodeFactor->value());
    kconf->writeEntry("filter_egde_radius", edgeRadius->value());
    kconf->writeEntry("filter_emboss_radius", embossRadius->value());
    kconf->writeEntry("filter_emboss_sigma", embossSigma->value());
    kconf->writeEntry("filter_sharpen_radius", sharpenRadius->value());
    kconf->writeEntry("filter_sharpen_sigma", sharpenSigma->value());
    kconf->writeEntry("filter_oil_radius", oilRadius->value());
    kconf->writeEntry("filter_filter", listMain->currentItem());

    SQ_ImageFilterOptions opt;
 
    opt.type = listMain->currentItem();

    switch(opt.type)
    {
        case F::fblend:
        {
             QColor c = pushBlendColor->color();
             opt.rgb1 = fmt_filters::rgb(c.red(), c.green(), c.blue());
             opt._float = blendOpacity->value();
        }
        break;
        case F::fblur: opt._double1 = blurRadius->value(); opt._double2 = blurSigma->value(); break;
        case F::fdesaturate: opt._float = (float)desaturateValue->value(); break;
        case F::fdespeckle: break;
        case F::fedge: opt._double1 = (double)edgeRadius->value(); break;
        case F::femboss: opt._double1 = embossRadius->value(); opt._double2 = embossSigma->value(); break;
        case F::fequalize: break;
        case F::ffade: 
        {
            QColor c = fadeColor->color();
            opt.rgb1 = fmt_filters::rgb(c.red(), c.green(), c.blue());
            opt._float = (float)fadeValue->value();
        }
        break;
        case F::fflatten: 
        {
             QColor c = flattenColor1->color();
             opt.rgb1 = fmt_filters::rgb(c.red(), c.green(), c.blue());
             c = flattenColor2->color();
             opt.rgb2 = fmt_filters::rgb(c.red(), c.green(), c.blue());
        }
        break;
        case F::fimplode: opt._double1 = implodeFactor->value(); break;
        case F::fnegative: break;
        case F::fnoise: opt._uint = buttonGroupNoise->selectedId(); break;
        case F::foil: opt._double1 = (double)oilRadius->value(); break;
        case F::fshade: opt._bool = shadeColor->isChecked(); opt._double1 = shadeAzim->value(); opt._double2 = shadeElev->value(); break;
        case F::fsharpen: opt._double1 = sharpenRadius->value(); opt._double2 = sharpenSigma->value();break;
        case F::fsolarize: opt._double1 = solarizeValue->value(); break;
        case F::fspread: opt._uint = spreadValue->value(); break;
        case F::fswapRGB: opt._uint = buttonGroupSwapRGB->selectedId(); break;
        case F::fswirl: opt._double1 = swirlAngle->value(); break;
        case F::fthreshold: opt._uint = thresholdValue->value(); break;
        case F::fgray: break;
        case F::fredeye: opt._uint = thresholdRE->value(); break;
    }

    emit filter(&opt);
}

void SQ_ImageFilter::slotShowPage()
{
    int id = listMain->currentItem();

    widgetStackFilters->raiseWidget(id);
    
    switch(id)
    {
        case F::fblend:       blend();      break;
        case F::fblur:        blur();       break;
        case F::fdesaturate:  desaturate(); break;
        case F::fdespeckle:   despeckle();  break;
        case F::fedge:        edge();       break;
        case F::femboss:      emboss();     break;
        case F::fequalize:    equalize();   break;
        case F::ffade:        fade();       break;
        case F::fflatten:     flatten();    break;
        case F::fimplode:     implode();    break;
        case F::fnegative:    negative();   break;
        case F::fnoise:       noise();      break;
        case F::foil:         oil();        break;
        case F::fshade:       shade();      break;
        case F::fsharpen:     sharpen();    break;
        case F::fsolarize:    solarize();   break;
        case F::fspread:      spread();     break;
        case F::fswapRGB:     swapRGB();    break;
        case F::fswirl:       swirl();      break;
        case F::fthreshold:   threshold();  break;
        case F::fgray:        togray();     break;
        case F::fredeye:   redeye();  break;
    }
}

void SQ_ImageFilter::setPreviewImage(const QImage &im)
{    
    if(im.isNull()) return;

    sample = im.copy();
    sample_saved = sample.copy();

    QPixmap p;

    p.convertFromImage(sample_saved);
    pixmap->setPixmap(p);
    pixmap1->setPixmap(p);

    slotShowPage();
}

void SQ_ImageFilter::assignNewImage(const QImage &im)
{
    QPixmap p;
    p.convertFromImage(im);
    pixmap1->setPixmap(p);
}

void SQ_ImageFilter::swapRGB()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    int id = buttonGroupSwapRGB->selectedId();

    if(id == -1)
        return;

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::swapRGB(im, (id ? fmt_filters::GBR : fmt_filters::BRG));

    assignNewImage(sample);
}

void SQ_ImageFilter::negative()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::negative(im);

    assignNewImage(sample);
}

void SQ_ImageFilter::blend()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    QColor c = pushBlendColor->color();

    fmt_filters::rgb rgb(c.red(), c.green(), c.blue());

    fmt_filters::blend(im, rgb, blendOpacity->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::fade()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    QColor c = fadeColor->color();

    fmt_filters::fade(im, fmt_filters::rgb(c.red(), c.green(), c.blue()), fadeValue->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::togray()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::gray(im);

    assignNewImage(sample);
}

void SQ_ImageFilter::desaturate()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::desaturate(im, desaturateValue->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::threshold()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::threshold(im, thresholdValue->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::solarize()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::solarize(im, solarizeValue->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::spread()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::spread(im, spreadValue->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::swirl()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::swirl(im, swirlAngle->value(), fmt_filters::rgba(255,255,255,255));

    assignNewImage(sample);
}

void SQ_ImageFilter::noise()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::noise(im, (fmt_filters::NoiseType)buttonGroupNoise->selectedId());

    assignNewImage(sample);
}

void SQ_ImageFilter::redeye()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::redeye(im, im.w, im.h, 0, 0, thresholdRE->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::flatten()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    QColor c1 = flattenColor1->color();
    QColor c2 = flattenColor2->color();

    fmt_filters::flatten(im, fmt_filters::rgb(c1.red(), c1.green(), c1.blue()), fmt_filters::rgb(c2.red(), c2.green(), c2.blue()));

    assignNewImage(sample);
}

void SQ_ImageFilter::shade()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::shade(im, shadeColor->isChecked(), shadeAzim->value(), shadeElev->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::equalize()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::equalize(im);

    assignNewImage(sample);
}

void SQ_ImageFilter::blur()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::blur(im, blurRadius->value(), blurSigma->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::despeckle()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::despeckle(im);

    assignNewImage(sample);
}

void SQ_ImageFilter::implode()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::implode(im, implodeFactor->value(), fmt_filters::white);

    assignNewImage(sample);
}

void SQ_ImageFilter::edge()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::edge(im, (double)edgeRadius->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::emboss()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::emboss(im, embossRadius->value(), embossSigma->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::sharpen()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::sharpen(im, sharpenRadius->value(), sharpenSigma->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::oil()
{
    if(sample.isNull() || sample_saved.isNull())
        return;

    sample = sample_saved.copy();

    fmt_filters::image im(sample.bits(), sample.width(), sample.height());

    fmt_filters::oil(im, oilRadius->value());

    assignNewImage(sample);
}

void SQ_ImageFilter::hackConnect()
{
    connect( blendOpacity, SIGNAL( valueChanged(double) ), this, SLOT( blend() ) );
    connect( pushBlendColor, SIGNAL( changed(const QColor&) ), this, SLOT( blend() ) );
    connect( blurRadius, SIGNAL( valueChanged(double) ), this, SLOT( blur() ) );
    connect( blurSigma, SIGNAL( valueChanged(double) ), this, SLOT( blur() ) );
    connect( desaturateValue, SIGNAL( valueChanged(double) ), this, SLOT( desaturate() ) );
    connect( edgeRadius, SIGNAL( valueChanged(int) ), this, SLOT( edge() ) );
    connect( embossRadius, SIGNAL( valueChanged(double) ), this, SLOT( emboss() ) );
    connect( embossSigma, SIGNAL( valueChanged(double) ), this, SLOT( emboss() ) );
    connect( fadeColor, SIGNAL( changed(const QColor&) ), this, SLOT( fade() ) );
    connect( fadeValue, SIGNAL( valueChanged(double) ), this, SLOT( fade() ) );
    connect( flattenColor1, SIGNAL( changed(const QColor&) ), this, SLOT( flatten() ) );
    connect( flattenColor2, SIGNAL( changed(const QColor&) ), this, SLOT( flatten() ) );
    connect( implodeFactor, SIGNAL( valueChanged(double) ), this, SLOT( implode() ) );
    connect( buttonGroupNoise, SIGNAL( clicked(int) ), this, SLOT( noise() ) );
    connect( oilRadius, SIGNAL( valueChanged(int) ), this, SLOT( oil() ) );
    connect( shadeAzim, SIGNAL( valueChanged(double) ), this, SLOT( shade() ) );
    connect( shadeColor, SIGNAL( toggled(bool) ), this, SLOT( shade() ) );
    connect( shadeElev, SIGNAL( valueChanged(double) ), this, SLOT( shade() ) );
    connect( sharpenRadius, SIGNAL( valueChanged(double) ), this, SLOT( sharpen() ) );
    connect( sharpenSigma, SIGNAL( valueChanged(double) ), this, SLOT( sharpen() ) );
    connect( solarizeValue, SIGNAL( valueChanged(double) ), this, SLOT( solarize() ) );
    connect( spreadValue, SIGNAL( valueChanged(int) ), this, SLOT( spread() ) );
    connect( buttonGroupSwapRGB, SIGNAL( clicked(int) ), this, SLOT( swapRGB() ) );
    connect( swirlAngle, SIGNAL( valueChanged(double) ), this, SLOT( swirl() ) );
    connect( thresholdValue, SIGNAL( valueChanged(int) ), this, SLOT( threshold() ) );
    connect( thresholdRE, SIGNAL( valueChanged(int) ), this, SLOT( redeye() ) );
}

SQ_ImageFilter* SQ_ImageFilter::instance()
{
    return m_inst;
}
