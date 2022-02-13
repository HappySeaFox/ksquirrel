/***************************************************************************
                          ksquirrel.cpp  -  description
                             -------------------
    begin                : Dec 10 2003
    copyright            : (C) 2003 by Baryshev Dmitry
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qapplication.h>
#include <qeventloop.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qvaluevector.h>
#include <qvaluelist.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qfile.h>
#include <qtoolbutton.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qwidgetstack.h>

#include <kapplication.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kwin.h>
#include <kstatusbar.h>
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
#include <kglobalsettings.h>
#include <kactioncollection.h>
#include <konq_operations.h>

#include "ksquirrel.h"
#include "sq_iconloader.h"
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
#include "sq_archivehandler.h"
#include "sq_dir.h"
#include "sq_thumbnailjob.h"
#include "sq_navigatordropmenu.h"
#include "sq_errorstring.h"
#include "sq_thumbnailcachemaster.h"
#include "sq_checkversion.h"
#include "sq_slideshow.h"
#include "sq_glwidget_helpers.h"
#include "sq_pluginsinfo.h"
#include "sq_imageloader.h"
#include "sq_multibar.h"
#include "sq_mountview.h"
#include "sq_sidebaractions.h"
#include "sq_glinfo.h"

#include "sq_converter.h"
#include "sq_resizer.h"
#include "sq_contraster.h"
#include "sq_rotater.h"
#include "sq_printer.h"
#include "sq_filter.h"

#include <cstdlib>

static const int menuParam = 100000;

KSquirrel * KSquirrel::m_instance = NULL;

KSquirrel::KSquirrel(QWidget *parent, const char *name) : KMainWindow (parent, name), DCOPObject(name)
{
    // singleton setup
    m_instance = this;

    kdDebug() << "+KSquirrel" << endl;

    first_time = false;
    slideShowStop = true;
    slideShowPaused = false;

    QString testConf = locateLocal("config", "ksquirrelrc");

    // check if user-depended config file exist. If exist, check
    // if it has newer version (did user forget to delete old config file ?)
    if(!QFile::exists(testConf) || checkConfigFileVersion())
    {
        // if config doesn't exist or it is out-of-date,
        // copy newer config
        QFile::remove(testConf);
        writeDefaultEntries();
        first_time = true;
    }

    // create KConfig instance.
    // It will help us to read and write config entries
    kconf = new SQ_Config("ksquirrelrc");
    old_id = 0;

    kconf->setGroup("Thumbnails");

    thumbSize = new SQ_ThumbnailSize(this, static_cast<SQ_ThumbnailSize::Size>(kconf->readNumEntry("size",
                            SQ_ThumbnailSize::Large)));
    thumbSize->setExtended(kconf->readBoolEntry("extended", false));

    // Additional setup
    new SQ_PixmapCache(this);
    new SQ_ArchiveHandler(this);
    new SQ_IconLoader(this);
    new SQ_ImageLoader(this);

    mainBox = NULL;
    sqFiltersName = NULL;
    pCurrentURL = NULL;

    fillMessages();
    preCreate();

    // When SQ_LibraryHandler loaded all found libraries, it will emit 
    // "finishedInit" signal. We will catch it and continue loading process
    connect(libl, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));
    connect(this, SIGNAL(continueLoading()), this, SLOT(slotContinueLoading2()));

    // load libraries from disk!
    slotRescan();
}

KSquirrel::~KSquirrel()
{
    kdDebug() << "-KSquirrel" << endl;

    delete sqFiltersExt;
    delete sqFiltersName;

    if(gl_view->isSeparate())
        delete gl_view;

    delete kconf;
}

/*
 *  Restore saved position & size
 */
void KSquirrel::handlePositionSize()
{
    QPoint p_def(0,0);
    QSize  sz_def(800, 600);

    kconf->setGroup("Interface");

    QPoint p = SQ_Config::instance()->readPointEntry("pos", &p_def);
    QSize sz = SQ_Config::instance()->readSizeEntry("size", &sz_def);

    resize(sz);
    move(p);
}

/*
 *  Invoke 'Options' dialog.
 */
void KSquirrel::slotOptions()
{
    // create dialog
    SQ_Options optd(this, "sq_options", true);

    kconf->setGroup("Fileview");
    old_disable = kconf->readBoolEntry("disable_dirs", false);

    kconf->setGroup("Thumbnails");
    old_ext = kconf->readBoolEntry("extended", false);

    kconf->setGroup("GL view");
    old_marks = kconf->readBoolEntry("marks", true);

    // if user clicked "OK", apply new settings
    if(optd.start() == QDialog::Accepted)
        applyDefaultSettings();
}

/*
 *  Catches close events.
 */
void KSquirrel::closeEvent(QCloseEvent *ev)
{
    kconf->setGroup("Main");

    // Minimize to tray ?
    if(kconf->readBoolEntry("minimize to tray", false))
    {
        // Yes, let's hide to tray
        slotGotoTray();

        // ignore close event
        ev->ignore();
    }
    else // No, close app
    {
        if(gl_view->isSeparate())
            gl_view->hide();

        // do final stuff
        finalActions();

        // accept close event - exit
        ev->accept();
    }
}

// Show List view
void KSquirrel::slotRaiseListView()
{
    pWidgetStack->raiseWidget(SQ_DirOperatorBase::TypeList);
}

// Show icon view
void KSquirrel::slotRaiseIconView()
{
    pWidgetStack->raiseWidget(SQ_DirOperatorBase::TypeIcons);
}

// Show Detailed view
void KSquirrel::slotRaiseDetailView()
{
    pWidgetStack->raiseWidget(SQ_DirOperatorBase::TypeDetailed);
}

// Show Thumbnail view
void KSquirrel::slotRaiseThumbView()
{
    pWidgetStack->raiseWidget(SQ_DirOperatorBase::TypeThumbs);
}

// Create location toolbar
void KSquirrel::createLocationToolbar(SQ_LocationToolbar *pTLocation)
{
    // create new KHistoryCombo
    pCurrentURL = new KHistoryCombo(true, pTLocation, "history combobox");

    // some additional setup
    pTLocation->setFullSize();
    pTLocation->insertButton("button_cancel", 0, SIGNAL(clicked()), pCurrentURL, SLOT(clearHistory()), true, i18n("Clear history"));
    pTLocation->insertButton("locationbar_erase", 1, SIGNAL(clicked()), pCurrentURL, SLOT(clearEdit()), true, i18n("Clear adress"));
    pTLocation->insertWidget(2, 10, new QLabel("URL:", pTLocation, "kde toolbar widget"));
    pTLocation->setItemAutoSized(2);
    pTLocation->insertWidget(3, 10, pCurrentURL);
    pTLocation->setItemAutoSized(3);
    pTLocation->insertButton("goto", 4, SIGNAL(clicked()), this, SLOT(slotGo()), true, i18n("Go!"));

    kconf->setGroup("History");

    // some additional setup
    pCurrentURL->setDuplicatesEnabled(false);
    pCurrentURL->setSizeLimit(20);
    pCurrentURL->setHistoryItems(kconf->readListEntry("items"), true);
}

