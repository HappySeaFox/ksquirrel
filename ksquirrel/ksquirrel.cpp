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
#include <qfileinfo.h>

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
#include "sq_dir.h"
#include "sq_thumbnailjob.h"
#include "sq_navigatordropmenu.h"
#include "sq_converter.h"
#include "sq_resizer.h"

#include <unistd.h>

static const int menuParam = 100000;

KSquirrel * KSquirrel::m_app = 0L;
KIconLoader * KSquirrel::iconL = 0L;

KSquirrel::KSquirrel(SQ_HLOptions *HL_Options, QWidget *parent, const char *name) : KMainWindow (parent, name)
{
	m_app = this;
	first_time = false;

	QString testConf = locateLocal("config", "ksquirrelrc");

	if(!QFile::exists(testConf))
	{
		writeDefaultEntries(testConf);
		first_time = true;
	}

	kconf = new SQ_Config("ksquirrelrc");
	iconL = kapp->iconLoader();
	sqHighLevel = HL_Options;
	old_id = 0;

	thumbSize = new SQ_ThumbnailSize((SQ_ThumbnailSize::Size)kconf->readNumEntry("Thumbnails", "size", SQ_ThumbnailSize::Huge));
	thumbSize->setExtended(kconf->readBoolEntry("Thumbnails", "extended", false));

	cache = new SQ_PixmapCache;
	ar = new SQ_ArchiveHandler;

	mainBox = 0L;
	sqCurrentURL = 0L;
	mainSplitter = 0L;
	sqBookmarks = 0L;
	sqFiltersName = 0L;

	preCreate();

	libhandler = new SQ_LibraryHandler;
	libl = new SQ_LibraryListener;

	libl->setAutoErrorHandlingEnabled(false, 0L);

	connect(libl, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));
	connect(this, SIGNAL(continueLoading()), this, SLOT(slotContinueLoading2()));

	slotRescan();
}

KSquirrel::~KSquirrel()
{}

void KSquirrel::handlePositionSize()
{
	QRect rect(0,0,800,600);
	setGeometry(kconf->readRectEntry("Interface", "geometry", &rect));
}

void KSquirrel::slotOptions()
{
	SQ_Options optd(this, "sq_options", true);

	old_disable = kconf->readBoolEntry("Fileview", "disable_dirs", false);
	old_ext = kconf->readBoolEntry("Thumbnails", "extended", false);
	old_marks = kconf->readBoolEntry("GL view", "marks", true);

	if(optd.start() == QDialog::Accepted)
		applyDefaultSettings();
}

void KSquirrel::closeEvent(QCloseEvent *ev)
{
	if(kconf->readBoolEntry("Main", "minimize to tray", false))
	{
		slotGotoTray();
		ev->ignore();
	}
	else
	{
		if(gl_view->isSeparate())
			gl_view->hide();

		finalActions();
		ev->accept();
	}
}

void KSquirrel::slotRaiseListView()
{
	pWidgetStack->raiseWidget(0);
}

void KSquirrel::slotRaiseIconView()
{
	pWidgetStack->raiseWidget(1);
}

void KSquirrel::slotRaiseDetailView()
{
	pWidgetStack->raiseWidget(2);
}

void KSquirrel::slotRaiseThumbView()
{
	pWidgetStack->raiseWidget(3);
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
	sqCurrentURL->setHistoryItems(kconf->readListEntry("History", "items"), true);
}

void KSquirrel::initFilterMenu()
{
	QString ext, tmp;
	unsigned int i = menuParam;

	QString last;

	if(!sqFiltersName)
	{
		sqFiltersName = new QStringList(kconf->readListEntry("Filters", "items"));
		sqFiltersExt = new QStringList(kconf->readListEntry("Filters", "extensions"));

		last = kconf->readEntry("Filters", "last", "*");

		actionFilterMenu->setCheckable(true);
	}
	else
		last = pWidgetStack->getNameFilter();

	actionFilterMenu->clear();
	int id, Id = old_id;
	bool both = kconf->readBoolEntry("Filters", "menuitem both", true);
	QStringList quickInfo;

	SQ_LibraryHandler::instance()->allFilters(libFilters, quickInfo);

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

	QString allF = SQ_LibraryHandler::instance()->allFiltersString();

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
	pWidgetStack->setURLForCurrent(pCurrentURL->currentText());
}

