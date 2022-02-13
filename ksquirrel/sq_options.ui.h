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
    
	checkMinimize->setChecked(sqConfig->readBoolEntry("Main", "minimize to tray", true));
	checkSync->setChecked(sqConfig->readBoolEntry("Main", "sync", true));
	checkCheck->setChecked(sqConfig->readBoolEntry("Main", "check", true));
    
	checkMonitor->setChecked(sqConfig->readBoolEntry("Libraries", "monitor", true));
	checkFAMMessage->setChecked(sqConfig->readBoolEntry("Libraries", "show dialog", true));
   
	kURLReqLibs->setURL(sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/"));
	KFile::Mode mode = static_cast<KFile::Mode>(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);
	kURLReqLibs->setMode(mode);
	slotShowLibs();
    
	tp = sqConfig->readNumEntry("Fileview", "sync type", 0);
	buttonGroupSync->setButton(tp);
	tp = sqConfig->readNumEntry("Fileview", "set path", 0);
	buttonGroupSetPath->setButton(tp);
	kURLReqOpenCustom->setURL(sqConfig->readEntry("Fileview", "custom directory", "/"));
	tp = sqConfig->readNumEntry("Fileview", "click policy", 0);
	buttonGroupClick->setButton(tp);
	
	checkSaveHistory->setChecked(sqConfig->readBoolEntry("Fileview", "history", true));
	checkRunUnknown->setChecked(sqConfig->readBoolEntry("Fileview", "run unknown", true));
	checkSupportAr->setChecked(sqConfig->readBoolEntry("Fileview", "archives", true));
	checkJumpFirst->setChecked(sqConfig->readBoolEntry("Fileview", "tofirst", true));
	checkDisableDirs->setChecked(sqConfig->readBoolEntry("Fileview", "disable_dirs", false));

// Init GLView page
	QPixmap p1;
	checkStatus->setChecked(sqConfig->readBoolEntry("GL view", "hide_sbar", true));
	checkDrawQuads->setChecked(sqConfig->readBoolEntry("GL view", "alpha_bkgr", true));	  checkMarks->setChecked(sqConfig->readBoolEntry("GL view", "marks", true));    
	
	tp = sqConfig->readNumEntry("GL view", "GL view background type", 0);
	buttonGroupColor->setButton(tp);
	widgetStack4->raiseWidget(tp);
	
	kColorSystem->setColor(colorGroup().color(QColorGroup::Base));
	
	tp = sqConfig->readNumEntry("GL view", "zoom limit", 0);
	buttonGroupZoomLimit->setButton(tp);
	spinZoomMin->setValue(sqConfig->readNumEntry("GL view", "zoom_min", 1));
	spinZoomMax->setValue(sqConfig->readNumEntry("GL view", "zoom_max", 10000));

	custpixmap = sqConfig->readEntry("GL view", "GL view custom texture", "");
	kURLReqCustomTexture->setURL(custpixmap);
	
	if(!custpixmap.isEmpty())
	    slotValidateCustomTexture();

	tp = sqConfig->readNumEntry("GL view", "scroll", 0);
	buttonGroupScrolling->setButton(tp);
	sliderAngle->setValue(sqConfig->readNumEntry("GL view", "angle", 90));
	spinZoomFactor->setValue(sqConfig->readNumEntry("GL view", "zoom", 25));
	sliderMove->setValue(sqConfig->readNumEntry("GL view", "move", 5));
    
	QColor color;
	color.setNamedColor(sqConfig->readEntry("GL view", "GL view background", "#cccccc"));
	kColorGLbackground->setColor(color);
    
	spinMargin->setValue(sqConfig->readNumEntry("Thumbnails", "margin", 2));
	spinCacheSize->setValue(sqConfig->readNumEntry("Thumbnails", "cache", 1024*5));
	if(sqConfig->readBoolEntry("Thumbnails", "disable_mime", true)) checkMime->toggle();
	if(sqConfig->readBoolEntry("Thumbnails", "dont write", false)) checkNoWriteThumbs->toggle();
	if(sqConfig->readBoolEntry("Thumbnails", "extended", false)) checkExtended->toggle();
	if(sqConfig->readBoolEntry("Thumbnails", "tooltips", false)) checkTooltips->toggle();
    
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("display", KIcon::Desktop, 32), i18n("Main"));	
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("folder", KIcon::Desktop, 32), i18n("Filing"));
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("images", KIcon::Desktop, 32), i18n("Thumbnails"));
	(void)new SQ_IconListItem(listMain, QPixmap::fromMimeSource(locate("appdata", "images/listbox/image_win.png")), i18n("Image window"));
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("binary", KIcon::Desktop, 32), i18n("Libraries"));
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
    
	WST->raiseWidget((sqLibHandler->count())?0:1);

	tableLib->clear();
	libPrefix = sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/");
	if(!libPrefix.endsWith("/"))	libPrefix += "/";
	
	// return anyway
	if(!sqLibHandler->count())
	    return;

	QValueVector<SQ_LIBRARY>::iterator   BEGIN = sqLibHandler->begin();
	QValueVector<SQ_LIBRARY>::iterator      END = sqLibHandler->end();
	
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
		sqConfig->setGroup("Fileview");
		sqConfig->writeEntry("set path", buttonGroupSetPath->id(buttonGroupSetPath->selected()));
		sqConfig->writeEntry("custom directory", kURLReqOpenCustom->url());
		sqConfig->writeEntry("sync type", buttonGroupSync->id(buttonGroupSync->selected()));
		sqConfig->writeEntry("click policy", buttonGroupClick->id(buttonGroupClick->selected()));
		sqConfig->writeEntry("history", checkSaveHistory->isChecked());
		sqConfig->writeEntry("run unknown", checkRunUnknown->isChecked());
		sqConfig->writeEntry("archives", checkSupportAr->isChecked());
		sqConfig->writeEntry("tofirst", checkJumpFirst->isChecked());
		sqConfig->writeEntry("disable_dirs", checkDisableDirs->isChecked());

		sqConfig->setGroup("Main");
		sqConfig->writeEntry("minimize to tray", checkMinimize->isChecked());
		sqConfig->writeEntry("sync", checkSync->isChecked());
		sqConfig->writeEntry("check", checkCheck->isChecked());

		sqConfig->setGroup("Thumbnails");
		sqConfig->writeEntry("margin", spinMargin->value());
		sqConfig->writeEntry("cache", spinCacheSize->value());
		sqConfig->writeEntry("disable_mime", checkMime->isChecked());
		sqConfig->writeEntry("dont write", checkNoWriteThumbs->isChecked());
		sqConfig->writeEntry("extended", checkExtended->isChecked());
		sqConfig->writeEntry("tooltips", checkTooltips->isChecked());
		
		sqConfig->setGroup("GL view");
		sqConfig->writeEntry("GL view background", (kColorGLbackground->color()).name());
		sqConfig->writeEntry("GL view custom texture", custpixmap);
		sqConfig->writeEntry("GL view background type", buttonGroupColor->id(buttonGroupColor->selected()));
		sqConfig->writeEntry("zoom limit", buttonGroupZoomLimit->id(buttonGroupZoomLimit->selected()));
		sqConfig->writeEntry("alpha_bkgr", checkDrawQuads->isChecked());
		sqConfig->writeEntry("marks", checkMarks->isChecked());
		sqConfig->writeEntry("hide_sbar", checkStatus->isChecked());
		sqConfig->writeEntry("scroll", buttonGroupScrolling->id(buttonGroupScrolling->selected()));
		sqConfig->writeEntry("angle", sliderAngle->value());
		sqConfig->writeEntry("zoom", spinZoomFactor->value());
		sqConfig->writeEntry("zoom_min", spinZoomMin->value());
		sqConfig->writeEntry("zoom_max", spinZoomMax->value());
		sqConfig->writeEntry("move", sliderMove->value());

		sqConfig->setGroup("Libraries");
		sqConfig->writeEntry("monitor", checkMonitor->isChecked());
		sqConfig->writeEntry("show dialog", checkFAMMessage->isChecked());
		sqConfig->writeEntry("prefix", kURLReqLibs->url());
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
    sqCache->clear();
    slotCalcCacheMemory();
    QApplication::restoreOverrideCursor();
}

void SQ_Options::slotCalcCacheMemory()
{
    int size = sqCache->totalSize();

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
    
    sqCache->sync();
    
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