// Create menu with filters
void KSquirrel::initFilterMenu()
{
    QString ext, tmp;
    unsigned int i = menuParam;

    QString last;

    kconf->setGroup("Filters");

    // create QStringLists with names and extensions
    // of custom filters
    if(!sqFiltersName)
    {
        sqFiltersName = new QStringList(kconf->readListEntry("items"));
        sqFiltersExt = new QStringList(kconf->readListEntry("extensions"));

        // last used filter
        last = kconf->readEntry("last", "*");

        // allow user to check/uncheck menuitems
        actionFilterMenu->setCheckable(true);
    }
    else // if QStringLists already exist
        last = pWidgetStack->nameFilter();

    actionFilterMenu->clear();
    int id, Id = old_id;
    bool both = kconf->readBoolEntry("menuitem both", true);
    QStringList quickInfo;

    // Get extensions and names of libraries' filters
    SQ_LibraryHandler::instance()->allFilters(libFilters, quickInfo);

    actionFilterMenu->insertTitle(i18n("Libraries' filters"));

    // insert libraries' filters to menu
    for(QValueList<QString>::iterator it = libFilters.begin(), it1 = quickInfo.begin();
                it != libFilters.end();++it, ++it1)
    {
        // show both name and extension ?
        if(both)
            id = actionFilterMenu->insertItem(*it1 + " (" + *it + ")");
        else
            id = actionFilterMenu->insertItem(*it1);

        actionFilterMenu->setItemParameter(id, i++);

        if(last == *it && !last.isEmpty())
            Id = id;
    }

    // all filters' extension in one string
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
//    actionFilterMenu->setItemParameter(id, i++);

    // add custom filters
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

    // finally, set current filter
    setFilter(last, Id);
}

// "Go" clicked in url box
void KSquirrel::slotGo()
{
    pWidgetStack->setURLForCurrent(pCurrentURL->currentText());
}

// Set filter for a filemanager and check appropriate
// menuitem (by 'id')
void KSquirrel::setFilter(const QString &f, const int id)
{
    if(pWidgetStack->nameFilter() != f)
        pWidgetStack->setNameFilter(f);

//    actionFilterMenu->setItemChecked(old_id, false);
    actionFilterMenu->setItemChecked(id, true);
    old_id = id;
}

// User seleced some filter from menu
void KSquirrel::slotSetFilter(int id)
{
    // uncheck old item being checked
    actionFilterMenu->setItemChecked(old_id, false);

    // new item's index
    int index = actionFilterMenu->itemParameter(id);

    QString filt;

    // Is it libraries' filter or custom ?
    if(index >= menuParam)
        filt = libFilters[index - menuParam];
    else
        filt = (*sqFiltersExt)[index];

    // If new filter differences from current -
    // let's apply it
    if(pWidgetStack->nameFilter() != filt)
        pWidgetStack->setNameFilter(filt);

    // finally, check new item
    actionFilterMenu->setItemChecked(id, true);
    old_id = id;
}

// Create all widgets (toolbar, menubar, image window ...)
void KSquirrel::createWidgets(int createFirst)
{
    kconf->setGroup("Interface");

    // check if location toolbar should be separated
    m_urlbox = kconf->readBoolEntry("has_url", false);
    m_sep = kconf->readBoolEntry("gl_separate", true);

    // main QVBox
    mainBox = new QVBox(this);
    mainBox->resize(size());

    // menubar
    menubar = new KMenuBar(mainBox);

    viewBrowser = new QWidgetStack(mainBox, QString::fromLatin1("SQ_BROWSER_WIDGET_STACK")); Q_ASSERT(viewBrowser);
    QVBox *b1 = new QVBox(viewBrowser);
    tools = new KToolBar(b1);

    // location toolbar
    pTLocation = new SQ_LocationToolbar(b1, QString::fromLatin1("Location toolbar"));
    pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    // main splitter
    QHBox *mainView = new QHBox(b1); Q_ASSERT(mainView);

    KStatusBar *s = new KStatusBar(b1);

    createLocationToolbar(pTLocation);
    createStatusBar(s);

    new SQ_NavigatorDropMenu(this);

    // sidebar with tree, actions...
    sideBar = new SQ_MultiBar(mainView);

    // create widgetstack containing views
    pWidgetStack = new SQ_WidgetStack(mainView, createFirst);

    mainView->setStretchFactor(pWidgetStack, 1);

    SQ_MountView *mv = new SQ_MountView;
    SQ_SideBarActions *sa = new SQ_SideBarActions(NULL, "kde toolbar widget");
    ptree = new SQ_TreeView;
    sideBar->addWidget(ptree, "File tree", "view_tree");
    sideBar->addWidget(mv, "Mount points", "blockdevice");
    sideBar->addWidget(sa, "Actions", "edit_picture");

    connect(mv, SIGNAL(path(const QString &)), pWidgetStack, SLOT(setURLForCurrent(const QString &)));

    pTLocation->setShown(m_urlbox);

    pAURL->setChecked(m_urlbox);
    pASeparateGL->setChecked(m_sep);

    b1->setStretchFactor(mainView, 1);

    // connect signals from location toolbar
    connect(pCurrentURL, SIGNAL(returnPressed(const QString&)), pWidgetStack, SLOT(setURLForCurrent(const QString&)));
    connect(pCurrentURL, SIGNAL(activated(const QString&)), pWidgetStack, SLOT(setURLForCurrent(const QString&)));

    b2 = new QVBox(viewBrowser);

    if(!m_sep)
        gl_view = new SQ_GLView(b2);
    else
        gl_view = new SQ_GLView;

    // insert actions in toolbar and menu
    createToolbar(tools);
    createMenu(menubar);

    switch(createFirst)
    {
        case 1: pARaiseIconView->setChecked(true); break;
        case 2: pARaiseDetailView->setChecked(true); break;
        case 3: pARaiseThumbView->setChecked(true); break;

        default:
            pARaiseListView->setChecked(true);
    }

    viewBrowser->addWidget(b1, 0);
    viewBrowser->addWidget(b2, 1);

    // connect signals from widgets
    connect(pAURL, SIGNAL(toggled(bool)), pTLocation, SLOT(setShown(bool)));
    connect(pASeparateGL, SIGNAL(toggled(bool)), this, SLOT(slotSeparateGL(bool)));
    connect(pASelectGroup, SIGNAL(activated()), pWidgetStack, SLOT(slotSelectGroup()));
    connect(pADeselectGroup, SIGNAL(activated()), pWidgetStack, SLOT(slotDeselectGroup()));
    connect(pASelectAll, SIGNAL(activated()), pWidgetStack, SLOT(slotSelectAll()));
    connect(pADeselectAll, SIGNAL(activated()), pWidgetStack, SLOT(slotDeselectAll()));

    kconf->setGroup("Interface");
}

