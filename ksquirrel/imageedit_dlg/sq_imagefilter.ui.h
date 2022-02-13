/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
void SQ_ImageFilter::init()
{
    pushOptions->setPixmap(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall));
    pixmapA->setPixmap(QPixmap(locate("appdata", "images/imageedit/squirrels/squirrel_filter.png")));
    pixmapA->setPaletteBackgroundColor(pixmapA->colorGroup().background().light(90));

    imageopt.putto = SQ_Config::instance()->readEntry("Image edit options", "filter_putto", QString::null);
    imageopt.where_to_put = SQ_Config::instance()->readNumEntry("Image edit options", "filter_where_to_put", 0);
    imageopt.close = SQ_Config::instance()->readBoolEntry("Image edit options", "filter_close", true);

    checkDontShow->setChecked(SQ_Config::instance()->readBoolEntry("Image edit options", "filter_dontshowhelp", false));

    // restore NumInputs...
    const double sigmaMax = 99.9, sigmaMin = 0.01;
    const double radiusMax = 99.9, radiusMin = 0.01;

    blendOpacity->setRange(0.0f, 1.0f, 0.01, true);
    fadeValue->setRange(0.0f, 1.0f, 0.01f, true);
    desaturateValue->setRange(0.0f, 1.0f, 0.01, true);
    thresholdValue->setRange(0, 255, 1, true);
    solarizeValue->setRange(0.0f, 50.0f, 0.01f, true);
    spreadValue->setRange(1, 10, 1, true);
    swirlAngle->setRange(-720.0f, 720.0f, 0.1f, true);
    waveAmpl->setRange(0.01f, 50.0f, 0.01f, true);
    waveLength->setRange(0.01f, 360.0f, 0.01f, true);
    shadeAzim->setRange(0.0f, 90.0f, 0.01f, true);
    shadeElev->setRange(0.0f, 90.0f, 0.01f, true);
    blurRadius->setRange(radiusMin, radiusMax, 0.01f, true);
    blurSigma->setRange(sigmaMin, 50.0, 0.01f, true);
    implodeFactor->setRange(0, 100, 1, true);
    edgeRadius->setRange(radiusMin, 30.0, 0.01f, true);
    embossRadius->setRange(radiusMin, radiusMax, 0.01f, true);
    embossSigma->setRange(sigmaMin, sigmaMax, 0.01f, true);
    sharpenRadius->setRange(radiusMin, radiusMax, 0.01f, true);
    sharpenSigma->setRange(sigmaMin, 30.0, 0.01f, true);
    oilRadius->setRange(1.0, 5.0, 1, true);

    buttonGroupSwapRGB->setButton(SQ_Config::instance()->readNumEntry("Image edit options", "filter_swapRGB", 0));

    // blend
    QColor c;
    c.setNamedColor(SQ_Config::instance()->readEntry("Image edit options", "filter_blend_color", "#00ff00"));
    pushBlendColor->setColor(c);
    blendOpacity->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_blend_opacity", 0.5));

    // fade
    c.setNamedColor(SQ_Config::instance()->readEntry("Image edit options", "filter_flend_color", "#00ff00"));
    fadeColor->setColor(c);
    fadeValue->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_fade_value", 0.5));

    // desaturate
    desaturateValue->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_desaturate_value", 0.5));

    // threshold
    thresholdValue->setValue(SQ_Config::instance()->readNumEntry("Image edit options", "filter_threshold_value", 1));

    // solarize
    solarizeValue->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_solarize_value", 0.5));

    // spread
    spreadValue->setValue(SQ_Config::instance()->readNumEntry("Image edit options", "filter_spread_value", 1));

    // swirl
    swirlAngle->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_swirl_value", 0.0));

    // noise
    buttonGroupNoise->setButton(SQ_Config::instance()->readNumEntry("Image edit options", "filter_noise", 0));

    // flatten
    c.setNamedColor(SQ_Config::instance()->readEntry("Image edit options", "filter_flatten_color1", "#00ff00"));
    flattenColor1->setColor(c);
    c.setNamedColor(SQ_Config::instance()->readEntry("Image edit options", "filter_flatten_color2", "#00ff00"));
    flattenColor2->setColor(c);

    // wave
    waveAmpl->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_wave_ampl", 0.1));
    waveLength->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_wave_length", 0.1));

    // shade
    shadeAzim->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_shade_azim", 0.1));
    shadeElev->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_shade_elev", 0.1));
    if(SQ_Config::instance()->readBoolEntry("Image edit options", "filter_shade_color", false))
	shadeColor->toggle();

    // blur
    blurRadius->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_blur_radius", 0.1));
    blurSigma->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_blur_sigma", 0.1));
    
    // implode
    implodeFactor->setValue(SQ_Config::instance()->readNumEntry("Image edit options", "filter_implode_factor", 1));

    // edge
    edgeRadius->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_egde_radius", 0.1));

    // emboss
    embossRadius->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_emboss_radius", 0.1));
    embossSigma->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_emboss_sigma", 0.1));

    // sharpen
    sharpenRadius->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_sharpen_radius", 0.1));
    sharpenSigma->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_sharpen_sigma", 0.1));

    // oil
    oilRadius->setValue(SQ_Config::instance()->readDoubleNumEntry("Image edit options", "filter_oil_radius", 0.1));
    
    if(checkDontShow->isChecked())
	slotNext();

    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_blend.png")), i18n("Blend"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_blur.png")), i18n("Blur"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_desaturate.png")), i18n("Disaturate"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_despeckle.png")), i18n("Despeckle"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_edge.png")), i18n("Edge"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_emboss.png")), i18n("Emboss"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_equalize.png")), i18n("Equalize"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_fade.png")), i18n("Fade"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_flatten.png")), i18n("Flatten"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_implode.png")), i18n("Implode"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_negative.png")), i18n("Negative"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_noise.png")), i18n("Noise"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_oil.png")), i18n("Oil"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_shade.png")), i18n("Shade"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_sharpen.png")), i18n("Sharpen"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_solarize.png")), i18n("Solarize"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_spread.png")), i18n("Spread"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_swaprgb.png")), i18n("Swap RGB"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_swirl.png")), i18n("Swirl"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_threshold.png")), i18n("Threshold"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_togray.png")), i18n("Gray"));
    (void)new SQ_IconListItem(listMain, QPixmap(locate("appdata", "images/imageedit/filter_wave.png")), i18n("Wave"));

    listMain->updateAndInstall(this);

    connect(listMain, SIGNAL(selectionChanged()), this, SLOT(slotShowPage()));

    initPreviewImage();

    done = true;
}

