/***************************************************************************
                          ksquirrel.cpp  -  description
                             -------------------
    begin                : Dec 10 2003
    copyright            : (C) 2003-2004 by ckult
    email                : squirrel-sf@yandex.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qpixmap.h>
#include <qprocess.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qvaluevector.h>
#include <qvaluelist.h>
#include <qmessagebox.h>
#include <qptrlist.h>
#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qcursor.h>

#include <kapp.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kurlcompletion.h>
#include <kwin.h>
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <ksystemtray.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kaction.h>
#include <kbookmarkmanager.h>
#include <kbookmarkmenu.h>
#include <kstandarddirs.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_tray.h"
#include "sq_treeview.h"
#include "sq_runprocess.h"
#include "sq_options.h"
#include "sq_about.h"
#include "sq_glviewgeneral.h"
#include "sq_glviewspec.h"
#include "sq_glviewwidget.h"
#include "sq_libraryhandler.h"
#include "sq_librarylistener.h"
#include "sq_externaltool.h"
#include "sq_externaltools.h"
#include "sq_fileviewfilter.h"
#include "sq_cachehandler.h"
#include "sq_config.h"
#include "sq_filters.h"
#include "sq_bookmarkowner.h"
#include "sq_hloptions.h"

Squirrel * Squirrel::App = 0;

Squirrel::Squirrel(SQ_HLOptions *HL_Options, QWidget *parent, const char *name) : KDockMainWindow (parent, name), DCOPObject(name), mainSplitter(0L)
{
	App = this;

	sqConfig = new SQ_Config("ksquirrelrc");
	sqLoader = KGlobal::iconLoader();
	sqWStack = 0L;
	sqCurrentURL = 0L;
	mainSplitter = 0L;
	tray = 0L;
	filterList = 0L;
	actionFilterMenu = 0L;
	sqExternalTool = 0L;
	sqTree = 0L;
	sqBookmarks = 0L;
	sqHighLevel = HL_Options;

	QString sqLibPrefix = sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/");
	if(!sqLibPrefix.endsWith("/"))	sqLibPrefix += "/";

	bool first_time = sqConfig->readBoolEntry("Main", "first_time", true);

	iconSizeList = new QValueVector<int>;
	iconSizeList->append(16);
	iconSizeList->append(22);
	iconSizeList->append(32);
	iconSizeList->append(48);

	toolbarIconSize = (*iconSizeList)[sqConfig->readNumEntry("Interface", "toolbar icon size", 0)];
	createFirst = sqConfig->readNumEntry("Interface", "create first", 0);

	// create view
	sqViewType = (Squirrel::ViewType)sqConfig->readNumEntry("Interface", "ViewType",  Squirrel::SQuirrel);

	InitExternalTools();

	sqCache = new SQ_CacheHandler;

	switch(sqViewType)
	{
        	case Squirrel::SQuirrel:		createWidgetsLikeSQuirrel(); break;
        	case Squirrel::Gqview:		createWidgetsLikeGqview();	 break;
        	case Squirrel::Kuickshow:	createWidgetsLikeKuickshow();break;
        	case Squirrel::WinViewer:	createWidgetsLikeWinViewer();  break;
        	case Squirrel::Xnview:		createWidgetsLikeXnview();  break;
        	case Squirrel::ShowImg:		createWidgetsLikeShowImg();  break;
        	case Squirrel::Browser:		createWidgetsLikeBrowser();  break;

        	default:
         		createWidgetsLikeSQuirrel();
	}

	// !! Find libraries
	sqLibHandler = new SQ_LibraryHandler;
	sqLibUpdater = new SQ_LibraryListener;
	connect(sqLibUpdater, SIGNAL(finishedInit()), SLOT(slotSetFile()));
	sqLibUpdater->openURL(KURL(sqLibPrefix), false, true);

	handlePositionSize();
	show();

	if(first_time)
		if(QMessageBox::information(sqApp, "Squirrel", "You are running ksquirrel first time (config file dosn't exist).\nI am using now default values.\n\nPlease visit \"Options\" dialog to adjust appearence.\n\nDo it now ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
			pAConfigure->activate();
}

Squirrel::~Squirrel()
{}

void Squirrel::handlePositionSize()
{
	int posX, posY, sizeX, sizeY;

	if(sqConfig->readBoolEntry("Interface", "save pos", true))
	{
		posX = sqConfig->readNumEntry("Interface", "posX", 0);
		posY = sqConfig->readNumEntry("Interface", "posY", 0);
	}
	else
		posX = posY = 0;

	if(sqConfig->readBoolEntry("Interface", "save size", true))
	{
		sizeX = sqConfig->readNumEntry("Interface", "sizeX", 0);
		sizeY = sqConfig->readNumEntry("Interface", "sizeY", 0);
	}
	else
	{
		sizeX = QApplication::desktop()->width();
		sizeY = QApplication::desktop()->height();
	}

	move(posX, posY);
	resize(sizeX, sizeY);
}

void Squirrel::slotExecuteRunMenu()
{
	pmLaunch->exec(QCursor::pos());
}

void Squirrel::slotOptions()
{
	SQ_Options optd(this, "sq_options", true);

	if(optd.start() != QDialog::Accepted)
		return;

	sqGLView->getGL()->setClearColor(); // values are taken from sqConfig
	sqGLView->getGL()->setTextureParams();
	sqGLView->getGL()->setAcceptDrops(sqConfig->readBoolEntry("GL view", "enable drop", true));
	sqGLView->getGL()->setZoomFactor(sqConfig->readNumEntry("GL view", "zoom", 25));
	sqGLView->getGL()->setMoveFactor(sqConfig->readNumEntry("GL view", "move", 5));
	sqLibUpdater->setAutoUpdate(sqConfig->readBoolEntry("Libraries", "monitor", true));
	sqLibHandler->clear();
	sqLibUpdater->openURL(KURL(sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/")), false, true);

	if(sqConfig->readBoolEntry("Main", "sync", true))
		sqConfig->sync();
}

void Squirrel::closeEvent(QCloseEvent *ev)
{
	hide();

	if(sqConfig->readBoolEntry("Main", "minimize to tray", false))
	{
		if(!tray)
		{
			tray = new SQ_SystemTray;
			tray->setPixmap(sqLoader->loadIcon("redo", KIcon::Desktop, KIcon::SizeSmall));
			tray->show();
		}
		tray->show();
		ev->ignore();
	}
	else
	{
		sqGLView->hide();
		sqFilters->writeEntries();
		sqExternalTool->writeEntries();

		sqConfig->setGroup("Filters");
		sqConfig->writeEntry("menuitem both", sqFilters->getShowBoth());

		sqConfig->setGroup("Fileview");
		sqConfig->writeEntry("last visited", (sqWStack->getURL()).path());
		sqConfig->deleteGroup("History");

		if(sqConfig->readBoolEntry("Fileview", "history", true) && sqCurrentURL)
		{
			QString tmp;
			int i = 1;
			sqConfig->setGroup("History");

			QStringList urls = sqCurrentURL->historyItems();

			QValueList<QString>::iterator   BEGIN = urls.begin();
			QValueList<QString>::iterator      END = urls.end();

			for(QValueList<QString>::iterator it = BEGIN;it != END;it++)
			{
				tmp.sprintf("%d", i++);
				sqConfig->writeEntry(tmp, *it);
			}
		}

		sqConfig->setGroup("Main");
		sqConfig->writeEntry("first_time", false);

		if(sqConfig->readBoolEntry("Interface", "save pos", true))
		{
			sqConfig->writeEntry("posX", x());
			sqConfig->writeEntry("posY", y());
		}

		if(sqConfig->readBoolEntry("Interface", "save size", true))
		{
			sqConfig->writeEntry("sizeX", width());
			sqConfig->writeEntry("sizeY", height());
		}

		sqConfig->sync();
		ev->accept();
	}
}

void Squirrel::slotRaiseListView()
{
	sqWStack->raiseWidget(0);
}

void Squirrel::slotRaiseIconView()
{
	sqWStack->raiseWidget(1);
}

void Squirrel::slotRaiseDetailView()
{
	sqWStack->raiseWidget(2);
}

void Squirrel::InitRunMenu()
{
	QString program, name;

	pMenuProc = new SQ_RunProcess;

	for(int i = 1;;i++)
	{
		program = sqConfig->readEntry("Run program", QString("%1").arg(i, 0, 10), "");

		if(program == "") break;

		name = sqConfig->readEntry("Run name", QString("%1").arg(i, 0, 10), "");
		pMenuProc->AddItem(name, program);
	}

	pmLaunch = new QPopupMenu;

	int i, getcount = pMenuProc->GetCount(), param = 0, id;

	for(i = 0;i < getcount;i++)
		if(pMenuProc->GetName(i) != "SEPARATOR")
		{
			id = pmLaunch->insertItem(pMenuProc->GetPixmap(i), pMenuProc->GetName(i));
			pmLaunch->setItemParameter(id, param++);
		}
		else
			pmLaunch->insertSeparator();

	connect(pmLaunch, SIGNAL(activated(int)), SLOT(slotRunCommand(int)));
}

void Squirrel::CreateLocationToolbar()
{
	int 		i;
	QString	name, tmp;

	sqCurrentURL = new KHistoryCombo(true, 0, "history combobox");

	new QToolButton(sqLoader->loadIcon("button_cancel", KIcon::Desktop, KIcon::SizeSmall), "Clear history", QString::null, sqCurrentURL, SLOT(clearHistory()), pTLocation);
	new QToolButton(sqLoader->loadIcon("locationbar_erase", KIcon::Desktop, KIcon::SizeSmall), "Clear adress", QString::null, sqCurrentURL, SLOT(clearEdit()), pTLocation);
	new QLabel("URL:", pTLocation, "kde toolbar widget");

	sqCurrentURL->reparent(pTLocation, 0, QPoint(0,0));

	pTLocation->setStretchableWidget(pCurrentURL);
	sqCurrentURL->setDuplicatesEnabled(false);
	sqCurrentURL->setSizeLimit(30);

	new QToolButton(sqLoader->loadIcon("goto", KIcon::Desktop, KIcon::SizeSmall), "Go!", QString::null, this, SLOT(slotGo()), pTLocation);

	KURLCompletion *pURLCompletion = new KURLCompletion(KURLCompletion::DirCompletion);
	pURLCompletion->setDir("/");

	sqCurrentURL->setCompletionObject(pURLCompletion);
	sqCurrentURL->setAutoDeleteCompletionObject(true);
	sqCurrentURL->clearHistory();

	for(i = 1;;i++)
	{
		name.sprintf("%d", i);
		tmp = sqConfig->readEntry("History", name, "");

		if(tmp == "") break;

		sqCurrentURL->addToHistory(tmp);
	}
}

void Squirrel::InitFilterMenu()
{
	FILTER tmp_filter;
	QString ext, tmp;
	unsigned int i;
	bool first = false;

	if(!filterList)
	{
		first = true;
		filterList = new SQ_FileviewFilter;
		actionFilterMenu = new KPopupMenu;

		sqConfig->setGroup("Filters");

		for(i = 1;;i ++)
		{
			ext.sprintf("%d", i);
			tmp = sqConfig->readEntry("Filters", ext, "");

			if(tmp == "") break;

			tmp_filter.name = tmp;
			tmp = sqConfig->readEntry("Filters ext", ext, "");
			tmp_filter.filter = tmp;

			filterList->addFilter(tmp_filter);
		}
	}

	actionFilterMenu->clear();
	int id;
	bool both = sqConfig->readBoolEntry("Filters", "menuitem both", true);

	for(i = 0;i < filterList->count();i++)
	{
		if(both)
			id = actionFilterMenu->insertItem(filterList->getFilterName(i) + "  (" + filterList->getFilterExt(i) + ")");
		else
			id = actionFilterMenu->insertItem(filterList->getFilterName(i));

		actionFilterMenu->setItemParameter(id, i);
	}
/*
	if(filterList->count() && first)
		slotSetFilter(7000);
*/
	disconnect(actionFilterMenu, SIGNAL(activated(int)), this, SLOT(slotSetFilter(int)));
	connect(actionFilterMenu, SIGNAL(activated(int)), SLOT(slotSetFilter(int)));
}

