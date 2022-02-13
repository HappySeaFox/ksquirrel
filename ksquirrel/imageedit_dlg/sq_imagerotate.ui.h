/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_ImageRotate::init()
{
    pixmapA->setPixmap(QPixmap(locate("appdata", "images/imageedit/squirrels/squirrel_rotate.png")));
    pixmapA->setPaletteBackgroundColor(pixmapA->colorGroup().background().light(90));
    pushOptions->setPixmap(KSquirrel::loader()->loadIcon("configure", KIcon::Desktop, 16));
    pushRotateL->setPixmap(locate("appdata", "images/imageedit/rotateL.png"));
    pushRotateR->setPixmap(locate("appdata", "images/imageedit/rotateR.png"));
    pushFlipV->setPixmap(locate("appdata", "images/imageedit/flipV.png"));
    pushFlipH->setPixmap(locate("appdata", "images/imageedit/flipH.png"));
    pushReset->setPixmap(locate("appdata", "images/imageedit/reset_value.png"));

    imageopt.putto = SQ_Config::instance()->readEntry("Image edit options", "rotate_putto", QString::null);
    imageopt.prefix = SQ_Config::instance()->readEntry("Image edit options", "rotate_prefix", QString::null);
    imageopt.where_to_put = SQ_Config::instance()->readNumEntry("Image edit options", "rotate_where_to_put", 0);
    imageopt.close = SQ_Config::instance()->readBoolEntry("Image edit options", "rotate_close", true);

    sample.load(locate("appdata", "images/imageedit/edit_sample.png"));
    
    if(sample.isNull())
	return;

    sample = sample.convertDepth(32);
    sample_saved = sample.copy();

    if(sample.width() != sample.height())
    {
	qWarning("SQ_ImageRotate::init: sample's w & h are not equal");
	sample = sample_saved = QImage();
    }

    slotReset();
    
    done = true;
}

void SQ_ImageRotate::slotStartRotate()
{
    pushRotate->setDefault(false);
    pushRotate->setDisabled(true);
    pushCancel->setDefault(true);
    pushCancel->setFocus();
    widgetStack->raiseWidget(1);
    
    qApp->processEvents();
    
    SQ_ImageRotateOptions ropt;

    ropt.angle = angle;
    ropt.flipv = flipv;
    ropt.fliph = fliph;
    
    if(flipv && fliph)
    {
	if(angle == 180)
	{
	    ropt.angle = 0;
	    ropt.flipv = false;
	    ropt.fliph = false;
	}
	else if(angle == 90)
	{
	    ropt.angle = 270;
	    ropt.flipv = false;
	    ropt.fliph = false;
	}
	else if(angle == 270)
	{
	    ropt.angle = 90;
	    ropt.flipv = false;
	    ropt.fliph = false;
	}
    }

    emit rotate(&imageopt, &ropt);
}

void SQ_ImageRotate::slotDebugText(const QString &text, bool bold)
{
    textResult->setBold(bold);
    textResult->insertAt(text, paragraph, textResult->paragraphLength(paragraph));
    textResult->setBold(false);
}

void SQ_ImageRotate::startRotation(int count)
{
    done = false;
    paragraph = 0;
    progress->setProgress(0);
    progress->setTotalSteps(count);
}

void SQ_ImageRotate::slotOneProcessed()
{
    progress->advance(1);
    paragraph++;
}

void SQ_ImageRotate::slotReset()
{
    flipv = fliph = false;
    angle = 0;

    updateAngle();
    updateFlipping();

    sample = sample_saved.copy();

    setImage(sample);
}

void SQ_ImageRotate::slotFlipSampleV()
{
    if(sample.isNull()) return;

    fmt_utils::flipv((s8 *)sample.bits(), sample.bytesPerLine(), sample.height());
    
    flipv = !flipv;
    updateFlipping();

    setImage(sample);
}

void SQ_ImageRotate::slotFlipSampleH()
{
    if(sample.isNull()) return;

    fmt_utils::fliph((s8 *)sample.bits(), sample.width(), sample.height(), 4);

    fliph = !fliph;
    updateFlipping();

    setImage(sample);
}

void SQ_ImageRotate::slotRotateSampleL()
{
    rotateImage(false);

    angle -= 90;
    
    if(angle < 0)
    {
	angle = -angle;

	if(angle == 360) angle = 0;
	else angle = 360 - angle;
    }
    
    updateAngle();
}

void SQ_ImageRotate::slotRotateSampleR()
{
    rotateImage(true);

    angle += 90;
    if(angle == 360) angle = 0;
    updateAngle();
}

void SQ_ImageRotate::setImage(const QImage &im)
{
    if(im.isNull())
	return;

    QPixmap pix;
    pix.convertFromImage(im);
    pixmap->setPixmap(pix);
}

void SQ_ImageRotate::rotateImage(bool right)
{
    if(sample.isNull()) return;

    QImage res(sample.width(), sample.height(), 32);

    if(res.isNull())
    {
	qWarning("SQ_ImageRotate::slotRotateSample: temporary image is not created");
	return;
    }

    unsigned int *line, *line2;
    int x2;

    for(int y = 0;y < sample.height();++y)
    {
	line = (unsigned int *)sample.scanLine(y);
	x2 = (right) ? (sample.height() - y - 1) : y;

	if(right)
	    for(int x = 0;x < sample.height();++x)
	    {
	        line2 = (unsigned int *)res.scanLine(x);
	        *(line2 + x2) = *(line+x);
	    }
	else
	    for(int x = sample.height()-1, x3 = 0;x > 0;--x,++x3)
	    {
	         line2 = (unsigned int *)res.scanLine(x);
	         *(line2 + x2) = *(line+x3);
	    }
    }

    sample = res.copy();
    sample.setAlphaBuffer(true);

    setImage(sample);
}

void SQ_ImageRotate::slotOptions()
{
    SQ_ImageEditOptions *o = new SQ_ImageEditOptions(this);

    if(o->exec(&imageopt) == QDialog::Accepted)
    {
	SQ_Config::instance()->setGroup("Image edit options");
	SQ_Config::instance()->writeEntry("rotate_putto", imageopt.putto);
	SQ_Config::instance()->writeEntry("rotate_prefix", imageopt.prefix);
	SQ_Config::instance()->writeEntry("rotate_where_to_put", imageopt.where_to_put);
	SQ_Config::instance()->writeEntry("rotate_close", imageopt.close);
    }
}

void SQ_ImageRotate::updateAngle()
{
    textAngle->setText(QString::fromLatin1("%1").arg(angle));
}

void SQ_ImageRotate::updateFlipping()
{
    textFlipping->setText(QString::fromLatin1("%1%2").arg(flipv?"V":"").arg(fliph?"H":""));
}

void SQ_ImageRotate::slotDone(bool close)
{
    done = true;
    
    if(close)
	reject();
}

void SQ_ImageRotate::slotReject()
{
    if(done)
	reject();
}