void KSquirrel::setFilter(const QString &f, const int id)
{
	if(pWidgetStack->getNameFilter() != f)
		pWidgetStack->setNameFilter(f);

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

	if(pWidgetStack->getNameFilter() != filt)
		pWidgetStack->setNameFilter(filt);

	actionFilterMenu->setItemChecked(id, true);
	old_id = id;
}

void KSquirrel::createWidgets(int createFirst)
{
	hastree = kconf->readBoolEntry("Interface", "has_tree", true);
	m_urlbox = kconf->readBoolEntry("Interface", "has_url", true);
	m_sep = kconf->readBoolEntry("Interface", "gl_separate", false);

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

	new SQ_NavigatorDropMenu;

	pWidgetStack = new SQ_WidgetStack(mainSplitter); Q_CHECK_PTR(pWidgetStack);
	pWidgetStack->raiseFirst(createFirst);

	ptree = new SQ_TreeView(mainSplitter); Q_CHECK_PTR(ptree);

	mainSplitter->moveToFirst(ptree);

	ptree->setShown(hastree);
	pTLocation->setShown(m_urlbox);

	pATree->setChecked(hastree);
	pAURL->setChecked(m_urlbox);
	pASeparateGL->setChecked(m_sep);

	b1->setStretchFactor(mainSplitter, 1);

	connect(sqCurrentURL, SIGNAL(returnPressed(const QString&)), pWidgetStack, SLOT(setURLForCurrent(const QString&)));
	connect(sqCurrentURL, SIGNAL(activated(const QString&)), pWidgetStack, SLOT(setURLForCurrent(const QString&)));

	b2 = new QVBox(viewBrowser);
	Q_CHECK_PTR(b2);

	if(!m_sep)
		gl_view = new SQ_GLView(b2);
	else
		gl_view = new SQ_GLView;

	Q_CHECK_PTR(gl_view);

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

	mainSizes = kconf->readIntListEntry("Interface", "splitter1");
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
	pWidgetStack->pAHome->plug(pop_file);
	pWidgetStack->pAUp->plug(pop_file);
	pWidgetStack->pABack->plug(pop_file);
	pWidgetStack->pAForw->plug(pop_file);
	pWidgetStack->pAMkDir->plug(pop_file);
	pop_file->insertSeparator();
	pWidgetStack->pAHidden->plug(pop_file);
	pWidgetStack->pAProp->plug(pop_file);
	pWidgetStack->pADelete->plug(pop_file);
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
	tools->insertWidget(0, 0, new QToolButton(KSquirrel::loader()->loadIcon("back", KIcon::Desktop, 22), i18n("Back"), QString::null, pWidgetStack->pABack, SLOT(activate()), tools));
	tools->insertWidget(0, 0, new QToolButton(KSquirrel::loader()->loadIcon("forward", KIcon::Desktop, 22), i18n("Forward"), QString::null, pWidgetStack->pAForw, SLOT(activate()), tools));
	tools->insertWidget(0, 0, new QToolButton(KSquirrel::loader()->loadIcon("up", KIcon::Desktop, 22), i18n("Up"), QString::null, pWidgetStack->pAUp, SLOT(activate()), tools));

//	pWidgetStack->pABack->plug(tools);
//	pWidgetStack->pAForw->plug(tools);
//	pWidgetStack->pAUp->plug(tools);
	pWidgetStack->pAHome->plug(tools);
	pWidgetStack->pAMkDir->plug(tools);
	pWidgetStack->pAHidden->plug(tools);

	pARaiseListView->plug(tools);
	pARaiseIconView->plug(tools);
	pARaiseDetailView->plug(tools);
	pARaiseThumbView->plug(tools);
	pAThumbs->plug(tools);

	pAGLView->plug(tools);
	pAExtTools->plug(tools);
	pAFilters->plug(tools);
	pAConfigure->plug(tools);
//	pAGotoTray->plug(tools);
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
	pAThumbs = new KActionMenu(i18n("Thumbnail size"), "thumbnail");
	pAExit = KStdAction::quit(this, SLOT(close()), actionCollection(), "SQ close");
  	pAConfigure = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "SQ edit options");
	pAGLView = new KAction(i18n("Image window"), "desktop", KShortcut(CTRL+Key_I), this, SLOT(raiseGLWidget()), actionCollection(), "SQ gl view widget");
	pARescan = KStdAction::redisplay(this, SLOT(slotRescan()), actionCollection(), "SQ rescan libraries");
	pARescan->setText(i18n("Rescan libraries"));
	pAExtTools = new KAction(i18n("Configure external tools ..."), "runprog", KShortcut(CTRL + Key_E), this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");
	pAFilters = new KAction(i18n("Configure filters ..."), "filefind", KShortcut(CTRL + Key_F), this, SLOT(slotFilters()), actionCollection(), "SQ filters");
	pAGotoTray = new KAction(i18n("Goto tray"), "idea", 0, this, SLOT(slotGotoTray()), actionCollection(), "SQ goto tray");
	pAOpenAndSet = new KAction(i18n("Open file") + QString::fromLatin1(" #2"), "fileopen", 0, this, SLOT(slotOpenFileAndSet()), actionCollection(), "SQ open and set");
	pAOpen = new KAction(i18n("Open file"), "fileopen", 0, this, SLOT(slotOpenFile()), actionCollection(), "SQ open file");

	pAImageActions = new KActionMenu(i18n("Image actions"), "images");
	pAImageActions->insert(new KAction(i18n("Convert images"), QString::null, 0, conv, SLOT(slotStartEdit()), actionCollection(), "SQ Menu Image Convert"));
	pAImageActions->insert(new KAction(i18n("Resize images"), QString::null, 0, rsz, SLOT(slotStartEdit()), actionCollection(), "SQ Menu Image Resize"));

	pARaiseListView = new KRadioAction(i18n("List"), "view_icon", KShortcut(CTRL + Key_1), this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
	pARaiseIconView = new KRadioAction(i18n("Icons"), "view_multicolumn", KShortcut(CTRL + Key_2), this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
	pARaiseDetailView = new KRadioAction(i18n("Details"), "view_detailed", KShortcut(CTRL + Key_3), this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
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

	switch(thumbSize->value())
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

	if(gl_view->isSeparate())
		gl_view->hide();

	hide();
}

void KSquirrel::initExternalTools()
{
	tryCopyDesktops();

	extool = new SQ_ExternalTool;
	extool->setAutoDelete(true);
	extool->getNewPopupMenu();
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
		extool->getNewPopupMenu();
}

void KSquirrel::raiseGLWidget()
{
	if(gl_view->isSeparate())
	{
		gl_view->show();
		gl_view->raise();
		KWin::activateWindow(gl_view->winId());
	}
	else
	{
		viewBrowser->raiseWidget(1);
	}
}

void KSquirrel::slotCloseGLWidget()
{
	if(gl_view->isSeparate())
	{
		gl_view->hide();
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

	connect(sqBookmarks, SIGNAL(openURL(const KURL&)), pWidgetStack, SLOT(setURLForCurrent(const KURL&)));
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
	bool updateDirs, updateThumbs;

	SQ_GLWidget::window()->setClearColor();
	SQ_GLWidget::window()->setZoomFactor(kconf->readNumEntry("GL view", "zoom", 25));
	SQ_GLWidget::window()->setMoveFactor(kconf->readNumEntry("GL view", "move", 5));
	SQ_GLWidget::window()->setRotateFactor(kconf->readNumEntry("GL view", "angle", 90));
	libl->setAutoUpdate(kconf->readBoolEntry("Libraries", "monitor", true));
	thumbSize->setExtended(kconf->readBoolEntry("Thumbnails", "extended", false));

	if(old_marks != kconf->readBoolEntry("GL view", "marks", true))
		SQ_GLWidget::window()->updateGLA();

	updateDirs = (old_disable != kconf->readBoolEntry("Fileview", "disable_dirs", false));

	updateDirs &= (bool)pWidgetStack->visibleWidget()->numDirs();

	updateThumbs = (old_ext != kconf->readBoolEntry("Thumbnails", "extended", false));

	pWidgetStack->configureClickPolicy();
	pWidgetStack->updateGrid(!updateThumbs | updateDirs);

	cache->setCacheLimit(kconf->readNumEntry("Thumbnails", "cache", 1024*10));

	if(kconf->readBoolEntry("Main", "sync", true))
		kconf->sync();

	if(updateDirs || updateThumbs)
		pWidgetStack->updateView();
}

void KSquirrel::slotFullScreen(bool full)
{
	WId id = (gl_view->isSeparate()) ? gl_view->winId() : winId();
	KStatusBar *s = gl_view->statusbar();

	if(s && kconf->readBoolEntry("GL view", "hide_sbar", true))
	{
		s->setShown(!full);
		SQ_GLWidget::window()->pAStatus->setChecked(!full);
	}

	if(full)
	{
		if(!gl_view->isSeparate())
			menubar->hide();

		KWin::setState(id, NET::FullScreen);
	}
	else
	{
		KWin::clearState(id, NET::FullScreen);

		if(!gl_view->isSeparate())
			menubar->show();
	}
}

void KSquirrel::saveValues()
{
	extool->writeEntries();

	kconf->setGroup("Filters");
	kconf->writeEntry("items", *sqFiltersName);
	kconf->writeEntry("extensions", *sqFiltersExt);
	kconf->writeEntry("last", pWidgetStack->getNameFilter());

	kconf->setGroup("Fileview");
	kconf->writeEntry("last visited", (pWidgetStack->getURL()).path());

	if(kconf->readBoolEntry("Fileview", "history", true) && sqCurrentURL)
	{
		kconf->setGroup("History");
		kconf->writeEntry("items", sqCurrentURL->historyItems());
	}

	kconf->setGroup("GL view");

	if(gl_view->isSeparate())
		kconf->writeEntry("geometry", gl_view->geometry());

	kconf->writeEntry("statusbar", SQ_GLWidget::window()->pAStatus->isChecked());
	kconf->writeEntry("quickGeometry", SQ_QuickBrowser::window()->geometry());
	kconf->writeEntry("ignore", SQ_GLWidget::window()->pAIfLess->isChecked());
	kconf->writeEntry("zoom type", SQ_GLWidget::window()->zoomType());
	kconf->writeEntry("actions", SQ_GLWidget::window()->actions());
	kconf->writeEntry("toolbars_hidden", SQ_GLWidget::window()->actionsHidden());

	kconf->setGroup("Interface");
	kconf->writeEntry("last view", pWidgetStack->id((QWidget*)pWidgetStack->visibleWidget()));
	kconf->writeEntry("geometry", geometry());
	kconf->writeEntry("has_tree", ptree->configVisible());
	kconf->writeEntry("has_url", pTLocation->configVisible());
	kconf->writeEntry("gl_separate", gl_view->isSeparate());

	if(ptree->configVisible())
		kconf->writeEntry("splitter1", mainSplitter->sizes());
	else
		kconf->writeEntry("splitter1", mainSizes);

	kconf->setGroup("Thumbnails");
	kconf->writeEntry("size", thumbSize->value());
}

QString KSquirrel::slotRescan()
{
	QString libPrefix = "/usr/lib/ksquirrel-libs/";

	KURL url = libPrefix;

	SQ_LibraryHandler::instance()->clear();
	libl->slotOpenURL(url, false, true);

	return libPrefix;
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
	    todo = KSquirrel::loader()->loadIcon("ksquirrel", KIcon::Desktop, 16);

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

	bool create_splash = (!kconf->readBoolEntry("Thumbnails", "dont write", false)) & (!cache->empty());

	if(create_splash)
	{
		createPostSplash();
		qApp->processEvents();
	}

	saveValues();
	kconf->sync();

	if(create_splash)
		cache->sync();
}

void KSquirrel::enableThumbsMenu(bool enable)
{
	pAThumbs->setEnabled(enable);
}

void KSquirrel::preCreate()
{
	initExternalTools();

	conv = new SQ_Converter;
	rsz = new SQ_Resizer;

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
		_exit(EXIT_SUCCESS);

	disconnect(libl, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));

	if(!sqHighLevel->thumbs)
		emit continueLoading();
	else
	{
		job = (sqHighLevel->recurs) ? KIO::listRecursive(KURL(sqHighLevel->thumbs_p), false, true) : KIO::listDir(KURL(sqHighLevel->thumbs_p), false, true);
		connect(job, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)), this, SLOT(slotUDSEntries(KIO::Job*, const KIO::UDSEntryList&)));
		connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(listResult(KIO::Job*)));
		dir = new SQ_Dir;
		dir->setRoot(QString::fromLatin1("thumbnails"));
	}
}