void Squirrel::slotGo()
{
	sqWStack->setURL(pCurrentURL->currentText(), true);
}

void Squirrel::slotSetFilter(int id)
{
	int index = actionFilterMenu->itemParameter(id);
	sqWStack->setNameFilter(((*filterList)[index]).filter);
}

void Squirrel::slotGLView()
{
	sqGLView->show();
	KWin::setActiveWindow(sqGLView->winId());
}

void Squirrel::createWidgetsLikeSQuirrel()
{
	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	pTLocation = new KToolBar(this, QMainWindow::Top, true, "Location toolbar");
	CreateLocationToolbar();

	CreateStatusBar(statusBar());

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	
	mainSplitter = new QSplitter(mainDock);

	sqWStack = new SQ_WidgetStack(mainSplitter);
	sqWStack->raiseFirst(createFirst);

	mainDock->setWidget(mainSplitter);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	sqTree = new SQ_TreeView(mainSplitter);
	mainSplitter->moveToFirst(sqTree);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURL(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURL(const QString&)));

	CreateToolbar(fileTools);

	menubar = menuBar();
	CreateMenu(menubar);

	sqGLView = new SQ_GLViewSpec;

	setSplitterSizes(1, 4);
}

void Squirrel::createWidgetsLikeGqview()
{
	CreateStatusBar(statusBar());

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	mainDock->dockManager()->setSplitterOpaqueResize(true);
	mainSplitter = new QSplitter(Qt::Horizontal, mainDock);
	mainDock->setWidget(mainSplitter);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	QVBox *V = new QVBox(mainSplitter);
	V->setMinimumWidth(20);

	KMenuBar *pMenuBar = new KMenuBar(V);
	KToolBar *pToolbar = new KToolBar(sqApp, V, true, "GQview toolbar");

	pTLocation = new KToolBar(sqApp, V, true, "Location toolbar");
	pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	CreateLocationToolbar();

	sqWStack = new SQ_WidgetStack(V);
	sqWStack->raiseFirst(createFirst);

	CreateToolbar(pToolbar);
	CreateMenu(pMenuBar);

	// "GQview" specific
	sqWStack->pAIconBigger->unplug(pToolbar);
	sqWStack->pAIconSmaller->unplug(pToolbar);
	pAGLView->unplug(pToolbar);
	sqWStack->pANewDir->unplug(pToolbar);
	pAPrevFile->unplug(pToolbar);
	pANextFile->unplug(pToolbar);
	pAGotoTray->unplug(pToolbar);

	sqGLView = new SQ_GLViewGeneral(mainSplitter);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURL(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURL(const QString&)));

	setSplitterSizes(10, 21);
}

