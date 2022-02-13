/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_Options::slotSelectPage(int page)
{
    widgetStack1->raiseWidget(page);
}


void SQ_Options::init()
{
    int tp;
    a[0] = QPixmap::fromMimeSource("images/view_squirrel.png");
    a[1] = QPixmap::fromMimeSource("images/view_gqview.png");
    a[2] = QPixmap::fromMimeSource("images/view_kuickshow.png");
    a[3] = QPixmap::fromMimeSource("images/view_winviewer.png");
    a[4] = QPixmap::fromMimeSource("images/view_xnview.png");
    a[5] = QPixmap::fromMimeSource("images/view_xnview.png");
    
    checkRestart->setChecked(sqConfig->readBoolEntry("Main", "restart", true));
    checkMinimize->setChecked(sqConfig->readBoolEntry("Main", "minimize to tray", true));
    checkOneInstance->setChecked(sqConfig->readBoolEntry("Main", "activate another", true));
    
    if(sqConfig->readBoolEntry("Libraries", "monitor", true)) checkMonitor->toggle();
    checkFAMMessage->setChecked(sqConfig->readBoolEntry("Libraries", "show dialog", true));
   linePrefix->setText(sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/"));
   if(sqConfig->readBoolEntry("Libraries", "continue", true)) checkContinue->toggle();

    
    comboToolbarIconSize->setCurrentItem(sqConfig->readNumEntry("Interface", "toolbar icon size", 0));
    tp = sqConfig->readNumEntry("Interface", "create first", 0);
    ((QRadioButton*)(buttonGroupCreateFirst->find(tp)))->setChecked(true);
    
    comboIconIndex->setCurrentItem(sqConfig->readNumEntry("Fileview", "iCurrentIconIndex", 0));
    comboListIndex->setCurrentItem(sqConfig->readNumEntry("Fileview", "iCurrentListIndex", 0));
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

    tp = sqViewType;
    pixmapShowView->setPixmap(a[tp]);
    ((QRadioButton*)(buttonGroupViewType->find(tp)))->setChecked(true);

    tp = sqConfig->readNumEntry("GL view", "zoom model", 0);
    ((QRadioButton*)(buttonGroupZoomModel->find(tp)))->setChecked(true);
    tp = sqConfig->readNumEntry("GL view", "shade model", 0);
    ((QRadioButton*)(buttonGroupShadeModel->find(tp)))->setChecked(true);
    QColor color;
    color.setNamedColor(sqConfig->readEntry("GL view", "GL view background", "#cccccc"));
    kColorGLbackground->setColor(color);
    checkDrop->setChecked(sqConfig->readBoolEntry("GL view", "enable drop", true));
    tp = sqConfig->readNumEntry("GL view", "angle", 90);
    spinAngle->setValue(tp);
    tp = sqConfig->readNumEntry("GL view", "zoom", 25);
    spinZoomFactor->setValue(tp);
    if(sqConfig->readBoolEntry("GL view", "system color", true)) checkSystemColor->toggle();
    if(sqConfig->readBoolEntry("GL view", "background", true)) checkBackgroundTransparent->toggle();
    if(sqConfig->readBoolEntry("GL view", "border", true)) checkBorder->toggle();
    if(sqConfig->readBoolEntry("GL view", "step-by-step", true)) checkStepByStep->toggle();
        
    checkShowLinks->toggle();

    listMain->setSorting(-1);
    itemMain = new QListViewItem(listMain, 0);
    itemMain->setText(0, tr2i18n("Main" ));
    itemInterface = new QListViewItem(listMain, itemMain);
    itemInterface->setOpen(true);
    itemFileView = new QListViewItem(itemInterface, itemMain);
    itemFileView->setText( 0, tr2i18n( "FileView" ) );
    itemGLview = new QListViewItem(itemInterface, itemFileView);
    itemInterface->setText(0, tr2i18n("Interface"));
    itemGLview->setText(0, tr2i18n("GL view"));
    itemLibraries = new QListViewItem(listMain, itemInterface);
    itemLibraries->setText(0, tr2i18n("Libraries"));
    itemCache = new QListViewItem(listMain, itemLibraries);
    itemCache->setText(0, tr2i18n("Caching"));
    
    connect(listMain, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(slotClicked(QListViewItem *)));
    listMain->header()->hide();
    listMain->setCurrentItem(itemMain);
    listMain->setSelected(itemMain, true);
}



void SQ_Options::slotSetViewPixmap(int id)
{
    pixmapShowView->setPixmap(a[id]);
}


void SQ_Options::slotClicked( QListViewItem *item )
{
    const int count = 6;

    ListViewItemID ListViewItemID_[count] = {
	{itemMain, 0},
	{itemInterface,1},
	{itemFileView,2},
	{itemGLview,5},
	{itemLibraries,3},
	{itemCache,4}};

    for(int i = 0;i < count;i++)
    {
	if(ListViewItemID_[i].item == item)
	{
	    widgetStack1->raiseWidget(ListViewItemID_[i].id);
	    break;
	}
    }
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

    tableLib->clear();

    for(int i = 0;i < totalRows;i++)
    {
	SQ_LIBRARY tmplib = sqLibHandler->getLibByIndex(i);
	path = tmplib.libpath;

	if(!showl)
	{
	    QFileInfo libfileinfo(path);
	    if(libfileinfo.isSymLink()) continue;
	}
	path.replace(sqLibPrefix, "");
	tableLib->insertItem(new QListViewItem(tableLib, path, QString(tmplib.fmt_quickinfo()), QString(tmplib.fmt_version()), QString(tmplib.sinfo)));
    }

}


void SQ_Options::slotNewPrefix()
{
    QString s = KFileDialog::getExistingDirectory("/", this, "Choose a directory");

    if(!s.isEmpty())
	linePrefix->setText(s);

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

		sqConfig->setGroup("Fileview");
		sqConfig->writeEntry("set path", buttonGroupSetPath->id(buttonGroupSetPath->selected()));
		sqConfig->writeEntry("custom directory", lineEditCustomDir->text());
		sqConfig->writeEntry("sync type", buttonGroupSync->id(buttonGroupSync->selected()));
		sqConfig->writeEntry("iCurrentIconIndex", comboIconIndex->currentItem());
		sqConfig->writeEntry("iCurrentListIndex", comboListIndex->currentItem());
		sqConfig->writeEntry("click policy custom", buttonGroupClickPolicy->id(buttonGroupClickPolicy->selected()));
		sqConfig->writeEntry("click policy system", checkClickSystem->isChecked());
		sqConfig->writeEntry("history", checkSaveHistory->isChecked());
		sqConfig->writeEntry("run unknown", checkRunUnknown->isChecked());

		sqConfig->setGroup("Main");
		sqConfig->writeEntry("minimize to tray", checkMinimize->isChecked());
		sqConfig->writeEntry("restart", checkRestart->isChecked());
		sqConfig->writeEntry("activate another", checkOneInstance->isChecked());

		sqConfig->setGroup("GL view");
		sqConfig->writeEntry("system color", checkSystemColor->isChecked());
		sqConfig->writeEntry("GL view background", (kColorGLbackground->color()).name());
		sqConfig->writeEntry("zoom model", buttonGroupZoomModel->id(buttonGroupZoomModel->selected()));
		sqConfig->writeEntry("shade model", buttonGroupShadeModel->id(buttonGroupShadeModel->selected()));
		sqConfig->writeEntry("enable drop", checkDrop->isChecked());
		sqConfig->writeEntry("angle", spinAngle->value());
		sqConfig->writeEntry("zoom", spinZoomFactor->value());
		sqConfig->writeEntry("border", checkBorder->isChecked());
		sqConfig->writeEntry("background", checkBackgroundTransparent->isChecked());
		sqConfig->writeEntry("step-by-step", checkStepByStep->isChecked());

		sqConfig->setGroup("Libraries");
		sqConfig->writeEntry("monitor", checkMonitor->isChecked());
		sqConfig->writeEntry("show dialog", checkFAMMessage->isChecked());
		sqConfig->writeEntry("prefix", linePrefix->text());
		sqConfig->writeEntry("continue", checkContinue->isChecked());
	}

	return result;
}
