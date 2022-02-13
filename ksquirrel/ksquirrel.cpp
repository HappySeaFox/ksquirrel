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
#include <qstylesheet.h>
#include <qwidgetstack.h>
#include <qtoolbutton.h>

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
#include <konq_pixmapprovider.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <kinputdialog.h>
#include <ksystemtray.h>
#include <ksqueezedtextlabel.h>
#include <ktabbar.h>

#include "ksquirrel.h"
#include "sq_iconloader.h"
#include "sq_widgetstack.h"
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
#include "sq_dirthumbs.h"
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
#include "sq_progress.h"
#include "sq_thumbnailsunused.h"
#include "sq_downloader.h"
#include "sq_slideshowlisting.h"
#include "sq_dragprovider.h"
#include "sq_directorybasket.h"

#ifdef SQ_HAVE_KIPI
#include "sq_kipimanager.h"
#endif

#include <cstdlib>

static const int menuParam = 100000;

KSquirrel * KSquirrel::m_instance = 0;

KSquirrel::KSquirrel(QWidget *parent, const char *name) 
    : KMainWindow (parent, name), DCOPObject(name)
{
    // singleton setup
    m_instance = this;

    kdDebug() << "+KSquirrel" << endl;

    slideShowStop = true;
    slideShowPaused = false;
    m_intray = false;
    waitForShow = true;
    m_demo = false;

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
    new SQ_DragProvider(this);

    sqFiltersName = 0;
    pCurrentURL = 0;
    mainPage = 0;

    SQ_SplashScreen::advance();

    fillMessages();

    preCreate();
}

KSquirrel::~KSquirrel()
{
    kdDebug() << "-KSquirrel" << endl;

    delete sqFiltersExt;
    delete sqFiltersName;
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
        emit resetToolTip();

        // create or delete animated logo
        configAnime(false);
    }
}

bool KSquirrel::eventFilter(QObject *o, QEvent *e)
{
    if(o == mainPage && e->type() == QEvent::Show)
    {
        if(waitForShow)
        {
            kconf->setGroup("Interface");

            // restore opened page in sidebar
            int pg = kconf->readNumEntry("last page", -1);

            QPushButton *b = 0;

            if(pg >= 0 && (b = sideBar->multiBar()->tab(pg)))
                b->animateClick();

            waitForShow = false;
        }
    }

    return false;
}

/*
 *  Catches close events.
 */
