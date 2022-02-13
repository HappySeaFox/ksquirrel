/***************************************************************************
                          ksquirrel.cpp  -  description
                             -------------------
    begin                : Dec 10 2003
    copyright            : (C) 2003-2004 by CKulT
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
#include <qhbox.h>
#include <qvaluevector.h>
#include <qvaluelist.h>
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
#include <klocale.h>
#include <kdeversion.h>
#include <kcombobox.h>
#include <kstdaction.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_tray.h"
#include "sq_treeview.h"
#include "sq_options.h"
#include "sq_about.h"
#include "sq_glwidget.h"
#include "sq_glview.h"
#include "sq_libraryhandler.h"
#include "sq_librarylistener.h"
#include "sq_externaltool.h"
#include "sq_externaltools.h"
#include "sq_config.h"
#include "sq_filters.h"
#include "sq_bookmarkowner.h"
#include "sq_hloptions.h"
#include "sq_quickbrowser.h"
#include "sq_pixmapcache.h"
#include "sq_thumbnailsize.h"
#include "sq_pixmapcache.h"

#if KDE_IS_VERSION(3,2,0)
	#define SQ_ACTIVATE_WINDOW(id) KWin::activateWindow(id)
#else
	#define SQ_ACTIVATE_WINDOW(id) KWin::setActiveWindow(id)
#endif

Squirrel * Squirrel::App = 0;

Squirrel::Squirrel(SQ_HLOptions *HL_Options, QWidget *parent, const char *name) : KDockMainWindow (parent, name), DCOPObject(name)
{
	App = this;

//	KApplication::setDoubleClickInterval(10);

	sqConfig = new SQ_Config("ksquirrelrc");
	sqLoader = kapp->iconLoader();
	sqHighLevel = HL_Options;
	old_id = -1;

	sqThumbSize = new SQ_ThumbnailSize((SQ_ThumbnailSize::Size)sqConfig->readNumEntry("Thumbnails", "size", SQ_ThumbnailSize::Huge));
	sqCache = new SQ_PixmapCache;

	sqWStack = 0L;
	sqCurrentURL = 0L;
	sqGLView = 0L;
	mainSplitterV = mainSplitter = 0L;
	tray = 0L;
	actionFilterMenu = 0L;
	sqExternalTool = 0L;
	sqTree = 0L;
	sqBookmarks = 0L;
	sqFiltersName = 0L;

	first_time = sqConfig->readBoolEntry("Main", "first_time", true);

	if(first_time)
		writeDefaultEntries();

	createFirst = sqConfig->readNumEntry("Interface", "last view", 0);

	// create view
	sqViewType = (Squirrel::ViewType)sqConfig->readNumEntry("Interface", "view type",  Squirrel::SQuirrel);

	initExternalTools();
	sqExternalTool->getNewPopupMenu();

	switch(sqViewType)
	{
		case Squirrel::SQuirrel:			createWidgetsLikeSQuirrel();	break;
		case Squirrel::Gqview:			createWidgetsLikeGqview();	break;
		case Squirrel::Kuickshow:		createWidgetsLikeKuickshow();	break;
		case Squirrel::NoComponents:	createWidgetsLikeNoComponents();  break;
		case Squirrel::Xnview:			createWidgetsLikeXnview();	break;
		case Squirrel::ShowImg:		createWidgetsLikeShowImg();	break;
		case Squirrel::Browser:			createWidgetsLikeBrowser();	break;

 	       	default:
         		createWidgetsLikeSQuirrel();
	}

	// !! Find libraries
	sqLibHandler = new SQ_LibraryHandler;
	sqLibUpdater = new SQ_LibraryListener;
	connect(sqLibUpdater, SIGNAL(finishedInit()), SLOT(slotSetFile()));
	slotRescan();

	handlePositionSize();

	if(first_time)
	{
		applyDefaultSettings();
		pAConfigure->activate();
	}

	show();
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
		sizeX = sqConfig->readNumEntry("Interface", "sizeX", 800);
		sizeY = sqConfig->readNumEntry("Interface", "sizeY", 600);
	}
	else
	{
		sizeX = QApplication::desktop()->width();
		sizeY = QApplication::desktop()->height();
	}

	move(posX, posY);
	resize(sizeX, sizeY);
}

void Squirrel::slotOptions()
{
	SQ_Options optd(this, "sq_options", true);

	if(first_time)
	{
		optd.setGeometry(QApplication::desktop()->width() / 2 - optd.width() / 2, QApplication::desktop()->height() / 2 - optd.height() / 2, optd.width(), optd.height());
		first_time = false;
	}

	if(optd.start() == QDialog::Accepted)
		applyDefaultSettings();
}

void Squirrel::closeEvent(QCloseEvent *ev)
{
	hide();

	if(sqConfig->readBoolEntry("Main", "minimize to tray", false))
	{
		if(!tray)
		{
			tray = new SQ_SystemTray;
			tray->show();
		}
		tray->show();
		ev->ignore();
	}
	else
	{
		createPostSplash();
		
		sqGLView->hide();
		saveValues();
		sqConfig->sync();
		sqWStack->cleanUnpacked();

		if(!sqConfig->readBoolEntry("Thumbnails", "dont write", true))
			sqCache->sync();

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

void Squirrel::slotRaiseThumbView()
{
	sqWStack->raiseWidget(3);
}

void Squirrel::createLocationToolbar(KToolBar *pTLocation)
{
	sqCurrentURL = new KHistoryCombo(true, 0, "history combobox");

	new QToolButton(sqLoader->loadIcon("button_cancel", KIcon::Desktop, KIcon::SizeSmall), i18n("Clear history"), QString::null, sqCurrentURL, SLOT(clearHistory()), pTLocation);
	new QToolButton(sqLoader->loadIcon("locationbar_erase", KIcon::Desktop, KIcon::SizeSmall), i18n("Clear adress"), QString::null, sqCurrentURL, SLOT(clearEdit()), pTLocation);
	new QLabel("URL:", pTLocation, "kde toolbar widget");

	sqCurrentURL->reparent(pTLocation, 0, QPoint(0,0));

	pTLocation->setStretchableWidget(pCurrentURL);
	sqCurrentURL->setDuplicatesEnabled(false);
	sqCurrentURL->setSizeLimit(30);

	new QToolButton(sqLoader->loadIcon("goto", KIcon::Desktop, KIcon::SizeSmall), i18n("Go!"), QString::null, this, SLOT(slotGo()), pTLocation);

	KURLCompletion *pURLCompletion = new KURLCompletion(KURLCompletion::DirCompletion);
	pURLCompletion->setDir("/");

	sqCurrentURL->setCompletionObject(pURLCompletion);
	sqCurrentURL->setAutoDeleteCompletionObject(true);
	sqCurrentURL->clearHistory();
	sqCurrentURL->setHistoryItems(sqConfig->readListEntry("History", "items"), true);
}

void Squirrel::initFilterMenu()
{
	QString ext, tmp;
	unsigned int i = 0;

	if(!sqFiltersName)
	{
		sqFiltersName = new QStringList(sqConfig->readListEntry("Filters", "items"));
		sqFiltersExt = new QStringList(sqConfig->readListEntry("Filters", "extensions"));

		actionFilterMenu = new KPopupMenu;
		actionFilterMenu->setCheckable(true);
	}

	actionFilterMenu->clear();
	int id;
	bool both = sqConfig->readBoolEntry("Filters", "menuitem both", true);

	QValueList<QString>::iterator nEND = sqFiltersName->end();
	QValueList<QString>::iterator it_name = sqFiltersName->begin();
	QValueList<QString>::iterator it_ext = sqFiltersExt->begin();

	for(;it_name != nEND;it_name++,it_ext++)
	{
		if(both)
			id = actionFilterMenu->insertItem(*it_name + "  (" + *it_ext + ")");
		else
			id = actionFilterMenu->insertItem(*it_name);

		actionFilterMenu->setItemParameter(id, i++);
	}

	disconnect(actionFilterMenu, SIGNAL(activated(int)), this, SLOT(slotSetFilter(int)));
	connect(actionFilterMenu, SIGNAL(activated(int)), SLOT(slotSetFilter(int)));

	if(sqFiltersName->count())
		slotSetFilter(sqConfig->readNumEntry("Fileview", "filter_id", 0));
}

void Squirrel::slotGo()
{
	sqWStack->setURLForCurrent(pCurrentURL->currentText());
}

void Squirrel::slotSetFilter(int id)
{
	actionFilterMenu->setItemChecked(old_id, false);
	int index = actionFilterMenu->itemParameter(id);
	sqWStack->setNameFilter((*sqFiltersExt)[index]);

	actionFilterMenu->setItemChecked(id, true);
	old_id = id;
}

void Squirrel::slotGLView()
{
	sqGLView->show();
	SQ_ACTIVATE_WINDOW(sqGLView->winId());
}

void Squirrel::createWidgetsLikeSQuirrel()
{
	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	createLocationToolbar(new KToolBar(this, QMainWindow::Top, true, "Location toolbar"));

	createStatusBar(statusBar());
	sqStatus->removeWidget(sqSBGLreport);
	sqStatus->removeWidget(sqSBDecoded);

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

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));

	createToolbar(fileTools);

	menubar = menuBar();
	createMenu(menubar);

	sqGLView = new SQ_GLView;

	mainSplitter->setSizes(sqConfig->readIntListEntry("Interface", "splitter1"));
}

void Squirrel::createWidgetsLikeGqview()
{
	createStatusBar(statusBar());

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
	createLocationToolbar(pTLocation);

	sqWStack = new SQ_WidgetStack(V);
	sqWStack->raiseFirst(createFirst);

	createToolbar(pToolbar);
	createMenu(pMenuBar);

	// "GQview" specific
	pAGLView->unplug(pToolbar);
	sqWStack->pAMkDir->unplug(pToolbar);
	pAGotoTray->unplug(pToolbar);
	pAFilters->unplug(pToolbar);
	pAExtTools->unplug(pToolbar);

	sqGLView = new SQ_GLView(mainSplitter);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));

	mainSplitter->setSizes(sqConfig->readIntListEntry("Interface", "splitter1"));
}

void Squirrel::createWidgetsLikeKuickshow()
{
	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	createStatusBar(statusBar());
	sqStatus->removeWidget(sqSBDecoded);
	sqStatus->removeWidget(sqSBGLreport);

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

	createToolbar(fileTools);

	menubar = menuBar();
	createMenu(menubar);

	sqGLView = new SQ_GLView;
}

void Squirrel::createWidgetsLikeNoComponents()
{
	createStatusBar(statusBar());

	// WinViewer specific
	sqStatus->removeWidget(sqSBdirInfo);
	sqStatus->removeWidget(sqSBfileIcon);
	sqStatus->removeWidget(sqSBfileName);

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	mainDock->dockManager()->setSplitterOpaqueResize(true);
	viewBrowser = new QWidgetStack(mainDock, "SQ_BROWSER_WIDGET_STACK");
	mainDock->setWidget(viewBrowser);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	sqWStack = new SQ_WidgetStack(viewBrowser);
	sqWStack->raiseFirst(createFirst);
	sqGLView = new SQ_GLView(viewBrowser);

	createActions();
	menubar = menuBar();
	createMenu(menubar);

	pARaiseIconView->setEnabled(false);
	pARaiseListView->setEnabled(false);
	pARaiseDetailView->setEnabled(false);
	pARaiseThumbView->setEnabled(false);

	viewBrowser->addWidget(sqWStack, 0);
	viewBrowser->addWidget(sqGLView, 1);
	viewBrowser->raiseWidget(1);
}

void Squirrel::createWidgetsLikeXnview()
{
	createStatusBar(statusBar());

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	mainSplitterV = new QSplitter(Qt::Vertical, mainDock);
	mainDock->setWidget(mainSplitterV);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	mainSplitter = new QSplitter(Qt::Horizontal, mainSplitterV);
	sqWStack = new SQ_WidgetStack(mainSplitter);
	sqWStack->raiseFirst(createFirst);

	sqTree = new SQ_TreeView(mainSplitter);
	mainSplitter->moveToFirst(sqTree);

	sqGLView = new SQ_GLView(mainSplitterV);

	KToolBar *tt = new KToolBar(this, QMainWindow::Top, true, "SQ_____Tools");
	createToolbar(tt);
	createLocationToolbar(new KToolBar(this, QMainWindow::Top, true, "Location toolbar"));
	createMenu(menuBar());

	// "XnView" specific
	pAGLView->unplug(tt);

	mainSplitter->setSizes(sqConfig->readIntListEntry("Interface", "splitter1"));
	mainSplitterV->setSizes(sqConfig->readIntListEntry("Interface", "splitter2"));
}

void Squirrel::createWidgetsLikeShowImg()
{
	createStatusBar(statusBar());

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	mainDock->dockManager()->setSplitterOpaqueResize(true);
	mainSplitter = new QSplitter(Qt::Horizontal, mainDock);
	mainDock->setWidget(mainSplitter);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	mainSplitterV = new QSplitter(Qt::Vertical, mainSplitter);
	
	sqWStack = new SQ_WidgetStack(mainSplitterV);

	QVBox *V = new QVBox(mainSplitterV);
	V->setMinimumWidth(20);

	KMenuBar *pMenuBar = new KMenuBar(V);
	KToolBar *pToolbar = new KToolBar(sqApp, V, true, "GQview toolbar");

	pTLocation = new KToolBar(sqApp, V, true, "Location toolbar");
	pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	createLocationToolbar(pTLocation);

	sqTree = new SQ_TreeView(V);

	sqWStack->raiseFirst(createFirst);

	createToolbar(pToolbar);
	createMenu(pMenuBar);

	// "ShowImg" specific
	pAGLView->unplug(pToolbar);
	sqWStack->pAMkDir->unplug(pToolbar);
	pAGotoTray->unplug(pToolbar);
	pAFilters->unplug(pToolbar);
	pAExtTools->unplug(pToolbar);

	sqGLView = new SQ_GLView(mainSplitter);

	mainSplitterV->moveToFirst(V);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));

	mainSplitter->setSizes(sqConfig->readIntListEntry("Interface", "splitter1"));
	mainSplitterV->setSizes(sqConfig->readIntListEntry("Interface", "splitter2"));
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
	createLocationToolbar(pTLocation);
	sqWStack = new SQ_WidgetStack(b1);
	KStatusBar *stat = new KStatusBar(b1);
	createStatusBar(stat);

	sqWStack->raiseFirst(createFirst);

	b1->setStretchFactor(sqWStack, 1);

	createToolbar(t1);
	createMenu(pMenuBar);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));

	QVBox *b2 = new QVBox(viewBrowser);
	sqGLView = new SQ_GLView(b2);

	viewBrowser->addWidget(b1, 0);
	viewBrowser->addWidget(b2, 1);
}

void Squirrel::createStatusBar(KStatusBar *bar)
{
	sbar = bar;
	sbar->setSizeGripEnabled(true);
	sbar->show();

	sqSBdirInfo = new QLabel(sbar);
	QHBox *vb = new QHBox(sbar);

	sqSBfileIcon = new QLabel(vb);
	sqSBfileIcon->setScaledContents(false);

	sqSBfileName = new QLabel(vb);
	sqSBDecoded = new QLabel(sbar);
	sqSBDecoded->setText(i18n("No files decoded"));
	sqSBGLreport = new QLabel(sbar);
	QLabel *levak = new QLabel(sbar);

	sbar->addWidget(sqSBdirInfo, 0, true);
	sbar->addWidget(vb, 0, true);
	sbar->addWidget(sqSBDecoded, 0, true);
	sbar->addWidget(sqSBGLreport, 0, true);
	sbar->addWidget(levak, 1, true);
}

void Squirrel::createMenu(KMenuBar *menubar)
{
	pop_file = new KPopupMenu(menubar);
	pop_edit = new KPopupMenu(menubar);
	pop_view = new KPopupMenu(menubar);

	initFilterMenu();
	initBookmarks();

	menubar->insertItem(i18n("&File"), pop_file);
	menubar->insertItem(i18n("&Edit"), pop_edit);
	menubar->insertItem(i18n("&View"), pop_view);
	bookmarks->plug(menubar);
	menubar->insertItem(i18n("Fi&lter"), actionFilterMenu);
	menubar->insertItem(i18n("&Help"), helpMenu());

	sqWStack->pAHome->plug(pop_file);
	sqWStack->pAUp->plug(pop_file);
	sqWStack->pABack->plug(pop_file);
	sqWStack->pAForw->plug(pop_file);
	sqWStack->pAMkDir->plug(pop_file);
	pop_file->insertSeparator();
	sqWStack->pAProp->plug(pop_file);
	sqWStack->pADelete->plug(pop_file);
	pop_file->insertSeparator();
	pAExit->plug(pop_file);

	pAGotoTray->plug(pop_view);
	pop_view->insertSeparator();
	pARaiseIconView->plug(pop_view);
	pARaiseListView->plug(pop_view);
	pARaiseDetailView->plug(pop_view);
	pARaiseThumbView->plug(pop_view);
	pARescan->plug(pop_edit);
	pAExtTools->plug(pop_edit);
	pAFilters->plug(pop_edit);
	pAConfigure->plug(pop_edit);
}

void Squirrel::createToolbar(KToolBar *tools)
{
	createActions();
//	tools->setIconSize(KIcon::SizeMedium, true);

	sqWStack->pABack->plug(tools);
	sqWStack->pAForw->plug(tools);
	sqWStack->pAUp->plug(tools);
	sqWStack->pAHome->plug(tools);
	sqWStack->pAMkDir->plug(tools);

	pARaiseListView->plug(tools);
	pARaiseIconView->plug(tools);
	pARaiseDetailView->plug(tools);
	pARaiseThumbView->plug(tools);
	pAThumbs->plug(tools);

	switch(createFirst)
	{
		case 0: pARaiseListView->setChecked(true); break;
		case 1: pARaiseIconView->setChecked(true); break;
		case 2: pARaiseDetailView->setChecked(true); break;
		case 3: pARaiseThumbView->setChecked(true); break;

		default: pARaiseListView->setChecked(true);
	}

	pAGLView->plug(tools);
	pAExtTools->plug(tools);
	pAFilters->plug(tools);
	pAConfigure->plug(tools);
	pAGotoTray->plug(tools);
	pAExit->plug(tools);
}

void Squirrel::createActions()
{
	pAExit = KStdAction::quit(this, SLOT(close()), actionCollection(), "SQ close");
  	pAConfigure = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "SQ edit options");
	pAGLView = new KAction(i18n("GL window"), "desktop", KShortcut(CTRL+Key_H), this, SLOT(slotGLView()), actionCollection(), "SQ gl view widget");
	pARescan = KStdAction::redisplay(this, SLOT(slotRescan()), actionCollection(), "SQ rescan libraries");
	pARescan->setText(i18n("Rescan libraries"));
	pAExtTools = new KAction(i18n("Configure external tools ..."), "memory", KShortcut(), this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");
	pAFilters = new KAction(i18n("Configure filters ..."), "filefind", KShortcut(), this, SLOT(slotFilters()), actionCollection(), "SQ filters");
	pAGotoTray = new KAction(i18n("Goto tray"), "compfile", KShortcut(), this, SLOT(slotGotoTray()), actionCollection(), "SQ goto tray");

	pARaiseListView = new KRadioAction(i18n("List"), "view_icon", 0, this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
	pARaiseIconView = new KRadioAction(i18n("Icons"), "view_choose", 0, this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
	pARaiseDetailView = new KRadioAction(i18n("Detailes"), "view_detailed", 0, this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
	pARaiseThumbView = new KRadioAction(i18n("Thumbnails"), "view_detailed", 0, this, SLOT(slotRaiseThumbView()), actionCollection(), "SQ raise thumbs view");

	pAThumbs = new KActionMenu(i18n("Thumbnail size"), QPixmap(0));

	pAThumb0 = new KRadioAction(i18n("Small thumbnails"), QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_small.png")), 0, this, SLOT(slotThumbsSmall()), actionCollection(), "SQ thumbs0");
	pAThumb1 = new KRadioAction(i18n("Medium thumbnails"), QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_medium.png")), 0, this, SLOT(slotThumbsMedium()), actionCollection(), "SQ thumbs1");
	pAThumb2 = new KRadioAction(i18n("Large thumbnails"), QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_large.png")), 0, this, SLOT(slotThumbsLarge()), actionCollection(), "SQ thumbs2");
	pAThumb3 = new KRadioAction(i18n("Huge thumbnails"), QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_huge.png")), 0, this, SLOT(slotThumbsHuge()), actionCollection(), "SQ thumbs3");

	pAThumb0->setExclusiveGroup(QString("thumbs_size__"));
	pAThumb1->setExclusiveGroup(QString("thumbs_size__"));
	pAThumb2->setExclusiveGroup(QString("thumbs_size__"));
	pAThumb3->setExclusiveGroup(QString("thumbs_size__"));

	switch(sqThumbSize->value())
	{
		case SQ_ThumbnailSize::Small:
			pAThumb0->setChecked(true);
		break;

		case SQ_ThumbnailSize::Medium:
			pAThumb1->setChecked(true);
		break;

		case SQ_ThumbnailSize::Large:
			pAThumb2->setChecked(true);
		break;

		default:
			pAThumb3->setChecked(true);
	}
	
	pAThumbs->insert(pAThumb0);
	pAThumbs->insert(pAThumb1);
	pAThumbs->insert(pAThumb2);
	pAThumbs->insert(pAThumb3);

	pARaiseListView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseIconView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseDetailView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseThumbView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
}

void Squirrel::slotGotoTray()
{
	if(!tray)
	{
		tray = new SQ_SystemTray;
		tray->show();
	}

	hide();
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

void Squirrel::initExternalTools()
{
	QString str, tmp;
	SQ_EXT_TOOL tmp_tool;
	int i;

	sqExternalTool = new SQ_ExternalTool;

	for(i = 1;;i ++)
	{
		str.sprintf("%d", i);
		tmp = sqConfig->readEntry("External tool name", str, "");

         	if(tmp.isEmpty())
          		break;

		tmp_tool.name = tmp;
		tmp_tool.command = sqConfig->readEntry("External tool program", str, "");
		tmp_tool.pixmap = sqConfig->readEntry("External tool pixmap", str, "");

		sqExternalTool->addTool(tmp_tool);
	}
}

void Squirrel::slotFilters()
{
	SQ_Filters f(this);

	if(f.start() != QDialog::Accepted) return;

	initFilterMenu();
}

void Squirrel::slotExtTools()
{
	SQ_ExternalTools etd(this, "sq_exttools", true);

	if(etd.start() == QDialog::Accepted)
		sqExternalTool->getNewPopupMenu();
}

void Squirrel::raiseGLWidget()
{
	switch(curViewType)
	{
		case Squirrel::SQuirrel:
        	case Squirrel::Kuickshow:
			sqGLView->show();
			SQ_ACTIVATE_WINDOW(sqGLView->winId());
		break;

		case Squirrel::Gqview:
	        	case Squirrel::NoComponents:
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
	       case Squirrel::NoComponents:
        	case Squirrel::Xnview:
         	case Squirrel::ShowImg:
		return;

		case Squirrel::Browser:
			viewBrowser->raiseWidget(0);
		break;

		default: return;
	}
}

void Squirrel::initBookmarks()
{
	QString file = locate("data", "kfile/bookmarks.xml");

	if(file.isEmpty())
		file = locateLocal("data", "kfile/bookmarks.xml");

	KBookmarkManager *bmanager=KBookmarkManager::managerForFile(file, false);
	bmanager->setUpdate(true);
	bmanager->setShowNSBookmarks(false);

	sqBookmarks = new SQ_BookmarkOwner(this);

	bookmarks = new KActionMenu(i18n("&Bookmarks"), "bookmark", actionCollection(), "bookmarks");
	new KBookmarkMenu(bmanager, bookmarkOwner, bookmarks->popupMenu(), this->actionCollection(), true);

	connect(sqBookmarks, SIGNAL(openURL(const KURL&)), sqWStack, SLOT(setURLForCurrent(const KURL&)));
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
			SQ_ACTIVATE_WINDOW(winId());
		break;

		case 1:
		{
			show();
			SQ_ACTIVATE_WINDOW(winId());

			QString path = str.right(str.length() - 19);
			QFileInfo fm(path);
			KURL url = path;

			if(url.fileName(false).isEmpty())
				sqWStack->setURLForCurrent(url);
			else
				sqWStack->emitSelected(path);
		}
		break;

		default:
			printf("control(): unknown parameter: \"%s\"\n", str.ascii());
	}
}

void Squirrel::writeDefaultEntries()
{
		sqConfig->setGroup("Main");
		sqConfig->writeEntry("minimize to tray", false);
		sqConfig->writeEntry("restart", true);
		sqConfig->writeEntry("activate another", true);
		sqConfig->writeEntry("sync", false);
		sqConfig->writeEntry("first_time", false);

		sqConfig->setGroup("Interface");
		sqConfig->writeEntry("last view", 0);
		sqConfig->writeEntry("view type", 1);
		sqConfig->writeEntry("save pos", true);
		sqConfig->writeEntry("save size", true);
		sqConfig->writeEntry("posX", 0);
		sqConfig->writeEntry("posY", 0);
		sqConfig->writeEntry("sizeX", 800);
		sqConfig->writeEntry("sizeY", 600);
		sqConfig->writeEntry("splitter1", "1,2");
		sqConfig->writeEntry("splitter2", "1,2");

		sqConfig->setGroup("Fileview");
		sqConfig->writeEntry("set path", 0);
		sqConfig->writeEntry("last visited", "/");
		sqConfig->writeEntry("custom directory", "/");
		sqConfig->writeEntry("sync type", 1);
		sqConfig->writeEntry("click policy", 0);
		sqConfig->writeEntry("history", false);
		sqConfig->writeEntry("run unknown", false);
		sqConfig->writeEntry("archives", true);
		sqConfig->writeEntry("tofirst", true);
		sqConfig->writeEntry("iCurrentIconIndex", 2);
		sqConfig->writeEntry("iCurrentListIndex", 0);
		sqConfig->writeEntry("filter_id", 0);

		sqConfig->setGroup("GL view");
		sqConfig->writeEntry("GL view background type", 0);
		sqConfig->writeEntry("GL view background", "#a8a8a8");
		sqConfig->writeEntry("GL view custom texture", "");
		sqConfig->writeEntry("GL view border", "#ff0000");
		sqConfig->writeEntry("border size", 2);
		sqConfig->writeEntry("zoom type", 0);
		sqConfig->writeEntry("zoom pretty", false);
		sqConfig->writeEntry("manipulation center", 0);
		sqConfig->writeEntry("scroll", 0);
		sqConfig->writeEntry("render file type", 1);
		sqConfig->writeEntry("angle", 90);
		sqConfig->writeEntry("zoom", 25);
		sqConfig->writeEntry("move", 5);
		sqConfig->writeEntry("border", true);
		sqConfig->writeEntry("image background", true);
		sqConfig->writeEntry("save pos", true);
		sqConfig->writeEntry("save size", true);
		sqConfig->writeEntry("render path", "");
		sqConfig->writeEntry("render ext",  "PNG");
		sqConfig->writeEntry("render file format",  "ksquirrel GL rendered pixmap - %dd.%dmn.%dy %dh:%dm:%ds sec.");
		sqConfig->writeEntry("quickbrowser",  true);
		sqConfig->writeEntry("quickGeometry", QRect(2,32,220,250));
		sqConfig->writeEntry("ignore", false);
		sqConfig->writeEntry("actions", false);
		sqConfig->writeEntry("progress", true);

		sqConfig->setGroup("Thumbnails");
		sqConfig->writeEntry("margin", 2);
		sqConfig->writeEntry("cache", 1024*10);
		sqConfig->writeEntry("disable_mime", true);
		sqConfig->writeEntry("dont write", false);

		sqConfig->setGroup("Libraries");
		sqConfig->writeEntry("monitor", true);
		sqConfig->writeEntry("show dialog", false);
		sqConfig->writeEntry("prefix", "/usr/lib/squirrel/");

		sqConfig->setGroup("External tool pixmap");
		sqConfig->writeEntry("1", "kfm");
		sqConfig->writeEntry("2", "gimp");
		sqConfig->writeEntry("3", "kuickshow");
		sqConfig->writeEntry("4", "gqview");
		sqConfig->writeEntry("5", "kview");
		sqConfig->writeEntry("6", "kwrite");
		sqConfig->writeEntry("7", "kdeprintfax");
		sqConfig->writeEntry("8", "mac");
		sqConfig->writeEntry("9", "mac");

		sqConfig->setGroup("External tool name");
		sqConfig->writeEntry("1", "Run within KDE");
		sqConfig->writeEntry("2", "Open with GIMP");
		sqConfig->writeEntry("3", "Open with KuickShow");
		sqConfig->writeEntry("4", "Open with GQview");
		sqConfig->writeEntry("5", "Open with KView");
		sqConfig->writeEntry("6", "Open with KWrite");
		sqConfig->writeEntry("7", "Print file");
		sqConfig->writeEntry("8", "Set as wallpaper on desktop");
		sqConfig->writeEntry("9", "Set as tiled wallpaper on desktop");

		sqConfig->setGroup("External tool program");
		sqConfig->writeEntry("1", "kfmclient exec %s");
		sqConfig->writeEntry("2", "gimp %s");
		sqConfig->writeEntry("3", "kuickshow %s");
		sqConfig->writeEntry("4", "gqview %s");
		sqConfig->writeEntry("5", "kview %s");
		sqConfig->writeEntry("6", "kwrite %s");
		sqConfig->writeEntry("7", "kprinter %s");
		sqConfig->writeEntry("8", "dcop kdesktop KBackgroundIface setWallpaper %s 6");
		sqConfig->writeEntry("9", "dcop kdesktop KBackgroundIface setWallpaper %s 2");

		sqConfig->setGroup("Filters");
		sqConfig->writeEntry("items", "All files,Windows BMP,Windows icons and cursors,PNG,JPEG,Compuserve GIF,X11 Monochrome Bitmaps,X11 PixMap,ZSoft PCX,TIFF,PNM,X Window Dump,Video tapes,Audio files");
		sqConfig->writeEntry("extensions", "*,*.bmp *.dib,*.ico *.cur,*.png,*.jpeg *.jpg *.jpe *.jp2,*.gif,*.xbm,*.xpm,*.pcx,*.tiff *.tif,*.pnm *.ppm *.pgm *.pbm,*.xwd,*.mpg *.mpeg *.avi *.asf *.divx,*.mp3 *.ogg *.wma *.wav *.it");
		sqConfig->writeEntry("menuitem both", true);

		sqConfig->sync();
}

void Squirrel::applyDefaultSettings()
{
	sqGLWidget->setClearColor(); // values are taken from sqConfig
	sqGLWidget->setTextureParams();
	sqGLWidget->setAcceptDrops(sqConfig->readBoolEntry("GL view", "enable drop", true));
	sqGLWidget->setZoomFactor(sqConfig->readNumEntry("GL view", "zoom", 25));
	sqGLWidget->setMoveFactor(sqConfig->readNumEntry("GL view", "move", 5));
	sqGLWidget->setRotateFactor(sqConfig->readNumEntry("GL view", "angle", 90));
	sqLibUpdater->setAutoUpdate(sqConfig->readBoolEntry("Libraries", "monitor", true));
	sqLibHandler->clear();
	sqLibUpdater->openURL(KURL(sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/")), false, true);
	sqWStack->configureClickPolicy();
	sqWStack->updateGrid();

	sqCache->setCacheLimit(sqConfig->readNumEntry("Thumbnails", "cache", 1024*10));

	if(sqConfig->readBoolEntry("Main", "sync", true))
		sqConfig->sync();
}

void Squirrel::slotFullScreen(bool full)
{
	static QValueList<int> sizes;

	switch(curViewType)
	{
        		case Squirrel::SQuirrel:
        		case Squirrel::Kuickshow:
         		if(full)
				sqGLView->showFullScreen();
			else
				sqGLView->showNormal();
		break;

		case Squirrel::Gqview:
         	case Squirrel::ShowImg:
          		if(full)
                        {
				sizes = mainSplitter->sizes();
				sqGLView->reparent(0L, QPoint(), true);
				sqGLView->showFullScreen();
			}
			else
			{
				sqGLView->showNormal();
				sqGLView->reparent(mainSplitter, QPoint(), true);
				mainSplitter->setSizes(sizes);
			}
		break;

        		case Squirrel::Xnview:
          		if(full)
                        {
				sizes = mainSplitterV->sizes();
				sqGLView->reparent(0L, QPoint(), true);
				sqGLView->showFullScreen();
			}
			else
			{
				sqGLView->showNormal();
				sqGLView->reparent(mainSplitterV, QPoint(), true);
				mainSplitterV->setSizes(sizes);
			}
		break;

		case Squirrel::NoComponents:
			if(full)
                        {
				menubar->hide();
				showFullScreen();
			}
			else
			{
				showNormal();
				menubar->show();
			}
		break;

		case Squirrel::Browser:
			if(full)
			{
				viewBrowser->raiseWidget(1);
				showFullScreen();
			}
			else
			{
				viewBrowser->raiseWidget(0);
				showNormal();
			}
		break;

		default: ;
	}

	if(sqConfig->readBoolEntry("GL view", "quickbrowser", true))
		if(full && !sqGLWidget->pAQuick->isChecked() || !full && sqGLWidget->pAQuick->isChecked())
			sqGLWidget->pAQuick->activate();
}

void Squirrel::saveValues()
{
	sqExternalTool->writeEntries();

	sqConfig->setGroup("Filters");
	sqConfig->writeEntry("items", *sqFiltersName);
	sqConfig->writeEntry("extensions", *sqFiltersExt);

	sqConfig->setGroup("Fileview");
	sqConfig->writeEntry("last visited", (sqWStack->getURL()).path());
	sqConfig->writeEntry("filter_id", (old_id < 0)?0:old_id);

	if(sqConfig->readBoolEntry("Fileview", "history", true) && sqCurrentURL)
	{
		sqConfig->setGroup("History");
		sqConfig->writeEntry("items", sqCurrentURL->historyItems());
	}

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

	sqConfig->writeEntry("last view", sqWStack->id(sqWStack->visibleWidget()));

	if(sqConfig->readBoolEntry("GL view", "save pos", true))
	{
		sqConfig->writeEntry("posX", sqGLView->x());
		sqConfig->writeEntry("posY", sqGLView->y());
	}

	if(sqConfig->readBoolEntry("GL view", "save size", true))
	{
		sqConfig->writeEntry("sizeX", sqGLView->width());
		sqConfig->writeEntry("sizeY", sqGLView->height());
	}

	sqConfig->writeEntry("quickGeometry", sqQuickBrowser->geometry());
	sqConfig->writeEntry("ignore", sqGLWidget->pAIfLess->isChecked());
	sqConfig->writeEntry("zoom type", sqGLWidget->zoomType());
	sqConfig->writeEntry("actions", sqGLWidget->actions());

	sqConfig->setGroup("Interface");
	if(mainSplitter) sqConfig->writeEntry("splitter1", mainSplitter->sizes());
	if(mainSplitterV) sqConfig->writeEntry("splitter2", mainSplitterV->sizes());

	sqConfig->setGroup("Thumbnails");
	sqConfig->writeEntry("size", sqThumbSize->value());
}

QString Squirrel::slotRescan()
{
	QString sqLibPrefix = sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/");
	if(!sqLibPrefix.endsWith("/"))	sqLibPrefix += "/";

	sqLibHandler->clear();
	sqLibUpdater->slotOpenURL(KURL(sqLibPrefix), false, true);

	return sqLibPrefix;
}

void Squirrel::createPostSplash()
{
	QColor cc(255,255,255);
	QHBox *hbox = new QHBox(0, 0, WStyle_Customize | WStyle_Splash);
	hbox->setFrameShape(QFrame::Box);
	QLabel *pp = new QLabel(hbox);
	pp->setPalette(QPalette(cc, cc));
	pp->setPixmap(sqLoader->loadIcon("folder", KIcon::Desktop, 16));
	pp->setFixedWidth(17);
	QLabel *l = new QLabel(i18n("<u><b>ksquirrel</b></u>: writing settings and thumbnails ...  "), hbox);
	l->setPalette(QPalette(cc, cc));
	l->setAlignment(Qt::AlignHCenter);
	hbox->setStretchFactor(pp, 0);
	hbox->setStretchFactor(l, 1);
	int w = 300, h = 25;
	hbox->setGeometry(QApplication::desktop()->width() / 2 - w / 2, QApplication::desktop()->height() / 2 - h / 2, w, h);
	hbox->show();
	pp->repaint();
	l->repaint();
}

void Squirrel::slotThumbsSmall()
{
	emit thumbSizeChanged("small");
}

void Squirrel::slotThumbsMedium()
{
	emit thumbSizeChanged("medium");
}

void Squirrel::slotThumbsLarge()
{
	emit thumbSizeChanged("large");
}

void Squirrel::slotThumbsHuge()
{
	emit thumbSizeChanged("huge");
}

