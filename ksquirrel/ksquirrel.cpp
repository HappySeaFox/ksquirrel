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
#include <qimage.h>
#include <qvaluelist.h>
#include <qprocess.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qvbox.h>
#include <qlayout.h>

#include <kapp.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kurlcompletion.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_tray.h"
#include "sq_treeview.h"
#include "sq_runprocess.h"
#include "sq_options.h"
#include "sq_about.h"
#include "sq_splash.h"
#include "sq_glviewwidget.h"
#include "sq_libraryhandler.h"
#include "sq_dir.h"
#include "sq_bookmarks.h"

#include "sq_convertwidget.h"


Squirrel * Squirrel::App = 0;
KDockWidget *pdock1;


Squirrel::Squirrel(QWidget *parent, const char *name) : KDockMainWindow (parent, name), mainSplitter(0L)
{
	App = this;
	sqLibPrefix = "/usr/lib/squirrel/";
	
	sqConfig = new KConfig(QString("ksquirrelrc"));
	sqLoader = new KIconLoader(*(KGlobal::iconLoader()));
	pWidgetStack = 0L;
	sqCurrentURL = 0L;
	mainSplitter = 0L;
	tray = 0L;

	iconSizeList = new QValueList<int>;
	iconSizeList->append(16);
	iconSizeList->append(22);
	iconSizeList->append(32);
	iconSizeList->append(48);
	

	sqConfig->setGroup("Interface");
	toolbarIconSize = (*iconSizeList)[sqConfig->readNumEntry("toolbar icon size", 0)];

	sqConfig->setGroup("Main");
	bool showsplash = sqConfig->readBoolEntry("show splash", true);

        if(showsplash)
        {
		splash = new SQ_Splash(this);
		splash->show();
	}

	
	// !! Find libraries
	KShellProcess procLibFinder;
	procLibFinder << "find" << sqLibPrefix << "-name" << "libSQ*.so";
	connect(&procLibFinder, SIGNAL(receivedStdout(KProcess*,char*,int)), SLOT(receivedStdoutFromLibFinder(KProcess*,char*,int)));
	procLibFinder.start(KProcess::Block, KProcess::Stdout);
	disconnect(&procLibFinder, SIGNAL(receivedStdout(KProcess*,char*,int)), this, SLOT(receivedStdoutFromLibFinder(KProcess*,char*,int)));
	
	// Init LibraryHandler, load libraries, resolve functions ...
	sqLibHandler = new SQ_LibraryHandler(strlibFound);

	// create view
	sqConfig->setGroup("Interface");
	curViewType = (Squirrel::ViewType)sqConfig->readNumEntry("ViewType",  Squirrel::SQuirrel);
	dirForViewer = new SQ_Dir("/", sqLibHandler->allSupportedForFilter());

	// set background color for GLview
	sqConfig->setGroup("GL view");
	sqGLViewBGColor.setNamedColor(sqConfig->readEntry("GL view background", "#cccccc"));

	switch(curViewType)
	{
        	case Squirrel::SQuirrel:	createWidgetsLikeSQuirrel();	break;
        	case Squirrel::Gqview:		createWidgetsLikeGqview();		break;
        	case Squirrel::Kuickshow:	createWidgetsLikeKuickshow();	break;
        	case Squirrel::WinViewer:	createWidgetsLikeWinViewer();	break;
        	case Squirrel::Xnview:		createWidgetsLikeXnview();		break;

        	default:
         		createWidgetsLikeSQuirrel();
	}
	
	move(0,0);
	resize(QApplication::desktop()->width(), QApplication::desktop()->height());
	show();

        if(showsplash)
		QTimer::singleShot(500*1, this, SLOT(slotSplashClose()));
}

void Squirrel::slotExecuteRunMenu()
{
	pmLaunch->exec(QCursor::pos());
}

void Squirrel::slotConvert()
{
	SQ_ConvertWidget editpict(this);
	editpict.exec();
}

void Squirrel::slotOptions()
{
	SQ_Options optd(this, "sq_options", true);
	optd.start();
}

void Squirrel::closeEvent(QCloseEvent *ev)
{
	sqConfig->setGroup("Main");

	if(sqConfig->readBoolEntry("minimize to tray", false))
	{
		hide();
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
		sqConfig->sync();
		ev->accept();
	}
}

