#include <qsplitter.h>
#include <qapplication.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qvaluelist.h>
#include <qkeysequence.h>
#include <qnamespace.h>
#include <qhbox.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qdir.h>
#include <qtimer.h>

#include <kapp.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

#include "ksquirrel.h"
#include "sq_treeview.h"
#include "sq_menu_proc.h"
#include "sq_options.h"
#include "sq_specialpage.h"
#include "sq_page.h"
#include "sq_about.h"
#include "sq_splash.h"

#include "sq_editpicturedialog.h"


Squirrel * Squirrel::App = 0;
KDockWidget *pdock1;


Squirrel::Squirrel(QWidget *parent, const char *name) : KDockMainWindow (parent, name)
{
	splash = new SQ_Splash(this);
	splash->addInfo("Common initialization ...");

	App = this;
	static const int toolbarIconSize = 22;
	menuBookmarksID = 8000;

	kconf = new KConfig(QString("squirrelrc"));
	iconL = new KIconLoader(*(KGlobal::iconLoader()));
	bookmarks = new QValueList<KURL>;

	folderPix = sqLoader->loadIcon("folder", KIcon::Desktop, 16);

	setNewUrl = KURL("/");
	KAction *pARunMenu = new KAction("Show 'Run ...' menu", "", KShortcut(CTRL+Key_R), this, SLOT(slotExecuteRunMenu()), actionCollection(), "execute 'Run' menu");

///////////////////////////////////
// Insert toolbar

	splash->addInfo("Creating toolbar ...");
	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("edit_picture", KIcon::Desktop, toolbarIconSize), "Edit picture", QString::null,  this, SLOT(slotEdit()), fileTools);
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("configure", KIcon::Desktop, toolbarIconSize), "Edit options", QString::null, this, SLOT(slotOptions()), fileTools);
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);

	SQ_MenuProcess *menuproc = new SQ_MenuProcess;

	sqConfig->setGroup("Run");
	int number = sqConfig->readNumEntry("number", 1);

	for(int i = 1;i <= number;i++)
		menuproc->AddItem(sqConfig->readEntry(QString("%1").arg(i, 0, 10)), sqConfig->readEntry(QString("%1a").arg(i, 0, 10)));

	pmLaunch = new QPopupMenu;

	int getcount = menuproc->GetCount();

	for(int i = 0;i < getcount;i++)
		if(strcmp(menuproc->GetName(i), "SEPARATOR"))
		{
			QPixmap px(sqLoader->loadIcon(menuproc->GetProgName(i), KIcon::Desktop, 16));
			QImage im1;
			im1 = px;
			QImage im2;
			im2 = sqLoader->loadIcon("unknown", KIcon::Desktop, 16);
			
			if(im1 == im2)
				pmLaunch->insertItem(menuproc->GetName(i), menuproc->GetElementId(i));
			else
				pmLaunch->insertItem(px, menuproc->GetName(i), menuproc->GetElementId(i));
		}
		else
			pmLaunch->insertSeparator();

	connect(pmLaunch, SIGNAL(activated(int)), menuproc, SLOT(slotRunCommand(int)));

	fileTools->addSeparator();
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("view_icon", KIcon::Desktop, toolbarIconSize), "New Page: Icon View", QString::null, this, SLOT(slotNewPageIcon()), fileTools);
	(void)new KAction("New Page: Icon View", "", KShortcut(ALT+Key_1), this, SLOT(slotNewPageIcon()), actionCollection(), "New Page: Icon View");
	
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("view_choose", KIcon::Desktop, toolbarIconSize), "New Page: List View", QString::null, this, SLOT(slotNewPageList()), fileTools);
	(void)new KAction("New Page: List View", "", KShortcut(ALT+Key_2), this, SLOT(slotNewPageList()), actionCollection(), "New Page: List View");

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("view_detailed", KIcon::Desktop, toolbarIconSize), "New Page: Detailed View", QString::null, this, SLOT(slotNewPageDetailed()), fileTools);
	(void)new KAction("New Page: Detailed View", "", KShortcut(ALT+Key_3), this, SLOT(slotNewPageDetailed()), actionCollection(), "New Page: Detail View");
	
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("konsole", KIcon::Desktop, toolbarIconSize), "New Page: Konsole module", QString::null, this, SLOT(slotNewPageKonsole()), fileTools);
	(void)new KAction("New Page: Konsole", "", KShortcut(ALT+Key_4), this, SLOT(slotNewPageKonsole()), actionCollection(), "New Page: Konsole");
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);
	
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("delslide", KIcon::Desktop, toolbarIconSize), "Close current page", QString::null, this, SLOT(slotCloseCurrentPage()), fileTools);
	(void)new KAction("Close current page", "", KShortcut(CTRL+Key_X), this, SLOT(slotCloseCurrentPage()), actionCollection(), "Close current page");
	fileTools->addSeparator();
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("launch", KIcon::Desktop, toolbarIconSize), "Run external program", QString::null, this, SLOT(slotDoNothing()), fileTools);
	pTBAbstractButton->setPopup(pmLaunch);
	pTBAbstractButton->setPopupDelay(10);
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);


	pTBBookmarksButton = new QToolButton(sqLoader->loadIcon("bookmark_add", KIcon::Desktop, toolbarIconSize), "Add current path to bookmarks", QString::null, this, SLOT(slotAddBookmark()), fileTools);
	menuBookmarks = new QPopupMenu(this);
	pTBBookmarksButton->setPopupDelay(200);
	pTBBookmarksButton->setPopup(menuBookmarks);
	connect(menuBookmarks, SIGNAL(activated(int)), this, SLOT(slotSetURLfromMenu(int)));

	sqConfig->setGroup("Bookmarks");
	QString s, read_;

	if(sqBookmarks->count() > 0)
	{
		for(unsigned i = 0;i < sqBookmarks->count();i++)
			menuBookmarks->insertItem(folderPix, (*sqBookmarks)[i].path(), menuBookmarksID++);
	}
	else for(int i = 1;;i++)
	{
		s.sprintf("%d", i);
		read_ = sqConfig->readEntry(s, "");

		if(read_ == QString(""))
			break;

		if(sqBookmarks->find(KURL(read_)) == sqBookmarks->end())
		{
			sqBookmarks->append(KURL(read_));
			menuBookmarks->insertItem(folderPix, read_, menuBookmarksID++);
		}
	}


	fileTools->addSeparator();

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("exit", KIcon::Desktop, toolbarIconSize), "Close Squirrel", QString::null, this, SLOT(slotExit()), fileTools);
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);