void KSquirrel::closeEvent(QCloseEvent *ev)
{
    kconf->setGroup("Main");

    if((m_demo && !m_intray) || kapp->sessionSaving())
    {
        // finalActions() called by slotSaveYourself()
        if(!kapp->sessionSaving())
            finalActions();

        ev->accept();
    }
    else
    {
        // Minimize to tray ?
        if(kconf->readBoolEntry("minimize to tray", false) || m_intray)
        {
            // Yes, let's hide to tray
            slotGotoTray();
            // ignore close event
            ev->ignore();
        }
        else // No, close app
        {
            // do final stuff
            finalActions();
            // accept close event - exit
            ev->accept();
        }
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

    pCurrentURL->setPixmapProvider(KonqPixmapProvider::self());
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
    libhandler->allFilters(libFilters, quickInfo);

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
    QString allF = libhandler->allFiltersString();

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
    // check if location toolbar should be separated
    m_urlbox = kconf->readBoolEntry("has_url", false);

    // main QVBox
    mainPage = new QVBox(this, QString::fromLatin1("SQ_BROWSER_VBOX"));
    mainPage->resize(size());

    SQ_SplashScreen::advance();

    mainPage->installEventFilter(this);

    // menubar & toolbar
    menubar = new KMenuBar(mainPage);
    tools = new KToolBar(mainPage);

    // location toolbar
    pTLocation = new KToolBar(mainPage, QString::fromLatin1("Location toolbar"));
    pTLocation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    // main splitter
    mainView = new QSplitter(QSplitter::Horizontal, mainPage); Q_ASSERT(mainView);

    KStatusBar *s = new KStatusBar(mainPage);

    createLocationToolbar(pTLocation);
    createStatusBar(s);

    new SQ_NavigatorDropMenu(this);

    sideBar = new SQ_MultiBar(mainView);

    SQ_CategoriesBox *cb = new SQ_CategoriesBox;

    SQ_SplashScreen::advance();

    // create widgetstack containing views
    pWidgetStack = new SQ_WidgetStack(mainView, createFirst);

    kconf->setGroup("Interface");
    QValueList<int> sz = SQ_Config::instance()->readIntListEntry("splitter");

    if(sz.count() != 2)
    {
        sz.clear();
        sz.append(1500);
        sz.append(3500);
    }

    mainView->setSizes(sz);

    SQ_SplashScreen::advance();

    SQ_MountView *mv = new SQ_MountView;

    ptree = new SQ_TreeView;

    SQ_SplashScreen::advance();

    sideBar->addWidget(ptree, i18n("Folder tree"), "view_tree");
    sideBar->addWidget(mv, i18n("Mount points"), "blockdevice");
    sideBar->addWidget(cb, i18n("Categories"), "bookmark");
    sideBar->addWidget(new SQ_ImageBasket, i18n("Image basket"), "folder_image");
    sideBar->addWidget(new SQ_DirectoryBasket, i18n("Folder basket"), "folder");
    sideBar->updateLayout();

    connect(mv, SIGNAL(path(const QString &)), pWidgetStack, SLOT(setURLForCurrent(const QString &)));
    connect(ptree, SIGNAL(urlAdded(const KURL &)), pWidgetStack->diroperator(), SLOT(urlAdded(const KURL &)));
    connect(ptree, SIGNAL(urlRemoved(const KURL &)), pWidgetStack->diroperator(), SLOT(urlRemoved(const KURL &)));
    connect(pWidgetStack->diroperator(), SIGNAL(urlEntered(const KURL &)), ptree, SLOT(slotClearChecked()));
    connect(pWidgetStack, SIGNAL(newLastURL(const QString &)), pWidgetStack->action("dirop_repeat"), SLOT(setText(const QString &)));
    connect(SQ_PreviewWidget::instance(), SIGNAL(next()), this, SLOT(slotPreviewWidgetNext()));
    connect(SQ_PreviewWidget::instance(), SIGNAL(previous()), this, SLOT(slotPreviewWidgetPrevious()));
    connect(SQ_PreviewWidget::instance(), SIGNAL(execute()), this, SLOT(slotPreviewWidgetExecute()));

    pWidgetStack->init();

    pTLocation->setShown(m_urlbox);
    pAURL->setChecked(m_urlbox);
    mainPage->setStretchFactor(mainView, 1);

    // connect signals from location toolbar
    connect(pCurrentURL, SIGNAL(returnPressed(const QString&)), pWidgetStack, SLOT(setURLForCurrent(const QString&)));
    connect(pCurrentURL, SIGNAL(activated(const QString&)), pWidgetStack, SLOT(setURLForCurrent(const QString&)));

    gl_view = new SQ_GLView;

    SQ_SplashScreen::advance();

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

    // connect signals from widgets
    connect(pAURL, SIGNAL(toggled(bool)), pTLocation, SLOT(setShown(bool)));
    connect(pASelectGroup, SIGNAL(activated()), pWidgetStack, SLOT(slotSelectGroup()));
    connect(pADeselectGroup, SIGNAL(activated()), pWidgetStack, SLOT(slotDeselectGroup()));
    connect(pASelectAll, SIGNAL(activated()), pWidgetStack, SLOT(slotSelectAll()));
    connect(pADeselectAll, SIGNAL(activated()), pWidgetStack, SLOT(slotDeselectAll()));

    SQ_SplashScreen::advance();
}

// Create statusbar and all needed QLabels
void KSquirrel::createStatusBar(KStatusBar *bar)
{
    sbar = bar;
    sbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sbar->setSizeGripEnabled(true);
    sbar->show();

    // directory information (for example "3 dirs, 12 files")
    dirInfo = new QLabel(QString::null, sbar, "dirInfo");

    QHBox *vb = new QHBox(sbar);

    // mime icon of current file
    fileIcon = new QLabel(QString::null, vb, "fileIcon");
    fileIcon->setScaledContents(false);

    // name of current file
    fileName = new KSqueezedTextLabel(QString::fromLatin1("----"), vb, "fileName");
    fileName->setTextFormat(Qt::RichText);
    fileName->setAlignment(Qt::AlignAuto | Qt::AlignVCenter | Qt::ExpandTabs);

    diskSpace = new QLabel(QString::null, sbar, "diskSpace");

    // insert pointers to map, now it is possible
    // to call KSquirrel::sbarWidget(name)
    sbarwidgets["dirInfo"] = dirInfo;
    sbarwidgets["fileIcon"] = fileIcon;
    sbarwidgets["fileName"] = fileName;
    sbarwidgets["diskSpace"] = diskSpace;

    // finally, add QLabels to statusbar
    sbar->addWidget(dirInfo, 0, true);
    sbar->addWidget(vb, 1, true);

    QToolButton *fireDisk = new QToolButton(sbar, "reload disk size");
    fireDisk->setIconSet(SQ_IconLoader::instance()->loadIcon("reload", KIcon::Desktop, KIcon::SizeSmall));
    fireDisk->setTextLabel(i18n("Reload"));
    fireDisk->setUsesTextLabel(false);
    connect(fireDisk, SIGNAL(clicked()), this, SLOT(slotFireDisk()));

    diskProg = new SQ_Progress(sbar, "SQ_Progress [disk space]");
    diskProg->setFixedWidth(150);

    sbar->addWidget(fireDisk, 0, true);
    sbar->addWidget(diskSpace, 0, true);
    sbar->addWidget(diskProg, 0, true);
}

void KSquirrel::slotFireDisk()
{
    pWidgetStack->diroperator()->fireDiskSize(pWidgetStack->diroperator()->url());
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
    pARename->plug(pop_file);
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
    pWidgetStack->action("dirop_repeat")->plug(tools);
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
    pAConfigure->plug(tools);
    pAExit->plug(tools);

    // insert animated widget
    configAnime();
}

// Create all KActions
void KSquirrel::createActions()
{
    pAThumbs = new KActionMenu(i18n("Thumbnail size"), "thumbnail");
    pAExit = KStdAction::quit(this, SLOT(slotClose()), actionCollection(), "SQ close");
    pAConfigure = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "SQ Configure");
    pAGLView = new KAction(i18n("Image window"), "raise", CTRL+Qt::Key_I, this, SLOT(raiseGLWidget()), actionCollection(), "SQ gl view widget");
    pARescan = KStdAction::redisplay(this, SLOT(slotRescan()), actionCollection(), "SQ rescan libraries");
    pARescan->setText(i18n("Reload codecs from disk"));
    pAExtTools = new KAction(i18n("Configure external tools..."), "launch", 0, this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");
    pAFilters = new KAction(i18n("Configure filters..."), "filefind", 0, this, SLOT(slotFilters()), actionCollection(), "SQ filters");
    pAGotoTray = new KAction(i18n("Goto tray"), "background", CTRL+Qt::Key_T, this, SLOT(slotGotoTray()), actionCollection(), "SQ goto tray");
    pAOpenAndSet = new KAction(i18n("Open file and change directory"), "fileopen", CTRL+ALT+Qt::Key_O, this, SLOT(slotOpenFileAndSet()), actionCollection(), "SQ open and set");
    pAOpen = new KAction(i18n("Open file"), "fileopen", CTRL+Qt::Key_O, this, SLOT(slotOpenFile()), actionCollection(), "SQ open file");
    pATCMaster = new KAction(i18n("Thumbnail cache manager..."), "cache", 0, this, SLOT(slotTCMaster()), actionCollection(), "SQ TC Master");
    pASelectAll = KStdAction::selectAll(0, 0, actionCollection(), "SQ Select All");
    pADeselectAll = KStdAction::deselect(0, 0, actionCollection(), "SQ Deselect All");

    pARaiseListView = new KRadioAction(i18n("List"), "view_multicolumn", CTRL+Qt::Key_1, this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
    pARaiseIconView = new KRadioAction(i18n("Icons"), "view_icon", CTRL+Qt::Key_2, this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
    pARaiseDetailView = new KRadioAction(i18n("Details"), "view_detailed", CTRL+Qt::Key_3, this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
    pARaiseThumbView = new KRadioAction(i18n("Thumbnails"), "view_icon", CTRL+Qt::Key_4, this, SLOT(slotRaiseThumbView()), actionCollection(), "SQ raise thumbs view");

    pAURL = new KToggleAction(i18n("Show URL box"), "history", CTRL+Qt::Key_U, 0, 0, actionCollection(), "SQ toggle url box");
    pASlideShow = new KAction(i18n("Slideshow"), "folder_video", CTRL+Qt::Key_S, this, SLOT(slotSlideShowStart()), actionCollection(), "SQ Slideshow");
    pASlideShowDialog = new KAction(i18n("Slideshow advanced"), 0, CTRL+ALT+Qt::Key_S, this, SLOT(slotSlideShowDialog()), actionCollection(), "SQ SlideShow Dialog");

    pARename = new KAction(i18n("Rename"), "file_move", Qt::Key_F2, this, SLOT(slotRename()), actionCollection(), "SQ Rename");

    pAPluginsInfo = new KAction(i18n("Codecs information..."), "info", 0, this, SLOT(slotPluginsInfo()), actionCollection(), "SQ Plugins Info");
    pAGLInfo = new KAction(i18n("OpenGL information..."), 0, 0, this, SLOT(slotGLInfo()), actionCollection(), "SQ OpenGL Info");

    pAThumb1 = new KRadioAction(i18n("Medium thumbnails"), locate("appdata", "images/thumbs/thumbs_medium.png"), 0, this, SLOT(slotThumbsMedium()), actionCollection(), "SQ thumbs1");
    pAThumb2 = new KRadioAction(i18n("Large thumbnails"), locate("appdata", "images/thumbs/thumbs_large.png"), 0, this, SLOT(slotThumbsLarge()), actionCollection(), "SQ thumbs2");
    pAThumb3 = new KRadioAction(i18n("Huge thumbnails"), locate("appdata", "images/thumbs/thumbs_huge.png"), 0, this, SLOT(slotThumbsHuge()), actionCollection(), "SQ thumbs3");

    pASelectGroup = new KAction(i18n("Select group"), "viewmag+", CTRL+Qt::Key_Plus, 0, 0, actionCollection(), "SQ Select Group");
    pADeselectGroup = new KAction(i18n("Deselect group"), "viewmag-", CTRL+Qt::Key_Minus, 0, 0, actionCollection(), "SQ Deselect Group");

    const QString thumbs_size__ = QString::fromLatin1("thumbs_size__");
    pAThumb1->setExclusiveGroup(thumbs_size__);
    pAThumb2->setExclusiveGroup(thumbs_size__);
    pAThumb3->setExclusiveGroup(thumbs_size__);

    switch(thumbSize->value())
    {
        case SQ_ThumbnailSize::Medium: pAThumb1->setChecked(true); break;
        case SQ_ThumbnailSize::Large:  pAThumb2->setChecked(true); break;

        default: pAThumb3->setChecked(true);
    }

    pAThumbs->insert(pAThumb1);
    pAThumbs->insert(pAThumb2);
    pAThumbs->insert(pAThumb3);
    pAThumbs->insert(new KActionSeparator(actionCollection()));

    pAThumbsE = new KToggleAction(i18n("Extended thumbnails"), "", CTRL+Qt::Key_E, 0, 0, actionCollection(), "SQ Extended thumbs");
    kconf->setGroup("Thumbnails");
    pAThumbsE->setChecked(kconf->readBoolEntry("extended", false));
    connect(pAThumbsE, SIGNAL(toggled(bool)), this, SLOT(slotExtendedToggled(bool)));
    pAThumbs->insert(pAThumbsE);

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

    // hide image window
    gl_view->hide();

    m_intray = true;

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
    gl_view->show();
    gl_view->raise();
    KWin::forceActiveWindow(gl_view->winId());

    if(SQ_GLWidget::window()->fullscreen())
    {
        gl_view->statusbar()->setShown(false);
        gl_view->boxBar()->setShown(false);
    }
}

// Hide image window
void KSquirrel::closeGLWidget()
{
    gl_view->hide();

    if(m_demo)
    {
        finalActions();
        qApp->quit();
    }
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

    bookmarkOwner->setURL(pWidgetStack->diroperator()->url());
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

    // Show/hide tickmarks if needed
    if(old_marks != kconf->readBoolEntry("marks", true))
        SQ_GLWidget::window()->updateGLA();

    gl_view->setupTabbar();

    kconf->setGroup("Fileview");

    if(kconf->readBoolEntry("calculate", false))
        pWidgetStack->diroperator()->calcTotalSize();

    updateDirs = (old_disable != kconf->readBoolEntry("disable_dirs", false));

    updateDirs &= (bool)pWidgetStack->diroperator()->numDirs();

    kconf->setGroup("Thumbnails");
    thumbSize->setExtended(kconf->readBoolEntry("extended", false));
    pAThumbsE->setChecked(thumbSize->extended());
    bool lazy = kconf->readBoolEntry("lazy", true);
    int lazyDelay = kconf->readNumEntry("lazy_delay", 500);
    int lazyRows = kconf->readNumEntry("rows", 2);

    updateThumbs = (old_ext != kconf->readBoolEntry("extended", false));

    pWidgetStack->updateGrid(!updateThumbs | updateDirs);

    // set cache limit for pixmap cache
    SQ_PixmapCache::instance()->setCacheLimit(kconf->readNumEntry("cache", 1024*10));

    kconf->setGroup("Main");

    if(kconf->readBoolEntry("sync", false))
        kconf->sync();

    pWidgetStack->diroperator()->setLazy(lazy, lazyDelay, lazyRows);

    // reload directory
    if(updateDirs && !updateThumbs)
        pWidgetStack->updateView();
    else if(updateThumbs) // just update icons...
        pWidgetStack->diroperator()->slotSetThumbSize(SQ_ThumbnailSize::instance()->pixelSizeString());
}

void KSquirrel::slotExtendedToggled(bool e)
{
    thumbSize->setExtended(e);
    pWidgetStack->diroperator()->slotSetThumbSize(SQ_ThumbnailSize::instance()->pixelSizeString());
}

// Toggle fullscreen state for image window
void KSquirrel::slotFullScreen(bool full)
{
    WId id = gl_view->winId();

    kconf->setGroup("GL view");

    // avoid automatic image resizing
    gl_view->statusbar()->setShown(!full);
    gl_view->boxBar()->setShown(!full);

    // goto fullscreen
    if(full)
    {
        gl_view->saveGeometry();
        KWin::setState(id, NET::FullScreen);
    }
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
    pWidgetStack->saveState();
    SQ_ImageBasket::instance()->saveConfig();

    if(kconf->readBoolEntry("history", true) && pCurrentURL)
    {
        kconf->setGroup("History");
        kconf->writeEntry("items", pCurrentURL->historyItems());
    }

    kconf->setGroup("GL view");

    if(!SQ_GLWidget::window()->fullscreen())
        gl_view->saveGeometry();

    kconf->writeEntry("ignore", dynamic_cast<KToggleAction *>(SQ_GLWidget::window()->actionCollection()->action("if less"))->isChecked());
    kconf->writeEntry("zoom type", SQ_GLWidget::window()->zoomType());

    kconf->setGroup("Interface");
    kconf->writeEntry("last view", pWidgetStack->diroperator()->viewType());
    if(!waitForShow) kconf->writeEntry("last page", sideBar->currentPage());
    kconf->writeEntry("pos", pos());
    kconf->writeEntry("size", size());
    kconf->writeEntry("has_url", pTLocation->isShown());

    // splitter sizes are saved automatically
    // only when all sidebar pages are closed
    if(sideBar->currentPage() != -1)
        saveLayout();

    kconf->setGroup("Thumbnails");
    kconf->writeEntry("size", thumbSize->value());
    kconf->writeEntry("extended", thumbSize->extended());
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
    gl_view->hide();

    // save parameters to config file
    saveValues();

    hide();

    diskProg->flush();

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
    pAThumbsE->setEnabled(enable); // extended thumbnails
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
    new SQ_ThumbnailsUnused(this);

    listingDialog = new SQ_SlideShowListing(this, "SlideShow Listing", true, Qt::WStyle_Customize | Qt::WStyle_NoBorder);

    connect(listingDialog, SIGNAL(kill()), this, SLOT(slotSlideShowListingKill()));

    slideShowItems.setAutoDelete(true);

    down = new SQ_Downloader(this);

    connect(down, SIGNAL(result(const KURL &)), this, SLOT(slotDownloaderResult(const KURL &)));

    SQ_SlideShowWidget *sls = new SQ_SlideShowWidget(0, "ksquirrel-slideshow");

    connect(sls, SIGNAL(stopSlideShow()), this, SLOT(slotStopSlideShow()));
    connect(sls, SIGNAL(pause()), this, SLOT(slotPauseSlideShow()));
    connect(sls, SIGNAL(next()), this, SLOT(slotNextSlideShow()));
    connect(sls, SIGNAL(previous()), this, SLOT(slotPreviousSlideShow()));

    SQ_SplashScreen::advance();

    // timer for slideshow
    slideShowTimer = new QTimer(this);
    timerShowListing = new QTimer(this);

    connect(timerShowListing, SIGNAL(timeout()), listingDialog, SLOT(show()));

    // create main actions
    createActions();

    SQ_SplashScreen::advance();

    actionFilterMenu = new KPopupMenu;

    actionViews = new KPopupMenu;

    pARaiseListView->plug(actionViews);
    pARaiseIconView->plug(actionViews);
    pARaiseDetailView->plug(actionViews);
    pARaiseThumbView->plug(actionViews);

    SQ_SplashScreen::advance();

    // if -l option specified, exit
    if(SQ_HLOptions::instance()->showLibsAndExit)
        exit(0);

    if(!SQ_HLOptions::instance()->param.isEmpty())
    {
        // at first try to stat as local url
        KURL url;

        if(QDir::isRelativePath(SQ_HLOptions::instance()->param))
        {
            url = KURL::fromPathOrURL(QDir::currentDirPath());
            url.addPath(SQ_HLOptions::instance()->param);
        }
        else
            url.setPath(SQ_HLOptions::instance()->param);

        statStage = false;
        KIO::StatJob *job = KIO::stat(url, false);
        job->setSide(true);
        job->setDetails(0);
        connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotStatResult(KIO::Job *)));
    }
    else
        continueLoading();
}

void KSquirrel::slotStatResult(KIO::Job *_job)
{
    if(_job)
    {
        KIO::StatJob *job = static_cast<KIO::StatJob *>(_job);

        if(job->error())
        {
            // both local and remote urls are failed
            if(statStage)
            {
                job->showErrorDialog(this);
                continueLoading();
                return;
            }
        }

        // found local/remote URL
        KIO::UDSEntry udsEntry = job->statResult();
        KIO::UDSEntry::ConstIterator itEnd = udsEntry.end();

        for(KIO::UDSEntry::ConstIterator it = udsEntry.begin(); it != itEnd;++it)
        {
            // we need just file type...
            if((*it).m_uds == KIO::UDS_FILE_TYPE)
            {
                if(S_ISDIR((mode_t)((*it).m_long)))
                {
                    SQ_HLOptions::instance()->dir = job->url();
                }
                else
                {
                    SQ_HLOptions::instance()->file = job->url();
                    SQ_HLOptions::instance()->dir = job->url();
                    SQ_HLOptions::instance()->dir.cd("..");
                }

                m_demo = !SQ_HLOptions::instance()->file.isEmpty();

                // Ok, it's existing URL
                continueLoading();
                return;
            }
        }

        // this should not occur, but for safety reasons...
        if(statStage)
        {
            continueLoading();
            return;
        }

        // local url is not found. construct remote url and stat() it again
        KURL url = KURL::fromPathOrURL(SQ_HLOptions::instance()->param);
        statStage = true;
        KIO::StatJob *job2 = KIO::stat(url, false);
        job2->setSide(true);
        job2->setDetails(0);
        connect(job2, SIGNAL(result(KIO::Job *)), this, SLOT(slotStatResult(KIO::Job *)));
    }
    else
        continueLoading();
}

void KSquirrel::resizeEvent(QResizeEvent *e)
{
    if(mainPage)
        mainPage->resize(e->size());
}

void KSquirrel::continueLoading()
{
    kconf->setGroup("Interface");
    int createFirst = kconf->readNumEntry("last view", 0);

    // create toolbar, menubar, tree, filemanager ...
    createWidgets(createFirst);

    // set position & size
    handlePositionSize();

    if(m_demo)
        raiseGLWidget();

    // don't show navigator when running with file argument
    if(!m_demo && !kapp->isRestored())
        show();

    // in demo mode may be invisible
    if(mainPage->isVisible())
    {
        QEvent e(QEvent::Show);
        eventFilter(mainPage, &e);
    }

#ifdef SQ_HAVE_KIPI
    kconf->setGroup("Main");

    // load KIPI plugins on demand ?
    if(!kconf->readBoolEntry("kipi_ondemand", true))
        kipiManager->loadPlugins();
#endif

    tray = new KSystemTray(this);
    tray->setPixmap(QPixmap(locate("appdata", "images/tray.png")));

    connect(tray, SIGNAL(quitSelected()), this, SLOT(slotTrayQuit()));

    SQ_SplashScreen::advance();
    SQ_SplashScreen::finish();

    // Check if we need to load a file at startup
    // (if one was specified in command line)
    if(!SQ_HLOptions::instance()->file.isEmpty())
    {
        raiseGLWidget();

        if(libhandler->maybeSupported(SQ_HLOptions::instance()->file) != SQ_LibraryHandler::No)
        {
            pWidgetStack->diroperator()->setPendingFile(SQ_HLOptions::instance()->file.fileName(false));
            KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, SQ_HLOptions::instance()->file);
            pWidgetStack->diroperator()->execute(&fi);
        }
    }

    pWidgetStack->diroperator()->setFocus();

    // show tray icon when restored
    if(kapp->isRestored())
        slotGotoTray();

    connect(kapp, SIGNAL(saveYourself()), this, SLOT(slotSaveYourself()));
}

// Set caption to main window or to image window
void KSquirrel::setCaption(const QString &cap)
{
    KMainWindow::setCaption(cap);

    gl_view->setCaption(cap.isEmpty() ? "ksquirrel" : cap);
}

// User selected "Open file" or "Open file #2" from menu.
// Let's load file and set new URL for filemanager (if parseURL == true)
void KSquirrel::openFile(bool parseURL)
{
    KURL url = KFileDialog::getOpenURL();

    if(url.isEmpty())
        return;

    // if it is known file type
    if(libhandler->maybeSupported(url) != SQ_LibraryHandler::No)
    {
        if(parseURL)
        {
            KURL d = url;
            d.cd("..");
            pWidgetStack->diroperator()->setPendingFile(url.fileName(false));
            pWidgetStack->setURLForCurrent(d);
        }

        KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, url);
        pWidgetStack->diroperator()->execute(&fi);
    }
    else
    {
        kconf->setGroup("Fileview");

        if(kconf->readBoolEntry("run unknown", false))
            new KRun(url);
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
    else if(fun == "load(QString)")
    {
        QString arg = getArg(data);

        KURL url = KURL::fromPathOrURL(arg);

        KIO::StatJob *job = KIO::stat(url, false);
        job->setSide(true);
        job->setDetails(0);
        connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(slotDCOPStatResult(KIO::Job *)));

        replyType = "void";

        return true;
    }

    // we have to call it
    return DCOPObject::process(fun, data, replyType, replyData);
}

