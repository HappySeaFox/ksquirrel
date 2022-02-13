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
    pixmapA->setPaletteBackgroundColor(pixmapA->colorGroup().background().dark(115));
    pushOptions->setPixmap(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall));
    pushRotateL->setPixmap(locate("appdata", "images/imageedit/rotateL.png"));
    pushRotateR->setPixmap(locate("appdata", "images/imageedit/rotateR.png"));
    pushFlipV->setPixmap(locate("appdata", "images/imageedit/flipV.png"));
    pushFlipH->setPixmap(locate("appdata", "images/imageedit/flipH.png"));
    pushReset->setPixmap(locate("appdata", "images/imageedit/reset_value.png"));

    SQ_Config::instance()->setGroup("Image edit options");

    imageopt.putto = SQ_Config::instance()->readEntry("rotate_putto", QString::null);
    imageopt.where_to_put = SQ_Config::instance()->readNumEntry("rotate_where_to_put", 0);
    imageopt.close = SQ_Config::instance()->readBoolEntry("rotate_close", true);

    checkDontShow->setChecked(SQ_Config::instance()->readBoolEntry("rotate_dontshowhelp", false));

    flipv = fliph = false;
    angle = 0;

    done = true;

    if(checkDontShow->isChecked())
        slotNext();
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

    SQ_Config::instance()->writeEntry("rotate_dontshowhelp", checkDontShow->isChecked());

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

    sample = right ? KImageEffect::rotate(sample, KImageEffect::Rotate90) : KImageEffect::rotate(sample, KImageEffect::Rotate270);

    setImage(sample);

    pixmap->erase();
    pixmap->update();
}

void SQ_ImageRotate::slotOptions()
{
    SQ_ImageEditOptions o(this);

    // SQ_ImageEditOptions will write needed KConfig entries, if
    // exec() will return QDialog::Accepted
    o.setConfigPrefix("rotate");

    o.exec(&imageopt);
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

void SQ_ImageRotate::closeEvent(QCloseEvent *e)
{
    if(done)
        e->accept();
    else
    {
        e->ignore();
        QWhatsThis::display(SQ_ErrorString::instance()->string(SQE_NOTFINISHED));
    }
}

void SQ_ImageRotate::slotNext()
{
    pushNext->setDefault(false);
    pushRotate->setDefault(true);
    pushRotate->setFocus();

    widgetStackWizard->raiseWidget(1);
}

void SQ_ImageRotate::slotBack()
{
    pushNext->setDefault(true);
    pushRotate->setDefault(false);

    widgetStackWizard->raiseWidget(0);
}

void SQ_ImageRotate::setPreviewImage(const QImage &im)
{    
    if(im.isNull()) return;

    sample = im.copy();
    sample_saved = sample.copy();

    QPixmap p;

    p.convertFromImage(sample_saved);
    pixmap->setPixmap(p);
}