void Squirrel::createWidgetsLikeKuickshow()
{
	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	CreateStatusBar(statusBar());

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");

	//@todo option "show content when resizing" [true|false]
	mainDock->dockManager()->setSplitterOpaqueResize(true);

	sqWStack = new SQ_WidgetStack;
	sqWStack->raiseFirst(createFirst);

	mainDock->setWidget(sqWStack);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	CreateToolbar(fileTools);

	menubar = menuBar();
	CreateMenu(menubar);

	sqGLView = new SQ_GLViewSpec;
}

void Squirrel::createWidgetsLikeWinViewer()
{
	CreateStatusBar(statusBar());

	// WinViewer specific
	sqStatus->removeWidget(sqSBdirInfo);
	sqStatus->removeWidget(sqSBcurFileInfo);
	sqStatus->removeWidget(sqSBfileIcon);
	sqStatus->removeWidget(sqSBfileName);

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	mainDock->dockManager()->setSplitterOpaqueResize(true);
	QVBox *V = new QVBox(mainDock);
	mainDock->setWidget(V);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	KMenuBar *pMenuBar = new KMenuBar(V);

	sqWStack = new SQ_WidgetStack(V);
	sqWStack->raiseFirst(createFirst);
	sqGLView = new SQ_GLViewGeneral(V);

	QHBox *H = new QHBox(V);
	QLabel *ltmp = new QLabel("", H);
	KToolBar *pToolbarTools = new KToolBar(sqApp, H, true, "WinViewer tools");
	QLabel *ltmp2 = new QLabel("", H);

	/* We'll create toolbar manually, w/o call "CreateToolbar()", cause of we have to unplug too many KAction's  */
	CreateActions();
	pToolbarTools->setIconSize(toolbarIconSize, true);
	sqWStack->pAHome->plug(pToolbarTools);
	sqWStack->pANewDir->plug(pToolbarTools);
	pAPrevFile->plug(pToolbarTools);
	pANextFile->plug(pToolbarTools);
	pAExtTools->plug(pToolbarTools);
	pAFilters->plug(pToolbarTools);
	pAConfigure->plug(pToolbarTools);
	pAGotoTray->plug(pToolbarTools);
	pAExit->plug(pToolbarTools);
	CreateMenu(pMenuBar);

	H->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	H->setStretchFactor(ltmp, 1);
	H->setStretchFactor(pToolbarTools, 0);
	H->setStretchFactor(ltmp2, 1);

	// WinViewer specific
	pARaiseIconView->setEnabled(false);
	pARaiseListView->setEnabled(false);
	pARaiseDetailView->setEnabled(false);
	sqWStack->hide();
}