void KSquirrel::slotDCOPStatResult(KIO::Job *_job)
{
    if(_job)
    {
        KIO::StatJob *job = static_cast<KIO::StatJob *>(_job);

        if(job->error())
        {
            job->showErrorDialog(this);
            return;
        }

        KURL url = job->url();
        bool isdir = false, setc = true;
        KURL dir = url;

        KIO::UDSEntry udsEntry = job->statResult();
        KIO::UDSEntry::ConstIterator itEnd = udsEntry.end();

        for(KIO::UDSEntry::ConstIterator it = udsEntry.begin(); it != itEnd;++it)
        {
            // we need just file type...
            if((*it).m_uds == KIO::UDS_FILE_TYPE)
            {
                isdir = S_ISDIR((mode_t)((*it).m_long));
                break;
            }
        }

        if(!isdir) dir.cd("..");

        if(!pWidgetStack->diroperator()->url().equals(dir, true))
        {
            // don't set current item
            setc = false;
            pWidgetStack->diroperator()->setPendingFile(url.fileName(false));
            pWidgetStack->diroperator()->setURL(dir, true);
        }

        // activate this window
        if(isdir)
            activate();
        else
        {
            raiseGLWidget();

            if(setc)
            {
                KFileView *v = pWidgetStack->diroperator()->view();

                if(v)
                {
                    v->clearSelection();
                    v->setCurrentItem(url.fileName(false));
                    pWidgetStack->diroperator()->setCurrentItem(v->currentFileItem());
                }
            }

            KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, url);

            if(libhandler->maybeSupported(fi.url()) != SQ_LibraryHandler::No)
            {
                // stop slideshow if running
                slotStopSlideShow();

                // load
                pWidgetStack->diroperator()->execute(&fi);
            }
        }
    }
}

