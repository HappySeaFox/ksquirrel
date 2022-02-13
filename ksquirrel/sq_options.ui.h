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
    pixmapView4->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_winviewer.png")));
    pixmapView5->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_xnview.png")));
    pixmapView6->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_showimg.png")));
    pixmapView7->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/view_konqueror.png")));
    QPixmap pixNF = QPixmap::fromMimeSource(locate("appdata", "images/libs_notfound.png"));
    pixmapNotFound->setPixmap(pixNF);
    
    checkRestart->setChecked(sqConfig->readBoolEntry("Main", "restart", true));
    checkMinimize->setChecked(sqConfig->readBoolEntry("Main", "minimize to tray", true));
    checkOneInstance->setChecked(sqConfig->readBoolEntry("Main", "activate another", true));
    checkSync->setChecked(sqConfig->readBoolEntry("Main", "sync", true));
    
    if(sqConfig->readBoolEntry("Libraries", "monitor", true)) checkMonitor->toggle();
    checkFAMMessage->setChecked(sqConfig->readBoolEntry("Libraries", "show dialog", true));
   
  QString lp = sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/");
   comboPrefix->setEditText(lp);
   comboPrefix->addToHistory(lp);
   comboPrefix->setDuplicatesEnabled(false);
   KURLCompletion *pURLCompletion = new KURLCompletion(KURLCompletion::DirCompletion);
   pURLCompletion->setDir("/");
   comboPrefix->setCompletionObject(pURLCompletion);
   comboPrefix->setAutoDeleteCompletionObject(true);
   if(sqConfig->readBoolEntry("Libraries", "continue", true))
       checkContinue->toggle();
    
    comboToolbarIconSize->setCurrentItem(sqConfig->readNumEntry("Interface", "toolbar icon size", 0));
    tp = sqConfig->readNumEntry("Interface", "create first", 0);
    ((QRadioButton*)(buttonGroupCreateFirst->find(tp)))->setChecked(true);
    if(sqConfig->readBoolEntry("Interface", "save pos", true)) checkSavePos->toggle();
    if(sqConfig->readBoolEntry("Interface", "save size", true)) checkSaveSize->toggle();
    
    tp = sqConfig->readNumEntry("Fileview", "sync type", 0);
    ((QRadioButton*)(buttonGroupSync->find(tp)))->setChecked(true);
    tp = sqConfig->readNumEntry("Fileview", "set path", 0);
    ((QRadioButton*)(buttonGroupSetPath->find(tp)))->setChecked(true);
    lineEditCustomDir->setText(sqConfig->readEntry("Fileview", "custom directory", "/"));
    tp = sqConfig->readNumEntry("Fileview", "click policy custom", 0);
    ((QRadioButton*)(buttonGroupClickPolicy->find(tp)))->setChecked(true);
    if(sqConfig->readBoolEntry("Fileview", "click policy system", true)) checkClickSystem->toggle();
    if(sqConfig->readBoolEntry("Fileview", "history", true)) checkSaveHistory->toggle();
    if(sqConfig->readBoolEntry("Fileview", "run unknown", true)) checkRunUnknown->toggle();

// Init GLView page
    tp = sqViewType;
    widgetStackView->raiseWidget(tp);
    ((QRadioButton*)(buttonGroupViewType->find(tp)))->setChecked(true);
    tp = sqConfig->readNumEntry("GL view", "zoom model", 0);
    ((QRadioButton*)(buttonGroupZoomModel->find(tp)))->setChecked(true);
    tp = sqConfig->readNumEntry("GL view", "shade model", 0);
    ((QRadioButton*)(buttonGroupShadeModel->find(tp)))->setChecked(true);
    tp = sqConfig->readNumEntry("GL view", "manipulation center", 0);
    ((QRadioButton*)(buttonGroupCenter->find(tp)))->setChecked(true);
    tp = sqConfig->readNumEntry("GL view", "scroll", 1);
    ((QRadioButton*)(buttonGroupScrolling->find(tp)))->setChecked(true);
    QColor color;
    color.setNamedColor(sqConfig->readEntry("GL view", "GL view background", "#cccccc"));
    kColorGLbackground->setColor(color);
    checkDrop->setChecked(sqConfig->readBoolEntry("GL view", "enable drop", true));
    tp = sqConfig->readNumEntry("GL view", "angle", 90);
    sliderAngle->setValue(tp);
    tp = sqConfig->readNumEntry("GL view", "zoom", 25);
    spinZoomFactor->setValue(tp);
    tp = sqConfig->readNumEntry("GL view", "move", 5);
    sliderMove->setValue(tp);
    tp = sqConfig->readNumEntry("GL view", "swap on", 2);
    spinSwapFactor->setValue(tp);
    if(sqConfig->readBoolEntry("GL view", "system color", true)) checkSystemColor->toggle();
    if(sqConfig->readBoolEntry("GL view", "background", true)) checkBackgroundTransparent->toggle();
    if(sqConfig->readBoolEntry("GL view", "border", true)) checkBorder->toggle();
    if(sqConfig->readBoolEntry("GL view", "save pos", true)) checkGLSavePos->toggle();
    if(sqConfig->readBoolEntry("GL view", "save size", true)) checkGLSaveSize->toggle();
    
