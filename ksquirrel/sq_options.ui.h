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
	QPixmap pixNF = QPixmap::fromMimeSource(locate("appdata", "images/libs_notfound.png"));
	pixmapNotFound->setPixmap(pixNF);
    
	checkMinimize->setChecked(SQ_Config::instance()->readBoolEntry("Main", "minimize to tray", true));
	checkSync->setChecked(SQ_Config::instance()->readBoolEntry("Main", "sync", true));
	checkCheck->setChecked(SQ_Config::instance()->readBoolEntry("Main", "check", true));
    
	checkMonitor->setChecked(SQ_Config::instance()->readBoolEntry("Libraries", "monitor", true));
	checkFAMMessage->setChecked(SQ_Config::instance()->readBoolEntry("Libraries", "show dialog", true));
   
	KFile::Mode mode = static_cast<KFile::Mode>(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);
	slotShowLibs();

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
    
	(void)new SQ_IconListItem(listMain, KSquirrel::loader()->loadIcon("display", KIcon::Desktop, 32), i18n("Main"));	
	(void)new SQ_IconListItem(listMain, KSquirrel::loader()->loadIcon("folder", KIcon::Desktop, 32), i18n("Filing"));
	(void)new SQ_IconListItem(listMain, KSquirrel::loader()->loadIcon("images", KIcon::Desktop, 32), i18n("Thumbnails"));
	(void)new SQ_IconListItem(listMain, QPixmap::fromMimeSource(locate("appdata", "images/listbox/image_win.png")), i18n("Image window"));
	(void)new SQ_IconListItem(listMain, KSquirrel::loader()->loadIcon("binary", KIcon::Desktop, 32), i18n("Libraries"));
	listMain->invalidateHeight();
	listMain->invalidateWidth();
	listMain->updateWidth();

	QFont listFont(listMain->font());
	listFont.setBold(true);
	listMain->setFont(listFont);
	listMain->verticalScrollBar()->installEventFilter(this);
	listMain->setCurrentItem(0);
	listMain->setSelected(0, true);
    
	connect(listMain, SIGNAL(selectionChanged()), SLOT(slotShowPage()));
    
	kURLReqOpenCustom->setMode(mode);
	
	pushCache->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_disk.png")));
	pushCacheMemory->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_mem.png")));
	pushClearCache->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_disk_clear.png")));
	pushClearCacheMemory->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_mem_clear.png")));
	pushShowCache->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_mem_view.png")));
	pushSyncCache->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_mem_sync.png")));
	
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
}

void SQ_Options::slotShowLibs()
{
	QString path;
    
	WST->raiseWidget((SQ_LibraryHandler::instance()->count())?0:1);

	tableLib->clear();
	
	// return anyway
	if(!SQ_LibraryHandler::instance()->count())
	    return;

	QValueVector<SQ_LIBRARY>::iterator   BEGIN = SQ_LibraryHandler::instance()->begin();
	QValueVector<SQ_LIBRARY>::iterator      END = SQ_LibraryHandler::instance()->end();
	
	QPixmap pix;
	
	for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;++it)
	{
		QFileInfo libfileinfo((*it).libpath);
		QListViewItem *item = new QListViewItem(tableLib, QString::null, libfileinfo.fileName(), QString((*it).quickinfo), QString((*it).version), QString((*it).filter));

		if(pix.convertFromImage((*it).mime))
		    item->setPixmap(0, pix);

		tableLib->insertItem(item);
	}
}

