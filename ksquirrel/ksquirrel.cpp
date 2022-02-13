#include <qsplitter.h>
#include <qapplication.h>
#include <qpixmap.h>
//#include <qmainwindow.h>
#include <qvaluelist.h>
#include <qkeysequence.h>
#include <qnamespace.h>
#include <qpushbutton.h>
//#include <qvbox.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <qcolor.h>
#include <qlabel.h>
//#include <qmotifstyle.h>
//#include <qprogressbar.h>
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

    kconf = new KConfig(QString("squirrelrc"));
    iconL = new KIconLoader(*(KGlobal::iconLoader()));
    bookmarks = new QValueList<KURL>;

    sqConfig->setGroup("Main");

//    tray = new SQ_SystemTray(this);
//    tray->setPixmap(sqLoader->loadIcon("kalarm", KIcon::Desktop, 22));
//    tray->show();

//    QVBoxLayout *l = new QVBoxLayout (this);


///////////////////////////////////
// Insert toolbar

    this->setUsesBigPixmaps(true);

    fileTools = toolBar("tools");//new QToolBar("tools", this, this, true);
    fileTools->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    tbEdit = new QToolButton(sqLoader->loadIcon("edit", KIcon::Desktop, 32), "Edit picture", QString::null,  this, SLOT(slotEdit()), fileTools);
    tbEdit->setUsesTextLabel(true);
    tbEdit->setTextLabel("Edit", false);
    tbEdit->setTextPosition(QToolButton::Under);

    tbGrab = new QToolButton(sqLoader->loadIcon("ksnapshot", KIcon::Desktop, 32), "Take screenshot", QString::null, this, SLOT(slotGrab()), fileTools);
    tbGrab->setUsesTextLabel(true);
    tbGrab->setTextLabel("Screenshot");
    tbGrab->setTextPosition(QToolButton::Under);

    
    fullIcon = sqLoader->loadIcon("window_fullscreen", KIcon::Desktop, 32);
    unfullIcon = sqLoader->loadIcon("window_nofullscreen", KIcon::Desktop, 32);
    tbFull = new QToolButton(fullIcon, "Toggle fullscreen", QString::null, this, SLOT(slotShowFullScreen()), fileTools);
    tbFull->setUsesTextLabel(true);
    tbFull->setTextLabel("Fullscreen", false);
    tbFull->setTextPosition(QToolButton::Under);

    tbOptions = new QToolButton(sqLoader->loadIcon("penguin", KIcon::Desktop, 32), "Edit options", QString::null, this, SLOT(slotOptions()), fileTools);
    tbOptions->setUsesTextLabel(true);
    tbOptions->setTextLabel("Options", false);
    tbOptions->setTextPosition(QToolButton::Under);

    SQ_MenuProcess *menuproc = new SQ_MenuProcess;

    sqConfig->setGroup("Run");
    int number = sqConfig->readNumEntry("number", 1);

	for(int i = 1;i <= number;i++)
	{
		char str1[5], str2[5];        	
		sprintf(str1, "%d", i);
		sprintf(str2, "%da", i);
		menuproc->AddItem(sqConfig->readEntry(str1), sqConfig->readEntry(str2));
	}

	QPopupMenu *pmLaunch = new QPopupMenu;

    int getcount = menuproc->GetCount();

    for(int i = 0;i < getcount;i++)
    {
        if(strcmp(menuproc->GetName(i), "SEPARATOR"))
	        pmLaunch->insertItem(sqLoader->loadIcon(menuproc->GetProgName(i), KIcon::Desktop, 16), menuproc->GetName(i), menuproc->GetElementId(i));
        else
            pmLaunch->insertSeparator();
    }

    connect(pmLaunch, SIGNAL(activated(int)), menuproc, SLOT(slotRunCommand(int)));

    tbRun = new QToolButton(sqLoader->loadIcon("launch", KIcon::Desktop, 32), "Run external program", QString::null, this, SLOT(slotDoNothing()), fileTools);
    tbRun->setUsesTextLabel(true);
    tbRun->setTextLabel("Run ...", false);
    tbRun->setTextPosition(QToolButton::Under);
    tbRun->setPopup(pmLaunch);
    tbRun->setPopupDelay(10);


    tbNewPage = new QToolButton(sqLoader->loadIcon("windows_list", KIcon::Desktop, 32), "New Page in main view", QString::null, this, SLOT(slotDoNothing()), fileTools);
    tbNewPage->setUsesTextLabel(true);
    tbNewPage->setTextLabel("New page", false);
    tbNewPage->setTextPosition(QToolButton::Under);

    QPopupMenu *pmNewPageMenu = new QPopupMenu;

    pmNewPageMenu->insertItem(sqLoader->loadIcon("view_icon", KIcon::Desktop, 16), "Icon view", 10000);
    pmNewPageMenu->insertItem(sqLoader->loadIcon("view_choose", KIcon::Desktop, 16), "List view", 10001);
    pmNewPageMenu->insertItem(sqLoader->loadIcon("view_detailed", KIcon::Desktop, 16), "Detailed view", 10002);
    pmNewPageMenu->insertSeparator();
    pmNewPageMenu->insertItem(sqLoader->loadIcon("konsole", KIcon::Desktop, 16), "Konsole", 10003);
    pmNewPageMenu->insertSeparator();
    pmNewPageMenu->insertItem(sqLoader->loadIcon("tab_remove", KIcon::Desktop, 16), "Close current", 10004);
    tbNewPage->setPopup(pmNewPageMenu);
    tbNewPage->setPopupDelay(10);
    connect(pmNewPageMenu, SIGNAL(activated(int)), this, SLOT(slotNewPage(int)));


    tbExit = new QToolButton(sqLoader->loadIcon("error", KIcon::Desktop, 32), "Close Squirrel", QString::null, this, SLOT(slotExit()), fileTools);
    tbExit->setUsesTextLabel(true);
    tbExit->setTextLabel("Quit", false);
    tbExit->setTextPosition(QToolButton::Under);


