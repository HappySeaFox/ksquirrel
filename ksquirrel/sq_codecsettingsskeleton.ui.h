/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_CodecSettingsSkeleton::init()
{
	w = 0;
	sett = 0;
}

void SQ_CodecSettingsSkeleton::addSettingsWidget(const QString &path)
{
    w = QWidgetFactory::create(path, 0, this, "skeleton_settings");
    QWidget *fake;

    if(w)
        fake = w;
    else
	{
		pushApply->setEnabled(false);
		pushOK->setEnabled(false);

		QTextEdit *t = new QTextEdit(i18n("Error loading widget from <b>%1</b>. Please check your installation or contact <a href=\"mailto:ksquirrel@mail.ru\">ksquirrel@mail.ru</a>").arg(path), QString::null, groupBox);		
		t->setReadOnly(true);
		fake = t;
	}

    fake->reparent(groupBox, QPoint(0,0), true);

    QGridLayout *grid = new QGridLayout(groupBox, 1, 1, 11, 6);
    grid->addMultiCellWidget(fake, 1, 1, 0, 3);

    QSpacerItem *spacer = new QSpacerItem(15, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    grid->addItem(spacer, 2, 1);
}

void SQ_CodecSettingsSkeleton::recursivelyReadWrite(fmt_settings &settings, bool r)
{
    if(!w)
	return;

    QObjectList *ch = const_cast<QObjectList *>(w->children());
    fmt_settings::iterator t;

    for(QObjectList::iterator it = ch->begin();it != ch->end();++it)
    {
	t = settings.find((*it)->name());

	if((*it)->inherits("QCheckBox"))
	{
	    QCheckBox *c = dynamic_cast<QCheckBox *>(*it);

	    if(c && t != settings.end())
	    {
		if(r)
		    c->setChecked((*t).second.bVal);
		else
		    (*t).second.bVal = c->isChecked();
	    }
	}
	else if((*it)->inherits("QButtonGroup"))
	{
	    QButtonGroup *c = dynamic_cast<QButtonGroup *>(*it);

	    if(c && t != settings.end())
	    {
		if(r)
		    c->setButton((*t).second.iVal);
		else
		    (*t).second.iVal = c->selectedId();
	    }
	}
	else if((*it)->inherits("QSlider"))
	{
	    QSlider *c = dynamic_cast<QSlider *>(*it);

	    if(c && t != settings.end())
	    {
		if(r)
		    c->setValue((*t).second.iVal);
		else
		    (*t).second.iVal = c->value();
	    }
	}
	else if((*it)->inherits("KURLRequester"))
	{
	    KURLRequester *u = dynamic_cast<KURLRequester *>(*it);

	    if(u && t != settings.end())
	    {
		if(r)
		    u->setURL((*t).second.sVal);
		else
		{
                    KURL url = u->url(); // get rid of "file://" if present
		    (*t).second.sVal = url.isEmpty() ? "" : url.path().ascii();
		}
	    }
	}
	else if((*it)->inherits("KDoubleSpinBox"))
	{
	    KDoubleSpinBox *d = dynamic_cast<KDoubleSpinBox *>(*it);

	    if(d && t != settings.end())
	    {
		if(r)
		    d->setValue((*t).second.dVal);
		else
		    (*t).second.dVal = d->value();
	    }
	}
	// QSpinBox should be checked after KDoubleSpinBox !
	else if((*it)->inherits("QSpinBox"))
	{
	    QSpinBox *c = dynamic_cast<QSpinBox *>(*it);

	    if(c && t != settings.end())
	    {
		if(r)
		    c->setValue((*t).second.iVal);
		else
		    (*t).second.iVal = c->value();
	    }
        }
	else if((*it)->inherits("KColorButton"))
	{
	    KColorButton *c = dynamic_cast<KColorButton *>(*it);

	    if(c && t != settings.end())
	    {
		if(r)
		    c->setColor(QColor((*t).second.sVal));
		else
		    (*t).second.sVal = c->color().name().ascii();
	    }
	}
    }
}

int SQ_CodecSettingsSkeleton::exec(fmt_settings &settings)
{
    // read settings
    recursivelyReadWrite(settings, true);

    sett = &settings;

    int result = QDialog::exec();

    // save settings
    if(result == QDialog::Accepted)
	recursivelyReadWrite(settings, false);

    return result;
}

void SQ_CodecSettingsSkeleton::setCodecInfo( const QPixmap &pixmap, const QString &text )
{
	codecIcon->setPixmap(pixmap);
	codecName->setText(text);
}

void SQ_CodecSettingsSkeleton::slotApply()
{
	recursivelyReadWrite(*sett, false);
	emit apply();
}
