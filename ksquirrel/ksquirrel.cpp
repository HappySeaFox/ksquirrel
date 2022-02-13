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

#include <kapp.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kurlcompletion.h>
#include <kconfig.h>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_tray.h"
#include "sq_treeview.h"
#include "sq_runprocess.h"
#include "sq_options.h"
#include "sq_about.h"
#include "sq_glviewwidget.h"
#include "sq_libraryhandler.h"
#include "sq_bookmarks.h"
#include "sq_librarylistener.h"
#include "sq_externaltools.h"
#include "sq_fileviewfilter.h"


Squirrel * Squirrel::App = 0;


Squirrel::Squirrel(QWidget *parent, const char *name) : KDockMainWindow (parent, name), DCOPObject(name), mainSplitter(0L)
{
	App = this;

	sqLibPrefix = "/usr/lib/squirrel/";
	
	sqConfig = kapp->config();
	sqLoader = new KIconLoader(*(KGlobal::iconLoader()));
	sqWStack = 0L;
	sqCurrentURL = 0L;
	mainSplitter = 0L;
	tray = 0L;

	sqConfig->setGroup("Main");
	bool first_time = sqConfig->readBoolEntry("first_time", true);

	iconSizeList = new QValueVector<int>;
	iconSizeList->append(16);
	iconSizeList->append(22);
	iconSizeList->append(32);
	iconSizeList->append(48);

	sqConfig->setGroup("Interface");
	toolbarIconSize = (*iconSizeList)[sqConfig->readNumEntry("toolbar icon size", 0)];
	createFirst = sqConfig->readNumEntry("create first", 0);

	// !! Find libraries
	sqLibHandler = new SQ_LibraryHandler;
	sqLibUpdater = new SQ_LibraryListener;
	sqLibUpdater->openURL(KURL(sqLibPrefix), false, true);

	// create view
	sqConfig->setGroup("Interface");
	curViewType = (Squirrel::ViewType)sqConfig->readNumEntry("ViewType",  Squirrel::SQuirrel);

	// set background color for GLview
	sqConfig->setGroup("GL view");
	sqGLViewBGColor.setNamedColor(sqConfig->readEntry("GL view background", "#cccccc"));

	switch(curViewType)
	{
        	case Squirrel::SQuirrel:		createWidgetsLikeSQuirrel(); break;
        	case Squirrel::Gqview:		createWidgetsLikeGqview();	 break;
        	case Squirrel::Kuickshow:	createWidgetsLikeKuickshow();break;
        	case Squirrel::WinViewer:	createWidgetsLikeWinViewer();  break;
        	case Squirrel::Xnview:		createWidgetsLikeXnview();  break;
        	case Squirrel::Browser:		createWidgetsLikeBrowser();  break;

        	default:
         		createWidgetsLikeSQuirrel();
	}
	
	move(0,0);
	resize(QApplication::desktop()->width(), QApplication::desktop()->height());

	show();

	if(first_time)
		if(QMessageBox::information(sqApp, "Squirrel", "You are running ksquirrel at first time (config file dosn't exist).\nI am using now default values.\n\nPlease visit \"Options\" dialog to adjust appearence.\n\nDo it now ?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
			pAConfigure->activate();
}

Squirrel::~Squirrel()
{}

void Squirrel::slotExecuteRunMenu()
{
	pmLaunch->exec(QCursor::pos());
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
		sqConfig->setGroup("Fileview");
		sqConfig->writeEntry("last visited", (sqWStack->getURL()).path());

		sqConfig->deleteGroup("Filters");
		sqConfig->deleteGroup("Filters ext");

		int count = sqFilters->count(), cur = 1;
		QString num;

		for(int i = 0;i < count;i++,cur++)
		{
			sqConfig->setGroup("Filters");
			num.sprintf("%d", cur);
			sqConfig->writeEntry(num, sqFilters->getFilterName(i));
			sqConfig->setGroup("Filters ext");
			sqConfig->writeEntry(num, sqFilters->getFilterExt(i));
		}

		sqConfig->setGroup("Main");
		sqConfig->writeEntry("first_time", false);

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
			pmLaunch->insertItem(pMenuProc->GetPixmap(i), pMenuProc->GetName(i), pMenuProc->GetElementId(i));
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
	filterList = new SQ_FileviewFilter;

	FILTER tmp_filter;
	QString ext, tmp;
	unsigned int i;
	
	actionFilterMenu = new KPopupMenu;
	sqConfig->setGroup("Filters");
/*
	tmp_filter.name = "All files";
	tmp_filter.filter = "*";
	filterList->addFilter(tmp_filter);
	tmp_filter.name = "All supported files";
	tmp_filter.filter = sqLibHandler->allSupportedForFilter();
	filterList->addFilter(tmp_filter);
*/
	for(i = 1;;i ++)
	{
		ext.sprintf("%d", i);
		sqConfig->setGroup("Filters");
		tmp = sqConfig->readEntry(ext, "");
		
		if(tmp == "") break;

		tmp_filter.name = tmp;
		sqConfig->setGroup("Filters ext");
		tmp = sqConfig->readEntry(ext, "");
		tmp_filter.filter = tmp;

		filterList->addFilter(tmp_filter);
	}

	for(i = 0;i < filterList->count();i++)
		actionFilterMenu->insertItem(filterList->getFilterName(i), 7000+i);

	if(filterList->count()) slotSetFilter(7000);
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
	sqWStack->setNameFilter(((*filterList)[id-7000]).filter);
}

void Squirrel::slotGLView()
{
	sqGLView->show();
}

void Squirrel::createWidgetsLikeSQuirrel()
{
	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	pTLocation = new KToolBar(this, QMainWindow::Top, true, "Location toolbar");
	pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	CreateLocationToolbar();

	CreateStatusBar();

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");

	mainSplitter = new QSplitter(mainDock);

	sqWStack = new SQ_WidgetStack(mainSplitter);
	sqWStack->raiseFirst(createFirst);

	mainDock->setWidget(mainSplitter);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	SQ_TreeView *pTree = new SQ_TreeView(mainSplitter);
	mainSplitter->moveToFirst(pTree);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURL(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURL(const QString&)));

	CreateToolbar(fileTools);

	menubar = menuBar();
	CreateMenu(menubar);

	sqGLView = new SQ_GLViewWidget;

	QValueList<int> l;
	l.append(1);
	l.append(4);
	mainSplitter->setSizes(l);
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

	glView = new SQ_GLViewWidget(mainSplitter);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURL(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURL(const QString&)));

	QValueList<int> l;
	l.append(3);
	l.append(7);
	mainSplitter->setSizes(l);
}