// Returnes all available DCOP methods.
// Used by DCOP infrastructure
QCStringList KSquirrel::functions()
{
    QCStringList result = DCOPObject::functions();

    result << "void control(QString)";
    result << "void navigator(QString)";
    result << "void load(QString)";
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
        {
            if(fi->isFile())
                pWidgetStack->diroperator()->execute(fi);
            else
                pWidgetStack->diroperator()->setURL(fi->url(), true);
        }
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

        KAction *a = SQ_GLWidget::window()->actionCollection()->action(QString::fromLatin1("action_%1").arg(id));

        if(a) a->activate();
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
        slideShowDir = SQ_WidgetStack::instance()->url().prettyURL();

    kconf->setGroup("Slideshow");

    // recursion ?
    bool recurs = kconf->readBoolEntry("recurs", false);

    listing = recurs ? KIO::listRecursive(slideShowDir, false, false)
                     : KIO::listDir(slideShowDir, false, false);

    connect(listing, SIGNAL(entries(KIO::Job *, const KIO::UDSEntryList &)),
            this, SLOT(slotSlideShowEntries(KIO::Job *, const KIO::UDSEntryList &)));

    connect(listing, SIGNAL(result(KIO::Job *)), this, SLOT(slotSlideShowJobResult(KIO::Job *)));

    timerShowListing->start(1000, true);
}