void Squirrel::createWidgetsLikeXnview()
{
	CreateStatusBar(statusBar());

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	mainSplitter = new QSplitter(Qt::Vertical, mainDock);
	mainDock->setWidget(mainSplitter);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	QSplitter *H = new QSplitter(Qt::Horizontal, mainSplitter);
	sqWStack = new SQ_WidgetStack(H);
	sqWStack->raiseFirst(createFirst);

	sqTree = new SQ_TreeView(H);
	H->moveToFirst(sqTree);

	sqGLView = new SQ_GLViewGeneral(mainSplitter);

	CreateToolbar(toolBar());
	CreateMenu(menuBar());

	// "XnView" specific
	pAGLView->unplug(toolBar());

	setSplitterSizes(1, 3);
}

void Squirrel::createWidgetsLikeShowImg()
{
	CreateStatusBar(statusBar());

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	mainDock->dockManager()->setSplitterOpaqueResize(true);
	mainSplitter = new QSplitter(Qt::Horizontal, mainDock);
	mainDock->setWidget(mainSplitter);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	QSplitter *mainSplitterV = new QSplitter(Qt::Vertical, mainSplitter);
	
	sqWStack = new SQ_WidgetStack(mainSplitterV);

	QVBox *V = new QVBox(mainSplitterV);
	V->setMinimumWidth(20);

	KMenuBar *pMenuBar = new KMenuBar(V);
	KToolBar *pToolbar = new KToolBar(sqApp, V, true, "GQview toolbar");

	pTLocation = new KToolBar(sqApp, V, true, "Location toolbar");
	pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	CreateLocationToolbar();

	sqTree = new SQ_TreeView(V);

	sqWStack->raiseFirst(createFirst);

	CreateToolbar(pToolbar);
	CreateMenu(pMenuBar);

	// "GQview" specific
	sqWStack->pAIconBigger->unplug(pToolbar);
	sqWStack->pAIconSmaller->unplug(pToolbar);
	pAGLView->unplug(pToolbar);
	sqWStack->pANewDir->unplug(pToolbar);
	pAPrevFile->unplug(pToolbar);
	pANextFile->unplug(pToolbar);
	pAGotoTray->unplug(pToolbar);

	sqGLView = new SQ_GLViewGeneral(mainSplitter);

	mainSplitterV->moveToFirst(V);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURL(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURL(const QString&)));

	setSplitterSizes(10, 21);

	QValueList<int> l;
	l.append(2);
	l.append(2);
	mainSplitterV->setSizes(l);
}

