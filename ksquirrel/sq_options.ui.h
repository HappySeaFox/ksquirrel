/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_Options::init()
{
	int tp;
    
	checkMinimize->setChecked(SQ_Config::instance()->readBoolEntry("Main", "minimize to tray", true));
	checkSync->setChecked(SQ_Config::instance()->readBoolEntry("Main", "sync", true));

	checkMonitor->setChecked(SQ_Config::instance()->readBoolEntry("Libraries", "monitor", true));
	checkFAMMessage->setChecked(SQ_Config::instance()->readBoolEntry("Libraries", "show dialog", true));
   
	KFile::Mode mode = static_cast<KFile::Mode>(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);

	tp = SQ_Config::instance()->readNumEntry("Fileview", "sync type", 0);
	buttonGroupSync->setButton(tp);
	tp = SQ_Config::instance()->readNumEntry("Fileview", "set path", 0);
	buttonGroupSetPath->setButton(tp);
	kURLReqOpenCustom->setURL(SQ_Config::instance()->readEntry("Fileview", "custom directory", "/"));
	tp = SQ_Config::instance()->readNumEntry("Fileview", "click policy", 0);
	buttonGroupClick->setButton(tp);
	
	checkSaveHistory->setChecked(SQ_Config::instance()->readBoolEntry("Fileview", "history", true));
	checkRunUnknown->setChecked(SQ_Config::instance()->readBoolEntry("Fileview", "run unknown", true));
	checkSupportAr->setChecked(SQ_Config::instance()->readBoolEntry("Fileview", "archives", true));
	checkJumpFirst->setChecked(SQ_Config::instance()->readBoolEntry("Fileview", "tofirst", true));
	checkDisableDirs->setChecked(SQ_Config::instance()->readBoolEntry("Fileview", "disable_dirs", false));

// Init GLView page
	QPixmap p1;
	checkStatus->setChecked(SQ_Config::instance()->readBoolEntry("GL view", "hide_sbar", true));
	checkDrawQuads->setChecked(SQ_Config::instance()->readBoolEntry("GL view", "alpha_bkgr", true));	  checkMarks->setChecked(SQ_Config::instance()->readBoolEntry("GL view", "marks", true));    
	
	tp = SQ_Config::instance()->readNumEntry("GL view", "GL view background type", 0);
	buttonGroupColor->setButton(tp);
	widgetStack4->raiseWidget(tp);
	
	kColorSystem->setColor(colorGroup().color(QColorGroup::Base));
	
	tp = SQ_Config::instance()->readNumEntry("GL view", "zoom limit", 0);
	buttonGroupZoomLimit->setButton(tp);
	spinZoomMin->setValue(SQ_Config::instance()->readNumEntry("GL view", "zoom_min", 1));
	spinZoomMax->setValue(SQ_Config::instance()->readNumEntry("GL view", "zoom_max", 10000));

	custpixmap = SQ_Config::instance()->readEntry("GL view", "GL view custom texture", "");
	kURLReqCustomTexture->setURL(custpixmap);
	
	if(!custpixmap.isEmpty())
	    slotValidateCustomTexture();

	tp = SQ_Config::instance()->readNumEntry("GL view", "scroll", 0);
	buttonGroupScrolling->setButton(tp);
	sliderAngle->setValue(SQ_Config::instance()->readNumEntry("GL view", "angle", 90));
	spinZoomFactor->setValue(SQ_Config::instance()->readNumEntry("GL view", "zoom", 25));
	sliderMove->setValue(SQ_Config::instance()->readNumEntry("GL view", "move", 5));
    
	QColor color;
	color.setNamedColor(SQ_Config::instance()->readEntry("GL view", "GL view background", "#cccccc"));
	kColorGLbackground->setColor(color);
    
	spinMargin->setValue(SQ_Config::instance()->readNumEntry("Thumbnails", "margin", 2));
	spinCacheSize->setValue(SQ_Config::instance()->readNumEntry("Thumbnails", "cache", 1024*5));
	if(SQ_Config::instance()->readBoolEntry("Thumbnails", "disable_mime", true)) checkMime->toggle();
	if(SQ_Config::instance()->readBoolEntry("Thumbnails", "dont write", false)) checkNoWriteThumbs->toggle();
	if(SQ_Config::instance()->readBoolEntry("Thumbnails", "extended", false)) checkExtended->toggle();
	if(SQ_Config::instance()->readBoolEntry("Thumbnails", "tooltips", false)) checkTooltips->toggle();

	if(SQ_Config::instance()->readBoolEntry("Edit tools", "preview", false)) checkPreview->toggle();
	if(SQ_Config::instance()->readBoolEntry("Edit tools", "preview_dont", true)) checkDontGenerate->toggle();
	spinLargerW->setValue(SQ_Config::instance()->readNumEntry("Edit tools", "preview_larger_w", 1024));
	spinLargerH->setValue(SQ_Config::instance()->readNumEntry("Edit tools", "preview_larger_h", 768));
	if(SQ_Config::instance()->readBoolEntry("Edit tools", "multi", true)) checkMultiPaged->toggle();

	(void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("display", KIcon::Desktop, 32), i18n("Main"));	
	(void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, 32), i18n("Filing"));
	(void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("images", KIcon::Desktop, 32), i18n("Thumbnails"));
	(void)new SQ_IconListItem(listMain, QPixmap::fromMimeSource(locate("appdata", "images/listbox/image_win.png")), i18n("Image window"));
	(void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("binary", KIcon::Desktop, 32), i18n("Libraries"));
	(void)new SQ_IconListItem(listMain, SQ_IconLoader::instance()->loadIcon("edit", KIcon::Desktop, 32), i18n("Edit tools"));

	listMain->updateAndInstall(this);

	connect(listMain, SIGNAL(selectionChanged()), SLOT(slotShowPage()));
    
	kURLReqOpenCustom->setMode(mode);
	
	listGL->header()->hide();
	listGL->setSorting(-1);
	QListViewItem *item;
	QMap<QString, GLenum> v;

	v["GL_VENDOR"] = GL_VENDOR;
	v["GL_RENDERER"] = GL_RENDERER;
	v["GL_VERSION"] = GL_VERSION;

	for(QMap<QString, GLenum>::ConstIterator it = v.constBegin(); it != v.constEnd(); ++it)
	{
	    item = listGL->findItem(it.key().latin1(), 0);
	    if(item)
	    {
		const GLubyte *b = glGetString(it.data());
		if(b)
		{
		    QString s = (const char*)b;
		    item->setText(1, s);
		}   
	    }
	}

	fillAltCombo();

	QString lib = SQ_Config::instance()->readEntry("Edit tools", "altlibrary", "Portable Network Graphics");

	if(SQ_LibraryHandler::instance()->libraryByName(lib))
	    comboAlt->setCurrentText(lib);
	else if(comboAlt->count() > 1)
	    comboAlt->setCurrentItem(1);
}

