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
#include <qdesktopwidget.h>
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
#include <qtimer.h>

#include <kapp.h>
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
#include "sq_diroperator.h"
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

#include "sq_imageeditsplash.h"
#include "sq_converter.h"
#include "sq_resizer.h"
#include "sq_contraster.h"
#include "sq_rotater.h"
#include "sq_printer.h"
#include "sq_filter.h"

#include <cstdlib>

static const int menuParam = 100000;

KSquirrel * KSquirrel::sing = NULL;

KSquirrel::KSquirrel(QWidget *parent, const char *name) : KMainWindow (parent, name), DCOPObject(name)
{
    // singleton setup
    sing = this;
    first_time = false;
    slideShowStop = true;
    slideShowPaused = false;

    QString testConf = locateLocal("config", "ksquirrelrc");

    // check if user-depended config file exist. If exist, check
    // if it has newer version (did user forget to delet old config file ?)
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

    thumbSize = new SQ_ThumbnailSize((SQ_ThumbnailSize::Size)kconf->readNumEntry("size", SQ_ThumbnailSize::Huge));
    thumbSize->setExtended(kconf->readBoolEntry("extended", false));

    // Additional setup
    new SQ_PixmapCache;
    new SQ_ArchiveHandler;
    new SQ_IconLoader;
    new SQ_ImageLoader;

    mainBox = NULL;
    mainSplitter = NULL;
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
{}

/*
 *  Restore saved position & size
 */
void KSquirrel::handlePositionSize()
{
    QRect rect(0,0,800,600);

    kconf->setGroup("Interface");

    // read geometry from config file and resize main window
    setGeometry(kconf->readRectEntry("geometry", &rect));
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
    pWidgetStack->raiseWidget(0);
}

// Show icon view
void KSquirrel::slotRaiseIconView()
{
    pWidgetStack->raiseWidget(1);
}

// Show Detailed view
void KSquirrel::slotRaiseDetailView()
{
    pWidgetStack->raiseWidget(2);
}

// Show Thumbnail view
void KSquirrel::slotRaiseThumbView()
{
    pWidgetStack->raiseWidget(3);
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

    QValueList<QString>::iterator BEGIN = libFilters.begin();
    QValueList<QString>::iterator END = libFilters.end();
    QValueList<QString>::iterator BEGIN1 = quickInfo.begin();

    actionFilterMenu->insertTitle(i18n("Libraries' filters"));

    // insert libraries' filters to menu
    for(QValueList<QString>::iterator it = BEGIN, it1 = BEGIN1;it != END;++it, ++it1)
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

    // check if  tree and location toolbar should be separated
    hastree = kconf->readBoolEntry("has_tree", true);
    m_urlbox = kconf->readBoolEntry("has_url", true);
    m_sep = kconf->readBoolEntry("gl_separate", false);

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
    mainSplitter = new QSplitter(Qt::Horizontal, b1); Q_ASSERT(mainSplitter);
    KStatusBar *s = new KStatusBar(b1);

    createLocationToolbar(pTLocation);
    createStatusBar(s);

    new SQ_NavigatorDropMenu;

    QVBox *v = new QVBox(mainSplitter);

    // create widgetstack containing views
    pWidgetStack = new SQ_WidgetStack(v);

    pWidgetStack->raiseFirst(createFirst);

    // tree
    ptree = new SQ_TreeView(mainSplitter);

    mainSplitter->moveToFirst(ptree);

    ptree->setShown(hastree);
    pTLocation->setShown(m_urlbox);

    pATree->setChecked(hastree);
    pAURL->setChecked(m_urlbox);
    pASeparateGL->setChecked(m_sep);

    b1->setStretchFactor(mainSplitter, 1);

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
        case 0: pARaiseListView->setChecked(true); break;
        case 1: pARaiseIconView->setChecked(true); break;
        case 2: pARaiseDetailView->setChecked(true); break;
        case 3: pARaiseThumbView->setChecked(true); break;

        default:
            pARaiseListView->setChecked(true);
    }

    viewBrowser->addWidget(b1, 0);
    viewBrowser->addWidget(b2, 1);

    // connect signals from widgets
    connect(pATree, SIGNAL(toggled(bool)), this, SLOT(slotSetTreeShown(bool)));
    connect(pAURL, SIGNAL(toggled(bool)), pTLocation, SLOT(setShown(bool)));
    connect(pASeparateGL, SIGNAL(toggled(bool)), this, SLOT(slotSeparateGL(bool)));
    connect(pASelectGroup, SIGNAL(activated()), pWidgetStack, SLOT(slotSelectGroup()));
    connect(pADeselectGroup, SIGNAL(activated()), pWidgetStack, SLOT(slotDeselectGroup()));
    connect(pASelectAll, SIGNAL(activated()), pWidgetStack, SLOT(slotSelectAll()));
    connect(pADeselectAll, SIGNAL(activated()), pWidgetStack, SLOT(slotDeselectAll()));

    kconf->setGroup("Interface");

    // finally, resize main splitter
    mainSizes = kconf->readIntListEntry("splitter1");
    mainSplitter->setSizes(mainSizes);
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
    pop_edit = new KPopupMenu(menubar);
    pop_view = new KPopupMenu(menubar);
    pop_action = new KPopupMenu(menubar);

    // create bookmarks and filters
    initBookmarks();
    initFilterMenu();

    // inset new submenus
    menubar->insertItem(i18n("&File"), pop_file);
    menubar->insertItem(i18n("&Edit"), pop_edit);
    menubar->insertItem(i18n("&Action"), pop_action);
    bookmarks->plug(menubar);
    menubar->insertItem(i18n("External Tools"), SQ_ExternalTool::instance()->constPopupMenu());
    menubar->insertItem(i18n("&Help"), helpMenu());

    // plug actions to menus
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
    pASelectAll->plug(pop_file);
    pADeselectAll->plug(pop_file);
    pASelectGroup->plug(pop_file);
    pADeselectGroup->plug(pop_file);
    pop_file->insertSeparator();
    pop_file->insertItem(i18n("Filter"), actionFilterMenu);
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
    pAURL->plug(pop_view);
    pASeparateGL->plug(pop_view);

    pAImageToolbar->plug(pop_edit);
    pop_edit->insertSeparator();
    pAImageConvert->plug(pop_edit);
    pAImageResize->plug(pop_edit);
    pAImageRotate->plug(pop_edit);
    pAImageBCG->plug(pop_edit);
    pAImageFilter->plug(pop_edit);
    pAPrintImages->plug(pop_edit);
    pop_edit->insertSeparator();
    pAConfigure->plug(pop_edit);

    pASlideShow->plug(pop_action);
    pASlideShowDialog->plug(pop_action);
    pop_action->insertSeparator();
    pATCMaster->plug(pop_action);
    pARescan->plug(pop_action);
    pAExtTools->plug(pop_action);
    pAFilters->plug(pop_action);
    pACheck->plug(pop_action);
    pAPluginsInfo->plug(pop_action);
}