void Squirrel::createWidgetsLikeBrowser()
{
	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	mainDock->dockManager()->setSplitterOpaqueResize(true);
	viewBrowser = new QWidgetStack(mainDock, "SQ_BROWSER_WIDGET_STACK");
	mainDock->setWidget(viewBrowser);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	QVBox *b1 = new QVBox(viewBrowser);
	KMenuBar *pMenuBar = new KMenuBar(b1);
	KToolBar *t1 = new KToolBar(b1);
	pTLocation = new KToolBar(sqApp, b1, true, "Location toolbar");
	pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	CreateLocationToolbar();
	sqWStack = new SQ_WidgetStack(b1);
	KStatusBar *stat = new KStatusBar(b1);
	CreateStatusBar(stat);

	sqWStack->raiseFirst(createFirst);

	b1->setStretchFactor(sqWStack, 1);

	CreateToolbar(t1);
	CreateMenu(pMenuBar);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURL(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURL(const QString&)));

	QVBox *b2 = new QVBox(viewBrowser);
	sqGLView = new SQ_GLViewGeneral(b2);

	viewBrowser->addWidget(b1, 0);
	viewBrowser->addWidget(b2, 1);
}

void Squirrel::setSplitterSizes(int s1, int s2)
{
	QValueList<int> l;
	l.append(s1);
	l.append(s2);
	mainSplitter->setSizes(l);
}

