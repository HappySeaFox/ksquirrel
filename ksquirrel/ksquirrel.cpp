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
#include <kmenubar.h>
#include <ktoolbar.h>
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
#include <kmultitabbar.h>
#include <kurlcompletion.h>

#include "ksquirrel.h"
#include "sq_iconloader.h"
#include "sq_widgetstack.h"
#include "sq_tray.h"
#include "sq_treeview.h"
#include "sq_options.h"
#include "sq_about.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_libraryhandler.h"
#include "sq_externaltool.h"
#include "sq_externaltools.h"
#include "sq_config.h"
#include "sq_filters.h"
#include "sq_bookmarkowner.h"
#include "sq_hloptions.h"
#include "sq_pixmapcache.h"
#include "sq_thumbnailsize.h"
#include "sq_pixmapcache.h"
#include "sq_archivehandler.h"
#include "sq_dir.h"
#include "sq_thumbnailloadjob.h"
#include "sq_navigatordropmenu.h"
#include "sq_errorstring.h"
#include "sq_thumbnailcachemaster.h"
#include "sq_slideshow.h"
#include "sq_glwidget_helpers.h"
#include "sq_pluginsinfo.h"
#include "sq_imageloader.h"
#include "sq_multibar.h"
#include "sq_mountview.h"
#include "sq_glinfo.h"
#include "sq_categoriesview.h"
#include "sq_splashscreen.h"
#include "sq_imagebasket.h"
#include "sq_converter.h"
#include "sq_previewwidget.h"
#include "sq_slideshowwidget.h"

#ifdef SQ_HAVE_KIPI
#include "sq_kipimanager.h"
#endif

#include <cstdlib>

static const int menuParam = 100000;

KSquirrel * KSquirrel::m_instance = 0;

KSquirrel::KSquirrel(QWidget *parent, const char *name, SQ_SplashScreen **splash) 
    : KMainWindow (parent, name), DCOPObject(name), splash_to_delete(*splash)
{
    // singleton setup
    m_instance = this;

    kdDebug() << "+KSquirrel" << endl;

    slideShowStop = true;
    slideShowPaused = false;

    writeDefaultEntries();

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

    viewBrowser = 0;
    sqFiltersName = 0;
    pCurrentURL = 0;

    if(splash_to_delete) splash_to_delete->advance();

    fillMessages();

    preCreate();
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
    {
        applyDefaultSettings();
        SQ_MountView::instance()->setupColumns();
        SQ_TreeView::instance()->setupRecursion();
        SQ_PreviewWidget::instance()->rereadColor();
        SQ_PreviewWidget::instance()->update();
    }
}

/*
 *  Catches close events.
 */
void KSquirrel::closeEvent(QCloseEvent *ev)
{
    kconf->setGroup("Main");

    if(builtin && viewBrowser->id(viewBrowser->visibleWidget()))
        slotCloseGLWidget();
    // Minimize to tray ?
    else if(kconf->readBoolEntry("minimize to tray", false))
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
    pWidgetStack->raiseWidget(SQ_DirOperator::TypeList);
}

// Show icon view
void KSquirrel::slotRaiseIconView()
{
    pWidgetStack->raiseWidget(SQ_DirOperator::TypeIcons);
}

// Show Detailed view
void KSquirrel::slotRaiseDetailView()
{
    pWidgetStack->raiseWidget(SQ_DirOperator::TypeDetailed);
}

// Show Thumbnail view
void KSquirrel::slotRaiseThumbView()
{
    pWidgetStack->raiseWidget(SQ_DirOperator::TypeThumbs);
}