void SQ_ImageFilter::slotDebugText(const QString &text, bool bold)
{
    textResult->setBold(bold);
    textResult->insertAt(text, paragraph, textResult->paragraphLength(paragraph));
    textResult->setBold(false);
}

void SQ_ImageFilter::slotOptions()
{
    SQ_ImageEditOptions *o = new SQ_ImageEditOptions(this);

    // SQ_ImageEditOptions will write needed KConfig entries, if
    // exec() will return QDialog::Accepted
    o->setConfigPrefix("filter");

    o->exec(&imageopt);
}

void SQ_ImageFilter::slotStartFiltering()
{
    pushFilter->setDefault(false);
    pushFilter->setDisabled(true);
    pushCancel->setDefault(true);
    pushCancel->setFocus();
    widgetStack->raiseWidget(1);

    qApp->processEvents();

    SQ_Config *kconf = SQ_Config::instance();

    kconf->setGroup("Image edit options");
    kconf->writeEntry("filter_swapRGB", buttonGroupSwapRGB->selectedId());
    kconf->writeEntry("filter_blend_color", pushBlendColor->color().name());
    kconf->writeEntry("filter_blend_opacity", blendOpacity->value());
    kconf->writeEntry("filter_fade_color", fadeColor->color().name());
    kconf->writeEntry("filter_fade_value", fadeValue->value());
    kconf->writeEntry("filter_desaturate_value", desaturateValue->value());
    kconf->writeEntry("filter_threshold_value", thresholdValue->value());
    kconf->writeEntry("filter_solarize_value", solarizeValue->value());
    kconf->writeEntry("filter_spread_value", spreadValue->value());
    kconf->writeEntry("filter_swirl_value", swirlAngle->value());
    kconf->writeEntry("filter_noise", buttonGroupNoise->selectedId());
    kconf->writeEntry("filter_flatten_color1", flattenColor1->color().name());
    kconf->writeEntry("filter_flatten_color2", flattenColor2->color().name());
    kconf->writeEntry("filter_wave_ampl", waveAmpl->value());
    kconf->writeEntry("filter_wave_length", waveLength->value());
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
//    kconf->writeEntry("filter_", ->value());

    SQ_ImageFilterOptions opt;
 
    opt.type = listMain->currentItem();

    switch(opt.type)
    {
    case 0:
	{
	     QColor c = pushBlendColor->color();
	     opt.rgb1 = fmt_filters::rgb(c.red(), c.green(), c.blue());
	     opt._float = blendOpacity->value();
	}
	break;
    case 1: opt._double1 = blurRadius->value(); opt._double2 = blurSigma->value(); break;
    case 2: opt._float = (float)desaturateValue->value(); break;
    case 3: break;
    case 4: opt._double1 = edgeRadius->value(); break;
    case 5: opt._double1 = embossRadius->value(); opt._double2 = embossSigma->value(); break;
    case 6: break;
    case 7: 
	{
	     QColor c = fadeColor->color();
	     opt.rgb1 = fmt_filters::rgb(c.red(), c.green(), c.blue());
	     opt._float = (float)fadeValue->value();
	}
	break;
    case 8: 
	{
	     QColor c = flattenColor1->color();
	     opt.rgb1 = fmt_filters::rgb(c.red(), c.green(), c.blue());
	     c = flattenColor2->color();
	     opt.rgb2 = fmt_filters::rgb(c.red(), c.green(), c.blue());
	}
	break;
    case 9: opt._double1 = implodeFactor->value(); break;
    case 10: break;
    case 11: opt._uint = buttonGroupNoise->selectedId(); break;
    case 12: opt._double1 = oilRadius->value(); break;
    case 13: opt._bool = shadeColor->isChecked(); opt._double1 = shadeAzim->value(); opt._double2 = shadeElev->value(); break;
    case 14: opt._double1 = sharpenRadius->value(); opt._double2 = sharpenSigma->value();break;
    case 15: opt._double1 = solarizeValue->value(); break;
    case 16: opt._uint = spreadValue->value(); break;
    case 17: opt._uint = buttonGroupSwapRGB->selectedId(); break;
    case 18: opt._double1 = swirlAngle->value(); break;
    case 19: opt._uint = thresholdValue->value(); break;
    case 20: break;
    case 21: opt._double1 = waveAmpl->value(); opt._double2 = waveLength->value(); break;
    }

    emit filter(&imageopt, &opt);
}