void Squirrel::slotSplashClose()
{
	splash->close();

	delete splash;
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
		sqConfig->setGroup("Run program");
		program = sqConfig->readEntry(QString("%1").arg(i, 0, 10), "");

		if(program == "") break;

		sqConfig->setGroup("Run name");
		name = sqConfig->readEntry(QString("%1").arg(i, 0, 10), "");

		pMenuProc->AddItem(name, program);
	}

	pmLaunch = new QPopupMenu;

	int getcount = pMenuProc->GetCount();

	for(int i = 0;i < getcount;i++)
		if(pMenuProc->GetName(i) != "SEPARATOR")
			pmLaunch->insertItem(QPixmap(pMenuProc->GetPixmap(i)), pMenuProc->GetName(i), pMenuProc->GetElementId(i));
		else
			pmLaunch->insertSeparator();

	connect(pmLaunch, SIGNAL(activated(int)), pMenuProc, SLOT(slotRunCommand(int)));
}

void Squirrel::CreateLocationToolbar()
{
	QString setNewUrl = QString("/");

	new QToolButton(sqLoader->loadIcon("locationbar_erase", KIcon::Desktop, KIcon::SizeSmall), "Clear adress", QString::null, this, SLOT(slotClearAdress()), pTLocation);
	new QLabel("URL:", pTLocation, "kde toolbar widget");

	sqCurrentURL = new KHistoryCombo(true, pTLocation, "history combobox");
	pTLocation->setStretchableWidget(pCurrentURL);
	sqCurrentURL->setDuplicatesEnabled(false);

	new QToolButton(sqLoader->loadIcon("goto", KIcon::Desktop, KIcon::SizeSmall), "Go!", QString::null, this, SLOT(slotGo()), pTLocation);

	KURLCompletion *pURLCompletion = new KURLCompletion(KURLCompletion::DirCompletion);
	pURLCompletion->setDir(setNewUrl);

	sqCurrentURL->setCompletionObject(pURLCompletion);
	sqCurrentURL->setAutoDeleteCompletionObject(true);
	sqCurrentURL->clearHistory();
	sqCurrentURL->setEditText(setNewUrl);
	sqCurrentURL->addToHistory(setNewUrl);
}

void Squirrel::InitFilterMenu()
{
	FILTER tmp_filter;
	QString ext, tmp;
	unsigned int i;
	
	actionFilterMenu = new KPopupMenu;
	sqConfig->setGroup("Filters");
	filterList = new QValueList<FILTER>;

	/*  filterList should contain "*" and "all supported" filters by default  */
	tmp_filter.name = "All files";
	tmp_filter.filter = "*";
	filterList->append(tmp_filter);
	tmp_filter.name = "All supported files";
	tmp_filter.filter = sqLibHandler->allSupportedForFilter();
	filterList->append(tmp_filter);

	for(i = 1;;i ++)
	{
		ext.sprintf("%d", i);
		sqConfig->setGroup("Filters");
		tmp = sqConfig->readEntry(ext, "");
		
		if(tmp == QString(""))
			break;

		tmp_filter.name = tmp;
		sqConfig->setGroup("Filters ext");
		tmp = sqConfig->readEntry(ext, "");
		tmp_filter.filter = tmp;

		filterList->append(tmp_filter);
	}

	for(i=0;i<filterList->count();i++)
		actionFilterMenu->insertItem((*filterList)[i].name, 7000+i);

	slotSetFilter(7001);
	connect(actionFilterMenu, SIGNAL(activated(int)), this, SLOT(slotSetFilter(int)));
}

void Squirrel::slotClearAdress()
{
	sqCurrentURL->setEditText("");
}

void Squirrel::slotGo()
{
	sqWStack->setURL(pCurrentURL->currentText(), true);
}

void Squirrel::slotSetFilter(int id)
{
	if(sqWStack) sqWStack->setNameFilter(((*filterList)[id-7000]).filter);
}

void Squirrel::slotGLView()
{
	sqGLView->show();
}

void Squirrel::createWidgetsLikeSQuirrel()
{
	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	if(curViewType != Squirrel::Kuickshow)
	{
		pTLocation = new KToolBar(this, QMainWindow::Top, true, "Location toolbar");
		pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
		CreateLocationToolbar();
	}

	CreateStatusBar();

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");

	//@todo option "show content when resizing" [true|false]
	mainDock->dockManager()->setSplitterOpaqueResize(true);

	QWidget *cw = new QWidget(mainDock);

	mainDock->setWidget(cw);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	pWidgetStack = new SQ_WidgetStack;
	SQ_TreeView *pTree = new SQ_TreeView;


	if(curViewType != Squirrel::Kuickshow)
	{
		pdockTree = createDockWidget("Tree view", 0L, 0L, "");
		pdockTree->setWidget(pTree);
		pdock1 = pdockTree->manualDock(mainDock, KDockWidget::DockLeft, 23);
	}

	pdockTabView = createDockWidget("File browser", 0L, 0L, "");
	pWidgetStack->setFocusPolicy(QTabWidget::NoFocus);
	pdockTabView->setWidget(pWidgetStack);
	pdockTabView->manualDock(mainDock, KDockWidget::DockLeft, 100);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURL(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURL(const QString&)));

	CreateToolbar(fileTools);

	menubar = menuBar();
	CreateMenu(menubar);

	sqGLView = new SQ_GLViewWidget;
}