void KSquirrel::slotSlideShowListingKill()
{
    if(listing) listing->kill();

    slideShowItems.clear();
    listingDialog->setFile(0, QString::null);
    listingDialog->hide();
}

void KSquirrel::slotSlideShowEntries(KIO::Job *, const KIO::UDSEntryList &list)
{
    KIO::UDSEntryListConstIterator itEnd = list.end();

    KURL url = KURL::fromPathOrURL(slideShowDir);
    QString sfile;

    // go through list of KIO::UDSEntrys
    for(KIO::UDSEntryListConstIterator it = list.begin(); it != itEnd; ++it)
    {
        KFileItem *file = new KFileItem(*it, url, true, true);

        if(file && file->isFile())
        {
            slideShowItems.append(file);

            if(sfile.isEmpty()) sfile = file->name();
        }
        else
            delete file;
    }

    if(!sfile.isEmpty())
        listingDialog->setFile(slideShowItems.count(), sfile);
}

void KSquirrel::slotSlideShowJobResult(KIO::Job *job)
{
    listingDialog->setFile(0, QString::null);
    listingDialog->hide();
    timerShowListing->stop();

    if(!job->error())
        slideShowPrivate();

    listing = 0;
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
    slideShowInit = true;
    slideShowDirection = KSquirrel::Default;

    disconnect(slideShowTimer, SIGNAL(timeout()), 0, 0);
    connect(slideShowTimer, SIGNAL(timeout()), this, SLOT(slotSlideShowNextImage()));

    kconf->setGroup("Slideshow");

    // determine delay
    slideShowDelay = kconf->readNumEntry("delay", 1000);
    slideShowRepeat = kconf->readNumEntry("repeat", 0);

    if(slideShowDelay < 0)     slideShowDelay = 1000;
    if(slideShowRepeat < 0)    slideShowRepeat = 0;
    if(slideShowRepeat > 1000) slideShowRepeat = 1000;

    slideShowTotal = slideShowItems.count();

    // No files to show ?
    if(!slideShowTotal)
    {
        slotStopSlideShow();
        return;
    }

    // initial setup
    slideShowIndex = -1;

    // start!
    SQ_SlideShowWidget::instance()->beginSlideShow(slideShowTotal);

    QTimer::singleShot(1, this, SLOT(slideShowDetermine()));
}