/////////////////////////////////////////////////////
// Insert tabwidgets & splitters
    menubar = menuBar();

    pop_file = new KPopupMenu(menubar);
    pop_edit = new KPopupMenu(menubar);
    pop_view = new KPopupMenu(menubar);


//////////////////////////////
// Menu

    menubar->insertItem("&File", pop_file);
    menubar->insertItem("&Edit", pop_edit);
    menubar->insertItem("&View", pop_view);

    aboutData.addAuthor("CKulT", 0, "ckult@yandex.ru");
    aboutData.addCredit("", "", "", 0);

    KHelpMenu *hlp = new KHelpMenu(this, &aboutData);
    menubar->insertItem("&Help", hlp->menu());

    pop_edit->insertItem("Options", this, SLOT(slotOptions()), CTRL+Key_P);
    pop_file->insertItem("Quit", this, SLOT(slotExit()), ALT+Key_Q);
    pop_view->insertItem("FullScreen", this, SLOT(slotShowFullScreen()), ALT+Key_F12);

    sbar = statusBar();
    sbar->setSizeGripEnabled(true);
    sbar->show();

/////////////////////////// views

    KDockWidget *mainDock;
    mainDock = createDockWidget("MainDockWidget", 0L, 0L, "main_dock_widget");

    QWidget *cw = new QWidget(mainDock);
    
    mainDock->setWidget(cw);
    mainDock->setDockSite(KDockWidget::DockCorner);
    mainDock->setEnableDocking(KDockWidget::DockNone);
    setView(mainDock);
    setMainDockWidget(mainDock);

	KDockWidget* dockTree = 0L;

        sqConfig->setGroup("Interface");
        if(sqConfig->readBoolEntry("load treeview", true))
        {
		dockTree = createDockWidget("Tree view", 0L, 0L, "");
		dockTree->setWidget(new SQ_TreeView);
		dockTree->manualDock(mainDock, KDockWidget::DockLeft, 25);
	}
	
    KDockWidget* dockTabView;
    dockTabView = createDockWidget("File browser", 0L, 0L, "");
    tbmain = new SQ_MyTabWidget;
    tbmain->setFocusPolicy(QTabWidget::NoFocus);
    dockTabView->setWidget(tbmain);
    dockTabView->manualDock(mainDock, KDockWidget::DockLeft, 100);

    KDockWidget* dockPreview;
    dockPreview = createDockWidget("File preview", 0L, 0L, "");
    QWidget *preview = new QWidget(dockPreview);
    preview->setPaletteBackgroundColor(QColor(82, 82, 82));
    dockPreview->setWidget(preview);
    dockPreview->manualDock(dockTree, KDockWidget::DockBottom, 60);

    
    slotNewPage(10001);
    
    this->move(0,0);
    this->resize(QApplication::desktop()->width(), QApplication::desktop()->height());
    this->show();
}

void Squirrel::slotShowFullScreen()
{
    if(isFullScreen())
    {
		showNormal();
		tbFull->setDown(false);
		tbFull->setIconSet(fullIcon);
    }
    else
    {
		showFullScreen();
		tbFull->setDown(true);
		tbFull->setIconSet(unfullIcon);
    }
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
/*
void Squirrel::resizeEvent(QResizeEvent *event)
{
//	left->resize(width(), height()-105);
//	left->resize(640, 480);
	QMainWindow::resizeEvent(event);
}
*/
void Squirrel::slotExit()
{
    this->hide();
    sqConfig->setGroup("Main");
    sqConfig->writeEntry("build", build);
    sqConfig->sync();
    close();
}

void Squirrel::slotNewPage(int id)
{
    SQ_Page *tmpw;
    QString head;

    if(id == 10000)
    {
        tmpw = new SQ_Page(0, "/", KFile::Simple, 1, true);
        head = "Icon view";
    }
    else if(id == 10001)
   {    
        tmpw = new SQ_Page(0, "/", KFile::Simple, 2, true);
        head = "List view";
   }
    else if(id == 10002)
   {
        tmpw = new SQ_Page(0, "/", KFile::Detail, -1, true);
        head = "Detailed view";
   }
   else if(id == 10003)
   {
	SQ_SpecialPage *spec = new SQ_SpecialPage(0xff, 0);
	tbmain->addTab(spec, "konsole");
	tbmain->setCurrentPage(tbmain->indexOf(spec));
	return;
   }
   else if(id == 10004)
   {
     slotCloseCurrentPage();
     return;
   }
    else return;

    tbmain->addTab(tmpw, head);
    tbmain->setCurrentPage(tbmain->indexOf(tmpw));
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
	if(tbmain->count() > 0)
	{
		QWidget *cur =(QWidget*)tbmain->currentPage();
		tbmain->removePage(cur);
	}
	else return;
}

Squirrel::~Squirrel()
{}
