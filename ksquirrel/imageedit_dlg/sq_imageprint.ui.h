/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void SQ_ImagePrint::init()
{
    pixmap->setPixmap(QPixmap(locate("appdata", "images/imageedit/squirrels/squirrel_print.png")));
    pixmap->setPaletteBackgroundColor(pixmap->colorGroup().background().dark(115));

    SQ_Config::instance()->setGroup("Image edit options");

    sliderX->setValue(SQ_Config::instance()->readNumEntry("print_X", 1));
    sliderY->setValue(SQ_Config::instance()->readNumEntry("print_Y", 1));

    SQ_PrintPanelFrame *fr = (SQ_PrintPanelFrame *)qt_find_obj_child(printpanel, "QFrame", SQ_Config::instance()->readEntry("print_alignment", QString::null));

    if(fr) printpanel->toggleCurrentFrameColor(fr);

    checkClose->setChecked(SQ_Config::instance()->readBoolEntry("print_close", true));
    slotXYChanged(0);

    buttonGroupSetups->setButton(SQ_Config::instance()->readNumEntry("print_layout", 0));
    widgetStackSetups->raiseWidget(buttonGroupSetups->selectedId());

    checkDontShow->setChecked(SQ_Config::instance()->readBoolEntry("print_dontshowhelp", false));

    buttonGroupTR->setButton(SQ_Config::instance()->readNumEntry("print_transp", 0));

    QColor cc;
    cc.setNamedColor(SQ_Config::instance()->readEntry("print_transp_color", "#000000"));
    color->setColor(cc);

    done = true;

    if(checkDontShow->isChecked())
        slotNext2();
}

void SQ_ImagePrint::slotXYChanged(int /* val */)
{
    textXY->setText(QString::fromLatin1("%1x%2").arg(sliderX->value()).arg(sliderY->value()));
}

void SQ_ImagePrint::slotNext()
{
    pushNext->setDefault(false);
    pushNext->setDisabled(true);
    pushClose->setDefault(true);
    pushClose->setFocus();
    widgetStack->raiseWidget(1);

    SQ_Config::instance()->setGroup("Image edit options");
    SQ_Config::instance()->writeEntry("print_X", sliderX->value());
    SQ_Config::instance()->writeEntry("print_Y", sliderY->value());
    SQ_Config::instance()->writeEntry("print_alignment", printpanel->currentFrame());
    SQ_Config::instance()->writeEntry("print_close", checkClose->isChecked());
    SQ_Config::instance()->writeEntry("print_layout", buttonGroupSetups->selectedId());
    SQ_Config::instance()->writeEntry("print_transp", buttonGroupTR->selectedId());
    SQ_Config::instance()->writeEntry("print_transp_color", color->color().name());

    SQ_ImagePrintOptions pr;

    pr.type = buttonGroupSetups->selectedId();
    pr.in_x = sliderX->value();
    pr.in_y = sliderY->value();
    pr.align = printpanel->currentFrame();
    pr.transp =  buttonGroupTR->selectedId();
    pr.transp_color = color->color();
    imageopt.close = checkClose->isChecked();

    SQ_Config::instance()->writeEntry("print_dontshowhelp", checkDontShow->isChecked());

    emit print(&imageopt, &pr);
}

void SQ_ImagePrint::slotDebugText(const QString &text, bool bold)
{
    textResult->setBold(bold);
    textResult->insertAt(text, paragraph, textResult->paragraphLength(paragraph));
    textResult->setBold(false);
}

void SQ_ImagePrint::startPrinting(int count)
{
    done = false;
    paragraph = 0;
    progress->setProgress(0);
    progress->setTotalSteps(count);
}

void SQ_ImagePrint::slotOneProcessed()
{
    progress->advance(1);
    paragraph++;
}

void SQ_ImagePrint::slotDone()
{
    reject();
}

void SQ_ImagePrint::slotDone(bool close)
{
    done = true;
    
    if(close)
        reject();
}

void SQ_ImagePrint::slotReject()
{
    if(done)
        reject();
}

void SQ_ImagePrint::closeEvent(QCloseEvent *e)
{
    if(done)
        e->accept();
    else
    {
        e->ignore();
        QWhatsThis::display(SQ_ErrorString::instance()->string(SQE_NOTFINISHED));
    }
}

void SQ_ImagePrint::slotNext2()
{
    pushNext2->setDefault(false);
    pushNext->setDefault(true);
    pushNext->setFocus();

    widgetStackWizard->raiseWidget(1);
}

void SQ_ImagePrint::slotBack()
{
    pushNext2->setDefault(true);
    pushNext->setDefault(false);

    widgetStackWizard->raiseWidget(0);
}
