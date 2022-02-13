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
    sliderX->setValue(SQ_Config::instance()->readNumEntry("Image edit options", "print_X", 1));
    sliderY->setValue(SQ_Config::instance()->readNumEntry("Image edit options", "print_Y", 1));

    SQ_PrintPanelFrame *fr = (SQ_PrintPanelFrame *)qt_find_obj_child(printpanel, "QFrame", SQ_Config::instance()->readEntry("Image edit options", "print_alignment", QString::null));

    if(fr) printpanel->toggleCurrentFrameColor(fr);

    imageopt.close = SQ_Config::instance()->readBoolEntry("Image edit options", "print_close", true);
    checkClose->setChecked(imageopt.close);
    slotXYChanged(0);

    buttonGroupSetups->setButton(SQ_Config::instance()->readNumEntry("Image edit options", "print_layout", 0));
    widgetStackSetups->raiseWidget(buttonGroupSetups->selectedId());

    done = true;
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

    SQ_ImagePrintOptions pr;

    pr.type = buttonGroupSetups->selectedId();
    pr.in_x = sliderX->value();
    pr.in_y = sliderY->value();
    pr.align = printpanel->currentFrame();

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
	QWhatsThis::display(tr2i18n("Editing process is not finished yet"));
    }
}