// Create statusbar and all needed QLabels
void KSquirrel::createStatusBar(KStatusBar *bar)
{
    sbar = bar;
    sbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sbar->setSizeGripEnabled(true);
    sbar->show();

    // directory information (for example "3 dirs, 12 iles")
    dirInfo = new QLabel(QString::null, sbar, "dirInfo");
    QHBox *vb = new QHBox(sbar);

    // mime icon of current file
    fileIcon = new QLabel(QString::null, vb, "fileIcon");
    fileIcon->setScaledContents(false);

    // name of current file
    fileName = new QLabel(QString::fromLatin1("----"), vb, "fileName");
    QLabel *levak = new QLabel(sbar);

    // insert pointers to map, now it is possible
    // to call KSquirrel::sbarWidget(name)
    sbarwidgets["dirInfo"] = dirInfo;
    sbarwidgets["fileIcon"] = fileIcon;
    sbarwidgets["fileName"] = fileName;

    // finally, add QLabels to statusbar
    sbar->addWidget(dirInfo, 0, true);
    sbar->addWidget(vb, 0, true);
    sbar->addWidget(levak, 1, true);
}

// Create menu
void KSquirrel::createMenu(KMenuBar *menubar)
{
    pop_file = new KPopupMenu(menubar);
    pop_view = new KPopupMenu(menubar);
    pop_action = new KPopupMenu(menubar);
    pop_nav = new KPopupMenu(menubar);

    // create bookmarks and filters
    initBookmarks();
    initFilterMenu();

    // inset new submenus
    menubar->insertItem(i18n("&File"), pop_file);
    menubar->insertItem(i18n("&View"), pop_view);
    menubar->insertItem(i18n("&Navigator"), pop_nav);
    menubar->insertItem(i18n("&Action"), pop_action);
    bookmarks->plug(menubar);
    menubar->insertItem(i18n("&External Tools"), SQ_ExternalTool::instance()->constPopupMenu());
    menubar->insertItem(i18n("&Help"), helpMenu());

    // plug actions to menus
    pAOpen->plug(pop_file);
    pAOpenAndSet->plug(pop_file);
    pop_file->insertSeparator();
    pWidgetStack->action("properties")->plug(pop_file);
    pWidgetStack->action("delete")->plug(pop_file);
    pAEditMime->plug(pop_file);
    pop_file->insertSeparator();
    pAExit->plug(pop_file);

    pWidgetStack->action("home")->plug(pop_nav);
    pWidgetStack->action("up")->plug(pop_nav);
    pWidgetStack->action("back")->plug(pop_nav);
    pWidgetStack->action("forward")->plug(pop_nav);
    pWidgetStack->action("mkdir")->plug(pop_nav);
    pop_nav->insertSeparator();
    pWidgetStack->action("show hidden")->plug(pop_nav);
    pop_nav->insertSeparator();
    pASelectAll->plug(pop_nav);
    pADeselectAll->plug(pop_nav);
    pASelectGroup->plug(pop_nav);
    pADeselectGroup->plug(pop_nav);
    pop_nav->insertSeparator();
    pop_nav->insertItem(i18n("Fi&lter"), actionFilterMenu);

    pAGotoTray->plug(pop_view);
    pop_view->insertSeparator();
    pARaiseListView->plug(pop_view);
    pARaiseIconView->plug(pop_view);
    pARaiseDetailView->plug(pop_view);
    pARaiseThumbView->plug(pop_view);
    pop_view->insertSeparator();
    pAURL->plug(pop_view);
    pASeparateGL->plug(pop_view);

    pASlideShow->plug(pop_action);
    pASlideShowDialog->plug(pop_action);
    pop_action->insertSeparator();
    pATCMaster->plug(pop_action);
    pARescan->plug(pop_action);
    pAExtTools->plug(pop_action);
    pAFilters->plug(pop_action);
    pACheck->plug(pop_action);
    pAPluginsInfo->plug(pop_action);
    pAGLInfo->plug(pop_action);
    pop_action->insertSeparator();
    pAConfigure->plug(pop_action);
}

// Create toolbar
void KSquirrel::createToolbar(KToolBar *tools)
{
    // insert "back", "forward" and "up" actions
    // from SQ_WidgetStack
/*
    tools->insertWidget(0, 0, new QToolButton(SQ_IconLoader::instance()->loadIcon("back", KIcon::Desktop, 22), i18n("Back"), QString::null, pWidgetStack->action("back"), SLOT(activate()), tools));
    tools->insertWidget(0, 0, new QToolButton(SQ_IconLoader::instance()->loadIcon("forward", KIcon::Desktop, 22), i18n("Forward"), QString::null, pWidgetStack->action("forward"), SLOT(activate()), tools));
    tools->insertWidget(0, 0, new QToolButton(SQ_IconLoader::instance()->loadIcon("up", KIcon::Desktop, 22), i18n("Up"), QString::null, pWidgetStack->action("up"), SLOT(activate()), tools));
*/
    pWidgetStack->action("back")->plug(tools);
    pWidgetStack->action("forward")->plug(tools);
    pWidgetStack->action("up")->plug(tools);
    pWidgetStack->action("home")->plug(tools);

    tools->insertLineSeparator();
    pARaiseListView->plug(tools);
    pARaiseIconView->plug(tools);
    pARaiseDetailView->plug(tools);
    pARaiseThumbView->plug(tools);
    pAThumbs->plug(tools);

    tools->insertLineSeparator();
    pAGLView->plug(tools);
    pAExtTools->plug(tools);
    pAFilters->plug(tools);

    tools->insertLineSeparator();
    pAURL->plug(tools);
    pASeparateGL->plug(tools);

    tools->insertLineSeparator();
    pAConfigure->plug(tools);
    pAExit->plug(tools);

    // insert animated widget
    tools->insertAnimatedWidget(1000, this, SLOT(slotAnimatedClicked()), locate("appdata", "images/anime.png"));
    tools->alignItemRight(1000, true);
    tools->animatedWidget(1000)->start();
}