int SQ_Options::start()
{
	int result = this->exec();

	if(result == QDialog::Accepted)
	{
		SQ_Config::instance()->setGroup("Fileview");
		SQ_Config::instance()->writeEntry("set path", buttonGroupSetPath->id(buttonGroupSetPath->selected()));
		SQ_Config::instance()->writeEntry("custom directory", kURLReqOpenCustom->url());
		SQ_Config::instance()->writeEntry("sync type", buttonGroupSync->id(buttonGroupSync->selected()));
		SQ_Config::instance()->writeEntry("click policy", buttonGroupClick->id(buttonGroupClick->selected()));
		SQ_Config::instance()->writeEntry("history", checkSaveHistory->isChecked());
		SQ_Config::instance()->writeEntry("run unknown", checkRunUnknown->isChecked());
		SQ_Config::instance()->writeEntry("archives", checkSupportAr->isChecked());
		SQ_Config::instance()->writeEntry("tofirst", checkJumpFirst->isChecked());
		SQ_Config::instance()->writeEntry("disable_dirs", checkDisableDirs->isChecked());

		SQ_Config::instance()->setGroup("Main");
		SQ_Config::instance()->writeEntry("minimize to tray", checkMinimize->isChecked());
		SQ_Config::instance()->writeEntry("sync", checkSync->isChecked());
		SQ_Config::instance()->writeEntry("check", checkCheck->isChecked());

		SQ_Config::instance()->setGroup("Thumbnails");
		SQ_Config::instance()->writeEntry("margin", spinMargin->value());
		SQ_Config::instance()->writeEntry("cache", spinCacheSize->value());
		SQ_Config::instance()->writeEntry("disable_mime", checkMime->isChecked());
		SQ_Config::instance()->writeEntry("dont write", checkNoWriteThumbs->isChecked());
		SQ_Config::instance()->writeEntry("extended", checkExtended->isChecked());
		SQ_Config::instance()->writeEntry("tooltips", checkTooltips->isChecked());
		
		SQ_Config::instance()->setGroup("GL view");
		SQ_Config::instance()->writeEntry("GL view background", (kColorGLbackground->color()).name());
		SQ_Config::instance()->writeEntry("GL view custom texture", custpixmap);
		SQ_Config::instance()->writeEntry("GL view background type", buttonGroupColor->id(buttonGroupColor->selected()));
		SQ_Config::instance()->writeEntry("zoom limit", buttonGroupZoomLimit->id(buttonGroupZoomLimit->selected()));
		SQ_Config::instance()->writeEntry("alpha_bkgr", checkDrawQuads->isChecked());
		SQ_Config::instance()->writeEntry("marks", checkMarks->isChecked());
		SQ_Config::instance()->writeEntry("hide_sbar", checkStatus->isChecked());
		SQ_Config::instance()->writeEntry("scroll", buttonGroupScrolling->id(buttonGroupScrolling->selected()));
		SQ_Config::instance()->writeEntry("angle", sliderAngle->value());
		SQ_Config::instance()->writeEntry("zoom", spinZoomFactor->value());
		SQ_Config::instance()->writeEntry("zoom_min", spinZoomMin->value());
		SQ_Config::instance()->writeEntry("zoom_max", spinZoomMax->value());
		SQ_Config::instance()->writeEntry("move", sliderMove->value());

		SQ_Config::instance()->setGroup("Libraries");
		SQ_Config::instance()->writeEntry("monitor", checkMonitor->isChecked());
		SQ_Config::instance()->writeEntry("show dialog", checkFAMMessage->isChecked());
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

void SQ_Options::slotCalcCache()
{
    KURL url;
    url.setPath(QDir::homeDirPath() + "/.ksquirrel/thumbnails");
    
    int size = KDirSize::dirSize(url);
    
    QString s = KIO::convertSize(size);
    
    textThumbSize->setText(s);
}

void SQ_Options::slotClearCache()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    KURL url = QDir::homeDirPath() + QString::fromLatin1("/.ksquirrel/thumbnails");

    KIO::DeleteJob *job = KIO::del(url);
    connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotClearFinished(KIO::Job*)));
}

void SQ_Options::slotClearFinished(KIO::Job*)
{
    QApplication::restoreOverrideCursor();
    slotCalcCache();
}

void SQ_Options::slotClearMemoryCache()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    SQ_PixmapCache::instance()->clear();
    slotCalcCacheMemory();
    QApplication::restoreOverrideCursor();
}

void SQ_Options::slotCalcCacheMemory()
{
    int size = SQ_PixmapCache::instance()->totalSize();

    QString s = KIO::convertSize(size);
    
    textCacheMemSize->setText(s);
}

void SQ_Options::slotShowDiskCache()
{
    SQ_ViewCache m_view(this);
    m_view.exec();
}

void SQ_Options::slotSyncCache()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    
    SQ_PixmapCache::instance()->sync();
    
    slotCalcCacheMemory();
    slotCalcCache();
    
    QApplication::restoreOverrideCursor();
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