int SQ_Options::start()
{
	int result = this->exec();

	if(result == QDialog::Accepted)
	{
		SQ_Config *kconf = SQ_Config::instance();

		kconf->setGroup("Fileview");
		kconf->writeEntry("set path", buttonGroupSetPath->selectedId());
		kconf->writeEntry("custom directory", kURLReqOpenCustom->url());
		kconf->writeEntry("sync type", buttonGroupSync->selectedId());
		kconf->writeEntry("click policy", buttonGroupClick->selectedId());
		kconf->writeEntry("history", checkSaveHistory->isChecked());
		kconf->writeEntry("run unknown", checkRunUnknown->isChecked());
		kconf->writeEntry("archives", checkSupportAr->isChecked());
		kconf->writeEntry("tofirst", checkJumpFirst->isChecked());
		kconf->writeEntry("disable_dirs", checkDisableDirs->isChecked());

		kconf->setGroup("Main");
		kconf->writeEntry("minimize to tray", checkMinimize->isChecked());
		kconf->writeEntry("sync", checkSync->isChecked());

		kconf->setGroup("Thumbnails");
		kconf->writeEntry("margin", spinMargin->value());
		kconf->writeEntry("cache", spinCacheSize->value());
		kconf->writeEntry("disable_mime", checkMime->isChecked());
		kconf->writeEntry("dont write", checkNoWriteThumbs->isChecked());
		kconf->writeEntry("extended", checkExtended->isChecked());
		kconf->writeEntry("tooltips", checkTooltips->isChecked());
		
		kconf->setGroup("GL view");
		kconf->writeEntry("GL view background", (kColorGLbackground->color()).name());
		kconf->writeEntry("GL view custom texture", custpixmap);
		kconf->writeEntry("GL view background type", buttonGroupColor->selectedId());
		kconf->writeEntry("zoom limit", buttonGroupZoomLimit->selectedId());
		kconf->writeEntry("alpha_bkgr", checkDrawQuads->isChecked());
		kconf->writeEntry("marks", checkMarks->isChecked());
		kconf->writeEntry("hide_sbar", checkStatus->isChecked());
		kconf->writeEntry("scroll", buttonGroupScrolling->selectedId());
		kconf->writeEntry("angle", sliderAngle->value());
		kconf->writeEntry("zoom", spinZoomFactor->value());
		kconf->writeEntry("zoom_min", spinZoomMin->value());
		kconf->writeEntry("zoom_max", spinZoomMax->value());
		kconf->writeEntry("move", sliderMove->value());

		kconf->setGroup("Libraries");
		kconf->writeEntry("monitor", checkMonitor->isChecked());
		kconf->writeEntry("show dialog", checkFAMMessage->isChecked());

		kconf->setGroup("Edit tools");
		kconf->writeEntry("preview", checkPreview->isChecked());
		kconf->writeEntry("preview_dont", checkDontGenerate->isChecked());
		kconf->writeEntry("preview_larger_w", spinLargerW->value());
		kconf->writeEntry("preview_larger_h", spinLargerH->value());
		kconf->writeEntry("altlibrary", comboAlt->currentText());
		kconf->writeEntry("multi", checkMultiPaged->isChecked());
	}

	return result;
}

