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
    pixmap->setPixmap(QPixmap(locate("appdata", "images/imageedit/squirrels/squirrel_filter.png")));
    pixmap->setPaletteBackgroundColor(pixmap->colorGroup().background().light(90));

    imageopt.putto = SQ_Config::instance()->readEntry("Image edit options", "filter_putto", QString::null);
    imageopt.prefix = SQ_Config::instance()->readEntry("Image edit options", "filter_prefix", QString::null);
    imageopt.where_to_put = SQ_Config::instance()->readNumEntry("Image edit options", "filter_where_to_put", 0);
    imageopt.close = SQ_Config::instance()->readBoolEntry("Image edit options", "filter_close", true);

    checkDontShow->setChecked(SQ_Config::instance()->readBoolEntry("Image edit options", "filter_dontshowhelp", false));

    buttonGroupSwapRGB->setButton(SQ_Config::instance()->readNumEntry("Image edit options", "filter_swapRGB", 0));

    if(checkDontShow->isChecked())
	slotNext();

    listFilters->insertItem(QPixmap(locate("appdata", "images/imageedit/filter_color.png")), tr2i18n("Color changes"));
    listFilters->setCurrentItem(0);
    listFilters->adjustSize();

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

    SQ_Config::instance()->setGroup("Image edit options");
    SQ_Config::instance()->writeEntry("filter_swapRGB", buttonGroupSwapRGB->selectedId());

    SQ_ImageFilterOptions opt;
 
    opt.type = listFilters->currentItem();

    switch(opt.type)
    {
              case 0:
	  opt.subtype = tabColor->currentPageIndex();

	  if(opt.subtype == 1)
	      opt.sb.swaprgb.type = buttonGroupSwapRGB->selectedId();
	break;
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