//////////////////////////////
// Menu
	splash->addInfo("Creating menu ...");
	menubar = menuBar();

	pop_file = new KPopupMenu(menubar);
	pop_edit = new KPopupMenu(menubar);
	pop_view = new KPopupMenu(menubar);

	menubar->insertItem("&File", pop_file);
	menubar->insertItem("&Edit", pop_edit);
	menubar->insertItem("&View", pop_view);
	menubar->insertItem("&Help", helpMenu());

	pARunMenu->plug(pop_view);

	pop_edit->insertItem("Options", this, SLOT(slotOptions()), CTRL+Key_P);
	pop_file->insertItem("Quit", this, SLOT(slotExit()), ALT+Key_Q);

///////////////////////////////
// StatusBar
	splash->addInfo("Creating statusbar ...");
	
	sbar = statusBar();
	sbar->setSizeGripEnabled(true);
	sbar->show();

	dirInfo = new QLabel(sbar);
	curFileInfo = new QLabel(sbar);
	QHBox *vb = new QHBox(sbar);
    
	fileIcon = new QLabel(vb);
	fileIcon->setScaledContents(true);

	fileName = new QLabel(vb);

	QLabel *levak = new QLabel(sbar);

	sbar->addWidget(dirInfo, 0, true);
	sbar->addWidget(curFileInfo, 0, true);
	sbar->addWidget(vb, 0, true);
	sbar->addWidget(levak, 2, true);

///////////////////////////
// Views

	splash->addInfo("Creating dockable windows ...");

	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");

	// TODO: option "show content when resizing" [true|false]
	mainDock->dockManager()->setSplitterOpaqueResize(true);

	QWidget *cw = new QWidget(mainDock);

	mainDock->setWidget(cw);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	pdockTree = createDockWidget("Tree view", 0L, 0L, "");
	pdockTree->setWidget(new SQ_TreeView);
	pdock1 = pdockTree->manualDock(mainDock, KDockWidget::DockLeft, 23);

	pdockTabView = createDockWidget("File browser", 0L, 0L, "");
	pMainTabWidget = new SQ_MyTabWidget;
	pMainTabWidget->setFocusPolicy(QTabWidget::NoFocus);
	pMainTabWidget->setTabShape(QTabWidget::Triangular);
	pMainTabWidget->setTabPosition(QTabWidget::Bottom);
	connect(pMainTabWidget, SIGNAL(currentChanged(QWidget*)), this, SLOT(slotCurrentChanged(QWidget*)));
	pdockTabView->setWidget(pMainTabWidget);
	pdockTabView->manualDock(mainDock, KDockWidget::DockLeft, 100);