void Squirrel::createWidgetsLikeKuickshow()
{
	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	CreateStatusBar();

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

	sqGLView = new SQ_GLViewWidget;
}

void Squirrel::createWidgetsLikeWinViewer()
{
	CreateStatusBar();

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
	sqGLView = new SQ_GLViewWidget(V);

	QHBox *H = new QHBox(V);
	QLabel *ltmp = new QLabel("", H);
	KToolBar *pToolbarTools = new KToolBar(sqApp, H, true, "WinViewer tools");
	QLabel *ltmp2 = new QLabel("", H);

	CreateToolbar(pToolbarTools);
	CreateMenu(pMenuBar);

	H->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	H->setStretchFactor(ltmp, 1);
	H->setStretchFactor(pToolbarTools, 0);
	H->setStretchFactor(ltmp2, 1);

	// WinViewer specific
	pARaiseIconView->setEnabled(false);
	pARaiseListView->setEnabled(false);
	pARaiseDetailView->setEnabled(false);
	pARaiseIconView->unplug(pToolbarTools);
	pARaiseListView->unplug(pToolbarTools);
	pARaiseDetailView->unplug(pToolbarTools);
	sqWStack->pAIconBigger->unplug(pToolbarTools);
	sqWStack->pAIconSmaller->unplug(pToolbarTools);
	pAGLView->unplug(pToolbarTools);
	sqWStack->hide();
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
	sqWStack = new SQ_WidgetStack(H);
	sqWStack->raiseFirst(createFirst);

	SQ_TreeView *pTree = new SQ_TreeView(H);
	H->moveToFirst(pTree);

	sqGLView = new SQ_GLViewWidget(V);

	CreateToolbar(toolBar());
	CreateMenu(menuBar());

	// "XnView" specific
	pAGLView->unplug(toolBar());

	QValueList<int> l;
	l.append(1);
	l.append(3);
	V->setSizes(l);
}

