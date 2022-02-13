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

#include <kapp.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <klocale.h>

#include <stdio.h>
#include <unistd.h>

#include "ksquirrel.h"
#include "sq_treeview.h"
#include "sq_menu_proc.h"
#include "sq_options.h"
#include "sq_editpicture.h"
#include "sq_grabwidget.h"
#include "sq_specialpage.h"
#include "sq_page.h"

#include "sq_about.h"


Squirrel * Squirrel::App = 0;


Squirrel::Squirrel(QWidget *parent, const char *name) : KDockMainWindow (parent, name)
{
	App = this;

	static const int toolbarIconSize = 22;

	kconf = new KConfig(QString("squirrelrc"));
	iconL = new KIconLoader(*(KGlobal::iconLoader()));
	bookmarks = new QValueList<KURL>;

///////////////////////////////////
// Insert toolbar

	fileTools = toolBar("tools");
	fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("edit", KIcon::Desktop, toolbarIconSize), "Edit picture", QString::null,  this, SLOT(slotEdit()), fileTools);
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("ksnapshot", KIcon::Desktop, toolbarIconSize), "Take screenshot", QString::null, this, SLOT(slotGrab()), fileTools);
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);
	
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("penguin", KIcon::Desktop, toolbarIconSize), "Edit options", QString::null, this, SLOT(slotOptions()), fileTools);
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
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("view_choose", KIcon::Desktop, toolbarIconSize), "New Page: List View", QString::null, this, SLOT(slotNewPageList()), fileTools);
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("view_detailed", KIcon::Desktop, toolbarIconSize), "New Page: Detailed View", QString::null, this, SLOT(slotNewPageDetailed()), fileTools);
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("konsole", KIcon::Desktop, toolbarIconSize), "New Page: Konsole module", QString::null, this, SLOT(slotNewPageKonsole()), fileTools);
	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("delslide", KIcon::Desktop, toolbarIconSize), "Close current page", QString::null, this, SLOT(slotCloseCurrentPage()), fileTools);
	fileTools->addSeparator();
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("launch", KIcon::Desktop, toolbarIconSize), "Run external program", QString::null, this, SLOT(slotDoNothing()), fileTools);
	pTBAbstractButton->setPopup(pmLaunch);
	pTBAbstractButton->setPopupDelay(10);
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);

	pTBAbstractButton = new QToolButton(sqLoader->loadIcon("exit", KIcon::Desktop, toolbarIconSize), "Close Squirrel", QString::null, this, SLOT(slotExit()), fileTools);
	(new QToolButton(QPixmap(0), QString::null, QString::null,  this, 0, fileTools))->setEnabled(false);

//////////////////////////////
// Menu
	menubar = menuBar();

	pop_file = new KPopupMenu(menubar);
	pop_edit = new KPopupMenu(menubar);
	pop_view = new KPopupMenu(menubar);

	menubar->insertItem("&File", pop_file);
	menubar->insertItem("&Edit", pop_edit);
	menubar->insertItem("&View", pop_view);

	menubar->insertItem("&Help", helpMenu());

	pop_edit->insertItem("Options", this, SLOT(slotOptions()), CTRL+Key_P);
	pop_file->insertItem("Quit", this, SLOT(slotExit()), ALT+Key_Q);

///////////////////////////////
// StatusBar

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

	KDockWidget *mainDock;
	mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");
	
	// TODO: option "show content when resizing" [true|false]
	mainDock->dockManager()->setSplitterOpaqueResize(true);

	QWidget *cw = new QWidget(mainDock);
    
	mainDock->setWidget(cw);
	mainDock->setDockSite(KDockWidget::DockCorner);
	mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(mainDock);
	setMainDockWidget(mainDock);

	KDockWidget *pdockTree = 0L;

	pdockTree = createDockWidget("Tree view", 0L, 0L, "");
	pdockTree->setWidget(new SQ_TreeView);
	pdockTree->manualDock(mainDock, KDockWidget::DockLeft, 23);
	
	KDockWidget *pdockTabView;
	pdockTabView = createDockWidget("File browser", 0L, 0L, "");
	pMainTabWidget = new SQ_MyTabWidget;
	pMainTabWidget->setFocusPolicy(QTabWidget::NoFocus);
	pdockTabView->setWidget(pMainTabWidget);
	pdockTabView->manualDock(mainDock, KDockWidget::DockLeft, 100);

	KDockWidget* pdockPreview;
	pdockPreview = createDockWidget("File preview", 0L, 0L, "");
	QWidget *pwPreview = new QWidget(pdockPreview);
	pwPreview->setPaletteBackgroundColor(QColor(82, 82, 82));
	pdockPreview->setWidget(pwPreview);
	pdockPreview->manualDock(pdockTree, KDockWidget::DockBottom, 70);
    
	slotNewPageIcon();

	KAction *pkRunMenu = new KAction("", "", KShortcut(CTRL+Key_M), this, SLOT(slotExecuteRunMenu()), this, "execute 'Run' menu");

	this->move(0,0);
	this->resize(QApplication::desktop()->width(), QApplication::desktop()->height());
	this->show();
}

void Squirrel::slotExecuteRunMenu()
{
	pmLaunch->exec(QCursor::pos());
}

void Squirrel::slotEdit()
{
    EditPicture *editpict = new EditPicture(this);
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
	tmpw = new SQ_Page(0, "/", KFile::Simple, 1);
	pMainTabWidget->addTab(tmpw, head);
	pMainTabWidget->setCurrentPage(pMainTabWidget->indexOf(tmpw));
}

void Squirrel::slotNewPageList()
{
	SQ_Page *tmpw;
	QString head("List view");
	tmpw = new SQ_Page(0, "/", KFile::Simple, 2);
	pMainTabWidget->addTab(tmpw, head);
	pMainTabWidget->setCurrentPage(pMainTabWidget->indexOf(tmpw));
}

void Squirrel::slotNewPageDetailed()
{
	SQ_Page *tmpw;
	QString head("Detailed view");
	tmpw = new SQ_Page(0, "/", KFile::Detail, -1);
	pMainTabWidget->addTab(tmpw, head);
	pMainTabWidget->setCurrentPage(pMainTabWidget->indexOf(tmpw));
}

void Squirrel::slotNewPageKonsole()
{
	SQ_SpecialPage *spec = new SQ_SpecialPage(0xff, 0);
	pMainTabWidget->addTab(spec, "konsole");
	pMainTabWidget->setCurrentPage(pMainTabWidget->indexOf(spec));
}

void Squirrel::slotGrab()
{
	new SQ_GrabWidget(this);
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

Squirrel::~Squirrel()
{}