// Create location toolbar
void KSquirrel::createLocationToolbar(KToolBar *pTLocation)
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
    pCurrentURL->setCompletionObject(new KURLCompletion(KURLCompletion::DirCompletion));
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
    builtin = kconf->readBoolEntry("builtin", false);

    pAInterface->setChecked(builtin);

    // main QVBox
    viewBrowser = new QWidgetStack(this, QString::fromLatin1("SQ_BROWSER_WIDGET_STACK"));
    viewBrowser->resize(size());

    if(splash_to_delete) splash_to_delete->advance();

    QVBox *b1 = new QVBox(viewBrowser);

    // menubar & toolbar
    menubar = new KMenuBar(b1);
    tools = new KToolBar(b1);

    // location toolbar
    pTLocation = new KToolBar(b1, QString::fromLatin1("Location toolbar"));
    pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    // main splitter
    mainView = new QSplitter(QSplitter::Horizontal, b1); Q_ASSERT(mainView);

    KStatusBar *s = new KStatusBar(b1);

    createLocationToolbar(pTLocation);
    createStatusBar(s);

    new SQ_NavigatorDropMenu(this);

    sideBar = new SQ_MultiBar(mainView);

    SQ_CategoriesBox *cb = new SQ_CategoriesBox;

    if(splash_to_delete) splash_to_delete->advance();

    // create widgetstack containing views
    pWidgetStack = new SQ_WidgetStack(mainView, createFirst);

    SQ_Config::instance()->setGroup("Interface");
    QValueList<int> sz = SQ_Config::instance()->readIntListEntry("splitter");

    if(sz.count() != 2)
    {
        sz.clear();
        sz.append(1500);
        sz.append(3500);
    }

    mainView->setSizes(sz);

    if(splash_to_delete) splash_to_delete->advance();

    SQ_MountView *mv = new SQ_MountView;

    ptree = new SQ_TreeView;

    if(splash_to_delete) splash_to_delete->advance();

    QString prep = QString::fromLatin1("  ");

    sideBar->addWidget(ptree, prep + i18n("Folder tree"), "view_tree");
    sideBar->addWidget(mv, prep + i18n("Mount points"), "blockdevice");
    sideBar->addWidget(cb, prep + i18n("Categories"), "folder_favorite");
    sideBar->addWidget(new SQ_ImageBasket, prep + i18n("Image basket"), "folder_image");
    sideBar->updateLayout();

    connect(mv, SIGNAL(path(const QString &)), pWidgetStack, SLOT(setURLForCurrent(const QString &)));
    connect(ptree, SIGNAL(urlAdded(const KURL &)), pWidgetStack->diroperator(), SLOT(urlAdded(const KURL &)));
    connect(ptree, SIGNAL(urlRemoved(const KURL &)), pWidgetStack->diroperator(), SLOT(urlRemoved(const KURL &)));
    connect(pWidgetStack->diroperator(), SIGNAL(urlEntered(const KURL &)), ptree, SLOT(slotClearChecked()));
    connect(ptree, SIGNAL(stopUpdate()), pWidgetStack->diroperator(), SLOT(stopThumbnailUpdate()));
    connect(ptree, SIGNAL(startUpdate()), pWidgetStack->diroperator(), SLOT(startThumbnailUpdate()));

    pTLocation->setShown(m_urlbox);
    pAURL->setChecked(m_urlbox);
    b1->setStretchFactor(mainView, 1);

    // connect signals from location toolbar
    connect(pCurrentURL, SIGNAL(returnPressed(const QString&)), pWidgetStack, SLOT(setURLForCurrent(const QString&)));
    connect(pCurrentURL, SIGNAL(activated(const QString&)), pWidgetStack, SLOT(setURLForCurrent(const QString&)));

    b2 = new QVBox(viewBrowser);

    if(builtin)
        gl_view = new SQ_GLView(b2);
    else
        gl_view = new SQ_GLView;

    if(splash_to_delete) splash_to_delete->advance();

// KIPI support
#ifdef SQ_HAVE_KIPI
    kipiManager = new SQ_KIPIManager(this);
#endif

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
    connect(pASelectGroup, SIGNAL(activated()), pWidgetStack, SLOT(slotSelectGroup()));
    connect(pADeselectGroup, SIGNAL(activated()), pWidgetStack, SLOT(slotDeselectGroup()));
    connect(pASelectAll, SIGNAL(activated()), pWidgetStack, SLOT(slotSelectAll()));
    connect(pADeselectAll, SIGNAL(activated()), pWidgetStack, SLOT(slotDeselectAll()));

    if(splash_to_delete) splash_to_delete->advance();
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

#ifdef SQ_HAVE_KIPI
    menubar->insertItem(i18n("&KIPI Plugins"), kipiManager->popup());