// Init cache page
   if(!sqConfig->readBoolEntry("Cache", "enable", true)) checkCacheEnable->toggle();
   lineCachePath->setText(sqConfig->readEntry("Cache", "place", ""));
    tp = sqConfig->readNumEntry("Cache", "cache policy", 0);
    ((QRadioButton*)(buttonGroupCachePolicy->find(tp)))->toggle();
    spinCacheImageX->setValue(sqConfig->readNumEntry("Cache", "cache image X", 300));
    spinCacheImageY->setValue(sqConfig->readNumEntry("Cache", "cache image Y", 300));
    spinCacheImageLarger->setValue(sqConfig->readNumEntry("Cache", "cache image larger", 5));
   if(sqConfig->readBoolEntry("Cache", "gzip", true)) checkCacheGZIP->toggle();
   if(sqConfig->readBoolEntry("Cache", "max", true)) checkCacheMax->toggle();
    spinCacheMax->setValue(sqConfig->readNumEntry("Cache", "max value", 50));
    
    checkShowLinks->toggle();

    listMain->insertItem(sqLoader->loadIcon("desktop", KIcon::Desktop, 32), "Main");
    listMain->insertItem(sqLoader->loadIcon("appearance", KIcon::Desktop, 32), "Interface");
    listMain->insertItem(sqLoader->loadIcon("fileopen", KIcon::Desktop, 32), "Filing");
    listMain->insertItem(sqLoader->loadIcon("terminal", KIcon::Desktop, 32), "GL viewer");
    listMain->insertItem(sqLoader->loadIcon("binary", KIcon::Desktop, 32), "Libraries");
    listMain->insertItem(sqLoader->loadIcon("hdd_mount", KIcon::Desktop, 32), "Caching");
    listMain->setCurrentItem(0);
    listMain->setSelected(0, true);
}

void SQ_Options::slotOpenDir()
{
	QString s = KFileDialog::getExistingDirectory("/", this, "Choose a directory");

	if(!s.isEmpty())
		lineEditCustomDir->setText(s);
}

void SQ_Options::slotDirCache()
{
	QString s = KFileDialog::getExistingDirectory("/", this, "Choose a directory");
    
	if(!s.isEmpty())
		lineCachePath->setText(s);
}

void SQ_Options::slotShowLinks( bool showl )
{
    QString path;
    int totalRows = sqLibHandler->count();
    
    WST->raiseWidget((sqLibHandler->count())?0:1);

    tableLib->clear();
    libPrefix = sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/");
    if(!libPrefix.endsWith("/"))	libPrefix += "/";

    for(int i = 0;i < totalRows;i++)
    {
	SQ_LIBRARY tmplib = sqLibHandler->getLibByIndex(i);
	path = tmplib.libpath;

	if(!showl)
	{
	    QFileInfo libfileinfo(path);
	    if(libfileinfo.isSymLink()) continue;
	}

	path.replace(libPrefix, "");

	tableLib->insertItem(new QListViewItem(tableLib, path, QString(tmplib.fmt_quickinfo()), QString(tmplib.fmt_version()), QString(tmplib.sinfo)));
    }
}


void SQ_Options::slotNewPrefix()
{
    QString s = KFileDialog::getExistingDirectory("/", this, "Choose a directory");

    if(!s.isEmpty())
    {
	comboPrefix->setEditText(s);
	comboPrefix->addToHistory(s);
	libPrefix = s;
	if(!libPrefix.endsWith("/"))
	    libPrefix += "/";
    }
}

