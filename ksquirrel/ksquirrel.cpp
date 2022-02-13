/***************************************************************************
                          ksquirrel.cpp  -  description
                             -------------------
    begin                : Dec 10 2003
    copyright            : (C) 2003-2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
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
#include <qeventloop.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qvaluevector.h>
#include <qvaluelist.h>
#include <qsplitter.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qfile.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>

#include <kapp.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kwin.h>
#include <kstatusbar.h>
#include <ksystemtray.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kaction.h>
#include <kbookmarkmanager.h>
#include <kbookmarkmenu.h>
#include <kstandarddirs.h>
#include <kcombobox.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <krun.h>
#include <kanimwidget.h>

#include "ksquirrel.h"
#include "sq_locationtoolbar.h"
#include "sq_widgetstack.h"
#include "sq_tray.h"
#include "sq_treeview.h"
#include "sq_options.h"
#include "sq_about.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
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
#include "sq_diroperator.h"
#include "sq_archivehandler.h"
#include "sq_updateksquirrelthread.h"

static const int menuParam = 100000;

KSquirrel * KSquirrel::App = 0;

KSquirrel::KSquirrel(SQ_HLOptions *HL_Options, QWidget *parent, const char *name) : KMainWindow (parent, name), DCOPObject(name)
{
	App = this;
	first_time = false;

	QString testConf = locateLocal("config", "ksquirrelrc", true);

	if(!QFile::exists(testConf))
	{
		writeDefaultEntries(testConf);
		first_time = true;
	}

	sqConfig = new SQ_Config("ksquirrelrc");
	sqLoader = kapp->iconLoader();
	sqHighLevel = HL_Options;
	old_id = 0;

	sqThumbSize = new SQ_ThumbnailSize((SQ_ThumbnailSize::Size)sqConfig->readNumEntry("Thumbnails", "size", SQ_ThumbnailSize::Huge));
	sqThumbSize->setExtended(sqConfig->readBoolEntry("Thumbnails", "extended", false));

	sqCache = new SQ_PixmapCache;
	sqArchive = new SQ_ArchiveHandler;

	mainBox = 0L;
	sqWStack = 0L;
	sqCurrentURL = 0L;
	mainSplitter = 0L;
	sqExternalTool = 0L;
	sqTree = 0L;
	sqBookmarks = 0L;
	sqFiltersName = 0L;

	preCreate();

	sqLibHandler = new SQ_LibraryHandler;
	sqLibUpdater = new SQ_LibraryListener;

	sqLibUpdater->setAutoErrorHandlingEnabled(false, 0L);

	connect(sqLibUpdater, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));
	
	slotRescan();
}

KSquirrel::~KSquirrel()
{}

void KSquirrel::handlePositionSize()
{
	QRect rect(0,0,800,600);
	setGeometry(sqConfig->readRectEntry("Interface", "geometry", &rect));
}

void KSquirrel::slotOptions()
{
	SQ_Options optd(this, "sq_options", true);

	old_disable = sqConfig->readBoolEntry("Fileview", "disable_dirs", false);
	old_ext = sqConfig->readBoolEntry("Thumbnails", "extended", false);
	old_marks = sqConfig->readBoolEntry("GL view", "marks", true);

	if(optd.start() == QDialog::Accepted)
		applyDefaultSettings();
}

void KSquirrel::closeEvent(QCloseEvent *ev)
{
	if(sqConfig->readBoolEntry("Main", "minimize to tray", false))
	{
		slotGotoTray();
		ev->ignore();
	}
	else
	{
		if(sqGLView->isSeparate())
			sqGLView->hide();

		finalActions();
		ev->accept();
	}
}

void KSquirrel::slotRaiseListView()
{
	sqWStack->raiseWidget(0);
}

void KSquirrel::slotRaiseIconView()
{
	sqWStack->raiseWidget(1);
}

void KSquirrel::slotRaiseDetailView()
{
	sqWStack->raiseWidget(2);
}

void KSquirrel::slotRaiseThumbView()
{
	sqWStack->raiseWidget(3);
}

void KSquirrel::createLocationToolbar(SQ_LocationToolbar *pTLocation)
{
	sqCurrentURL = new KHistoryCombo(true, pTLocation, "history combobox");

	pTLocation->setFullSize();
	pTLocation->insertButton("button_cancel", 0, SIGNAL(clicked()), sqCurrentURL, SLOT(clearHistory()), true, i18n("Clear history"));
	pTLocation->insertButton("locationbar_erase", 1, SIGNAL(clicked()), sqCurrentURL, SLOT(clearEdit()), true, i18n("Clear adress"));
	pTLocation->insertWidget(2, 10, new QLabel("URL:", pTLocation, "kde toolbar widget"));
	pTLocation->setItemAutoSized(2);
	pTLocation->insertWidget(3, 10, sqCurrentURL);
	pTLocation->setItemAutoSized(3);
	pTLocation->insertButton("goto", 4, SIGNAL(clicked()), this, SLOT(slotGo()), true, i18n("Go!"));

	sqCurrentURL->setDuplicatesEnabled(false);
	sqCurrentURL->setSizeLimit(20);
	sqCurrentURL->setHistoryItems(sqConfig->readListEntry("History", "items"), true);
}

void KSquirrel::initFilterMenu()
{
	QString ext, tmp;
	unsigned int i = menuParam;

	QString last;

	if(!sqFiltersName)
	{
		sqFiltersName = new QStringList(sqConfig->readListEntry("Filters", "items"));
		sqFiltersExt = new QStringList(sqConfig->readListEntry("Filters", "extensions"));

		last = sqConfig->readEntry("Filters", "last", "*");

		actionFilterMenu->setCheckable(true);
	}
	else
		last = sqWStack->getNameFilter();

	actionFilterMenu->clear();
	int id, Id = old_id;
	bool both = sqConfig->readBoolEntry("Filters", "menuitem both", true);
	QStringList quickInfo;

	sqLibHandler->allFilters(libFilters, quickInfo);

	QValueList<QString>::iterator BEGIN = libFilters.begin();
	QValueList<QString>::iterator END = libFilters.end();
	QValueList<QString>::iterator BEGIN1 = quickInfo.begin();

	actionFilterMenu->insertTitle(i18n("Libraries' filters"));

	for(QValueList<QString>::iterator it = BEGIN, it1 = BEGIN1;it != END;++it, ++it1)
	{
		if(both)
			id = actionFilterMenu->insertItem(*it1 + " (" + *it + ")");
		else
			id = actionFilterMenu->insertItem(*it1);

		actionFilterMenu->setItemParameter(id, i++);

		if(last == *it && !last.isEmpty())
			Id = id;
	}

	QString allF = sqLibHandler->allFiltersString();

	libFilters.append(allF);

	id = actionFilterMenu->insertItem(i18n("All supported formats"));
	actionFilterMenu->setItemParameter(id, i++);

	if(last == allF && !last.isEmpty())
		Id = id;

	QValueList<QString>::iterator nEND = sqFiltersName->end();
	QValueList<QString>::iterator it_name = sqFiltersName->begin();
	QValueList<QString>::iterator it_ext = sqFiltersExt->begin();

	actionFilterMenu->insertTitle(i18n("User's filters"));
	i = 0;
//	actionFilterMenu->setItemParameter(id, i++);

	for(;it_name != nEND;it_name++,it_ext++)
	{
		if(both)
			id = actionFilterMenu->insertItem(*it_name + "  (" + *it_ext + ")");
		else
			id = actionFilterMenu->insertItem(*it_name);

		if(last == *it_ext && !last.isEmpty())
			Id = id;

		actionFilterMenu->setItemParameter(id, i++);
	}

	disconnect(actionFilterMenu, SIGNAL(activated(int)), 0, 0);
	connect(actionFilterMenu, SIGNAL(activated(int)), this, SLOT(slotSetFilter(int)));

	setFilter(last, Id);
}

void KSquirrel::slotGo()
{
	sqWStack->setURLForCurrent(pCurrentURL->currentText());
}

void KSquirrel::setFilter(const QString &f, const int id)
{
	if(sqWStack->getNameFilter() != f)
		sqWStack->setNameFilter(f);

//	actionFilterMenu->setItemChecked(old_id, false);
	actionFilterMenu->setItemChecked(id, true);
	old_id = id;
}

void KSquirrel::slotSetFilter(int id)
{
	actionFilterMenu->setItemChecked(old_id, false);
	int index = actionFilterMenu->itemParameter(id);

	QString filt;

	if(index >= menuParam)
		filt = libFilters[index - menuParam];
	else
		filt = (*sqFiltersExt)[index];

	if(sqWStack->getNameFilter() != filt)
		sqWStack->setNameFilter(filt);

	actionFilterMenu->setItemChecked(id, true);
	old_id = id;
}

void KSquirrel::createWidgets(int createFirst)
{
	hastree = sqConfig->readBoolEntry("Interface", "has_tree", true);
	m_urlbox = sqConfig->readBoolEntry("Interface", "has_url", true);
	m_sep = sqConfig->readBoolEntry("Interface", "gl_separate", false);

	mainBox = new QVBox(this); Q_CHECK_PTR(mainBox);
	mainBox->resize(size());
	menubar = new KMenuBar(mainBox); Q_CHECK_PTR(menubar);

	viewBrowser = new QWidgetStack(mainBox, QString::fromLatin1("SQ_BROWSER_WIDGET_STACK")); Q_CHECK_PTR(viewBrowser);
	QVBox *b1 = new QVBox(viewBrowser); Q_CHECK_PTR(b1);
	tools = new KToolBar(b1); Q_CHECK_PTR(tools);

	pTLocation = new SQ_LocationToolbar(b1, QString::fromLatin1("Location toolbar")); Q_CHECK_PTR(pTLocation);

	pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	mainSplitter = new QSplitter(Qt::Horizontal, b1); Q_CHECK_PTR(mainSplitter);
	KStatusBar *s = new KStatusBar(b1); Q_CHECK_PTR(s);

	createLocationToolbar(pTLocation);
	createStatusBar(s);

	sqWStack = new SQ_WidgetStack(mainSplitter); Q_CHECK_PTR(sqWStack);
	sqWStack->raiseFirst(createFirst);

	sqTree = new SQ_TreeView(mainSplitter); Q_CHECK_PTR(sqTree);

	mainSplitter->moveToFirst(sqTree);

	sqTree->setShown(hastree);
	pTLocation->setShown(m_urlbox);

	pATree->setChecked(hastree);
	pAURL->setChecked(m_urlbox);
	pASeparateGL->setChecked(m_sep);

	b1->setStretchFactor(mainSplitter, 1);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), sqWStack, SLOT(setURLForCurrent(const QString&)));

	b2 = new QVBox(viewBrowser);
	Q_CHECK_PTR(b2);

	if(!m_sep)
		sqGLView = new SQ_GLView(b2);
	else
		sqGLView = new SQ_GLView;

	Q_CHECK_PTR(sqGLView);

	createToolbar(tools);
	createMenu(menubar);

	switch(createFirst)
	{
		case 0: pARaiseListView->setChecked(true); break;
		case 1: pARaiseIconView->setChecked(true); break;
		case 2: pARaiseDetailView->setChecked(true); break;
		case 3: pARaiseThumbView->setChecked(true); break;

		default:
			pARaiseListView->setChecked(true);
	}

	viewBrowser->addWidget(b1, 0);
	viewBrowser->addWidget(b2, 1);

	connect(pATree, SIGNAL(toggled(bool)), this, SLOT(slotSetTreeShown(bool)));
	connect(pAURL, SIGNAL(toggled(bool)), pTLocation, SLOT(setShown(bool)));
	connect(pASeparateGL, SIGNAL(toggled(bool)), this, SLOT(slotSeparateGL(bool)));

	mainSizes = sqConfig->readIntListEntry("Interface", "splitter1");
	mainSplitter->setSizes(mainSizes);
}

void KSquirrel::createStatusBar(KStatusBar *bar)
{
	sbar = bar;
	sbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sbar->setSizeGripEnabled(true);
	sbar->show();

	sqSBdirInfo = new QLabel(sbar);
	QHBox *vb = new QHBox(sbar);

	sqSBfileIcon = new QLabel(vb);
	sqSBfileIcon->setScaledContents(false);

	sqSBfileName = new QLabel(QString::fromLatin1("----"), vb);
	QLabel *levak = new QLabel(sbar);

	sbar->addWidget(sqSBdirInfo, 0, true);
	sbar->addWidget(vb, 0, true);
	sbar->addWidget(levak, 1, true);

	sqSBDecodedBox = new QHBox;
	sqSBDecodedBox->setSpacing(2);
	sqSBDecodedI = new QLabel(QString::fromLatin1("--"), sqSBDecodedBox);
	sqSBDecoded = new QLabel(QString::fromLatin1("----"), sqSBDecodedBox);

	sqSBGLZoom = new QLabel(QString::fromLatin1("----"), 0);
	sqSBGLAngle = new QLabel(QString::fromLatin1("----"), 0);
	sqSBGLCoord = new QLabel(QString::fromLatin1("----"), 0);
	sqSBLoaded = new QLabel(QString::fromLatin1("----"), 0);
	sqSBFrame = new QLabel(QString::fromLatin1("0/0"), 0);
	sqSBFrame->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter | Qt::ExpandTabs);
	sqSBFile = new QLabel(0);
	sqSBFile->setAlignment(Qt::AlignRight | Qt::AlignTop);
	QFont font = sqSBFile->font();
	font.setBold(true);
	sqSBFile->setFont(font);
}

void KSquirrel::createMenu(KMenuBar *menubar)
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

	pAOpen->plug(pop_file);
	pAOpenAndSet->plug(pop_file);
	pop_file->insertSeparator();
	sqWStack->pAHome->plug(pop_file);
	sqWStack->pAUp->plug(pop_file);
	sqWStack->pABack->plug(pop_file);
	sqWStack->pAForw->plug(pop_file);
	sqWStack->pAMkDir->plug(pop_file);
	pop_file->insertSeparator();
	sqWStack->pAHidden->plug(pop_file);
	sqWStack->pAProp->plug(pop_file);
	sqWStack->pADelete->plug(pop_file);
	pop_file->insertSeparator();
	pAExit->plug(pop_file);

	pAGotoTray->plug(pop_view);
	pop_view->insertSeparator();
	pARaiseListView->plug(pop_view);
	pARaiseIconView->plug(pop_view);
	pARaiseDetailView->plug(pop_view);
	pARaiseThumbView->plug(pop_view);
	pop_view->insertSeparator();
	pATree->plug(pop_view);
	pASeparateGL->plug(pop_view);
	pAURL->plug(pop_view);

	pARescan->plug(pop_edit);
	pAExtTools->plug(pop_edit);
	pAFilters->plug(pop_edit);
	pAConfigure->plug(pop_edit);
}

void KSquirrel::createToolbar(KToolBar *tools)
{
	sqWStack->pABack->plug(tools);
	sqWStack->pAForw->plug(tools);
	sqWStack->pAUp->plug(tools);
	sqWStack->pAHome->plug(tools);
	sqWStack->pAMkDir->plug(tools);
	sqWStack->pAHidden->plug(tools);

	pARaiseListView->plug(tools);
	pARaiseIconView->plug(tools);
	pARaiseDetailView->plug(tools);
	pARaiseThumbView->plug(tools);
	pAThumbs->plug(tools);

	pAGLView->plug(tools);
	pAExtTools->plug(tools);
	pAFilters->plug(tools);
	pAConfigure->plug(tools);
	pAGotoTray->plug(tools);
	pAExit->plug(tools);

	pATree->plug(tools);
	pASeparateGL->plug(tools);
	pAURL->plug(tools);

	tools->insertAnimatedWidget(1000, this, SLOT(slotAnimatedClicked()), locate("appdata", "images/anime.png"));
	tools->alignItemRight(1000, true);
	tools->animatedWidget(1000)->start();
}

void KSquirrel::createActions()
{
	pAExit = KStdAction::quit(this, SLOT(close()), actionCollection(), "SQ close");
  	pAConfigure = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "SQ edit options");
	pAGLView = new KAction(i18n("Image window"), "desktop", KShortcut(CTRL+Key_G), this, SLOT(raiseGLWidget()), actionCollection(), "SQ gl view widget");
	pARescan = KStdAction::redisplay(this, SLOT(slotRescan()), actionCollection(), "SQ rescan libraries");
	pARescan->setText(i18n("Rescan libraries"));
	pAExtTools = new KAction(i18n("Configure external tools ..."), "memory", KShortcut(CTRL + Key_E), this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");
	pAFilters = new KAction(i18n("Configure filters ..."), "filefind", KShortcut(CTRL + Key_F), this, SLOT(slotFilters()), actionCollection(), "SQ filters");
	pAGotoTray = new KAction(i18n("Goto tray"), "compfile", 0, this, SLOT(slotGotoTray()), actionCollection(), "SQ goto tray");
	pAOpenAndSet = new KAction(i18n("Open file") + QString::fromLatin1(" #2"), "fileopen", 0, this, SLOT(slotOpenFileAndSet()), actionCollection(), "SQ open and set");
	pAOpen = new KAction(i18n("Open file"), "fileopen", 0, this, SLOT(slotOpenFile()), actionCollection(), "SQ open file");

	pARaiseListView = new KRadioAction(i18n("List"), "view_icon", KShortcut(CTRL + Key_1), this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
	pARaiseIconView = new KRadioAction(i18n("Icons"), "view_multicolumn", KShortcut(CTRL + Key_2), this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
	pARaiseDetailView = new KRadioAction(i18n("Detailes"), "view_detailed", KShortcut(CTRL + Key_3), this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
	pARaiseThumbView = new KRadioAction(i18n("Thumbnails"), "view_icon", KShortcut(CTRL + Key_4), this, SLOT(slotRaiseThumbView()), actionCollection(), "SQ raise thumbs view");

	pATree = new KToggleAction(i18n("Toggle tree"), "view_tree", KShortcut(CTRL + Key_T), 0, 0, actionCollection(), "SQ toggle tree");
	pAURL = new KToggleAction(i18n("Toggle URL box"), "history", KShortcut(CTRL + Key_U), 0, 0, actionCollection(), "SQ toggle url box");
	pASeparateGL = new KToggleAction(i18n("Make image window separate/built-in"), "window_new", KShortcut(CTRL + Key_G), 0, 0, actionCollection(), "SQ toggle gl separate");

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
	pAThumbs->setDelayed(false);

	pARaiseListView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseIconView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseDetailView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
	pARaiseThumbView->setExclusiveGroup(QString("raise_some_widget_from_stack"));
}

void KSquirrel::slotGotoTray()
{
	tray->show();

	if(sqGLView->isSeparate())
		sqGLView->hide();

	hide();
}

void KSquirrel::initExternalTools()
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

void KSquirrel::slotFilters()
{
	SQ_Filters f(this);

	if(f.start() != QDialog::Accepted) return;

	initFilterMenu();
}

void KSquirrel::slotExtTools()
{
	SQ_ExternalTools etd(this, "sq_exttools", true);

	if(etd.start() == QDialog::Accepted)
		sqExternalTool->getNewPopupMenu();
}

void KSquirrel::raiseGLWidget()
{
	if(sqGLView->isSeparate())
	{
		sqGLView->show();
		sqGLView->raise();
		KWin::activateWindow(sqGLView->winId());
	}
	else
	{
		viewBrowser->raiseWidget(1);
	}
}

void KSquirrel::slotCloseGLWidget()
{
	if(sqGLView->isSeparate())
	{
		sqGLView->hide();
	}
	else
	{
		viewBrowser->raiseWidget(0);
	}
}

void KSquirrel::initBookmarks()
{
	QString file = locate("data", "kfile/bookmarks.xml");

	if(file.isEmpty())
		file = locateLocal("data", "kfile/bookmarks.xml");

	KBookmarkManager *bmanager = KBookmarkManager::managerForFile(file, false);
	bmanager->setUpdate(true);
	bmanager->setShowNSBookmarks(false);

	sqBookmarks = new SQ_BookmarkOwner(this);

	bookmarks = new KActionMenu(i18n("&Bookmarks"), "bookmark", (KActionCollection*)0L, "bookmarks");
	bookmarkMenu = new KBookmarkMenu(bmanager, bookmarkOwner, bookmarks->popupMenu(), (KActionCollection*)0L, true);

	connect(sqBookmarks, SIGNAL(openURL(const KURL&)), sqWStack, SLOT(setURLForCurrent(const KURL&)));
}

void KSquirrel::writeDefaultEntries(const QString &toConf)
{
	QString absConf = locate("appdata", "ksquirrelrc");

	QFile absFile(absConf);

	if(absFile.open(IO_ReadOnly))
	{
		QFile toFile(toConf);

		if(toFile.open(IO_WriteOnly))
		{
			toFile.writeBlock(absFile.readAll());
			toFile.close();
		}
		else
			KMessageBox::error(this, i18n("User depended config file couldn't be opened for writing!"));
	
		absFile.close();
	}
	else
		KMessageBox::error(this, i18n("Default (installed) config file couldn't be opened for reading!"));
}

void KSquirrel::applyDefaultSettings()
{
	bool updateDirs = false, updateThumbs = false;

	sqGLWidget->setClearColor();
	sqGLWidget->setZoomFactor(sqConfig->readNumEntry("GL view", "zoom", 25));
	sqGLWidget->setMoveFactor(sqConfig->readNumEntry("GL view", "move", 5));
	sqGLWidget->setRotateFactor(sqConfig->readNumEntry("GL view", "angle", 90));
	sqLibUpdater->setAutoUpdate(sqConfig->readBoolEntry("Libraries", "monitor", true));
	sqThumbSize->setExtended(sqConfig->readBoolEntry("Thumbnails", "extended", false));

	if(old_marks != sqConfig->readBoolEntry("GL view", "marks", true))
		sqGLWidget->updateGLA();

	if(old_disable != sqConfig->readBoolEntry("Fileview", "disable_dirs", false))
		updateDirs = true;

	updateDirs &= (bool)sqWStack->visibleWidget()->numDirs();

	if(old_ext != sqConfig->readBoolEntry("Thumbnails", "extended", false))
		updateThumbs = true;

	KURL _url;
	_url.setPath(sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/"));

	if(!_url.equals(sqLibUpdater->url(), true))
	{
		sqLibHandler->clear();
		sqLibUpdater->openURL(_url, false, true);
	}

	sqWStack->configureClickPolicy();
	sqWStack->updateGrid(!updateThumbs | updateDirs);

	sqCache->setCacheLimit(sqConfig->readNumEntry("Thumbnails", "cache", 1024*10));

	if(sqConfig->readBoolEntry("Main", "sync", true))
		sqConfig->sync();

	if(updateDirs || updateThumbs)
		sqWStack->updateView();
}

void KSquirrel::slotFullScreen(bool full)
{
	WId id = (sqGLView->isSeparate()) ? sqGLView->winId() : winId();
	KStatusBar *s = sqGLView->statusbar();

	if(s && sqConfig->readBoolEntry("GL view", "hide_sbar", true))
	{
		s->setShown(!full);
		sqGLWidget->pAStatus->setChecked(!full);
	}

	if(full)
	{
		if(!sqGLView->isSeparate())
			menubar->hide();

		KWin::setState(id, NET::FullScreen);
	}
	else
	{
		KWin::clearState(id, NET::FullScreen);

		if(!sqGLView->isSeparate())
			menubar->show();
	}
}

void KSquirrel::saveValues()
{
	sqExternalTool->writeEntries();

	sqConfig->setGroup("Filters");
	sqConfig->writeEntry("items", *sqFiltersName);
	sqConfig->writeEntry("extensions", *sqFiltersExt);
	sqConfig->writeEntry("last", sqWStack->getNameFilter());

	sqConfig->setGroup("Fileview");
	sqConfig->writeEntry("last visited", (sqWStack->getURL()).path());

	if(sqConfig->readBoolEntry("Fileview", "history", true) && sqCurrentURL)
	{
		sqConfig->setGroup("History");
		sqConfig->writeEntry("items", sqCurrentURL->historyItems());
	}

	sqConfig->setGroup("GL view");

	if(sqGLView->isSeparate())
		sqConfig->writeEntry("geometry", sqGLView->geometry());

	sqConfig->writeEntry("statusbar", sqGLWidget->pAStatus->isChecked());
	sqConfig->writeEntry("quickGeometry", sqQuickBrowser->geometry());
	sqConfig->writeEntry("ignore", sqGLWidget->pAIfLess->isChecked());
	sqConfig->writeEntry("zoom type", sqGLWidget->zoomType());
	sqConfig->writeEntry("actions", sqGLWidget->actions());
	sqConfig->writeEntry("toolbars_hidden", sqGLWidget->actionsHidden());

	sqConfig->setGroup("Interface");
	sqConfig->writeEntry("last view", sqWStack->id((QWidget*)sqWStack->visibleWidget()));
	sqConfig->writeEntry("geometry", geometry());
	sqConfig->writeEntry("has_tree", sqTree->configVisible());
	sqConfig->writeEntry("has_url", pTLocation->configVisible());
	sqConfig->writeEntry("gl_separate", sqGLView->isSeparate());

	if(sqTree->configVisible())
		sqConfig->writeEntry("splitter1", mainSplitter->sizes());
	else
		sqConfig->writeEntry("splitter1", mainSizes);

	sqConfig->setGroup("Thumbnails");
	sqConfig->writeEntry("size", sqThumbSize->value());
}

QString KSquirrel::slotRescan()
{
	QString sqLibPrefix = sqConfig->readEntry("Libraries", "prefix", "/usr/lib/squirrel/");

	if(!sqLibPrefix.endsWith("/"))
		sqLibPrefix += "/";

	KURL url = sqLibPrefix;
	
	sqLibHandler->clear();
	sqLibUpdater->slotOpenURL(url, false, true);

	return sqLibPrefix;
}

void KSquirrel::createPostSplash()
{
	QColor cc(255,255,255);
	QHBox *hbox = new QHBox(0, 0, WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder | WX11BypassWM);
	hbox->setFrameShape(QFrame::Box);
	
	QLabel *pp = new QLabel(hbox);
	pp->setPalette(QPalette(cc, cc));
	pp->setAlignment(Qt::AlignCenter);
	
	QPixmap todo = QPixmap::fromMimeSource(locate("appdata", "images/tray.png"));
	
	if(todo.isNull())
	    todo = sqLoader->loadIcon("ksquirrel", KIcon::Desktop, 16);

	pp->setPixmap(todo);
	pp->setFixedWidth(18);

	QLabel *l = new QLabel(i18n(" writing settings and thumbnails ...  "), hbox);
	l->setPalette(QPalette(cc, cc));
	l->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	hbox->setStretchFactor(pp, 0);
	hbox->setStretchFactor(l, 1);

	int w = 200, h = 20;
	hbox->setGeometry(QApplication::desktop()->width() / 2 - w / 2, QApplication::desktop()->height() / 2 - h / 2, w, h);

	hbox->show();
}

void KSquirrel::slotThumbsSmall()
{
	emit thumbSizeChanged("small");
}

void KSquirrel::slotThumbsMedium()
{
	emit thumbSizeChanged("medium");
}

void KSquirrel::slotThumbsLarge()
{
	emit thumbSizeChanged("large");
}

void KSquirrel::slotThumbsHuge()
{
	emit thumbSizeChanged("huge");
}

void KSquirrel::finalActions()
{
	hide();

	bool create_splash = (!sqConfig->readBoolEntry("Thumbnails", "dont write", false)) & (!sqCache->empty());

	if(create_splash)
	{
		createPostSplash();
		qApp->processEvents();
	}

	saveValues();
	sqConfig->sync();

	if(create_splash)
		sqCache->sync();
}

void KSquirrel::enableThumbsMenu(bool enable)
{
	pAThumbs->setEnabled(enable);
}

void KSquirrel::preCreate()
{
	initExternalTools();
	sqExternalTool->getNewPopupMenu();

	pAThumbs = new KActionMenu(i18n("Thumbnail size"), QPixmap::fromMimeSource(locate("appdata", "images/thumbs/thumbs_what.png")));
	createActions();

	actionFilterMenu = new KPopupMenu;
	actionViews = new KPopupMenu;

	pARaiseListView->plug(actionViews);
	pARaiseIconView->plug(actionViews);
	pARaiseDetailView->plug(actionViews);
	pARaiseThumbView->plug(actionViews);
}

void KSquirrel::resizeEvent(QResizeEvent *e)
{
	if(mainBox)
		mainBox->resize(e->size());
}

void KSquirrel::slotContinueLoading()
{
	if(sqHighLevel->showLibsAndExit)
	    exit(0);

	disconnect(sqLibUpdater, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));

	int createFirst = sqConfig->readNumEntry("Interface", "last view", 0);

	createWidgets(createFirst);

	qApp->processEvents();

	handlePositionSize();

	if(first_time)
	{
		applyDefaultSettings();
		first_time = false;
	}

	tray = new SQ_SystemTray;

	qApp->processEvents();

	if(sqGLView->isSeparate())
		sqGLView->show();

	show();

	if(sqGLView->isSeparate())
		sqGLView->hide();

	if(!sqHighLevel->path.isEmpty())
	{
		QFileInfo fm(sqHighLevel->path);

		if(fm.isFile() && fm.isReadable())
		{
			KFileView *v = sqWStack->visibleWidget()->view();
			v->clearSelection();
			v->setCurrentItem(fm.fileName());
			v->setSelected(v->currentFileItem(), true);

			sqGLWidget->slotStartDecoding(sqHighLevel->path, true);
		}
	}

	updater = new SQ_UpdateKsquirrelThread;

	connect(updater, SIGNAL(needUpdate(const QString &)), this, SLOT(slotNeedUpdate(const QString &)));

	if(sqConfig->readBoolEntry("Main", "check", true))
		updater->start();
}

void KSquirrel::slotSetTreeShown(bool shown)
{
	mainSizes = mainSplitter->sizes();
	sqTree->setShown(shown);
}

void KSquirrel::slotSeparateGL(bool sep)
{
	if(sep)
	{
		viewBrowser->raiseWidget(0);
		sqGLView->reparent(0, QPoint(0,0), false);
		QRect rect(0,0,320,200);
		sqGLView->setGeometry(sqConfig->readRectEntry("GL view", "geometry", &rect));
		raiseGLWidget();
	}
	else
	{
		sqConfig->setGroup("GL view");
		sqConfig->writeEntry("geometry", sqGLView->geometry());
		sqGLView->reparent(b2, QPoint(0,0), true);
	}
}

void KSquirrel::setCaption(const QString &cap)
{
	if(sqGLView->isSeparate())
		sqGLView->setCaption(cap);
	else
		KMainWindow::setCaption(cap);
}

void KSquirrel::openFile(bool parseURL)
{
	QString name = KFileDialog::getOpenFileName();

	if(name.isEmpty())
		return;

	QFileInfo f(name);

	if(!f.isReadable())
		return;

	if(sqLibHandler->supports(name))
	{
		if(parseURL)
			sqWStack->setURLForCurrent(f.dirPath(true));

		sqGLWidget->slotStartDecoding(name, true);
	}
	else
		if(sqConfig->readBoolEntry("Fileview", "run unknown", true))
				new KRun(name);
}

void KSquirrel::slotOpenFile()
{
	openFile();
}

void KSquirrel::slotOpenFileAndSet()
{
	openFile(true);
}

void KSquirrel::slotNeedUpdate(const QString &ver)
{
	(new KAction(i18n("New version is available"), "help", KShortcut(), this, SLOT(slotShowUpdate()), actionCollection(), "SQ show update"))->plug(tools);

	new_version = ver;
}

void KSquirrel::slotShowUpdate()
{
	QWhatsThis::display(i18n("<pre>Current version is <b>%1</b>\nNew version <b>%2</b> is available at <font color=\"#FF0000\">http://ksquirrel.sourceforge.net/download</font> </pre>").arg(QString(SQ_VERSION)).arg(new_version));
}

void KSquirrel::slotAnimatedClicked()
{
	kapp->invokeBrowser(QString::fromLatin1("http://ksquirrel.sourceforge.net/"));
}

KPopupMenu* KSquirrel::menuFilters()
{
	return actionFilterMenu;
}

KPopupMenu* KSquirrel::menuViews()
{
	return actionViews;
}