void Squirrel::createWidgetsLikeGqview()
{
	CreateStatusBar();

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

	pWidgetStack = new SQ_WidgetStack(V);

	CreateToolbar(pToolbar);
	CreateMenu(pMenuBar);

	// "GQview" specific
	sqWStack->pAIconBigger->unplug(pToolbar);
	sqWStack->pAIconSmaller->unplug(pToolbar);

	pAGLView->unplug(pToolbar);
	pAExit->unplug(pToolbar);
	sqWStack->pANewDir->unplug(pToolbar);
	////////

	glView = new SQ_GLViewWidget(mainSplitter);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURL(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURL(const QString&)));

	QValueList<int> l;
	l.append(2);
	l.append(5);
	mainSplitter->setSizes(l);
}

void Squirrel::createWidgetsLikeKuickshow()
{
	createWidgetsLikeSQuirrel();
}

void Squirrel::createWidgetsLikeWinViewer()
{
	CreateStatusBar();
	CreateActions();

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
	CreateMenu(pMenuBar);

//	KToolBar *pToolbar = new KToolBar(sqApp, V, true, "WinViewer toolbar");
	sqGLView = new SQ_GLViewWidget(V);

	QHBox *H = new QHBox(V);
	QLabel *ltmp = new QLabel("", H);
	KToolBar *pToolbarTools = new KToolBar(sqApp, H, true, "WinViewer tools");
	QLabel *ltmp2 = new QLabel("", H);

	CreateToolbar(pToolbarTools);
//	pAPrevFile->plug(pToolbarTools);
	//pANextFile->plug(pToolbarTools);

	H->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	H->setStretchFactor(ltmp, 1);
	H->setStretchFactor(pToolbarTools, 0);
	H->setStretchFactor(ltmp2, 1);

	// WinViewer specific
	pARaiseIconView->setEnabled(false);
	pARaiseListView->setEnabled(false);
	pARaiseDetailView->setEnabled(false);
}

void Squirrel::createWidgetsLikeXnview()
{
	CreateStatusBar();

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	QSplitter *V = new QSplitter(Qt::Vertical, mainDock);
	mainDock->setWidget(V);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	QSplitter *H = new QSplitter(Qt::Horizontal, V);
	pWidgetStack = new SQ_WidgetStack(H);
	SQ_TreeView *pTree = new SQ_TreeView(H);
	H->moveToFirst(pTree);

	sqGLView = new SQ_GLViewWidget(V);

	CreateToolbar(toolBar());
	CreateMenu(menuBar());

	QValueList<int> l;
	l.append(1);
	l.append(3);
	V->setSizes(l);
}

void Squirrel::receivedStdoutFromLibFinder(KProcess *pr, char *buf, int len)
{
	if(!pr) return;

	QString rec = QString::fromLatin1(buf, len), final;

	strlibFound += QStringList::split("\n", rec);
}

void Squirrel::CreateStatusBar()
{
	sbar = statusBar();
	sbar->setSizeGripEnabled(true);
	sbar->show();

	
	sqSBdirInfo = new QLabel(sbar);
	sqSBcurFileInfo = new QLabel(sbar);
//	sqSBcurFileInfo->setText("         ");
//	sqSBcurFileInfo->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
	QHBox *vb = new QHBox(sbar);

	sqSBfileIcon = new QLabel(vb);
	sqSBfileIcon->setScaledContents(false);

	sqSBfileName = new QLabel(vb);
	sqSBDecoded = new QLabel(sbar);
	sqSBDecoded->setText("No files decoded");

	QLabel *levak = new QLabel(sbar);

	sbar->addWidget(sqSBdirInfo, 0, true);
	sbar->addWidget(sqSBcurFileInfo, 0, true);
	sbar->addWidget(vb, 0, true);
	sbar->addWidget(sqSBDecoded, 0, true);
	sbar->addWidget(levak, 1, true);
}