void Squirrel::CreateStatusBar(KStatusBar *bar)
{
	sbar = bar;
	sbar->setSizeGripEnabled(true);
	sbar->show();

	sqSBdirInfo = new QLabel(sbar);
	sqSBcurFileInfo = new QLabel(sbar);
	QHBox *vb = new QHBox(sbar);

	sqSBfileIcon = new QLabel(vb);
	sqSBfileIcon->setScaledContents(false);

	sqSBfileName = new QLabel(vb);
	sqSBDecoded = new QLabel(sbar);
	sqSBDecoded->setText("No files decoded");
	sqSBGLreport = new QLabel( "zoom: ", sbar);
	QLabel *levak = new QLabel(sbar);

	sbar->addWidget(sqSBdirInfo, 0, true);
	sbar->addWidget(sqSBcurFileInfo, 0, true);
	sbar->addWidget(vb, 0, true);
	sbar->addWidget(sqSBDecoded, 0, true);
	sbar->addWidget(sqSBGLreport, 0, true);
	sbar->addWidget(levak, 1, true);
}

void Squirrel::CreateMenu(KMenuBar *menubar)
{
	pop_file = new KPopupMenu(menubar);
	pop_edit = new KPopupMenu(menubar);
	pop_view = new KPopupMenu(menubar);

	InitRunMenu();
	InitFilterMenu();
	InitBookmarks();

	menubar->insertItem("&File", pop_file);
	menubar->insertItem("&Edit", pop_edit);
	menubar->insertItem("&View", pop_view);
	bookmarks->plug(menubar);
	menubar->insertItem("&Launch", pmLaunch);
	menubar->insertItem("Fi&lter", actionFilterMenu);
	menubar->insertItem("&Help", helpMenu());

	sqWStack->pAHome->plug(pop_file);
	sqWStack->pAUp->plug(pop_file);
	sqWStack->pABack->plug(pop_file);
	sqWStack->pAForw->plug(pop_file);
	sqWStack->pANewDir->plug(pop_file);
	pop_file->insertSeparator();
	sqWStack->pAProp->plug(pop_file);
	sqWStack->pADelete->plug(pop_file);
	pop_file->insertSeparator();
	pANextFile->plug(pop_file);
	pAPrevFile->plug(pop_file);
	pop_file->insertSeparator();
	pAExit->plug(pop_file);

	pAGotoTray->plug(pop_view);
	pop_view->insertSeparator();
	sqWStack->pAIconBigger->plug(pop_view);
	sqWStack->pAIconSmaller->plug(pop_view);
	pop_view->insertSeparator();
	pARaiseIconView->plug(pop_view);
	pARaiseListView->plug(pop_view);
	pARaiseDetailView->plug(pop_view);
	pARescan->plug(pop_edit);
	pAExtTools->plug(pop_edit);
	pAFilters->plug(pop_edit);
	pAConfigure->plug(pop_edit);
}

void Squirrel::CreateToolbar(KToolBar *tools)
{
	CreateActions();
	tools->setIconSize(toolbarIconSize, true);

	sqWStack->pABack->plug(tools);
	sqWStack->pAForw->plug(tools);
	sqWStack->pAUp->plug(tools);
	sqWStack->pAHome->plug(tools);
	sqWStack->pANewDir->plug(tools);

	pAPrevFile->plug(tools);
	pANextFile->plug(tools);

	pARaiseListView->plug(tools);
	pARaiseIconView->plug(tools);
	pARaiseDetailView->plug(tools);

	sqWStack->pAIconBigger->plug(tools);
	sqWStack->pAIconSmaller->plug(tools);

	pAGLView->plug(tools);
	pAExtTools->plug(tools);
	pAFilters->plug(tools);
	pAConfigure->plug(tools);
	pAGotoTray->plug(tools);
	pAExit->plug(tools);
}

