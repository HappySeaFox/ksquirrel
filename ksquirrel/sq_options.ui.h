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
    
    sqConfig->setGroup("Main");
    checkRestart->setChecked(sqConfig->readBoolEntry("restart", true));
    checkMinimize->setChecked(sqConfig->readBoolEntry("minimize to tray", true));
    checkOneInstance->setChecked(sqConfig->readBoolEntry("activate another", true));
    
    sqConfig->setGroup("Libraries");
    if(sqConfig->readBoolEntry("monitor", true)) checkMonitor->toggle();
    checkFAMMessage->setChecked(sqConfig->readBoolEntry("show dialog", true));
    
    sqConfig->setGroup("Interface");
    comboToolbarIconSize->setCurrentItem(sqConfig->readNumEntry("toolbar icon size", 0));
    tp = sqConfig->readNumEntry("create first", 0);
    ((QRadioButton*)(buttonGroupCreateFirst->find(tp)))->setChecked(true);
    
    sqConfig->setGroup("Fileview");
    comboIconIndex->setCurrentItem(sqConfig->readNumEntry("iCurrentIconIndex", 0));
    comboListIndex->setCurrentItem(sqConfig->readNumEntry("iCurrentListIndex", 0));
    tp = sqConfig->readNumEntry("sync type", 0);
    ((QRadioButton*)(buttonGroupSync->find(tp)))->setChecked(true);
    tp = sqConfig->readNumEntry("set path", 0);
    ((QRadioButton*)(buttonGroupSetPath->find(tp)))->setChecked(true);
    lineEditCustomDir->setText(sqConfig->readEntry("custom directory", "/"));
    tp = sqConfig->readNumEntry("click policy custom", 0);
    ((QRadioButton*)(buttonGroupClickPolicy->find(tp)))->setChecked(true);
    if(sqConfig->readBoolEntry("click policy system", true)) checkClickSystem->toggle();

    tp = sqViewType;
    pixmapShowView->setPixmap(a[tp]);
    ((QRadioButton*)(buttonGroupViewType->find(tp)))->setChecked(true);

    sqConfig->setGroup("GL view");
    tp = sqConfig->readNumEntry("zoom model", 0);
    ((QRadioButton*)(buttonGroupZoomModel->find(tp)))->setChecked(true);
    tp = sqConfig->readNumEntry("shade model", 0);
    ((QRadioButton*)(buttonGroupShadeModel->find(tp)))->setChecked(true);
    kColorGLbackground->setColor(sqGLViewBGColor);
    checkDrop->setChecked(sqConfig->readBoolEntry("enable drop", true));
    
    checkShowLinks->toggle();
/*
    tableLib->addColumn("Library");
    tableLib->addColumn("Info");
    tableLib->addColumn("Version");
    tableLib->addColumn("Extensions");
    
    
    tableLib->setColumnWidthMode(0, QListView::Maximum);
    tableLib->setColumnWidthMode(1, QListView::Maximum);
    tableLib->setColumnWidthMode(2, QListView::Maximum);
    tableLib->setColumnWidthMode(3, QListView::Maximum);
    
    for(int i = 0;i < 4;i++)
        tableLib->setColumnWidth(i, tableLib->columnWidth(i)+20);
*/    
    textPrefix->setText(sqLibPrefix);
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
    
//    listFilters->header()->hide();

    listFilters->setSorting(-1);

    	int count = sqFilters->count();
	QListViewItem *itemafter = 0L, *item;

	for(int i = 0;i < count;i++)
	{
		if(itemafter)
		    item = new QListViewItem(listFilters, itemafter, sqFilters->getFilterName(i), sqFilters->getFilterExt(i));
		else
		item = new QListViewItem(listFilters, sqFilters->getFilterName(i), sqFilters->getFilterExt(i));

		itemafter = item;

	    item->setRenameEnabled(0, true);
	    item->setRenameEnabled(1, true);
	    item->setMultiLinesEnabled(false);
	    
	    listFilters->insertItem(item);
	}

	pushFilterUp->setPixmap(sqLoader->loadIcon("up", KIcon::Desktop, KIcon::SizeSmall));
	pushFilterDown->setPixmap(sqLoader->loadIcon("down", KIcon::Desktop, KIcon::SizeSmall));
	
	listFilters->setCurrentItem(listFilters->firstChild());
	listFilters->clearSelection();
	listFilters->setSelected(listFilters->currentItem(), true);
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
    
    lineEditCustomDir->setText(s);
}