int SQ_Options::start()
{
	int result = this->exec();

	if(result == QDialog::Accepted)
	{
		sqConfig->setGroup("Interface");
		sqConfig->writeEntry("ViewType", buttonGroupViewType->id(buttonGroupViewType->selected()));
		sqConfig->writeEntry("toolbar icon size", comboToolbarIconSize->currentItem());
		sqConfig->writeEntry("create first", buttonGroupCreateFirst->id(buttonGroupCreateFirst->selected()));
		sqConfig->writeEntry("save pos", checkSavePos->isChecked());
		sqConfig->writeEntry("save size", checkSaveSize->isChecked());
		
		sqConfig->setGroup("Fileview");
		sqConfig->writeEntry("set path", buttonGroupSetPath->id(buttonGroupSetPath->selected()));
		sqConfig->writeEntry("custom directory", lineEditCustomDir->text());
		sqConfig->writeEntry("sync type", buttonGroupSync->id(buttonGroupSync->selected()));
		sqConfig->writeEntry("click policy custom", buttonGroupClickPolicy->id(buttonGroupClickPolicy->selected()));
		sqConfig->writeEntry("click policy system", checkClickSystem->isChecked());
		sqConfig->writeEntry("history", checkSaveHistory->isChecked());
		sqConfig->writeEntry("run unknown", checkRunUnknown->isChecked());

		sqConfig->setGroup("Main");
		sqConfig->writeEntry("minimize to tray", checkMinimize->isChecked());
		sqConfig->writeEntry("restart", checkRestart->isChecked());
		sqConfig->writeEntry("activate another", checkOneInstance->isChecked());
		sqConfig->writeEntry("sync", checkSync->isChecked());

		sqConfig->setGroup("GL view");
		sqConfig->writeEntry("system color", checkSystemColor->isChecked());
		sqConfig->writeEntry("GL view background", (kColorGLbackground->color()).name());
		sqConfig->writeEntry("zoom model", buttonGroupZoomModel->id(buttonGroupZoomModel->selected()));
		sqConfig->writeEntry("manipulation center", buttonGroupCenter->id(buttonGroupCenter->selected()));
		sqConfig->writeEntry("shade model", buttonGroupShadeModel->id(buttonGroupShadeModel->selected()));
		sqConfig->writeEntry("scroll", buttonGroupScrolling->id(buttonGroupScrolling->selected()));
		sqConfig->writeEntry("enable drop", checkDrop->isChecked());
		sqConfig->writeEntry("angle", sliderAngle->value());
		sqConfig->writeEntry("zoom", spinZoomFactor->value());
		sqConfig->writeEntry("swap on", spinSwapFactor->value());
		sqConfig->writeEntry("move", sliderMove->value());
		sqConfig->writeEntry("border", checkBorder->isChecked());
		sqConfig->writeEntry("background", checkBackgroundTransparent->isChecked());
		sqConfig->writeEntry("save pos", checkGLSavePos->isChecked());
		sqConfig->writeEntry("save size", checkGLSaveSize->isChecked());

		sqConfig->setGroup("Libraries");
		sqConfig->writeEntry("monitor", checkMonitor->isChecked());
		sqConfig->writeEntry("show dialog", checkFAMMessage->isChecked());
		sqConfig->writeEntry("prefix", comboPrefix->currentText());
		sqConfig->writeEntry("continue", checkContinue->isChecked());
		
		sqConfig->setGroup("Cache");
		sqConfig->writeEntry("enable", checkCacheEnable->isChecked());
		sqConfig->writeEntry("cache policy", buttonGroupCachePolicy->id(buttonGroupCachePolicy->selected()));
		sqConfig->writeEntry("place", lineCachePath->text());
		sqConfig->writeEntry("cache image X", spinCacheImageX->value());
		sqConfig->writeEntry("cache image Y", spinCacheImageY->value());
		sqConfig->writeEntry("cache image larger", spinCacheImageLarger->value());
		sqConfig->writeEntry("gzip", checkCacheGZIP->isChecked());
		sqConfig->writeEntry("max", checkCacheMax->isChecked());
		sqConfig->writeEntry("max value", spinCacheMax->value());
		
	    }

	return result;
}