// Create toolbar
void KSquirrel::createToolbar(KToolBar *tools)
{
    // insert "back", "forward" and "up" actions
    // from SQ_WidgetStack
    tools->insertWidget(0, 0, new QToolButton(SQ_IconLoader::instance()->loadIcon("back", KIcon::Desktop, 22), i18n("Back"), QString::null, pWidgetStack->pABack, SLOT(activate()), tools));
    tools->insertWidget(0, 0, new QToolButton(SQ_IconLoader::instance()->loadIcon("forward", KIcon::Desktop, 22), i18n("Forward"), QString::null, pWidgetStack->pAForw, SLOT(activate()), tools));
    tools->insertWidget(0, 0, new QToolButton(SQ_IconLoader::instance()->loadIcon("up", KIcon::Desktop, 22), i18n("Up"), QString::null, pWidgetStack->pAUp, SLOT(activate()), tools));

    pWidgetStack->pAHome->plug(tools);

    tools->insertLineSeparator();
    pARaiseListView->plug(tools);
    pARaiseIconView->plug(tools);
    pARaiseDetailView->plug(tools);
    pARaiseThumbView->plug(tools);
    pAThumbs->plug(tools);
    pASlideShow->plug(tools);
/*
    tools->insertLineSeparator();
    pAImageToolbar->plug(tools);
    pAImageConvert->plug(tools);
    pAImageResize->plug(tools);
    pAImageRotate->plug(tools);
    pAImageBCG->plug(tools);
*/
//    tools->insertLineSeparator();
//    pAGotoTray->plug(tools);

    tools->insertLineSeparator();
//    pAImageToolbar->plug(tools);
    pAGLView->plug(tools);
    pAExtTools->plug(tools);
    pAFilters->plug(tools);

    tools->insertLineSeparator();
    pATree->plug(tools);
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
//    KAction *wwqwqw = new KAction(i18n("Eat memory"), QString::null, CTRL+Key_Y, this, SLOT(slotEat()), actionCollection(), "SQ Eatt");

    pAThumbs = new KActionMenu(i18n("Thumbnail size"), "thumbnail");
    pAExit = KStdAction::quit(this, SLOT(close()), actionCollection(), "SQ close");
    pAConfigure = KStdAction::preferences(this, SLOT(slotOptions()), actionCollection(), "SQ edit options");
    pAGLView = new KAction(i18n("Image window"), "desktop", CTRL+Key_I, this, SLOT(raiseGLWidget()), actionCollection(), "SQ gl view widget");
    pARescan = KStdAction::redisplay(this, SLOT(slotRescan()), actionCollection(), "SQ rescan libraries");
    pARescan->setText(i18n("Rescan libraries"));
    pAExtTools = new KAction(i18n("Configure external tools..."), "runprog", 0, this, SLOT(slotExtTools()), actionCollection(), "SQ external tools");
    pAFilters = new KAction(i18n("Configure filters..."), "filefind", 0, this, SLOT(slotFilters()), actionCollection(), "SQ filters");
    pAGotoTray = new KAction(i18n("Goto tray"), "idea", 0, this, SLOT(slotGotoTray()), actionCollection(), "SQ goto tray");
    pAOpenAndSet = new KAction(i18n("Open file") + QString::fromLatin1(" #2"), "fileopen", 0, this, SLOT(slotOpenFileAndSet()), actionCollection(), "SQ open and set");
    pAOpen = new KAction(i18n("Open file"), "fileopen", 0, this, SLOT(slotOpenFile()), actionCollection(), "SQ open file");
    pATCMaster = new KAction(i18n("Manipulate thumbnail cache"), "cache", 0, this, SLOT(slotTCMaster()), actionCollection(), "SQ TC Master");
    pASelectAll = KStdAction::selectAll(0, 0, actionCollection(), "SQ Select All");
    pADeselectAll = KStdAction::deselect(0, 0, actionCollection(), "SQ Deselect All");

    pARaiseListView = new KRadioAction(i18n("List"), "view_multicolumn", CTRL + Key_1, this, SLOT(slotRaiseListView()), actionCollection(), "SQ raise list view");
    pARaiseIconView = new KRadioAction(i18n("Icons"), "view_icon", CTRL + Key_2, this, SLOT(slotRaiseIconView()), actionCollection(), "SQ raise icon view");
    pARaiseDetailView = new KRadioAction(i18n("Details"), "view_detailed", CTRL + Key_3, this, SLOT(slotRaiseDetailView()), actionCollection(), "SQ raise detailed view");
    pARaiseThumbView = new KRadioAction(i18n("Thumbnails"), "view_icon", CTRL + Key_4, this, SLOT(slotRaiseThumbView()), actionCollection(), "SQ raise thumbs view");

    pATree = new KToggleAction(i18n("Show tree"), "view_tree", CTRL + Key_T, 0, 0, actionCollection(), "SQ toggle tree");
    pAURL = new KToggleAction(i18n("Show URL box"), "history", CTRL + Key_U, 0, 0, actionCollection(), "SQ toggle url box");
    pASeparateGL = new KToggleAction(i18n("Separate image window"), "window_new", 0, 0, 0, actionCollection(), "SQ toggle gl separate");
    pAImageToolbar = new KAction(i18n("Edit selected images"), "gimp", CTRL + Key_E, this, SLOT(slotShowImageEditActions()), actionCollection(), "SQ toggle imagepanel");

    pAImageResize = new KAction(i18n("Resize"), locate("appdata", "images/imageedit/resize.png"), CTRL+Key_R, SQ_Resizer::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Resize");
    pAImageConvert = new KAction(i18n("Convert"), locate("appdata", "images/imageedit/convert.png"), CTRL+Key_K, SQ_Converter::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Convert");
    pAImageBCG = new KAction(i18n("Colorize"), locate("appdata", "images/imageedit/bcg.png"), CTRL + Key_G, SQ_Contraster::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image BCG");
    pAImageRotate = new KAction(i18n("Rotate"), locate("appdata", "images/imageedit/rotate.png"), CTRL+Key_J, SQ_Rotater::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Rotate");
    pAImageFilter = new KAction(i18n("Filter"), locate("appdata", "images/imageedit/filter.png"), CTRL+Key_F, SQ_Filter::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Filter");
    pAPrintImages = new KAction(i18n("Print"), locate("appdata", "images/imageedit/print.png"), CTRL+Key_P, SQ_Printer::instance(), SLOT(slotStartEdit()), actionCollection(), "SQ Image Print");

    pASlideShow = new KToggleAction(i18n("Slideshow"), locate("appdata", "images/actions/slideshow.png"), CTRL+Key_S, 0, 0, actionCollection(), "SQ toggle slideshow");
    pASlideShowDialog = new KAction(i18n("Slideshow through dialog"), 0, CTRL+ALT+Key_S, this, SLOT(slotSlideShowDialog()), actionCollection(), "SQ SlideShow Dialog");
    connect(pASlideShow, SIGNAL(toggled(bool)), this, SLOT(slotSlideShowToggle(bool)));

    pAPluginsInfo = new KAction(i18n("Plugins information"), "info", 0, this, SLOT(slotPluginsInfo()), actionCollection(), "SQ Plugins Info");

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
    // copy installed desktops to local path,
    // if needed
    tryCopyDesktops();

    // create SQ_ExternalTool and create new
    // popup menu with tools
    extool = new SQ_ExternalTool;
    extool->setAutoDelete(true);
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
    bookmarks = new KActionMenu(i18n("&Bookmarks"), "bookmark", (KActionCollection*)NULL, "bookmarks");
    bookmarkMenu = new KBookmarkMenu(bmanager, bookmarkOwner, bookmarks->popupMenu(), (KActionCollection*)NULL, true);

    connect(bookmarkOwner, SIGNAL(openURL(const KURL&)), pWidgetStack, SLOT(setURLForCurrent(const KURL&)));
}

// Create new config file in
// local directory (~/.kde/share/config)
void KSquirrel::writeDefaultEntries()
{
    SQ_Config conf("ksquirrelrc");

#define conf_Recreate(a) \
    conf.deleteGroup(a); \
    conf.setGroup(a);   

    conf_Recreate("Edit tools");
    conf.writeEntry("preview", "false");
    conf.writeEntry("preview_dont", "true");
    conf.writeEntry("preview_larger_w", "800");
    conf.writeEntry("preview_larger_h", "600");
    conf.writeEntry("altlibrary", "Portable Network Graphics");
    conf.writeEntry("multi", "true");

    conf_Recreate("External tools");
    conf.writeEntry("1", "Run within KDE");
    conf.writeEntry("10", "View in hexadecimal mode");
    conf.writeEntry("11", "Print file");
    conf.writeEntry("12", "Set as wallpaper on desktop");
    conf.writeEntry("13", "Set as tiled wallpaper on desktop");
    conf.writeEntry("2", "Open with GIMP");
    conf.writeEntry("3", "Open with GQview");
    conf.writeEntry("4", "Open with ShowImg");
    conf.writeEntry("5", "Open with KuickShow");
    conf.writeEntry("6", "Open with KView");
    conf.writeEntry("7", "Open with Opera");
    conf.writeEntry("8", "Open with Konqueror");
    conf.writeEntry("9", "Open with KWrite");

    conf_Recreate("Fileview");
    conf.writeEntry("archives", "true");
    conf.writeEntry("click policy", "1");
    conf.writeEntry("click policy custom", "0");
    conf.writeEntry("click policy system", "true");
    conf.writeEntry("custom directory", "/");
    conf.writeEntry("disable_dirs", "false");
    conf.writeEntry("filter_id", "0");
    conf.writeEntry("history", "true");
    conf.writeEntry("last visited", "/");
    conf.writeEntry("run unknown", "false");
    conf.writeEntry("selectdeselecthistory", "");
    conf.writeEntry("set path", "0");
    conf.writeEntry("sync type", "0");
    conf.writeEntry("tofirst", "true");

    conf_Recreate("Filters");
    conf.writeEntry("extensions", "*,*.mpg *.mpeg *.avi *.asf *.divx,*.mp3 *.ogg *.wma *.wav *.it");
    conf.writeEntry("items", "All files,Video tapes,Audio files");
    conf.writeEntry("last", "*");
    conf.writeEntry("menuitem both", "false");

    conf_Recreate("GL view");
    conf.writeEntry("GL view background", "#4e4e4e");
    conf.writeEntry("GL view background type", "1");
    conf.writeEntry("GL view custom texture", "");
    conf.writeEntry("alpha_bkgr", "false");
    conf.writeEntry("angle", "90");
    conf.writeEntry("enable drop", "true");
    conf.writeEntry("geometry", "331,248,685,492");
    conf.writeEntry("hide_sbar", "true");
    conf.writeEntry("ignore", "true");
    conf.writeEntry("marks", "true");
    conf.writeEntry("move", "5");
    conf.writeEntry("progress", "true");
    conf.writeEntry("quickGeometry", "1,35,222,303");
    conf.writeEntry("quickbrowser", "true");
    conf.writeEntry("save pos", "true");
    conf.writeEntry("save size", "true");
    conf.writeEntry("scroll", "0");
    conf.writeEntry("statusbar", "true");
    conf.writeEntry("toolbars_hidden", "false");
    conf.writeEntry("winstate", "0");
    conf.writeEntry("zoom", "25");
    conf.writeEntry("zoom limit", "1");
    conf.writeEntry("zoom type", "2");
    conf.writeEntry("zoom_max", "10000");
    conf.writeEntry("zoom_min", "1");
    conf.writeEntry("zoom_nice", "true");

    conf_Recreate("ksquirrel-small");
    conf.writeEntry("alpha_bkgr", "false");
    conf.writeEntry("geometry", "0,0,640,480");
    conf.writeEntry("hide_sbar", "true");
    conf.writeEntry("ignore", "true");
    conf.writeEntry("marks", "true");
    conf.writeEntry("statusbar", "true");
    conf.writeEntry("toolbars_hidden", "false");
    conf.writeEntry("zoom type", "2");
    conf.writeEntry("zoom_nice", "true");

    conf_Recreate("History");
    conf.writeEntry("items", "");

    conf_Recreate("Image edit options");
    conf.writeEntry("bcg_close", "true");
    conf.writeEntry("bcg_putto", "");
    conf.writeEntry("bcg_where_to_put", "0");
    conf.writeEntry("convert_close", "true");
    conf.writeEntry("convert_putto", "");
    conf.writeEntry("convert_where_to_put", "0");
    conf.writeEntry("filter_close", "true");
    conf.writeEntry("filter_putto", "");
    conf.writeEntry("filter_where_to_put", "0");
    conf.writeEntry("filter_swapRGB", "0");
    conf.writeEntry("filter_blend_color", "#00FF00");
    conf.writeEntry("filter_blend_opacity", "0.5");
    conf.writeEntry("print_X", "1");
    conf.writeEntry("print_Y", "1");
    conf.writeEntry("print_alignment", "center");
    conf.writeEntry("print_close", "true");
    conf.writeEntry("print_layout", "0");
    conf.writeEntry("print_transp", "0");
    conf.writeEntry("print_transp_color", "#000000");
    conf.writeEntry("resize_applyto", "2");
    conf.writeEntry("resize_close", "true");
    conf.writeEntry("resize_fit", "2");
    conf.writeEntry("resize_h", "1");
    conf.writeEntry("resize_method", "LANCZOS3");
    conf.writeEntry("resize_percent", "100");
    conf.writeEntry("resize_preserve", "true");
    conf.writeEntry("resize_putto", "");
    conf.writeEntry("resize_w", "1");
    conf.writeEntry("resize_where_to_put", "0");
    conf.writeEntry("resize_which", "0");
    conf.writeEntry("rotate_close", "true");
    conf.writeEntry("rotate_putto", "");
    conf.writeEntry("rotate_where_to_put", "0");

    conf_Recreate("Interface");
    conf.writeEntry("geometry", "0,0,1016,740");
    conf.writeEntry("gl_separate", "true");
    conf.writeEntry("has_tree", "false");
    conf.writeEntry("has_url", "false");
    conf.writeEntry("last view", "0");
    conf.writeEntry("splitter1", "236,774");


    conf_Recreate("Libraries");
    conf.writeEntry("monitor", "true");
    conf.writeEntry("show dialog", "false");

    conf_Recreate("Main");
    conf.writeEntry("first_time", "true");
    conf.writeEntry("minimize to tray", "false");
    conf.writeEntry("sync", "false");
    conf.writeEntry("version", SQ_VERSION);

    conf_Recreate("Navigator Icons");
    conf.writeEntry("Preview Size", "60");
    conf.writeEntry("ShowPreviews", "false");
    conf.writeEntry("ViewMode", "LargeRows");

    conf_Recreate("Navigator List");
    conf.writeEntry("Preview Size", "60");
    conf.writeEntry("ShowPreviews", "false");
    conf.writeEntry("ViewMode", "SmallColumns");

    conf_Recreate("Slideshow");
    conf.writeEntry("delay", "1000");
    conf.writeEntry("fullscreen", "true");
    conf.writeEntry("force", "true");
    conf.writeEntry("history", "/");

    conf_Recreate("Thumbnails");
    conf.writeEntry("cache", "20000");
    conf.writeEntry("disable_mime", "false");
    conf.writeEntry("dont write", "false");
    conf.writeEntry("extended", "false");
    conf.writeEntry("margin", "0");
    conf.writeEntry("size", "2");
    conf.writeEntry("tooltips", "true");

    conf_Recreate("file browser");
    conf.writeEntry("Preview Size", "60");
    conf.writeEntry("ShowPreviews", "false");
    conf.writeEntry("ViewMode", "SmallColumns");

    conf.sync();

#undef conf_Recreate

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

    updateDirs &= (bool)pWidgetStack->visibleWidget()->numDirs();

    kconf->setGroup("Thumbnails");

    updateThumbs = (old_ext != kconf->readBoolEntry("extended", false));

    // Change ckicking policy
    pWidgetStack->configureClickPolicy();
    pWidgetStack->updateGrid(!updateThumbs | updateDirs);

    // set cache limit for pixmap cache
    SQ_PixmapCache::instance()->setCacheLimit(kconf->readNumEntry("cache", 1024*10));

    kconf->setGroup("Main");

    if(kconf->readBoolEntry("sync", true))
        kconf->sync();

    if(updateDirs || updateThumbs)
        pWidgetStack->updateView();
}

// Toggle fullscreen state for image window
void KSquirrel::slotFullScreen(bool full)
{
    WId id = (gl_view->isSeparate()) ? gl_view->winId() : winId();
    KStatusBar *s = gl_view->statusbar();

    kconf->setGroup("GL view");

    // hide statusbar in fullscreen mode ?
    if(s && kconf->readBoolEntry("hide_sbar", true))
    {
        s->setShown(!full);
        SQ_GLWidget::window()->pAStatus->setChecked(!full);
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
        kconf->writeEntry("geometry", gl_view->geometry());

    kconf->writeEntry("statusbar", SQ_GLWidget::window()->pAStatus->isChecked());
    kconf->writeEntry("quickGeometry", SQ_QuickBrowser::window()->geometry());
    kconf->writeEntry("ignore", SQ_GLWidget::window()->pAIfLess->isChecked());
    kconf->writeEntry("zoom type", SQ_GLWidget::window()->zoomType());
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

// Reload libraries from disk
QString KSquirrel::slotRescan()
{
    const QString libPrefix = "/usr/lib/ksquirrel-libs/";

    KURL url = libPrefix;

    SQ_LibraryHandler::instance()->clear();

    // SQ_LibraryListener will find libraries
    // and SQ_LibraryHandler will load them
    libl->slotOpenURL(url, false, true);

    return libPrefix;
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

    int w = 200, h = 20;
    hbox->setGeometry(QApplication::desktop()->width() / 2 - w / 2, QApplication::desktop()->height() / 2 - h / 2, w, h);

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
    hide();

    kconf->setGroup("Thumbnails");

    bool create_splash = (!kconf->readBoolEntry("dont write", false)) & (!SQ_PixmapCache::instance()->empty());

    // create post splash
    if(create_splash)
    {
        createPostSplash();
        qApp->processEvents();
    }

    // save parameters to config file
    saveValues();

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
    libhandler = new SQ_LibraryHandler;
    libl = new SQ_LibraryListener;

    libl->setAutoErrorHandlingEnabled(false, NULL);

    initExternalTools();

    new SQ_ErrorString;
    new SQ_Converter;
    new SQ_Resizer;
    new SQ_Rotater;
    new SQ_Contraster;
    new SQ_Printer;
    new SQ_Filter;

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

    tray = new SQ_SystemTray;

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
            KFileView *v = pWidgetStack->visibleWidget()->view();
            v->clearSelection();
            v->setCurrentItem(fm.fileName());
            v->setSelected(v->currentFileItem(), true);

            // if it is known image type - let's load it
            if(SQ_LibraryHandler::instance()->supports(SQ_HLOptions::instance()->path))
                SQ_GLWidget::window()->slotStartDecoding(SQ_HLOptions::instance()->path, true);
        }
    }
}

// Show/hide tree
void KSquirrel::slotSetTreeShown(bool shown)
{
    mainSizes = mainSplitter->sizes();
    ptree->setShown(shown);
}

// 
void KSquirrel::slotSeparateGL(bool sep)
{
    kconf->setGroup("GL view");

    if(sep)
    {
        viewBrowser->raiseWidget(0);
        gl_view->reparent(0, QPoint(0,0), false);
        QRect rect(0,0,320,200);
        gl_view->setGeometry(kconf->readRectEntry("geometry", &rect));
        raiseGLWidget();
    }
    else
    {
        kconf->writeEntry("geometry", gl_view->geometry());
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

KPopupMenu* KSquirrel::menuFilters()
{
    return actionFilterMenu;
}

KPopupMenu* KSquirrel::menuViews()
{
    return actionViews;
}

KSquirrel* KSquirrel::app()
{
    return sing;
}

// Copy installed .desktop files
// to local directory (to ~/.ksquirrel/desktop)
void KSquirrel::tryCopyDesktops()
{
    QString tmp, str;
    SQ_Dir dir(SQ_Dir::Desktops);

    kconf->setGroup("External tools");

    for(int i = 1;;i++)
    {
        str.sprintf("%d", i);
        tmp = kconf->readEntry(str, QString::null);

        if(tmp.isEmpty())
            break;

        QString name = tmp + ".desktop";
        QString system = ::locate("appdata", QString("desktop/%1").arg(name));
        QString local = dir.absPath(name);

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

        // read data from installed .dekstop file
        QByteArray ind = in.readAll();

        // write data to local .desktop
        out.writeBlock(ind);

        in.close();
        out.close();
    }
}

// Create a splash with image edit actions:
// Convert, Rotate, Resize ...
void KSquirrel::slotShowImageEditActions()
{
    SQ_ImageEditSplash *splash = new SQ_ImageEditSplash(0, "Splash with image actions", WStyle_Customize | WStyle_StaysOnTop);

    QDesktopWidget *d = QApplication::desktop();

    int w = d->width();
    int h = d->height();

    // move to center
    splash->move((w - splash->width()) / 2, (h - splash->height()) / 2);

    splash->show();
}

// Create SQ_ThumbnailCacheMaster, which can do something with thumbnails
// and thumbnail cache
void KSquirrel::slotTCMaster()
{
    SQ_ThumbnailCacheMaster *m = new SQ_ThumbnailCacheMaster(this);

    m->exec();
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
            KFileView *v = pWidgetStack->visibleWidget()->view();
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
        qDebug("\nList of available DCOP parameters for control() method:");
        qDebug("*****************************");

        QMap<QString, int>::iterator BEGIN = messages.begin();
        QMap<QString, int>::iterator   END = messages.end();

        for(QMap<QString, int>::iterator it = BEGIN;it != END;++it)
        {
            qDebug("%s", it.key().ascii());
        }

        qDebug("******************************\n");
    }
}

void KSquirrel::fillMessages()
{
    messages["image_next"]                 = Qt::Key_PageDown;
    messages["image_previous"]             = Qt::Key_PageUp;
    messages["image_first"]                = Qt::Key_Home;
    messages["image_last"]                 = Qt::Key_End;
    messages["image_reset"]                = Qt::Key_R;
    messages["image_information"]          = Qt::Key_I;
    messages["image_delete"]               = Qt::Key_Delete;

    messages["image_animation_toggle"]     = Qt::Key_A;

    messages["image_page_first"]           = Qt::Key_F1;
    messages["image_page_last"]            = Qt::Key_F4;
    messages["image_page_next"]            = Qt::Key_F3;
    messages["image_page_previous"]        = Qt::Key_F2;

    messages["image_window_fullscreen"]    = Qt::Key_F;
    messages["image_window_quickbrowser"]  = Qt::Key_Q;
    messages["image_window_close"]         = Qt::Key_X;
    messages["image_window_help"]          = Qt::Key_Slash;

    messages["zoom_plus"]                  = Qt::Key_Plus;
    messages["zoom_minus"]                 = Qt::Key_Minus;
    messages["zoom_1"]                     = Qt::Key_1;
    messages["zoom_2"]                     = Qt::Key_2;
    messages["zoom_3"]                     = Qt::Key_3;
    messages["zoom_4"]                     = Qt::Key_4;
    messages["zoom_5"]                     = Qt::Key_5;
    messages["zoom_6"]                     = Qt::Key_6;
    messages["zoom_7"]                     = Qt::Key_7;
    messages["zoom_8"]                     = Qt::Key_8;
    messages["zoom_9"]                     = Qt::Key_9;
    messages["zoom_10"]                    = Qt::Key_0;
}

// Check for a newer version
void KSquirrel::slotCheckVersion()
{
    SQ_CheckVersion *ch = new SQ_CheckVersion(this);

    ch->exec();
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
    SQ_SlideShow *s = new SQ_SlideShow(this);

    if(s->exec(slideShowDir) == QDialog::Accepted)
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

    if(slideShowDelay < 0) slideShowDelay = 1000;

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
    {
        SQ_GLWidget::window()->slotStartDecoding(slideShowName, true);
    }

    // goto next file
    slideShowIndex++;

    // all files loaded ?
    if(slideShowIndex == slideShowTotal)
    {
        stopSlideShow();
        return;
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
    if(SQ_GLWidget::window()->pAFull->isChecked() && kconf->readBoolEntry("fullscreen", true))
        SQ_GLWidget::window()->pAToolFull->animateClick();

    pASlideShow->setChecked(false);
    SQ_GLWidget::window()->updateSlideShowButton(false);
}

// Show information on libraries
void KSquirrel::slotPluginsInfo()
{
    SQ_PluginsInfo *pi = new SQ_PluginsInfo(this);

    pi->exec();
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

void KSquirrel::slotEat()
{
/*
    const int s = 1024*1024*30;

    char *p = new char [s];

    if(p)
        memset(p, 0, s);
*/
}