#endif

    bookmarks->plug(menubar);
    menubar->insertItem(i18n("&Help"), helpMenu());

    // plug actions to menus
    pAOpen->plug(pop_file);
    pAOpenAndSet->plug(pop_file);
    pop_file->insertSeparator();
    pWidgetStack->action("properties")->plug(pop_file);
    pWidgetStack->action("delete")->plug(pop_file);
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
    pAInterface->plug(pop_view);

    pASlideShow->plug(pop_action);
    pASlideShowDialog->plug(pop_action);
    pop_action->insertSeparator();
    pATCMaster->plug(pop_action);
    pARescan->plug(pop_action);
    pAExtTools->plug(pop_action);
    pAFilters->plug(pop_action);
    pAPluginsInfo->plug(pop_action);
    pAGLInfo->plug(pop_action);
    pop_action->insertSeparator();
    pAConfigure->plug(pop_action);
}

// Create toolbar
void KSquirrel::createToolbar(KToolBar *tools)
{
    pWidgetStack->action("back")->plug(tools);
    pWidgetStack->action("forward")->plug(tools);
    pWidgetStack->action("up")->plug(tools);
    pWidgetStack->action("home")->plug(tools);
    pASlideShow->plug(tools);

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
    pAInterface->plug(tools);
    pAConfigure->plug(tools);
    pAExit->plug(tools);

    // insert animated widget
    tools->insertAnimatedWidget(1000, this, SLOT(slotAnimatedClicked()), locate("appdata", "images/anime.png"));
    tools->alignItemRight(1000);
    tools->animatedWidget(1000)->start();
}