void SQ_ImageFilter::startFiltering(int count)
{
    done = false;
    paragraph = 0;
    progress->setProgress(0);
    progress->setTotalSteps(count);
}

void SQ_ImageFilter::slotOneProcessed()
{
    progress->advance(1);
    paragraph++;
}

void SQ_ImageFilter::slotDone(bool close)
{
    done = true;

    if(close)
	reject();
}

void SQ_ImageFilter::slotReject()
{
    if(done)
	reject();
}

void SQ_ImageFilter::closeEvent(QCloseEvent *e)
{
    if(done)
	e->accept();
    else
    {
	e->ignore();
	QWhatsThis::display(SQ_ErrorString::instance()->string(SQE_NOTFINISHED));
    }
}

void SQ_ImageFilter::slotNext()
{
    pushNext->setDefault(false);
    pushFilter->setDefault(true);
    pushFilter->setFocus();

    widgetStackWizard->raiseWidget(1);
}

void SQ_ImageFilter::slotBack()
{
    pushNext->setDefault(true);
    pushFilter->setDefault(false);

    widgetStackWizard->raiseWidget(0);
}

void SQ_ImageFilter::slotShowPage()
{
    int id = listMain->currentItem();

    widgetStackFilters->raiseWidget(id);
    
    switch(id)
    {
    case 0: blend();            break;
    case 1: blur();             break;
    case 2: desaturate();       break;
    case 3: despeckle();        break;
    case 4: edge();             break;
    case 5: emboss();           break;
    case 6: equalize();         break;
    case 7: fade();             break;
    case 8: flatten();          break;
    case 9: implode();         break;
    case 10: negative();        break;
    case 11: noise();           break;
    case 12: oil();             break;
    case 13: shade();           break;
    case 14: sharpen();         break;
    case 15: solarize();        break;
    case 16: spread();          break;
    case 17: swapRGB();         break;
    case 18: swirl();           break;
    case 19: threshold();       break;
    case 20: togray();          break;
    case 21: wave();            break;
    }

    qApp->processEvents();
}

void SQ_ImageFilter::initPreviewImage()
{
    QImage tmp;

    tmp.load(locate("appdata", "images/imageedit/edit_sample.png"));

    setPreviewImage(tmp);
}

void SQ_ImageFilter::setPreviewImage(const QImage &im)
{    
    if(im.isNull()) return;

    sample = im.convertDepth(32);
    sample.setAlphaBuffer(false);
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

void SQ_ImageFilter::wave()
{
    if(sample.isNull() || sample_saved.isNull())
	return;

    fmt_filters::rgba *n = NULL;
    s32 h;

    fmt_filters::image im(sample_saved.bits(), sample_saved.width(), sample_saved.height());

    fmt_filters::wave(im, waveAmpl->value(), waveLength->value(), fmt_filters::white, &h, &n);
    
    if(!n)
        return;

    QImage mm((u8 *)n, im.w, h, 32, 0, 0, QImage::LittleEndian);

    assignNewImage(mm);

    pixmap1->erase();
    pixmap1->update();

    delete [] n;
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

    fmt_filters::edge(im, edgeRadius->value());

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