void KSquirrel::slotDownloaderResult(const KURL &u)
{
    if(u.isEmpty()) // error occured
        slideShowDetermine();
    else
    {
        KURL u2 = u;
        slideShowName = u2;
        int delay = slideShowInit ? 0 : slideShowDelay;

        if(libhandler->libraryForFile(slideShowName.path()))
        {
            slideShowInit = false;
            slideShowTimer->start(delay, true);
        }
        else
            slideShowDetermine();
    }
}

void KSquirrel::slideShowDetermine()
{
    do
    {
        // goto next file
        if(slideShowDirection == KSquirrel::Default)
        {
            slideShowIndex++;

            // all files loaded ?
            if(slideShowIndex == slideShowTotal)
            {
                // slideshow done
                if(!slideShowRepeat)
                {
                    // nice hack to make final delay
                    disconnect(slideShowTimer, SIGNAL(timeout()), 0, 0);
                    connect(slideShowTimer, SIGNAL(timeout()), this, SLOT(slotStopSlideShow()));
                    slideShowTimer->start(slideShowDelay, true);
                    return;
                }
                // we should repeat slideshow
                else
                {
                    slideShowRepeat--;
                    slideShowIndex = 0;
                }
            }
        }
        else // KSquirrel::MBack
        {
            slideShowIndex--;

            if(slideShowIndex < 0)
            {
                slideShowDirection = KSquirrel::Default;
                slideShowIndex = -1;
                continue;
            }
        }

        // construct new file name
        slideShowFile = slideShowItems.at(slideShowIndex);
        slideShowName = slideShowFile->url();
    }
    while(libhandler->maybeSupported(slideShowName) == SQ_LibraryHandler::No);

    slideShowDirection = KSquirrel::Default;

    if(slideShowName.isLocalFile())
        slotDownloaderResult(slideShowName);
    else
        down->start(slideShowFile);
}