// Create all KActions
void KSquirrel::createActions()
{
    pAThumbs = new KActionMenu(i18n("Thumbnail size"), "thumbnail");
    pAExit = KStdAction::quit(this, SLOT(close()), actionCollection(), "SQ close");
    pAConfigure = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "SQ Configure");
    pAGLView = new KAction(i18n("Image window"), "raise", CTRL+Key_I, this, SLOT(raiseGLWidget()), actionCollection(), "SQ gl view widget");
    pARescan = KStdAction::redisplay(this, SLOT(slotRescan()), actionCollection(), "SQ rescan libraries");
    pARescan->setText(i18n("Reload codecs from disk"));
    pAExtTools = new KAction(i18n("Configure external tools..."), "runprog", 0, this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");
    pAFilters = new KAction(i18n("Configure filters..."), "filefind", 0, this, SLOT(slotFilters()), actionCollection(), "SQ filters");
    pAGotoTray = new KAction(i18n("Goto tray"), 0, 0, this, SLOT(slotGotoTray()), actionCollection(), "SQ goto tray");
    pAOpenAndSet = new KAction(i18n("Open file and change directory"), "fileopen", CTRL+ALT+Key_O, this, SLOT(slotOpenFileAndSet()), actionCollection(), "SQ open and set");
    pAOpen = new KAction(i18n("Open file"), "fileopen", CTRL+Key_O, this, SLOT(slotOpenFile()), actionCollection(), "SQ open file");
    pATCMaster = new KAction(i18n("Thumbnail cache manager..."), "cache", 0, this, SLOT(slotTCMaster()), actionCollection(), "SQ TC Master");
    pASelectAll = KStdAction::selectAll(0, 0, actionCollection(), "SQ Select All");
    pADeselectAll = KStdAction::deselect(0, 0, actionCollection(), "SQ Deselect All");

    pARaiseListView = new KRadioAction(i18n("List"), "view_multicolumn", CTRL + Key_1, this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
    pARaiseIconView = new KRadioAction(i18n("Icons"), "view_icon", CTRL + Key_2, this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
    pARaiseDetailView = new KRadioAction(i18n("Details"), "view_detailed", CTRL + Key_3, this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
    pARaiseThumbView = new KRadioAction(i18n("Thumbnails"), "view_icon", CTRL + Key_4, this, SLOT(slotRaiseThumbView()), actionCollection(), "SQ raise thumbs view");

    pAURL = new KToggleAction(i18n("Show URL box"), "history", CTRL + Key_U, 0, 0, actionCollection(), "SQ toggle url box");

    pAInterface = new KToggleAction(i18n("Built-in image window"), "view_choose", 0, 0, 0, actionCollection(), "SQ Change Interface");
    connect(pAInterface, SIGNAL(toggled(bool)), this, SLOT(slotChangeInterface(bool)));

    pASlideShow = new KAction(i18n("Slideshow"), "folder_video", CTRL+Key_S, this, SLOT(slotSlideShowStart()), actionCollection(), "SQ Slideshow");
    pASlideShowDialog = new KAction(i18n("Slideshow advanced"), 0, CTRL+ALT+Key_S, this, SLOT(slotSlideShowDialog()), actionCollection(), "SQ SlideShow Dialog");

    pAPluginsInfo = new KAction(i18n("Plugins information..."), "info", 0, this, SLOT(slotPluginsInfo()), actionCollection(), "SQ Plugins Info");
    pAGLInfo = new KAction(i18n("OpenGL information..."), 0, 0, this, SLOT(slotGLInfo()), actionCollection(), "SQ OpenGL Info");

    pAThumb1 = new KRadioAction(i18n("Medium thumbnails"), locate("appdata", "images/thumbs/thumbs_medium.png"), 0, this, SLOT(slotThumbsMedium()), actionCollection(), "SQ thumbs1");
    pAThumb2 = new KRadioAction(i18n("Large thumbnails"), locate("appdata", "images/thumbs/thumbs_large.png"), 0, this, SLOT(slotThumbsLarge()), actionCollection(), "SQ thumbs2");
    pAThumb3 = new KRadioAction(i18n("Huge thumbnails"), locate("appdata", "images/thumbs/thumbs_huge.png"), 0, this, SLOT(slotThumbsHuge()), actionCollection(), "SQ thumbs3");

    pASelectGroup = new KAction(i18n("Select group"), "viewmag+", CTRL+Key_Plus, 0, 0, actionCollection(), "SQ Select Group");
    pADeselectGroup = new KAction(i18n("Deselect group"), "viewmag-", CTRL+Key_Minus, 0, 0, actionCollection(), "SQ Deselect Group");

    QString thumbs_size__ = QString::fromLatin1("thumbs_size__");
    pAThumb1->setExclusiveGroup(thumbs_size__);
    pAThumb2->setExclusiveGroup(thumbs_size__);
    pAThumb3->setExclusiveGroup(thumbs_size__);

    switch(thumbSize->value())
    {
        case SQ_ThumbnailSize::Medium:    pAThumb1->setChecked(true);    break;
        case SQ_ThumbnailSize::Large:     pAThumb2->setChecked(true);    break;

        default:
            pAThumb3->setChecked(true);
    }

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
    if(builtin)
        viewBrowser->raiseWidget(1);
    else
    {
        gl_view->show();
        gl_view->raise();
        KWin::activateWindow(gl_view->winId());
    }
}

// Hide image window
void KSquirrel::slotCloseGLWidget()
{
    if(builtin)
        viewBrowser->raiseWidget(0);
    else
        gl_view->hide();
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

    if(!conf.hasGroup("External tools"))
    {
        conf.setGroup("External tools");
        conf.writeEntry("names", "Run within KDE,View in hexadecimal mode,Print file,Set as wallpaper on desktop,Set as tiled wallpaper on desktop,Open with GIMP,Open with GQview,Open with ShowImg,Open with KuickShow,Open with KView,Open with Opera,Open with Konqueror,Open with KWrite");
        conf.writeEntry("commands", "kfmclient exec %f,khexedit %f,kprinter %f,dcop kdesktop KBackgroundIface setWallpaper %f 6,dcop kdesktop KBackgroundIface setWallpaper %f 2,gimp %F,gqview %F,showimg %F,kuickshow %F,kview %F,opera %F,konqueror %F,kwrite %F");
        conf.writeEntry("icons", "kfm,khexedit,kdeprintfax,mac,mac,gimp,gqview,showimg,kuickshow,kview,opera,konqueror,kwrite");
    }

    if(!conf.hasGroup("Filters"))
    {
        conf.setGroup("Filters");
        conf.writeEntry("extensions", "*,*.mpg *.mpeg *.avi *.asf *.divx,*.mp3 *.ogg *.wma *.wav *.it");
        conf.writeEntry("items", "All files,Video tapes,Audio files");
        conf.writeEntry("last", "*");
        conf.writeEntry("menuitem both", "false");
    }

    if(!conf.hasGroup("Navigator Icons"))
    {
        conf.setGroup("Navigator Icons");
        conf.writeEntry("ShowPreviews", "false");
        conf.writeEntry("ViewMode", "LargeRows");
    }

    if(!conf.hasGroup("Navigator List"))
    {
        conf.setGroup("Navigator List");
        conf.writeEntry("ShowPreviews", "false");
        conf.writeEntry("ViewMode", "SmallColumns");
    }

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
    SQ_GLWidget::window()->updateFactors();

    kconf->setGroup("GL view");

    // Show/hide tickmarks if needed
    if(old_marks != kconf->readBoolEntry("marks", true))
        SQ_GLWidget::window()->updateGLA();

    kconf->setGroup("Fileview");

    updateDirs = (old_disable != kconf->readBoolEntry("disable_dirs", false));

    updateDirs &= (bool)pWidgetStack->diroperator()->numDirs();

    kconf->setGroup("Thumbnails");
    thumbSize->setExtended(kconf->readBoolEntry("extended", false));

    updateThumbs = (old_ext != kconf->readBoolEntry("extended", false));

    // Change clicking policy
    pWidgetStack->updateGrid(!updateThumbs | updateDirs);

    // set cache limit for pixmap cache
    SQ_PixmapCache::instance()->setCacheLimit(kconf->readNumEntry("cache", 1024*10));

    kconf->setGroup("Main");

    if(kconf->readBoolEntry("sync", false))
        kconf->sync();

    // reload directory
    if(updateDirs && !updateThumbs)
        pWidgetStack->updateView();
    else if(updateThumbs) // just update icons...
        pWidgetStack->diroperator()->slotSetThumbSize(SQ_ThumbnailSize::instance()->pixelSizeString());
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
        KWin::setState(id, NET::FullScreen);
    else // leave fullscreen
        KWin::clearState(id, NET::FullScreen);
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

    if(!builtin)
        gl_view->saveGeometry();

    kconf->writeEntry("statusbar", dynamic_cast<KToggleAction *>(SQ_GLWidget::window()->actionCollection()->action("toggle status"))->isChecked());
    kconf->writeEntry("ignore", dynamic_cast<KToggleAction *>(SQ_GLWidget::window()->actionCollection()->action("if less"))->isChecked());
    kconf->writeEntry("zoom type", SQ_GLWidget::window()->zoomType());
    kconf->writeEntry("toolbars_hidden", SQ_GLWidget::window()->actionsHidden());

    kconf->setGroup("Interface");
    kconf->writeEntry("last view", pWidgetStack->diroperator()->viewType());
    kconf->writeEntry("pos", pos());
    kconf->writeEntry("size", size());
    kconf->writeEntry("has_url", pTLocation->isShown());
    kconf->writeEntry("builtin", builtin);

    SQ_PreviewWidget::instance()->saveValues();

    kconf->setGroup("Thumbnails");
    kconf->writeEntry("size", thumbSize->value());
}

// Reload libraries from disk
void KSquirrel::slotRescan()
{
    libhandler->reload();
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
 */
void KSquirrel::preCreate()
{
    libhandler = new SQ_LibraryHandler(this);

    initExternalTools();

    new SQ_ErrorString(this);
    new SQ_Converter(this);
    SQ_SlideShowWidget *sls = new SQ_SlideShowWidget(0, "ksquirrel-slideshow");

    connect(sls, SIGNAL(stopSlideShow()), this, SLOT(slotStopSlideShow()));
    connect(sls, SIGNAL(pause()), this, SLOT(slotPauseSlideShow()));
    connect(sls, SIGNAL(next()), this, SLOT(slotNextSlideShow()));
    connect(sls, SIGNAL(previous()), this, SLOT(slotPreviousSlideShow()));

    if(splash_to_delete) splash_to_delete->advance();

    // timer for slideshow
    slideShowTimer = new QTimer(this);

    connect(slideShowTimer, SIGNAL(timeout()), this, SLOT(slotSlideShowNextImage()));

    // create main actions
    createActions();

    if(splash_to_delete) splash_to_delete->advance();

    actionFilterMenu = new KPopupMenu;

    actionViews = new KPopupMenu;

    pARaiseListView->plug(actionViews);
    pARaiseIconView->plug(actionViews);
    pARaiseDetailView->plug(actionViews);
    pARaiseThumbView->plug(actionViews);

    if(splash_to_delete) splash_to_delete->advance();

    // if -l option specified, exit
    if(SQ_HLOptions::instance()->showLibsAndExit)
        exit(0);

    // check if we need to find images and create thumbnails
    if(!SQ_HLOptions::instance()->thumbs)
        continueLoading();
    else // yes!
    {
        // create new KIO::Job, which will find files recursively or not.
        job = (SQ_HLOptions::instance()->recurs) ? KIO::listRecursive(KURL(SQ_HLOptions::instance()->thumbs_p), false, true) : KIO::listDir(KURL(SQ_HLOptions::instance()->thumbs_p), false, true);
        connect(job, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)), this, SLOT(slotUDSEntries(KIO::Job*, const KIO::UDSEntryList&)));
        connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(listResult(KIO::Job*)));
        dir = new SQ_Dir(SQ_Dir::Thumbnails);
    }
}

void KSquirrel::resizeEvent(QResizeEvent *e)
{
    if(viewBrowser)
        viewBrowser->resize(e->size());
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

    continueLoading();
}

void KSquirrel::continueLoading()
{
    kconf->setGroup("Interface");
    int createFirst = kconf->readNumEntry("last view", 0);

    // create toolbar, menubar, tree, filemanager ...
    createWidgets(createFirst);

    // set position & size
    handlePositionSize();

    show();

#ifdef SQ_HAVE_KIPI
    kipiManager->loadPlugins();
#endif

    tray = new SQ_Tray(this);

    if(splash_to_delete) splash_to_delete->advance();


    splash_to_delete->finish();
    delete splash_to_delete;

    // Check if we need to load a file at startup
    // (if one was specified in command line)
    if(!SQ_HLOptions::instance()->path.isEmpty())
    {
        QFileInfo fm(SQ_HLOptions::instance()->path);

        if(fm.isFile() && fm.isReadable())
        {
            KFileView *v = pWidgetStack->diroperator()->view();
            v->clearSelection();
            v->setCurrentItem(fm.fileName());
            v->setSelected(v->currentFileItem(), true);

            // if it is known image type - let's load it
            SQ_GLWidget::window()->startDecoding(SQ_HLOptions::instance()->path);
        }
    }
    else if(gl_view->isSeparate())
    {
        gl_view->show();
        gl_view->hide();
    }
}

void KSquirrel::slotChangeInterface(bool bin)
{
    builtin = bin;

    if(builtin)
    {
        gl_view->saveGeometry();
        gl_view->reparent(b2);
    }
    else
    {
        gl_view->reparent(0, QPoint(0,0), false);
        gl_view->restoreGeometry();
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
    if(SQ_LibraryHandler::instance()->libraryForFile(name))
    {
        if(parseURL)
            pWidgetStack->setURLForCurrent(f.dirPath(true));

        SQ_GLWidget::window()->startDecoding(name);
    }
    else
    {
        kconf->setGroup("Fileview");

        if(kconf->readBoolEntry("run unknown", false))
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
    else if(fun == "navigator(QString)")
    {
        navigatorSend(getArg(data));

        replyType = "void";

        return true;
    }
    else if(fun == "activate()")
    {
        activate();

        replyType = "void";

        return true;
    }
    else if(fun == "activate_image_window()")
    {
        raiseGLWidget();

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
            if(SQ_LibraryHandler::instance()->libraryForFile(arg))
            {
                // stop slideshow if running
                slotStopSlideShow();

                // activate this window
                activate();

                // finally, load the image
                SQ_GLWidget::window()->startDecoding(arg);
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
    result << "void navigator(QString)";
    result << "void load_image(QString)";
    result << "void activate()";
    result << "void activate_image_window()";

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

void KSquirrel::navigatorSend(const QString &command)
{
    if(command == "next")
    {
        pWidgetStack->moveTo(SQ_WidgetStack::Next, 0, false);
    }
    else if(command == "previous")
    {
        pWidgetStack->moveTo(SQ_WidgetStack::Previous, 0, false);
    }
    else if(command == "execute")
    {
        KFileItem *fi = pWidgetStack->diroperator()->view()->currentFileItem();

        if(fi)
            pWidgetStack->diroperator()->execute(fi);
    }
    else // unknown parameter !
    {
        kdDebug() << "List of available DCOP parameters for navigator() method:" << endl;
        kdDebug() << "*****************************" << endl;
        kdDebug() << "next" << endl;
        kdDebug() << "previous" << endl;
        kdDebug() << "execute" << endl;
        kdDebug() << "******************************" << endl;
    }
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

        QApplication::postEvent(SQ_GLWidget::window(), ev);
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

// Start/stop slideshow
void KSquirrel::slotSlideShowStart()
{
    if(slideShowDir.isEmpty())
        slideShowDir = SQ_WidgetStack::instance()->url().path();

    slideShowPrivate();
}

// Invoke dialog for advanced slideshow
void KSquirrel::slotSlideShowDialog()
{
    SQ_SlideShow s(this);

    if(s.exec(slideShowDir) == QDialog::Accepted)
        slotSlideShowStart();
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
        slotStopSlideShow();
        return;
    }

    // initial setup
    slideShowIndex = 0;
    slideShowName = slideShowDir + QDir::separator() + slideShowItems[slideShowIndex];

    // start!
    SQ_SlideShowWidget::instance()->beginSlideShow(slideShowTotal);
    slideShowTimer->start(1, true);
}

// Load the next image
void KSquirrel::slotSlideShowNextImage()
{
    // stopped ?
    if(slideShowStop)
        return;

    bool brk = false;

    do
    {
        // if it is known image type - let's load it
        if(SQ_LibraryHandler::instance()->libraryForFile(slideShowName))
        {
//            printf("SUPPORTS %s\n", slideShowName.ascii());
            brk = true;
            SQ_SlideShowWidget::instance()->loadImage(slideShowName, slideShowIndex);
        }

        // goto next file
        slideShowIndex++;

        // all files loaded ?
        if(slideShowIndex == slideShowTotal)
        {
            // slideshow done
            if(!slideShowRepeat)
            {
                slotStopSlideShow();
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
        slideShowName = slideShowDir + QDir::separator() + slideShowItems[slideShowIndex];

        if(brk) break;

//        printf("CONTINUE -> %s\n", slideShowName.ascii());
    }
    while(!brk);

    slideShowTimer->start(slideShowDelay, true);
}

void KSquirrel::slotNextSlideShow()
{
    slideShowTimer->stop();
    slideShowTimer->start(0, true);
}

void KSquirrel::slotPreviousSlideShow()
{
    slideShowTimer->stop();
    int delay = slideShowDelay;

    if(slideShowIndex > 1)
    {
        slideShowIndex -= 2;
        slideShowName = slideShowDir + QDir::separator() + slideShowItems[slideShowIndex];
        delay = 0;
    }

    slideShowTimer->start(delay, true);
}

// Stop slideshow, if running
void KSquirrel::slotStopSlideShow()
{
    if(!slideShowRunning())
        return;

    // reset values
    slideShowStop = true;
    slideShowTimer->stop();
    slideShowDir = QString::null;
    slideShowPaused = false;

    kconf->setGroup("Slideshow");

    SQ_SlideShowWidget::instance()->endSlideShow();
}

// Show information on libraries
void KSquirrel::slotPluginsInfo()
{
    SQ_PluginsInfo pi(this);

    pi.exec();
}

void KSquirrel::slotPauseSlideShow()
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

// show imformation on OpenGL
void KSquirrel::slotGLInfo()
{
    SQ_GLInfo gi(this);

    gi.exec();
}

void KSquirrel::saveLayout()
{
    kconf->setGroup("Interface");
    kconf->writeEntry("splitter", mainView->sizes());
}

#include "ksquirrel.moc"
