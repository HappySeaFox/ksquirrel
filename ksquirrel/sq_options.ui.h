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
	pixmapView1->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_squirrel.png")));
	pixmapView2->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_gqview.png")));
	pixmapView3->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_kuickshow.png")));
	pixmapView4->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_nointerface.png")));
	pixmapView5->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_xnview.png")));
	pixmapView6->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_showimg.png")));
	pixmapView7->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_konqueror.png")));
	QPixmap pixNF = QPixmap::fromMimeSource(locate("appdata", "images/libs_notfound.png"));
	pixmapNotFound->setPixmap(pixNF);
    
	checkMinimize->setChecked(sqConfig->readBoolEntry("Main", "minimize to tray", true));
	checkOneInstance->setChecked(sqConfig->readBoolEntry("Main", "activate another", true));
	checkSync->setChecked(sqConfig->readBoolEntry("Main", "sync", true));
    
	if(sqConfig->readBoolEntry("Libraries", "monitor", true)) checkMonitor->toggle();
	checkFAMMessage->setChecked(sqConfig->readBoolEntry("Libraries", "show dialog", true));
   
	kURLReqLibs->setURL(sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/"));
	slotShowLibs();
    
	if(sqConfig->readBoolEntry("Interface", "save pos", true)) checkSavePos->toggle();
	if(sqConfig->readBoolEntry("Interface", "save size", true)) checkSaveSize->toggle();
    
	tp = sqConfig->readNumEntry("Fileview", "sync type", 0);
	buttonGroupSync->setButton(tp);
	tp = sqConfig->readNumEntry("Fileview", "set path", 0);
	buttonGroupSetPath->setButton(tp);
	kURLReqOpenCustom->setURL(sqConfig->readEntry("Fileview", "custom directory", "/"));
	tp = sqConfig->readNumEntry("Fileview", "click policy", 0);
	buttonGroupClick->setButton(tp);
	if(sqConfig->readBoolEntry("Fileview", "history", true)) checkSaveHistory->toggle();
	if(sqConfig->readBoolEntry("Fileview", "run unknown", true)) checkRunUnknown->toggle();
	if(sqConfig->readBoolEntry("Fileview", "archives", true)) checkSupportAr->toggle();
	if(sqConfig->readBoolEntry("Fileview", "tofirst", true)) checkJumpFirst->toggle();

// Init GLView page
	QPixmap p1;
	if(sqConfig->readBoolEntry("GL view", "quickbrowser", true)) checkQuick->toggle();
    
	tp = sqConfig->readNumEntry("GL view", "GL view background type", 0);
	buttonGroupColor->setButton(tp);
   
	custpixmap = sqConfig->readEntry("GL view", "GL view custom texture", "");
	kURLReqCustomTexture->setURL(custpixmap);

	tp = sqViewType;
	widgetStackView->raiseWidget(tp);
	buttonGroupViewType->setButton(tp);
	checkZoomPretty->setChecked(sqConfig->readBoolEntry("GL view", "zoom pretty", true));
	tp = sqConfig->readNumEntry("GL view", "manipulation center", 0);
	buttonGroupCenter->setButton(tp);
	tp = sqConfig->readNumEntry("GL view", "scroll", 0);
	buttonGroupScrolling->setButton(tp);
	tp = sqConfig->readNumEntry("GL view", "render file type", 1);
	buttonGroupRenderFile->setButton(tp);
	sliderAngle->setValue(sqConfig->readNumEntry("GL view", "angle", 90));
	spinZoomFactor->setValue(sqConfig->readNumEntry("GL view", "zoom", 25));
	sliderMove->setValue(sqConfig->readNumEntry("GL view", "move", 5));
    
	QColor color;
	color.setNamedColor(sqConfig->readEntry("GL view", "GL view background", "#cccccc"));
	kColorGLbackground->setColor(color);
    
	if(sqConfig->readBoolEntry("GL view", "image background", true)) checkBackgroundTransparent->toggle();
	if(sqConfig->readBoolEntry("GL view", "border", true)) checkBorder->toggle();
	if(sqConfig->readBoolEntry("GL view", "progress", true)) checkProgress->toggle();
	color.setNamedColor(sqConfig->readEntry("GL view", "GL view border", "#00ff00"));
	kColorBorder->setColor(color);
	spinBorder->setValue(sqConfig->readNumEntry("GL view", "border size", 2));
	if(sqConfig->readBoolEntry("GL view", "save pos", true)) checkGLSavePos->toggle();
	if(sqConfig->readBoolEntry("GL view", "save size", true)) checkGLSaveSize->toggle();
	kURLReqSaveInto->setURL(sqConfig->readEntry("GL view", "render path", "/"));
	lineRFormat->setText(sqConfig->readEntry("GL view", "render file format", "ksquirrel %dd.%dmn.%dy, %dh.%dm.%ds"));
    
	spinMargin->setValue(sqConfig->readNumEntry("Thumbnails", "margin", 2));
	spinCacheSize->setValue(sqConfig->readNumEntry("Thumbnails", "cache", 1024*5));
	if(sqConfig->readBoolEntry("Thumbnails", "disable_mime", true)) checkMime->toggle();
	if(sqConfig->readBoolEntry("Thumbnails", "dont write", true)) checkNoWriteThumbs->toggle();
    
	int size = 32;
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("desktop", KIcon::Desktop, size), i18n("Main"));
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("appearance", KIcon::Desktop, size), i18n("Interface"));
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("fileopen", KIcon::Desktop, size), i18n("Filing"));
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("file", KIcon::Desktop, size), i18n("Thumbnails"));
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("terminal", KIcon::Desktop, size), i18n("GL viewer"));
	(void)new SQ_IconListItem(listMain, sqLoader->loadIcon("binary", KIcon::Desktop, size), i18n("Libraries"));
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
    
	QStringList formats = QImage::outputFormatList();
	comboRFormats->insertStringList(formats);
	comboRFormats->setCurrentText(sqConfig->readEntry("GL view", "render ext", "PNG"));
    
	kURLReqOpenCustom->setMode(KFile::Directory);
	kURLReqSaveInto->setMode(KFile::Directory);
}