void Squirrel::CreateMenu(KMenuBar *menubar)
{
	KAction *pARunMenu = new KAction("Show 'Run ...' menu", "", KShortcut(CTRL+ALT+Key_R), this, SLOT(slotExecuteRunMenu()), actionCollection(), "execute 'Run' menu");

	pop_file = new KPopupMenu(menubar);
	pop_edit = new KPopupMenu(menubar);
	pop_view = new KPopupMenu(menubar);

	menubar->insertItem("&File", pop_file);
	menubar->insertItem("&Edit", pop_edit);
	menubar->insertItem("&View", pop_view);
	InitRunMenu();
	menubar->insertItem("&Launch", pmLaunch);
	if(curViewType != Squirrel::WinViewer)
	{
		InitFilterMenu();
		menubar->insertItem("&Filter", actionFilterMenu);
	}
	menubar->insertItem("&Help", helpMenu());

	pARunMenu->plug(pop_view);
	pop_view->insertSeparator();
	sqWStack->pAIconBigger->plug(pop_view);
	sqWStack->pAIconSmaller->plug(pop_view);
	pop_view->insertSeparator();
	pARaiseIconView->plug(pop_view);
	pARaiseListView->plug(pop_view);
	pARaiseDetailView->plug(pop_view);
	pop_view->insertSeparator();

	pAExit->plug(pop_file);
	pAConvert->plug(pop_edit);
	pop_edit->insertSeparator();
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
	pAConfigure->plug(tools);

	sqWStack->pAIconBigger->plug(tools);
	sqWStack->pAIconSmaller->plug(tools);

	pAGLView->plug(tools);
	pAConvert->plug(tools);
	pAExit->plug(tools);

	pARaiseListView->activate();
}

void Squirrel::CreateActions()
{
	pARaiseListView = new KRadioAction("Raise 'list' view", QIconSet(sqLoader->loadIcon("view_icon", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_icon", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
	pARaiseIconView = new KRadioAction("Raise 'icon' view", QIconSet(sqLoader->loadIcon("view_choose", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_choose", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
	pARaiseDetailView = new KRadioAction("Raise 'detailed' view", QIconSet(sqLoader->loadIcon("view_detailed", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_detailed", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
	pAConvert = new KAction("Convert images", QIconSet(sqLoader->loadIcon("edit_picture", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("edit_picture", KIcon::Desktop, toolbarIconSize)), KShortcut(CTRL+Key_X), this, SLOT(slotConvert()), actionCollection(), "SQ call convert dialog");
	pAExit = new KAction("Quit", QIconSet(sqLoader->loadIcon("exit", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("exit", KIcon::Desktop, toolbarIconSize)), KShortcut(ALT+Key_Q), this, SLOT(close()), actionCollection(), "SQ close");
  	pAConfigure = new KAction("Options", QIconSet(sqLoader->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("configure", KIcon::Desktop, toolbarIconSize)), KShortcut(CTRL+Key_P), this, SLOT(slotOptions()), actionCollection(), "SQ edit options");
  	pAGLView = new KAction("GLview", QIconSet(sqLoader->loadIcon("desktop", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("desktop", KIcon::Desktop, toolbarIconSize)), KShortcut(CTRL+Key_H), this, SLOT(slotGLView()), actionCollection(), "SQ gl view widget");

	pANextFile = new KAction("Next file", QIconSet(sqLoader->loadIcon("next", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("next", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, SLOT(slotNextFile()), actionCollection(), "SQ select next file");
	pAPrevFile = new KAction("Previous file", QIconSet(sqLoader->loadIcon("previous", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("previous", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, SLOT(slotPreviousFile()), actionCollection(), "SQ select previous file");

	pARaiseListView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseIconView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseDetailView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
}

void Squirrel::slotNextFile()
{
	if(sqWStack)
		sqWStack->slotNext();
	else
	{

	}
}

void Squirrel::slotPreviousFile()
{
	if(sqWStack)
		sqWStack->slotPrevious();
	else
	{

	}
}

Squirrel::~Squirrel()
{}

void Squirrel::rescanLibraries()
{
	strlibFound.clear();	
	KShellProcess procLibFinder;
	procLibFinder << "find" << sqLibPrefix << "-name" << "libSQ*.so";
	connect(&procLibFinder, SIGNAL(receivedStdout(KProcess*,char*,int)), SLOT(receivedStdoutFromLibFinder(KProcess*,char*,int)));
	procLibFinder.start(KProcess::Block, KProcess::Stdout);
	disconnect(&procLibFinder, SIGNAL(receivedStdout(KProcess*,char*,int)), this, SLOT(receivedStdoutFromLibFinder(KProcess*,char*,int)));
}