// Load the next image
void KSquirrel::slotSlideShowNextImage()
{
    // stopped ?
    if(slideShowStop)
        return;

    SQ_SlideShowWidget::instance()->loadImage(slideShowName.path(), slideShowIndex);

    slideShowDetermine();
}

void KSquirrel::slotNextSlideShow()
{
    slideShowTimer->stop();
    slideShowTimer->start(0, true);
}

void KSquirrel::slotPreviousSlideShow()
{
    slideShowTimer->stop();
    slideShowDirection = KSquirrel::MBack;
    int ssi = slideShowIndex;
    KURL ssn;

    do
    {
        // goto next file
        ssi--;

        if(ssi < 0)
        {
            slideShowDirection = KSquirrel::Default;
            slideShowTimer->start(0, true);
            return;
        }

        // construct new file name
        ssn = slideShowItems.at(ssi)->url();
    }
    while(libhandler->maybeSupported(ssn) == SQ_LibraryHandler::No);

    slideShowIndex = ssi;
    slideShowName = ssn;

    slideShowInit = true;
    slideShowDetermine();
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
    slideShowItems.clear();
    slideShowDirection = KSquirrel::Default;

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
    if(!waitForShow)
    {
        kconf->setGroup("Interface");
        kconf->writeEntry("splitter", mainView->sizes());
    }
}