void SQ_Options::slotDirCache()
{
    QString s = KFileDialog::getExistingDirectory("/", this, "Choose a directory");
    
    lineCachePath->setText(s);
}


void SQ_Options::start()
{
    int result = exec();
    
    if(result == QDialog::Accepted)
    {
	//@todo write sqConfig and sync().
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

	sqConfig->setGroup("Main");
	sqConfig->writeEntry("minimize to tray", checkMinimize->isChecked());
	sqConfig->writeEntry("restart", checkRestart->isChecked());
	sqConfig->writeEntry("activate another", checkOneInstance->isChecked());

	sqConfig->setGroup("GL view");
	sqGLViewBGColor = kColorGLbackground->color();
	sqConfig->writeEntry("GL view background", sqGLViewBGColor.name());
	sqConfig->writeEntry("zoom model", buttonGroupZoomModel->id(buttonGroupZoomModel->selected()));
	sqConfig->writeEntry("shade model", buttonGroupShadeModel->id(buttonGroupShadeModel->selected()));
	sqConfig->writeEntry("enable drop", checkDrop->isChecked());
	
	sqConfig->setGroup("Libraries");
	sqConfig->writeEntry("monitor", checkMonitor->isChecked());
	sqConfig->writeEntry("show dialog", checkFAMMessage->isChecked());

	// @todo write filters
	QListViewItem *cur = listFilters->firstChild();
        sqFilters->clear();

	for(;cur;cur = cur->itemBelow())
	{
		FILTER tf = {cur->text(0), cur->text(1)};
		sqFilters->addFilter(tf);
	}
	
	sqConfig->sync();
    }
}


void SQ_Options::slotShowLinks( bool show )
{
    QString path;
    int totalRows = sqLibHandler->count();
    
    tableLib->clear();
    
    for(int i = 0;i < totalRows;i++)
    {
	SQ_LIBRARY tmplib = sqLibHandler->getLibByIndex(i);
	path = tmplib.libpath;
	
	if(!show)
	{
	    QFileInfo libfileinfo(path);
	    if(libfileinfo.isSymLink()) continue;
	}
	path.replace(sqLibPrefix, "");
	tableLib->insertItem(new QListViewItem(tableLib, path, QString(tmplib.fmt_quickinfo()), QString(tmplib.fmt_version()), QString(tmplib.sinfo)));
    }
}


void SQ_Options::slotClearFilter()
{
	QListViewItem *item = listFilters->currentItem();
    
	if(!item) return;
    
	listFilters->takeItem(item);
}

void SQ_Options::slotNewFilter()
{
	QListViewItem *itemafter = listFilters->lastItem(), *item;

	if(itemafter)
		item = new QListViewItem(listFilters, itemafter, "Name", "*.");
	else
		item = new QListViewItem(listFilters,  "Name", "*.");

	item->setRenameEnabled(0, true);
	item->setRenameEnabled(1, true);
	item->setMultiLinesEnabled(false);
	listFilters->insertItem(item);
	item->startRename(0);
}


void SQ_Options::slotFilterUp()
{
    QListViewItem *item = listFilters->currentItem();
    
    if(!item) return;
 
    QListViewItem *itemafter = item->itemAbove();
    
    if(!itemafter) return;
 
    itemafter->moveItem(item);
}

void SQ_Options::slotFilterDown()
{
    QListViewItem *item = listFilters->currentItem();
    
    if(!item) return;
 
    QListViewItem *itemafter = item->itemBelow();
    
    if(!itemafter) return;
 
    item->moveItem(itemafter);
}


void SQ_Options::slotFilterRenameRequest( QListViewItem *item, const QPoint &point, int pos )
{
    if(item)
	item->startRename(((pos>=0)?pos:0));
}