/*
	pdockPreview = createDockWidget("File preview", 0L, 0L, "");
	QWidget *pwPreview = new QWidget(pdockPreview);
	pwPreview->setPaletteBackgroundColor(QColor(82, 82, 82));
	pdockPreview->setWidget(pwPreview);
	pdockPreview->manualDock(pdockTree, KDockWidget::DockBottom, 70);
*/
	splash->addInfo("Creating page ...");
	slotNewPageIcon();

	this->move(0,0);
	this->resize(QApplication::desktop()->width(), QApplication::desktop()->height());
	this->show();

	splash->addInfo("SQuirrel loaded!");

	QTimer::singleShot(500*1, this, SLOT(slotSplashClose()));
}

void Squirrel::slotExecuteRunMenu()
{
	pmLaunch->exec(QCursor::pos());
}

void Squirrel::slotEdit()
{
	SQ_EditPictureDialog *editpict = new SQ_EditPictureDialog(this);
	editpict->show();
}

void Squirrel::slotOptions()
{
	SQ_Options *md = new SQ_Options(this, "sq_options", true);
	md->show();

	md->exec();
}

void Squirrel::slotExit()
{
	this->hide();
	sqConfig->setGroup("Main");
	sqConfig->writeEntry("build", build);
	sqConfig->sync();
	close();
}

void Squirrel::slotNewPageIcon()
{
	SQ_Page *tmpw;
	QString head("Icon view");
	tmpw = new SQ_Page(0, setNewUrl, KFile::Simple, 1);
	pMainTabWidget->addTab(tmpw, head);
	pMainTabWidget->setCurrentPage(pMainTabWidget->indexOf(tmpw));
	tmpw->pTBIconView->toggle();
}

void Squirrel::slotNewPageList()
{
	SQ_Page *tmpw;
	QString head("List view");
	tmpw = new SQ_Page(0, setNewUrl, KFile::Simple, 2);
	pMainTabWidget->addTab(tmpw, head);
	pMainTabWidget->setCurrentPage(pMainTabWidget->indexOf(tmpw));
	tmpw->pTBListView->toggle();
}

void Squirrel::slotNewPageDetailed()
{
	SQ_Page *tmpw;
	QString head("Detailed view");
	tmpw = new SQ_Page(0, setNewUrl, KFile::Detail, -1);
	pMainTabWidget->addTab(tmpw, head);
	pMainTabWidget->setCurrentPage(pMainTabWidget->indexOf(tmpw));
	tmpw->pTBDetailView->toggle();
}

void Squirrel::slotNewPageKonsole()
{
	SQ_SpecialPage *spec = new SQ_SpecialPage(0xff, 0);
	pMainTabWidget->addTab(spec, "Konsole");
	pMainTabWidget->setCurrentPage(pMainTabWidget->indexOf(spec));
}

void Squirrel::closeEvent(QCloseEvent *ev)
{
    sqConfig->setGroup("Main");

    if(sqConfig->readBoolEntry("Minimize to tray", false) == true)
    {
		hide();
		ev->ignore();
    }
    else
    {
		sqConfig->sync();
		ev->accept();
    }
}

void Squirrel::slotCloseCurrentPage()
{
	if(pMainTabWidget->count() > 0)
	{
		QWidget *cur =(QWidget*)pMainTabWidget->currentPage();
		pMainTabWidget->removePage(cur);
	}
	else return;
}

void Squirrel::slotSplashClose()
{
	splash->close();

	delete splash;
}

void Squirrel::slotAddBookmark()
{/*
	KURL ur = pDirOperator->url();

	if(sqBookmarks->find(ur) == sqBookmarks->end())
	{
		sqBookmarks->append(ur);
		menuBookmarks->insertItem(folderPix, ur.path(), menuBookmarksID++);
	}*/
}

void Squirrel::slotSetURLfromMenu(int id)
{
	if(pMainTabWidget->count() > 0)
		((SQ_Page*)(pMainTabWidget->currentPage()))->setURL(menuBookmarks->text(id));
}

void Squirrel::slotCurrentChanged(QWidget *w)
{
	static QPopupMenu *tmp = new QPopupMenu;

	if(pMainTabWidget->tabLabel(w) == "Konsole")
		pTBBookmarksButton->setPopup(tmp);
	else
		pTBBookmarksButton->setPopup(menuBookmarks);
}

Squirrel::~Squirrel()
{}