// Create all KActions
void KSquirrel::createActions()
{
    pAThumbs = new KActionMenu(i18n("Thumbnail size"), "thumbnail");
    pAExit = KStdAction::quit(this, SLOT(close()), actionCollection(), "SQ close");
    pAConfigure = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "SQ Configure");
    pAGLView = new KAction(i18n("Image window"), "desktop", CTRL+Key_I, this, SLOT(raiseGLWidget()), actionCollection(), "SQ gl view widget");
    pARescan = KStdAction::redisplay(this, SLOT(slotRescan()), actionCollection(), "SQ rescan libraries");
    pARescan->setText(i18n("Rescan libraries"));
    pAExtTools = new KAction(i18n("Configure external tools..."), "runprog", 0, this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");
    pAFilters = new KAction(i18n("Configure filters..."), "filefind", 0, this, SLOT(slotFilters()), actionCollection(), "SQ filters");
    pAGotoTray = new KAction(i18n("Goto tray"), "idea", 0, this, SLOT(slotGotoTray()), actionCollection(), "SQ goto tray");
    pAOpenAndSet = new KAction(i18n("Open file and change directory"), "fileopen", 0, this, SLOT(slotOpenFileAndSet()), actionCollection(), "SQ open and set");
    pAOpen = new KAction(i18n("Open file"), "fileopen", 0, this, SLOT(slotOpenFile()), actionCollection(), "SQ open file");
    pATCMaster = new KAction(i18n("Manipulate thumbnail cache"), "cache", 0, this, SLOT(slotTCMaster()), actionCollection(), "SQ TC Master");
    pASelectAll = KStdAction::selectAll(0, 0, actionCollection(), "SQ Select All");
    pADeselectAll = KStdAction::deselect(0, 0, actionCollection(), "SQ Deselect All");

    pARaiseListView = new KRadioAction(i18n("List"), "view_multicolumn", CTRL + Key_1, this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
    pARaiseIconView = new KRadioAction(i18n("Icons"), "view_icon", CTRL + Key_2, this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
    pARaiseDetailView = new KRadioAction(i18n("Details"), "view_detailed", CTRL + Key_3, this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
    pARaiseThumbView = new KRadioAction(i18n("Thumbnails"), "view_icon", CTRL + Key_4, this, SLOT(slotRaiseThumbView()), actionCollection(), "SQ raise thumbs view");

    pAURL = new KToggleAction(i18n("Show URL box"), "history", CTRL + Key_U, 0, 0, actionCollection(), "SQ toggle url box");
    pASeparateGL = new KToggleAction(i18n("Separate image window"), "window_new", 0, 0, 0, actionCollection(), "SQ toggle gl separate");

    new KAction(QString::null, 0, CTRL+Key_R, SQ_Resizer::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Resize");
    new KAction(QString::null, 0, CTRL+Key_K, SQ_Converter::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Convert");
    new KAction(QString::null, 0, CTRL + Key_G, SQ_Contraster::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image BCG");
    new KAction(QString::null, 0, CTRL+Key_J, SQ_Rotater::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Rotate");
    new KAction(QString::null, 0, CTRL+Key_F, SQ_Filter::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Filter");
    new KAction(QString::null, 0, CTRL+Key_P, SQ_Printer::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Print");

    pASlideShow = new KToggleAction(i18n("Slideshow"), 0, CTRL+Key_S, 0, 0, actionCollection(), "SQ Slideshow");
    pASlideShowDialog = new KAction(i18n("Slideshow through dialog"), 0, CTRL+ALT+Key_S, this, SLOT(slotSlideShowDialog()), actionCollection(), "SQ SlideShow Dialog");
    connect(pASlideShow, SIGNAL(toggled(bool)), this, SLOT(slotSlideShowToggle(bool)));

    pAPluginsInfo = new KAction(i18n("Plugins information"), "info", 0, this, SLOT(slotPluginsInfo()), actionCollection(), "SQ Plugins Info");
    pAGLInfo = new KAction(i18n("OpenGL information"), "info", 0, this, SLOT(slotGLInfo()), actionCollection(), "SQ OpenGL Info");
    pAEditMime = new KAction(i18n("Edit file type"), "image", 0, this, SLOT(slotEditMime()), actionCollection(), "SQ Edit Mime");

    pAThumb0 = new KRadioAction(i18n("Small thumbnails"), locate("appdata", "images/thumbs/thumbs_small.png"), 0, this, SLOT(slotThumbsSmall()), actionCollection(), "SQ thumbs0");
    pAThumb1 = new KRadioAction(i18n("Medium thumbnails"), locate("appdata", "images/thumbs/thumbs_medium.png"), 0, this, SLOT(slotThumbsMedium()), actionCollection(), "SQ thumbs1");
    pAThumb2 = new KRadioAction(i18n("Large thumbnails"), locate("appdata", "images/thumbs/thumbs_large.png"), 0, this, SLOT(slotThumbsLarge()), actionCollection(), "SQ thumbs2");
    pAThumb3 = new KRadioAction(i18n("Huge thumbnails"), locate("appdata", "images/thumbs/thumbs_huge.png"), 0, this, SLOT(slotThumbsHuge()), actionCollection(), "SQ thumbs3");

    pASelectGroup = new KAction(i18n("Select group"), "viewmag+", Key_Plus, 0, 0, actionCollection(), "SQ Select Group");
    pADeselectGroup = new KAction(i18n("Deselect group"), "viewmag-", Key_Minus, 0, 0, actionCollection(), "SQ Deselect Group");

    QString thumbs_size__ = QString::fromLatin1("thumbs_size__");
    pAThumb0->setExclusiveGroup(thumbs_size__);
    pAThumb1->setExclusiveGroup(thumbs_size__);
    pAThumb2->setExclusiveGroup(thumbs_size__);
    pAThumb3->setExclusiveGroup(thumbs_size__);

    pACheck = new KAction(i18n("Check for a newer version"), "network", CTRL+Key_Slash, this, SLOT(slotCheckVersion()), actionCollection(), "SQ Check Version");

    switch(thumbSize->value())
    {
        case SQ_ThumbnailSize::Small:     pAThumb0->setChecked(true);    break;
        case SQ_ThumbnailSize::Medium:    pAThumb1->setChecked(true);    break;
        case SQ_ThumbnailSize::Large:     pAThumb2->setChecked(true);    break;

        default:
            pAThumb3->setChecked(true);
    }

    pAThumbs->insert(pAThumb0);
    pAThumbs->insert(pAThumb1);
    pAThumbs->insert(pAThumb2);
    pAThumbs->insert(pAThumb3);
    pAThumbs->setDelayed(false);

    QString raise_some_widget_from_stack = QString::fromLatin1("raise_some_widget_from_stack");
    pARaiseListView->setExclusiveGroup(raise_some_widget_from_stack);
    pARaiseIconView->setExclusiveGroup(raise_some_widget_from_stack);
    pARaiseDetailView->setExclusiveGroup(raise_some_widget_from_stack);
    pARaiseThumbView->setExclusiveGroup(raise_some_widget_from_stack);
}

// Goto tray
void KSquirrel::slotGotoTray()
{
    // show tray icon
    tray->show();

    // hide image window if needed
    if(gl_view->isSeparate())
        gl_view->hide();

    // hide main window
    hide();
}

// create external tools
void KSquirrel::initExternalTools()
{
    // create SQ_ExternalTool and create new
    // popup menu with tools
    extool = new SQ_ExternalTool(this);
    extool->newPopupMenu();
}

// Edit custom filters
void KSquirrel::slotFilters()
{
    SQ_Filters f(this);

    if(f.start() != QDialog::Accepted) return;

    // Recreate menu with filters
    initFilterMenu();
}

// Edit external tools
void KSquirrel::slotExtTools()
{
    SQ_ExternalTools etd(this, "sq_exttools", true);

    // If user clicked "OK", recreate menu with
    // tools
    if(etd.start() == QDialog::Accepted)
        extool->newPopupMenu();
}

// Show image window
void KSquirrel::raiseGLWidget()
{
    // if image window is separate, just show and raise it
    if(gl_view->isSeparate())
    {
        gl_view->show();
        gl_view->raise();
        KWin::activateWindow(gl_view->winId());
    }
    else // raise a widget from widget stack otherwise
    {
        viewBrowser->raiseWidget(1);
    }
}

// Hide image window
void KSquirrel::slotCloseGLWidget()
{
    // stop slideshow, if running.
    // Thanks JaguarWan.
    stopSlideShow();

    if(gl_view->isSeparate())
        gl_view->hide();
    else
        viewBrowser->raiseWidget(0);
}

// create bookmarks
void KSquirrel::initBookmarks()
{
    // locate bookmarks file
    QString file = locate("data", "kfile/bookmarks.xml");

    if(file.isEmpty())
        file = locateLocal("data", "kfile/bookmarks.xml");

    KBookmarkManager *bmanager = KBookmarkManager::managerForFile(file, false);
    bmanager->setUpdate(true);
    bmanager->setShowNSBookmarks(false);

    SQ_BookmarkOwner *bookmarkOwner = new SQ_BookmarkOwner(this);

    // setup menu
    bookmarks = new KActionMenu(i18n("&Bookmarks"), "bookmark", actionCollection(), "bookmarks");
    bookmarkMenu = new KBookmarkMenu(bmanager, bookmarkOwner, bookmarks->popupMenu(), actionCollection(), true);

    connect(bookmarkOwner, SIGNAL(openURL(const KURL&)), pWidgetStack, SLOT(setURLForCurrent(const KURL&)));
}

// Create new config file in
// local directory (~/.kde/share/config)
void KSquirrel::writeDefaultEntries()
{
    SQ_Config conf("ksquirrelrc");

    conf.setGroup("Edit tools");
    conf.writeEntry("altlibrary", "Portable Network Graphics");

    conf.setGroup("External tools");
    conf.writeEntry("names", "Run within KDE,View in hexadecimal mode,Print file,Set as wallpaper on desktop,Set as tiled wallpaper on desktop,Open with GIMP,Open with GQview,Open with ShowImg,Open with KuickShow,Open with KView,Open with Opera,Open with Konqueror,Open with KWrite");
    conf.writeEntry("commands", "kfmclient exec %f,khexedit %f,kprinter %f,dcop kdesktop KBackgroundIface setWallpaper %f 6,dcop kdesktop KBackgroundIface setWallpaper %f 2,gimp %F,gqview %F,showimg %F,kuickshow %F,kview %F,opera %F,konqueror %F,kwrite %F");
    conf.writeEntry("icons", "kfm,khexedit,kdeprintfax,mac,mac,gimp,gqview,showimg,kuickshow,kview,opera,konqueror,kwrite");

    conf.setGroup("Filters");
    conf.writeEntry("extensions", "*,*.mpg *.mpeg *.avi *.asf *.divx,*.mp3 *.ogg *.wma *.wav *.it");
    conf.writeEntry("items", "All files,Video tapes,Audio files");
    conf.writeEntry("last", "*");
    conf.writeEntry("menuitem both", "false");

    conf.setGroup("Navigator Icons");
    conf.writeEntry("Preview Size", "60");
    conf.writeEntry("ShowPreviews", "false");
    conf.writeEntry("ViewMode", "LargeRows");

    conf.setGroup("Navigator List");
    conf.writeEntry("Preview Size", "60");
    conf.writeEntry("ShowPreviews", "false");
    conf.writeEntry("ViewMode", "SmallColumns");

    conf.setGroup("Main");
    conf.writeEntry("version", SQ_VERSION);

    conf.sync();
}

// Apply settings
void KSquirrel::applyDefaultSettings()
{
    bool updateDirs, updateThumbs;

    kconf->setGroup("GL view");

    // set background color, zoom & move factors
    SQ_GLWidget::window()->setClearColor();
    SQ_GLWidget::window()->setZoomFactor(kconf->readNumEntry("zoom", 25));
    SQ_GLWidget::window()->setMoveFactor(kconf->readNumEntry("move", 5));
    SQ_GLWidget::window()->setRotateFactor(kconf->readNumEntry("angle", 90));

    kconf->setGroup("Libraries");

    // Automatically update libraries ?
    libl->setAutoUpdate(kconf->readBoolEntry("monitor", true));

    kconf->setGroup("Thumbnails");
    thumbSize->setExtended(kconf->readBoolEntry("extended", false));

    kconf->setGroup("GL view");

    // Show/hide tickmarks if needed
    if(old_marks != kconf->readBoolEntry("marks", true))
        SQ_GLWidget::window()->updateGLA();

    kconf->setGroup("Fileview");

    updateDirs = (old_disable != kconf->readBoolEntry("disable_dirs", false));

    updateDirs &= (bool)pWidgetStack->diroperator()->numDirs();

    kconf->setGroup("Thumbnails");

    updateThumbs = (old_ext != kconf->readBoolEntry("extended", false));

    // Change clicking policy
    pWidgetStack->configureClickPolicy();
    pWidgetStack->updateGrid(!updateThumbs | updateDirs);

    // set cache limit for pixmap cache
    SQ_PixmapCache::instance()->setCacheLimit(kconf->readNumEntry("cache", 1024*10));

    kconf->setGroup("Main");

    if(kconf->readBoolEntry("sync", false))
        kconf->sync();

    // reload directory
    if(updateDirs && !updateThumbs)
    {
        pWidgetStack->updateView();
    }
    else if(updateThumbs) // just update icons...
    {
        pWidgetStack->diroperator()->smartUpdate();
        pWidgetStack->diroperator()->startOrNotThumbnailUpdate();
    }
}

// Toggle fullscreen state for image window
void KSquirrel::slotFullScreen(bool full)
{
    WId id = (gl_view->isSeparate()) ? gl_view->winId() : winId();

    kconf->setGroup("GL view");

    // hide statusbar in fullscreen mode ?
    if(kconf->readBoolEntry("hide_sbar", true))
    {
        gl_view->statusbar()->setShown(!full);
        dynamic_cast<KToggleAction *>(SQ_GLWidget::window()->actionCollection()->action("toggle status"))->setChecked(!full);
    }

    // hide toolbar in fullscreen mode ?
    if(kconf->readBoolEntry("hide_toolbar", true))
    {
        gl_view->toolbar()->setShown(!full);
        dynamic_cast<KToggleAction *>(SQ_GLWidget::window()->actionCollection()->action("toggle toolbar"))->setChecked(full);
    }

    // goto fullscreen
    if(full)
    {
        // if the image window is built-in, hide main menubar
        if(!gl_view->isSeparate())
            menubar->hide();

        KWin::setState(id, NET::FullScreen);
    }
    else // leave fullscreen
    {
        KWin::clearState(id, NET::FullScreen);

        // if the image window is built-in, show main menubar,
        // because it is hidden
        if(!gl_view->isSeparate())
            menubar->show();
    }
}

// Save parameters to config file
void KSquirrel::saveValues()
{
    extool->writeEntries();

    kconf->setGroup("Filters");
    kconf->writeEntry("items", *sqFiltersName);
    kconf->writeEntry("extensions", *sqFiltersExt);
    kconf->writeEntry("last", pWidgetStack->nameFilter());

    kconf->setGroup("Fileview");
    kconf->writeEntry("last visited", (pWidgetStack->url()).path());

    if(kconf->readBoolEntry("history", true) && pCurrentURL)
    {
        kconf->setGroup("History");
        kconf->writeEntry("items", pCurrentURL->historyItems());
    }

    kconf->setGroup("GL view");

    if(gl_view->isSeparate())
    {
        kconf->writeEntry("pos", gl_view->pos());
        kconf->writeEntry("size", gl_view->size());
    }

    kconf->writeEntry("statusbar", dynamic_cast<KToggleAction *>(SQ_GLWidget::window()->actionCollection()->action("toggle status"))->isChecked());
    kconf->writeEntry("quickGeometry", SQ_QuickBrowser::window()->geometry());
    kconf->writeEntry("ignore", dynamic_cast<KToggleAction *>(SQ_GLWidget::window()->actionCollection()->action("if less"))->isChecked());
    kconf->writeEntry("zoom type", SQ_GLWidget::window()->zoomType());
    kconf->writeEntry("toolbars_hidden", SQ_GLWidget::window()->actionsHidden());

    kconf->setGroup("Interface");
    kconf->writeEntry("last view", pWidgetStack->diroperator()->viewType());
    kconf->writeEntry("pos", pos());
    kconf->writeEntry("size", size());
    kconf->writeEntry("has_url", pTLocation->configVisible());
    kconf->writeEntry("gl_separate", gl_view->isSeparate());

    kconf->setGroup("Thumbnails");
    kconf->writeEntry("size", thumbSize->value());
}

// Reload libraries from disk
void KSquirrel::slotRescan()
{
    KURL url = "/usr/lib/ksquirrel-libs/";

    SQ_LibraryHandler::instance()->clear();

    // SQ_LibraryListener will find libraries
    // and SQ_LibraryHandler will load them
    libl->slotOpenURL(url, false, true);
}

// Create a final splash screen, if needed (if pixmap cache is not empty)
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
        todo = SQ_IconLoader::instance()->loadIcon("ksquirrel", KIcon::Desktop, 16);

    pp->setPixmap(todo);
    pp->setFixedWidth(18);

    QLabel *l = new QLabel(i18n(" writing settings and thumbnails...  "), hbox);
    l->setPalette(QPalette(cc, cc));
    l->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    hbox->setStretchFactor(pp, 0);
    hbox->setStretchFactor(l, 1);

    QRect rc = KGlobalSettings::splashScreenDesktopGeometry();

    int w = 200, h = 20;
    hbox->setGeometry(rc.center().x() - w/2, rc.center().y() - h/2, w, h);

    hbox->show();
}

// Change current thumbnail size
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

// Final actions before exiting
void KSquirrel::finalActions()
{
    // save parameters to config file
    saveValues();

    hide();

    kconf->setGroup("Thumbnails");

    bool create_splash = (!kconf->readBoolEntry("dont write", false)) & (!SQ_PixmapCache::instance()->empty());

    // create post splash
    if(create_splash)
    {
        createPostSplash();
        qApp->processEvents();
    }

    // write config file to disk (it is currently in memory cache)
    kconf->sync();

    // Save thumbnails from memory cache to disk
    if(create_splash)
        SQ_PixmapCache::instance()->sync();
}

/*
 *  Enable/disable popup menu with thumnail sizes. Called
 *  from SQ_WidgetStack, when user changed current view
 *  type (for example Thumbnail view -> Icon view).
 */
void KSquirrel::enableThumbsMenu(bool enable)
{
    pAThumbs->setEnabled(enable);
}

/*
 *  Create all needed objects
 *  (SQ_LibraryHandler, SQ_LibraryListener, edit tools, etc.)
 */
void KSquirrel::preCreate()
{
    libhandler = new SQ_LibraryHandler(this);
    libl = new SQ_LibraryListener(this);

    libl->setAutoErrorHandlingEnabled(false, NULL);

    initExternalTools();

    new SQ_ErrorString(this);
    new SQ_Converter(this);
    new SQ_Resizer(this);
    new SQ_Rotater(this);
    new SQ_Contraster(this);
    new SQ_Printer(this);
    new SQ_Filter(this);

    // timer for slideshow
    slideShowTimer = new QTimer(this);

    connect(slideShowTimer, SIGNAL(timeout()), this, SLOT(slotSlideShowNextImage()));

    // create main actions
    createActions();

    actionFilterMenu = new KPopupMenu;

    QFont f = actionFilterMenu->font();
    f.setPointSize(8);
    actionFilterMenu->setFont(f);

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

// Continue loading, because all found libraries now loaded.
// Find images if needed (if -t option specified).
void KSquirrel::slotContinueLoading()
{
    // if -l option specified, exit
    if(SQ_HLOptions::instance()->showLibsAndExit)
        exit(0);

    // disconnect signal "finishedInit".
    // now "finishedInit" won't invoke any slot.
    disconnect(libl, SIGNAL(finishedInit()), this, SLOT(slotContinueLoading()));

    // check if we need to find images and create thumbnails
    if(!SQ_HLOptions::instance()->thumbs)
        emit continueLoading(); // no!
    else // yes!
    {
        // create new KIO::Job, which will find files recursively or not.
        job = (SQ_HLOptions::instance()->recurs) ? KIO::listRecursive(KURL(SQ_HLOptions::instance()->thumbs_p), false, true) : KIO::listDir(KURL(SQ_HLOptions::instance()->thumbs_p), false, true);
        connect(job, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)), this, SLOT(slotUDSEntries(KIO::Job*, const KIO::UDSEntryList&)));
        connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(listResult(KIO::Job*)));
        dir = new SQ_Dir(SQ_Dir::Thumbnails);
    }
}

// Listing job found some new files
void KSquirrel::slotUDSEntries(KIO::Job *j, const KIO::UDSEntryList &list)
{
    if(j != job) return;

    QString m_file;
    KURL url = SQ_HLOptions::instance()->thumbs_p;
    SQ_Thumbnail t;
    bool b;
    QString other_thumb_path = QDir::homeDirPath() + "/.thumbnails";
    other_thumb_path = QDir::cleanDirPath(other_thumb_path);

    KIO::UDSEntryListConstIterator end = list.end();

    // go through list of KIO::UDSEntrys
    for(KIO::UDSEntryListConstIterator it = list.begin(); it != end; ++it)
    {
        KFileItem file(*it, url, true, true);
        m_file = file.url().path();

        QFileInfo ff(m_file);

        // Is it a file ? Is it readable ?
        if(!file.isDir() && ff.isReadable())
        // ignore some special paths - "~/.ksquirrel/thumbnails" and "~/.thumbnails"
        if(!m_file.startsWith(dir->root()) && !m_file.startsWith(other_thumb_path))
        {
            // Check if thumbnails should be updated
            if(dir->updateNeeded(m_file))
            {
                // Yes, create thumbnail
                b = SQ_ThumbnailLoadJob::loadThumbnail(m_file, t);

                // Save thumbnail to disk
                if(b)
                    dir->saveThumbnail(m_file, t);
            }
        }
    }
}

// Result of listing job
void KSquirrel::listResult(KIO::Job *j)
{
    if(j != job) return;

    // that's all. we loaded all libraries and
    // created thumbnails in specified directory. let's
    // now create widgets
    emit continueLoading();
}

void KSquirrel::slotContinueLoading2()
{
    kconf->setGroup("Interface");
    int createFirst = kconf->readNumEntry("last view", 0);

    // create toolbar, menubar, tree, filemanager ...
    createWidgets(createFirst);

    qApp->processEvents();

    // set position & size
    handlePositionSize();

    if(first_time)
    {
        applyDefaultSettings();
        first_time = false;
    }

    tray = new SQ_SystemTray(this);

    qApp->processEvents();

    if(gl_view->isSeparate())
        gl_view->show();

    show();

    if(gl_view->isSeparate())
        gl_view->hide();

    // Check if we need to load a file at startup
    // (if one was specified in command line)
    if(!SQ_HLOptions::instance()->path.isEmpty())
    {
        QFileInfo fm(SQ_HLOptions::instance()->path);

        if(!fm.isDir() && fm.isReadable())
        {
            KFileView *v = pWidgetStack->diroperator()->view();
            v->clearSelection();
            v->setCurrentItem(fm.fileName());
            v->setSelected(v->currentFileItem(), true);

            // if it is known image type - let's load it
            if(SQ_LibraryHandler::instance()->supports(SQ_HLOptions::instance()->path))
                SQ_GLWidget::window()->slotStartDecoding(SQ_HLOptions::instance()->path, true);
        }
    }
}

// Make image window separate ot built-in
void KSquirrel::slotSeparateGL(bool sep)
{
    kconf->setGroup("GL view");

    if(sep)
    {
        viewBrowser->raiseWidget(0);
        gl_view->reparent(0, QPoint(0,0), false);
        gl_view->restoreGeometry();
        raiseGLWidget();
    }
    else
    {
        gl_view->saveGeometry();
        gl_view->reparent(b2, QPoint(0,0), true);
    }
}

// Set caption to main window or to image window
void KSquirrel::setCaption(const QString &cap)
{
    if(gl_view->isSeparate())
        gl_view->setCaption(cap);
    else
        KMainWindow::setCaption(cap);
}

// User selected "Open file" or "Open file #2" from menu.
// Let's load file and set new URL for filemanager (if parseURL == true)
void KSquirrel::openFile(bool parseURL)
{
    QString name = KFileDialog::getOpenFileName();

    if(name.isEmpty())
        return;

    QFileInfo f(name);

    if(!f.isReadable())
        return;

    // if it is known file type
    if(SQ_LibraryHandler::instance()->supports(name))
    {
        if(parseURL)
            pWidgetStack->setURLForCurrent(f.dirPath(true));

        SQ_GLWidget::window()->slotStartDecoding(name, true);
    }
    else
    {
        kconf->setGroup("Fileview");

        if(kconf->readBoolEntry("run unknown", true))
            new KRun(name);
    }
}

// "Open file" in menu
void KSquirrel::slotOpenFile()
{
    openFile();
}

// "Open file #2" in menu
void KSquirrel::slotOpenFileAndSet()
{
    openFile(true);
}

// User clicked animated widget in toolbar,
// let's goto KSquirrel's homepage
void KSquirrel::slotAnimatedClicked()
{
    // invoke default browser
    kapp->invokeBrowser(QString::fromLatin1("http://ksquirrel.sourceforge.net/"));
}

// Create SQ_ThumbnailCacheMaster, which can do something with thumbnails
// and thumbnail cache
void KSquirrel::slotTCMaster()
{
    SQ_ThumbnailCacheMaster m(this);

    m.exec();
}

// Return a pointer to widget in statusbar by name
QLabel* KSquirrel::sbarWidget(const QString &name)
{
    return sbarwidgets[name];
}

// Convert QByteArray to QString
QString KSquirrel::getArg(const QByteArray &data)
{
    QDataStream args(data, IO_ReadOnly);

    QString arg;

    args >> arg;

    return arg;
}

// Process incoming DCOP messages
bool KSquirrel::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    if(fun == "control(QString)")
    {
        // Yes, user wants to call our "control" method
        control(getArg(data));

        replyType = "void";

        return true;
    }
    else if(fun == "activate()")
    {
        activate();

        replyType = "void";

        return true;
    }
    // load specified image
    else if(fun == "load_image(QString)")
    {
        QString arg = getArg(data);

        QFileInfo fm(arg);

        if(!fm.isDir() && fm.isReadable())
        {
            // select file in currently loaded directory
            // TODO: change url too ?
            KFileView *v = pWidgetStack->diroperator()->view();
            v->clearSelection();
            v->setCurrentItem(fm.fileName());
            v->setSelected(v->currentFileItem(), true);

            // load image
            if(SQ_LibraryHandler::instance()->supports(arg))
            {
                // stop slideshow if running
                stopSlideShow();

                // activate this window
                activate();

                // finally, load the image
                SQ_GLWidget::window()->slotStartDecoding(arg, true);
            }
        }

        replyType = "void";

        return true;
    }

    // we have to call it
    return DCOPObject::process(fun, data, replyType, replyData);
}

// Returnes all available DCOP methods.
// Used by DCOP infrastructure
QCStringList KSquirrel::functions()
{
    QCStringList result = DCOPObject::functions();

    result << "void control(QString)";
    result << "void load_image(QString)";
    result << "void activate()";

    return result;
}

// Activate this instance
void KSquirrel::activate()
{
    // just in case
    show();

    // force activation
    KWin::forceActiveWindow(winId());
}

// User sent us some message through DCOP, let's determine
// what he wants to do.
//
// The main idea is that SQ_GLWidget already has all needed actions,
// located in keyPressEvent() handler. Let's construct QKeyEvent and send
// it to SQ_GLWidget - SQ_GLWidget will catch this event and will do what
// user wants.
//
void KSquirrel::control(const QString &command)
{
    QMap<QString, int>::iterator it = messages.find(command);

    // known action
    if(it != messages.end())
    {
        int id = it.data();

        QKeyEvent *ev = new QKeyEvent(QEvent::KeyPress, id, id, Qt::NoButton);

        qApp->postEvent(SQ_GLWidget::window(), ev);
    }
    else // unknown parameter !
    {
        kdDebug() << "List of available DCOP parameters for control() method:" << endl;
        kdDebug() << "*****************************" << endl;

        for(QMap<QString, int>::iterator it = messages.begin();it != messages.end();++it)
        {
            kdDebug() << it.key() << endl;
        }

        kdDebug() << "******************************" << endl;
    }
}

void KSquirrel::fillMessages()
{
    messages.insert("image_next", Qt::Key_PageDown);
    messages.insert("image_previous", Qt::Key_PageUp);
    messages.insert("image_first", Qt::Key_Home);
    messages.insert("image_last", Qt::Key_End);
    messages.insert("image_reset", Qt::Key_R);
    messages.insert("image_information", Qt::Key_I);
    messages.insert("image_delete", Qt::Key_Delete);

    messages.insert("image_animation_toggle", Qt::Key_A);

    messages.insert("image_page_first", Qt::Key_F1);
    messages.insert("image_page_last", Qt::Key_F4);
    messages.insert("image_page_next", Qt::Key_F3);
    messages.insert("image_page_previous", Qt::Key_F2);

    messages.insert("image_window_fullscreen", Qt::Key_F);
    messages.insert("image_window_quickbrowser", Qt::Key_Q);
    messages.insert("image_window_close", Qt::Key_X);
    messages.insert("image_window_help", Qt::Key_Slash);

    messages.insert("zoom_plus", Qt::Key_Plus);
    messages.insert("zoom_minus", Qt::Key_Minus);
    messages.insert("zoom_1", Qt::Key_1);
    messages.insert("zoom_2", Qt::Key_2);
    messages.insert("zoom_3", Qt::Key_3);
    messages.insert("zoom_4", Qt::Key_4);
    messages.insert("zoom_5", Qt::Key_5);
    messages.insert("zoom_6", Qt::Key_6);
    messages.insert("zoom_7", Qt::Key_7);
    messages.insert("zoom_8", Qt::Key_8);
    messages.insert("zoom_9", Qt::Key_9);
    messages.insert("zoom_10", Qt::Key_0);
}

// Check for a newer version
void KSquirrel::slotCheckVersion()
{
    SQ_CheckVersion ch(this);

    ch.exec();
}

// Start/stop slideshow
void KSquirrel::slotSlideShowToggle(bool go)
{
    // check/uncheck slideshow button
    SQ_GLWidget::window()->updateSlideShowButton(go);

    // start slideshow
    if(go)
    {
        if(slideShowDir.isEmpty())
            slideShowDir = SQ_WidgetStack::instance()->url().path();

        slideShowPrivate();
    }
    else // stop slideshow
        stopSlideShow();
}

// Invoke dialog for advanced slideshow
void KSquirrel::slotSlideShowDialog()
{
    SQ_SlideShow s(this);

    if(s.exec(slideShowDir) == QDialog::Accepted)
        pASlideShow->activate();
}

// Start new slidehsow!
void KSquirrel::slideShowPrivate()
{
    // stop timer, if active
    slideShowTimer->stop();
    slideShowStop = false;

    kconf->setGroup("Slideshow");

    // determine delay
    slideShowDelay = kconf->readNumEntry("delay", 1000);
    slideShowRepeat = kconf->readNumEntry("repeat", 0);

    if(slideShowDelay < 0) slideShowDelay = 1000;
    if(slideShowRepeat < 0) slideShowRepeat = 0;
    if(slideShowRepeat > 1000) slideShowRepeat = 1000;

    slideShowItems.setSorting(QDir::Name);
    slideShowItems.setFilter(QDir::Files | QDir::Readable | QDir::Hidden);
    slideShowItems.setPath(slideShowDir);

    slideShowTotal = slideShowItems.count();

    // No files to show ?
    if(!slideShowTotal)
    {
        stopSlideShow();
        return;
    }

    // initial setup
    slideShowIndex = 0;
    slideShowName = slideShowDir + "/" + slideShowItems[slideShowIndex];

    // start!
    slideShowTimer->start(1, true);
}

// Load the next image
void KSquirrel::slotSlideShowNextImage()
{
    // stopped ?
    if(slideShowStop)
        return;

    // if it is known image type - let's load it
    if(SQ_LibraryHandler::instance()->supports(slideShowName))
        SQ_GLWidget::window()->slotStartDecoding(slideShowName, true);

    // goto next file
    slideShowIndex++;

    // all files loaded ?
    if(slideShowIndex == slideShowTotal)
    {
        // slideshow done
        if(!slideShowRepeat)
        {
            stopSlideShow();
            return;
        }
        // we should repeat slideshow
        else
        {
            slideShowRepeat--;
            slideShowIndex = 0;
        }
    }

    // construct new file name
    slideShowName = slideShowDir + "/" + slideShowItems[slideShowIndex];

    slideShowTimer->start(slideShowDelay, true);
}

// Stop slideshow, if running
void KSquirrel::stopSlideShow()
{
    if(!slideShowRunning())
        return;

    // reset values
    slideShowStop = true;
    slideShowTimer->stop();
    slideShowDir = QString::null;
    slideShowPaused = false;

    kconf->setGroup("Slideshow");

    // Toggle fullscreen, if needed
    if(SQ_GLWidget::window()->fullscreen() && kconf->readBoolEntry("fullscreen", true))
    {
        SQ_GLWidget::window()->toggleFullScreen();
        qApp->processEvents();
    }

    pASlideShow->setChecked(false);
    SQ_GLWidget::window()->updateSlideShowButton(false);
}

// Show information on libraries
void KSquirrel::slotPluginsInfo()
{
    SQ_PluginsInfo pi(this);

    pi.exec();
}

void KSquirrel::pauseSlideShow()
{
    if(!slideShowRunning())
        return;

    if(slideShowPaused)
    {
        slideShowTimer->start(slideShowDelay, true);
        slideShowPaused = false;
    }
    else
    {
        slideShowTimer->stop();
        slideShowPaused = true;
    }
}

// Create temporary config file object and read
// its version. Return true on version mismatch. It means,
// that KSquirrel should copy newer (installed) config file
// instead of old
bool KSquirrel::checkConfigFileVersion()
{
    SQ_Config tmpconf("ksquirrelrc");

    tmpconf.setGroup("Main");

    QString version = tmpconf.readEntry("version");

    return (version != SQ_VERSION);
}

// show imformation on OpenGL
void KSquirrel::slotGLInfo()
{
    SQ_GLInfo gi(this);

    gi.exec();
}

// edit mime type (Konqueror-related action)
void KSquirrel::slotEditMime()
{
    KFileItem *f = pWidgetStack->diroperator()->view()->currentFileItem();

    if(f)
        KonqOperations::editMimeType(f->mimetype());
}