void Squirrel::CreateActions()
{
	pAExit = new KAction("Quit", QIconSet(sqLoader->loadIcon("exit", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("exit", KIcon::Desktop, toolbarIconSize)), KShortcut(ALT+Key_Q), this, SLOT(close()), actionCollection(), "SQ close");
  	pAConfigure = new KAction("Options", QIconSet(sqLoader->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("configure", KIcon::Desktop, toolbarIconSize)), KShortcut(CTRL+Key_P), this, SLOT(slotOptions()), actionCollection(), "SQ edit options");
  	pAGLView = new KAction("GLview", QIconSet(sqLoader->loadIcon("desktop", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("desktop", KIcon::Desktop, toolbarIconSize)), KShortcut(CTRL+Key_H), this, SLOT(slotGLView()), actionCollection(), "SQ gl view widget");
	pANextFile = new KAction("Next file", QIconSet(sqLoader->loadIcon("next", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("next", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, SLOT(slotNextFile()), actionCollection(), "SQ select next file");
	pAPrevFile = new KAction("Previous file", QIconSet(sqLoader->loadIcon("previous", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("previous", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, SLOT(slotPreviousFile()), actionCollection(), "SQ select previous file");
	pARescan = new KAction("Rescan libraries", QIconSet(sqLoader->loadIcon("reload", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("reload", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, 0, actionCollection(), "SQ rescan libraries");
	pAExtTools = new KAction("Configure external tools ...", QIconSet(sqLoader->loadIcon("memory", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("memory", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");
	pAFilters = new KAction("Configure filters ...", QIconSet(sqLoader->loadIcon("filefind", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("filefind", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, SLOT(slotFilters()), actionCollection(), "SQ filters");
	pAGotoTray = new KAction("Goto tray", QIconSet(sqLoader->loadIcon("compfile", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("compfile", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, SLOT(slotGotoTray()), actionCollection(), "SQ goto tray");

	pARaiseListView = new KRadioAction("Raise 'list' view", QIconSet(sqLoader->loadIcon("view_icon", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_icon", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
	pARaiseIconView = new KRadioAction("Raise 'icon' view", QIconSet(sqLoader->loadIcon("view_choose", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_choose", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
	pARaiseDetailView = new KRadioAction("Raise 'detailed' view", QIconSet(sqLoader->loadIcon("view_detailed", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_detailed", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
	pARaiseListView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseIconView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseDetailView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
}

void Squirrel::slotGotoTray()
{
	if(!tray)
	{
		tray = new SQ_SystemTray;
		tray->setPixmap(sqLoader->loadIcon("redo", KIcon::Desktop, KIcon::SizeSmall));
		tray->show();
	}

	hide();
}

void Squirrel::slotNextFile()
{
	switch(curViewType)
	{
        	case Squirrel::SQuirrel:
        	case Squirrel::Kuickshow:
			sqWStack->slotNext();
		break;

		case Squirrel::Gqview:
        	case Squirrel::WinViewer:
        	case Squirrel::Xnview:
			sqWStack->emitNextSelected();
		return;

		case Squirrel::Browser:
		break;

		default: return;
	}
}

void Squirrel::slotPreviousFile()
{
	switch(curViewType)
	{
        	case Squirrel::SQuirrel:
        	case Squirrel::Kuickshow:
			sqWStack->slotPrevious();
		break;

		case Squirrel::Gqview:
        	case Squirrel::WinViewer:
        	case Squirrel::Xnview:
			sqWStack->emitPreviousSelected();
		return;

		case Squirrel::Browser:
		break;

		default: return;
	}
}

bool Squirrel::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
	if(fun == "control(QString)")
	{
		QDataStream args(data, IO_ReadOnly);
		QString arg1;
		args >> arg1;
		control(arg1);
		replyType = "void";
		return true;
	}

	return DCOPObject::process(fun, data, replyType, replyData);
}

QCStringList Squirrel::functions()
{
	QCStringList result = DCOPObject::functions();
	result << "void control(QString)";
	return result;
}

void Squirrel::slotRunCommand(int id)
{
	int index = pmLaunch->itemParameter(id);
	pMenuProc->slotRunCommand(index);
}

void Squirrel::InitExternalTools()
{
	QString str, tmp;
	int i;
	SQ_EXT_TOOL tmp_tool;
/*
	if(sqExternalTool)
	{
		sqExternalTool->clear();
		delete sqExternalTool;
	}
*/
	sqExternalTool	 = new SQ_ExternalTool;

	for(i = 1;;i ++)
	{
		str.sprintf("%d", i);
		tmp = sqConfig->readEntry("External tool name", str, "");

         	if(tmp == "") break;

		tmp_tool.name = tmp;
		tmp = sqConfig->readEntry("External tool program", str, "");
		tmp_tool.command = tmp;

		sqExternalTool->addTool(tmp_tool);
	}
}

void Squirrel::slotFilters()
{
	SQ_Filters f(this);

	if(f.start() != QDialog::Accepted) return;

	InitFilterMenu();
}

void Squirrel::slotExtTools()
{
	SQ_ExternalTools etd(this, "sq_exttools", true);

	if(etd.start() != QDialog::Accepted) return;

	sqWStack->reInitToolsMenu();
}

void Squirrel::raiseGLWidget()
{
	switch(curViewType)
	{
        	case Squirrel::SQuirrel:
        	case Squirrel::Kuickshow:
			sqGLView->show();
			KWin::setActiveWindow(sqGLView->winId());
		break;

		case Squirrel::Gqview:
        	case Squirrel::WinViewer:
        	case Squirrel::Xnview:
         	case Squirrel::ShowImg:
		return;

		case Squirrel::Browser:
			viewBrowser->raiseWidget(1);
		break;

		default: return;
	}
}

void Squirrel::slotCloseGLWidget()
{
	switch(curViewType)
	{
        	case Squirrel::SQuirrel:
        	case Squirrel::Kuickshow:
			sqGLView->close();
		break;

		case Squirrel::Gqview:
        	case Squirrel::WinViewer:
        	case Squirrel::Xnview:
         	case Squirrel::ShowImg:
		return;

		case Squirrel::Browser:
			viewBrowser->raiseWidget(0);
		break;

		default: return;
	}
}

void Squirrel::InitBookmarks()
{
	QString file = locate("data", "kfile/bookmarks.xml");

	if(file.isEmpty())
		file = locateLocal("data", "kfile/bookmarks.xml");

	KBookmarkManager *bmanager=KBookmarkManager::managerForFile(file, false);
	bmanager->setUpdate(true);
	bmanager->setShowNSBookmarks(false);

	sqBookmarks = new SQ_BookmarkOwner(this);

	bookmarks = new KActionMenu("&Bookmarks", "bookmark", actionCollection(), "bookmarks");
	new KBookmarkMenu(bmanager, bookmarkOwner, bookmarks->popupMenu(), this->actionCollection(), true);

	connect(sqBookmarks, SIGNAL(openURL(const KURL&)), sqWStack, SLOT(setURL(const KURL&)));
}

void Squirrel::slotSetFile()
{
	if(!sqHighLevel->HL_url.isEmpty())
        {
		if(!sqHighLevel->HL_url.fileName(false).isEmpty())
			sqWStack->emitSelected(sqHighLevel->HL_url.path());
	}

	sqHighLevel->HL_url = "";
}

int Squirrel::findProtocol(const QString &proc)
{
	const int P_NUM = 2;

	static const QString protocols[] = {
		"SQ_ACTIVATE",
		"SQ_ACTIVATEWITHFILE"
		};

	for(int i = 0;i < P_NUM;i++)
		if(protocols[i].startsWith(proc))
			return i;

	return -1;
}

void Squirrel::control(const QString &str)
{
	int id = findProtocol(str);

	switch(id)
	{
		case 0:
			show();
			KWin::setActiveWindow(winId());
		break;
		
		case 1:
		{
			show();
			KWin::setActiveWindow(winId());

			QString path = str.right(str.length() - 19);
			QFileInfo fm(path);
			KURL url = path;

			if(url.fileName(false).isEmpty())
				sqWStack->setURL(url, true);
			else
				sqWStack->emitSelected(path);
		}
		break;

		default:
			printf("control(): unknown parameter: \"%s\"\n", str.ascii());
	}
}