void SQ_Options::slotShowLibs()
{
	QString path;
    
	WST->raiseWidget((sqLibHandler->count())?0:1);

	tableLib->clear();
	libPrefix = sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/");
	if(!libPrefix.endsWith("/"))	libPrefix += "/";

	QValueVector<SQ_LIBRARY> vect = sqLibHandler->getLibs();
	QValueVector<SQ_LIBRARY>::iterator   BEGIN = vect.begin();
	QValueVector<SQ_LIBRARY>::iterator      END = vect.end();
	
	for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;it++)
	{
		QFileInfo libfileinfo((*it).libpath);
		tableLib->insertItem(new QListViewItem(tableLib, libfileinfo.fileName(), QString((*it).quickinfo), QString((*it).version), QString((*it).filter)));
	}
}

int SQ_Options::start()
{
	int result = this->exec();

	if(result == QDialog::Accepted)
	{
		sqConfig->setGroup("Interface");
		sqConfig->writeEntry("view type", buttonGroupViewType->id(buttonGroupViewType->selected()));
		sqConfig->writeEntry("save pos", checkSavePos->isChecked());
		sqConfig->writeEntry("save size", checkSaveSize->isChecked());
		
		sqConfig->setGroup("Fileview");
		sqConfig->writeEntry("set path", buttonGroupSetPath->id(buttonGroupSetPath->selected()));
		sqConfig->writeEntry("custom directory", kURLReqOpenCustom->url());
		sqConfig->writeEntry("sync type", buttonGroupSync->id(buttonGroupSync->selected()));
		sqConfig->writeEntry("click policy", buttonGroupClick->id(buttonGroupClick->selected()));
		sqConfig->writeEntry("history", checkSaveHistory->isChecked());
		sqConfig->writeEntry("run unknown", checkRunUnknown->isChecked());
		sqConfig->writeEntry("archives", checkSupportAr->isChecked());
		sqConfig->writeEntry("tofirst", checkJumpFirst->isChecked());

		sqConfig->setGroup("Main");
		sqConfig->writeEntry("minimize to tray", checkMinimize->isChecked());
		sqConfig->writeEntry("activate another", checkOneInstance->isChecked());
		sqConfig->writeEntry("sync", checkSync->isChecked());

		sqConfig->setGroup("Thumbnails");
		sqConfig->writeEntry("margin", spinMargin->value());
		sqConfig->writeEntry("cache", spinCacheSize->value());
		sqConfig->writeEntry("disable_mime", checkMime->isChecked());
		sqConfig->writeEntry("dont write", checkNoWriteThumbs->isChecked());
		
		sqConfig->setGroup("GL view");
		sqConfig->writeEntry("GL view background", (kColorGLbackground->color()).name());
		sqConfig->writeEntry("GL view custom texture", kURLReqCustomTexture->url());
		sqConfig->writeEntry("GL view border", (kColorBorder->color()).name());
		sqConfig->writeEntry("border size", spinBorder->value());
		sqConfig->writeEntry("GL view background type", buttonGroupColor->id(buttonGroupColor->selected()));
		sqConfig->writeEntry("zoom pretty", checkZoomPretty->isChecked());
		sqConfig->writeEntry("progress", checkProgress->isChecked());
		sqConfig->writeEntry("manipulation center", buttonGroupCenter->id(buttonGroupCenter->selected()));
		sqConfig->writeEntry("scroll", buttonGroupScrolling->id(buttonGroupScrolling->selected()));
		sqConfig->writeEntry("render file type", buttonGroupRenderFile->id(buttonGroupRenderFile->selected()));
		sqConfig->writeEntry("angle", sliderAngle->value());
		sqConfig->writeEntry("zoom", spinZoomFactor->value());
		sqConfig->writeEntry("move", sliderMove->value());
		sqConfig->writeEntry("border", checkBorder->isChecked());
		sqConfig->writeEntry("image background", checkBackgroundTransparent->isChecked());
		sqConfig->writeEntry("save pos", checkGLSavePos->isChecked());
		sqConfig->writeEntry("save size", checkGLSaveSize->isChecked());
		sqConfig->writeEntry("render path", kURLReqSaveInto->url());
		sqConfig->writeEntry("render ext",  comboRFormats->currentText());
		sqConfig->writeEntry("render file format",  lineRFormat->text());
		sqConfig->writeEntry("quickbrowser",  checkQuick->isChecked());

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
		custpixmap = path;
	else
		QMessageBox::critical(this, i18n("Error"), QString(i18n("Wrong dimensions: %1x%2.")).arg(p1.width()).arg(p1.height()), QMessageBox::Ok, QMessageBox::NoButton);
}


void SQ_Options::slotShowPage()
{
	int id = listMain->currentItem();
    
	widgetStack1->raiseWidget(id);
	widgetStackLines->raiseWidget(id);
}


void SQ_Options::slotCalcCache()
{
	textCacheSize->clear();
	KURL url;
	url.setPath(QDir::homeDirPath() + "/.ksquirrel/thumbnails");
	unsigned long size = KDirSize::dirSize(url);
	textCacheSize->setText(KIO::convertSize(size));
}