void KSquirrel::configAnime(bool init)
{
    KAnimWidget *anim = tools->animatedWidget(1000); // get animated widget, if exist

    kconf->setGroup("Main");

    // show animated logo in toolbar if needed
    if(!kconf->readBoolEntry("anime_dont", false) && (init || (!init && !anim)))
    {
        tools->insertAnimatedWidget(1000, this, SLOT(slotAnimatedClicked()), locate("appdata", "images/anime.png"));
        tools->alignItemRight(1000);
        tools->animatedWidget(1000)->start();
    }
    else if(!init && kconf->readBoolEntry("anime_dont", false) && anim)
        delete anim;
}

void KSquirrel::slotTrayQuit()
{
    m_demo = true;
    m_intray = false;
}

void KSquirrel::slotClose()
{
    m_demo = true;
    m_intray = false;

    close();
}

void KSquirrel::slotRename()
{
    bool ok;

    KFileItemList *items = const_cast<KFileItemList *>(pWidgetStack->diroperator()->view()->selectedItems());

    if(!items || !items->count())
        return;

    renameSrcURL = items->first()->url();

    QString filename = QStyleSheet::escape(renameSrcURL.filename());

    QString mNewFilename = KInputDialog::getText(i18n("Renaming File"),
            i18n("<p>Rename file <b>%1</b> to:</p>").arg(filename),
            renameSrcURL.filename(), &ok, this);

    if(!ok) return;

    renameDestURL = renameSrcURL;
    renameDestURL.setFileName(mNewFilename);

    KIO::Job *job = KIO::move(renameSrcURL, renameDestURL);
    connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotRenameResult(KIO::Job*)));
}

void KSquirrel::slotRenameResult(KIO::Job *job)
{
    if(job->error())
    {
        job->showErrorDialog(this);
        return;
    }

    SQ_PixmapCache::instance()->removeEntryFull(renameSrcURL);
}

void KSquirrel::slotRepeat()
{
    pWidgetStack->repeat();
}

void KSquirrel::slotSaveYourself()
{
    finalActions();
}

void KSquirrel::slotPreviewWidgetNext()
{
    pWidgetStack->moveTo(SQ_WidgetStack::Next);
}

void KSquirrel::slotPreviewWidgetPrevious()
{
    pWidgetStack->moveTo(SQ_WidgetStack::Previous);
}

void KSquirrel::slotPreviewWidgetExecute()
{
    KURL u = SQ_PreviewWidget::instance()->url();

    if(!u.isEmpty())
    {
        KFileItem fi(KFileItem::Unknown, KFileItem::Unknown, u);
        pWidgetStack->diroperator()->execute(&fi);
    }
}

#include "ksquirrel.moc"