void Squirrel::createWidgetsLikeBrowser() // simple hack
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
	sqWStack = new SQ_WidgetStack(H);
	sqWStack->raiseFirst(createFirst);

	SQ_TreeView *pTree = new SQ_TreeView(H);
	H->moveToFirst(pTree);

	sqGLView = new SQ_GLViewWidget(V);

	CreateToolbar(toolBar());
	CreateMenu(menuBar());

	// "XnView" specific
	pAGLView->unplug(toolBar());

	QValueList<int> l;
	l.append(1);
	l.append(3);
	V->setSizes(l);
}

void Squirrel::CreateStatusBar()
{
	sbar = statusBar();
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

	QLabel *levak = new QLabel(sbar);

	sbar->addWidget(sqSBdirInfo, 0, true);
	sbar->addWidget(sqSBcurFileInfo, 0, true);
	sbar->addWidget(vb, 0, true);
	sbar->addWidget(sqSBDecoded, 0, true);
	sbar->addWidget(levak, 1, true);
}

void Squirrel::CreateMenu(KMenuBar *menubar)
{
	pop_file = new KPopupMenu(menubar);
	pop_edit = new KPopupMenu(menubar);
	pop_view = new KPopupMenu(menubar);

	InitRunMenu();
	InitFilterMenu();

	menubar->insertItem("&File", pop_file);
	menubar->insertItem("&Edit", pop_edit);
	menubar->insertItem("&View", pop_view);
	menubar->insertItem("&Launch", pmLaunch);
	menubar->insertItem("&Filter", actionFilterMenu);
	menubar->insertItem("&Help", helpMenu());

	sqWStack->pAIconBigger->plug(pop_view);
	sqWStack->pAIconSmaller->plug(pop_view);
	pop_view->insertSeparator();
	pARaiseIconView->plug(pop_view);
	pARaiseListView->plug(pop_view);
	pARaiseDetailView->plug(pop_view);
	pAExit->plug(pop_file);
	pARescan->plug(pop_edit);
	pAConfigure->plug(pop_edit);
	pAExtTools->plug(pop_edit);
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
	pAConfigure->plug(tools);
	pAExtTools->plug(tools);
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
	pAExtTools = new KAction("External tools ...", QIconSet(sqLoader->loadIcon("memory", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("memory", KIcon::Desktop, toolbarIconSize)), KShortcut(), this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");

	pARaiseListView = new KRadioAction("Raise 'list' view", QIconSet(sqLoader->loadIcon("view_icon", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_icon", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
	pARaiseIconView = new KRadioAction("Raise 'icon' view", QIconSet(sqLoader->loadIcon("view_choose", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_choose", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
	pARaiseDetailView = new KRadioAction("Raise 'detailed' view", QIconSet(sqLoader->loadIcon("view_detailed", KIcon::Desktop, KIcon::SizeSmall),sqLoader->loadIcon("view_detailed", KIcon::Desktop, toolbarIconSize)), 0, this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
	pARaiseListView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseIconView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseDetailView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
}

void Squirrel::slotNextFile()
{
	sqWStack->slotNext();
}

void Squirrel::slotPreviousFile()
{
	sqWStack->slotPrevious();
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

void Squirrel::control(const QString &str)
{
	if(str == "ACTIVATE")
	{
		this->showNormal();
	}
}

void Squirrel::slotExtTools()
{
	SQ_ExternalTools etd(this, "sq_exttools", true);
	etd.exec();
}