void KSquirrel::slotUDSEntries(KIO::Job *j, const KIO::UDSEntryList &list)
{
	if(j != job) return;

	QString m_file;
	KURL url = sqHighLevel->thumbs_p;
	SQ_Thumbnail t;
	bool b;
	QString other_thumb_path = QDir::homeDirPath() + "/.thumbnails";
	other_thumb_path = QDir::cleanDirPath(other_thumb_path);

	KIO::UDSEntryListConstIterator end = list.end();

	for(KIO::UDSEntryListConstIterator it = list.begin(); it != end; ++it)
	{
		KFileItem file(*it, url, true, true);
		m_file = file.url().path();

		QFileInfo ff(m_file);

		if(ff.isReadable() && file.isFile())
			if(!m_file.startsWith(dir->root()) && !m_file.startsWith(other_thumb_path))
			{
				if(dir->updateNeeded(m_file))
				{
					b = SQ_ThumbnailLoadJob::loadThumbnail(m_file, t);

					if(b)
						dir->saveThumbnail(m_file, t);
				}
			}
	}
}

void KSquirrel::listResult(KIO::Job *j)
{
	if(j != job) return;

	emit continueLoading();
}

void KSquirrel::slotContinueLoading2()
{
	int createFirst = kconf->readNumEntry("Interface", "last view", 0);

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

	if(gl_view->isSeparate())
		gl_view->show();

	show();

	if(gl_view->isSeparate())
		gl_view->hide();

	if(!sqHighLevel->path.isEmpty())
	{
		QFileInfo fm(sqHighLevel->path);

		if(fm.isFile() && fm.isReadable())
		{
			KFileView *v = pWidgetStack->visibleWidget()->view();
			v->clearSelection();
			v->setCurrentItem(fm.fileName());
			v->setSelected(v->currentFileItem(), true);

			if(SQ_LibraryHandler::instance()->supports(sqHighLevel->path))
				SQ_GLWidget::window()->slotStartDecoding(sqHighLevel->path, true);
		}
	}

	updater = new SQ_UpdateKsquirrelThread;

	connect(updater, SIGNAL(needUpdate(const QString &)), this, SLOT(slotNeedUpdate(const QString &)));

	if(kconf->readBoolEntry("Main", "check", true))
		updater->start();
}