QString SQ_Options::stringList2QString(QStringList list )
{
	QStringList::Iterator it = list.begin();
	QString out;
 
	while(it != list.end())
	{
		out = out + "*." + *it + " ";
		++it;
	}
    
	out += "*.jpg *.jpe";

	return out.lower();
}

bool SQ_Options::validPixmap(QPixmap pix)
{
	return (isPowerOf2(pix.width()) && isPowerOf2(pix.height()) && pix.width() <= 256 && pix.width() == pix.height() && !pix.isNull());
}

bool SQ_Options::isPowerOf2( int tested )
{
	int i = 1;
    
	if(tested == 1)
		return true;
    
	for(;tested > i;i <<= 1)
		;

	return i == tested;
}

void SQ_Options::slotNewCustomTexture( const QString & path)
{
	QPixmap p1(path);
    
	if(validPixmap(p1))
	{
		custpixmap = path;
		textCustomValidate->setText(QString::fromLatin1("%1x%2: OK")
					    .arg(p1.width())
					    .arg(p1.height()));
	    }
	else
	    textCustomValidate->setText(i18n(QString::fromLatin1("Wrong dimensions: %1x%2."))
					.arg(p1.width())
					.arg(p1.height()));
}

void SQ_Options::slotShowPage()
{
	int id = listMain->currentItem();
    
	widgetStack1->raiseWidget(id);
	widgetStackLines->raiseWidget(id);
}

void SQ_Options::slotCustomTextureToggled( bool en)
{
    kURLReqCustomTexture->setEnabled(en);
    groupBox3->setEnabled(en);
}

void SQ_Options::slotValidateCustomTexture()
{
    slotNewCustomTexture(kURLReqCustomTexture->url());
}

void SQ_Options::paletteChange( const QPalette &oldPalette )
{
    QDialog::paletteChange(oldPalette);

    kColorSystem->setColor(colorGroup().color(QColorGroup::Base));
}

void SQ_Options::slotSetSystemBack( const QColor & )
{
    kColorSystem->setColor(colorGroup().color(QColorGroup::Base));
}

void SQ_Options::fillAltCombo()
{
    QPixmap pix;

    // return anyway
    if(!SQ_LibraryHandler::instance()->count())
	return;

    QValueVector<SQ_LIBRARY>::iterator   BEGIN = SQ_LibraryHandler::instance()->begin();
    QValueVector<SQ_LIBRARY>::iterator      END = SQ_LibraryHandler::instance()->end();

    for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;++it)
    {
	if(pix.convertFromImage((*it).mime))
	{
	    if((*it).writable)
		comboAlt->insertItem(pix, (*it).quickinfo);
	}
    }
}