void KSquirrel::slotSetTreeShown(bool shown)
{
	mainSizes = mainSplitter->sizes();
	ptree->setShown(shown);
}

void KSquirrel::slotSeparateGL(bool sep)
{
	if(sep)
	{
		viewBrowser->raiseWidget(0);
		gl_view->reparent(0, QPoint(0,0), false);
		QRect rect(0,0,320,200);
		gl_view->setGeometry(kconf->readRectEntry("GL view", "geometry", &rect));
		raiseGLWidget();
	}
	else
	{
		kconf->setGroup("GL view");
		kconf->writeEntry("geometry", gl_view->geometry());
		gl_view->reparent(b2, QPoint(0,0), true);
	}
}

void KSquirrel::setCaption(const QString &cap)
{
	if(gl_view->isSeparate())
		gl_view->setCaption(cap);
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

	if(SQ_LibraryHandler::instance()->supports(name))
	{
		if(parseURL)
			pWidgetStack->setURLForCurrent(f.dirPath(true));

		SQ_GLWidget::window()->slotStartDecoding(name, true);
	}
	else
		if(kconf->readBoolEntry("Fileview", "run unknown", true))
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

KIconLoader* KSquirrel::loader()
{
	return iconL;
}

KSquirrel* KSquirrel::app()
{
	return m_app;
}

void KSquirrel::tryCopyDesktops()
{
	QString tmp, str;
	SQ_Dir dir;

	dir.setRoot("desktop");

	for(int i = 1;;i++)
	{
		str.sprintf("%d", i);
		tmp = SQ_Config::instance()->readEntry("External tools", str, QString::null);

		if(tmp.isEmpty() || tmp.isNull())
			break;

		QString name = tmp + ".desktop";
		QString system = ::locate("appdata", QString("desktop/%1").arg(name));
		QString local = dir.getAbsPath(name);

		// if the system .desktop doesn't exist,
		// locate will return QString::null, so this
		// condition will be false
		if(!QFile::exists(system))
			continue;

		if(QFile::exists(local))
			continue;

		QFile in(system);
		QFile out(local);

		if(!in.open(IO_ReadOnly))
			continue;

		if(!out.open(IO_WriteOnly))
			continue;

		QByteArray ind = in.readAll();

		out.writeBlock(ind);

		in.close();
		out.close();
	}
}
