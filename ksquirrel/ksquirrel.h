/***************************************************************************
                          ksquirrel.h  -  description
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

#ifndef KSQUIRREL_H
#define KSQUIRREL_H

#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>

#include <kmainwindow.h>
#include <dcopobject.h>
#include <kio/job.h>

// forward declarations

class KMenuBar;
class KToolBar;
class KStatusBar;
class KSystemTray;
class KAction;
class KRadioAction;
class KActionMenu;
class KHistoryCombo;
class KToggleAction;
class KBookmarkMenu;

template <class T> class QValueVector;
class QLabel;
class QWidgetStack;
class QHBox;
class QVBox;
class QTimer;

class SQ_WidgetStack;
class SQ_SystemTray;
class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_Config;
class SQ_ExternalTool;
class SQ_TreeView;
class SQ_ThumbnailSize;
class SQ_PixmapCache;
class SQ_LocationToolbar;
class SQ_GLView;
class SQ_GLWidget;
class SQ_ArchiveHandler;
class SQ_MultiBar;
class SQ_Dir;

/*
 *  Main widget, which can accept DCOP messages
 */

class KSquirrel : public KMainWindow, public DCOPObject
{
    Q_OBJECT

    public:
        /*
         *  Constructor & destructor
         */
        KSquirrel(QWidget *parent = 0, const char *name = 0);
        ~KSquirrel();

        /*
         *  "history combo"
         */
        KHistoryCombo* historyCombo();

        /*
         *  Do some actions before exit - show final splash (if needed),
         *  save parameters to config file,
         *  write config file to disk, write cached thumbnails, etc.
         */
        void finalActions();

        /*
         *  Enable/disable popup menu with thumnail sizes. Called
         *  from SQ_WidgetStack, when user changed current view
         *  type (for example Thumbnail view -> Icon view).
         */
        void enableThumbsMenu(bool);

        /*
         *   Set caption. When SQ_GLView is separated, the caption
         *   will be set to it, and to KSquirrel's main window
         *   otherwise. Called from SQ_GLWidget.
         */
        void setCaption(const QString &cap);

        /*
         *  Stop slideshow, if running.
         */
        void stopSlideShow();

        /*
         *  Check if slideshow is running.
         */
        bool slideShowRunning() const;

        /*
         *  Return popup menu with filters for filemanager.
         */
        KPopupMenu* menuFilters();

        /*
         *  Return popup menu with view types - 
         *  Icon view, Detail view ...
         */
        KPopupMenu* menuViews();

        /*
         *  Get pointer to a widget, located in status bar.
         *  All pointers to statusbar widgets are saved in 
         *  'sbarwidgets' object (QMap).
         */
        QLabel* sbarWidget(const QString &name);

        /*
         *   DCOP methods
         */

        /*
         *  Return a list of available functions
         */
        QCStringList functions();

        /*
         *  Process incoming DCOP message
         */
        bool process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData);

        /*
         *  Pause or unpause slideshow, if running.
         */
        void pauseSlideShow();

        /*
         *  Names and extensions of name filters for
         *  file manager.
         */
        QStringList* filtersNames() const;
        QStringList* filtersExtensions() const;

        static KSquirrel*     app() { return m_instance; }

    protected:
        /*
         *  Catch some events and do specific actions.
         */
        void closeEvent(QCloseEvent *e);
        void resizeEvent(QResizeEvent *e);

    /*
     *  Internal methods
     */
    private:

        /*
         *  Check if current config file left from older KSquirrel's
         *  version. If so, return true, and new config file will be
         *  copied instead of old.
         */
        bool checkConfigFileVersion();

        /*
         *  Create a final splash. It will be shown only if SQ_PixmapCache
         *  is not empty.
         */
        void createPostSplash();

        /*
         *  Create location toolbar and store a pointer to
         *  it in passed argument.
         */
        void createLocationToolbar(SQ_LocationToolbar *);

        /*
         *  Create statusbar.
         */
        void createStatusBar(KStatusBar*);

        /*
         *  Create main toolbar.
         */
        void createToolbar(KToolBar *);

        /*
         *  Create main menu.
         */
        void createMenu(KMenuBar *);

        /*
         *  Create all KActions
         */
        void createActions();

        /*
         *  Create all needed objects
         *  (SQ_LibraryHandler, SQ_LibraryListener, edit tools, etc.)
         */
        void preCreate();

        /*
         *  Create QStrinLists, containing filters and filters' names (if needed)
         *  and fill popup menu with new filters.
         */
        void initFilterMenu();

        /*
         *  Do the same for external tools.
         */
        void initExternalTools();

        /*
         *  Init bookmarks and create popup menu for them.
         */
        void initBookmarks();

        /*
         *  Create all widgets (filemanager, file tree, toolbar, etc.)
         */
        void createWidgets(int);

        /*
         *  Restore saved position & size
         */
        void handlePositionSize();

        /*
         *  If user-depended config file doesn't exist (~/.kde/share/config/ksquirrelrc),
         *  create it and write default entries. Called before SQ_Config object is being
         *  created
         */
        void writeDefaultEntries();

        /*
         *  Fill all parameters with its default values (form config
         *  file). Affects to clicking policy in SQ_WidgetStack,
         *  cache limit in SQ_PixmapCache etc.
         */
        void applyDefaultSettings();

        /*
         *   Save all parameters to config file before exit.
         */
        void saveValues();

        /*
         *  Open image and/or change current directory
         *  to its URL. Called when user clicks "Open file" and
         *  "Open file #2" in main menu.
         */
        void openFile(bool parseURL = false);

        /*
         *  Run slideshow
         */
        void slideShowPrivate();

        /*
         *  Set filter for a filemanager to 'f' and check
         *  appropriate menu item
         */
        void setFilter(const QString &f, const int id);

        /*
         *  Fill 'messages' map with values. 'messages' map is used
         *  to determine which key sequence should KSquirrel pass to SQ_GLWidget
         *  to perform an action, needed by DCOP client.
         */
        void fillMessages();

        /*
         *  DCOP helper methods
         */

        /*
         *  Determine the incoming DCOP message, create appropriate
         *  key sequence and send this sequence to SQ_GLWidget.
         * 
         *  The main idea is that KSquirrel will find needed key sequence
         *  by its name (in 'messages' map), costruct QKeyEvent and send to 
         *  SQ_GLWidget. SQ_GLWidget already has keyEvent() handler, so it will
         *  do what DCOP client wants.
         *  
         *  For example, 
         *  
         *  $ dcop ksquirrel ksquirrel control image_next
         * 
         *  will call KSquirrel::control("image_next"), which will
         *  construct QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown,
         *  Qt::Key_PageDown, Qt::NoButton)
         *  and send it to SQ_GLWidget. SQ_GLWidget will catch keypress event,
         *  and load next image in the current directory.
         */
        void control(const QString &command);

        /*
         *  Activate this instance (DCOP method)
         */
        void activate();

        /*
         *  Get QString argument from incoming QByteArray.
         */
        QString getArg(const QByteArray &data);

    signals:

        /*
         *  emitted, when user chages thumbails' size (from menu).
         */
        void thumbSizeChanged(const QString&);

        /*
         *  emitted, when all KSquirrel created thumbnails for all
         *  found images (if -t option was specified).
         */
        void continueLoading();

    public slots:

        /*
         *  Close (hide) SQ_GLWidget.
         */
        void slotCloseGLWidget();

        /*
         *  Show SQ_GLWidget.
         */
        void raiseGLWidget();

        /*
         *  Switch fullscreen state of SQ_GLView.
         */
        void slotFullScreen(bool full);

    private slots:

        /*
         *  Invoke 'Options' dialog.
         */
        void slotOptions();

        /*
         *  Invoke 'Filters' dialog.
         */
        void slotFilters();

        /*
         *  Invoke 'External tools' dialog.
         */
        void slotExtTools();

        /*
         *  Raise list view in filemanager.
         */
        void slotRaiseListView();

        /*
         *  Raise icon view in filemanager.
         */
        void slotRaiseIconView();

        /*
         *  Raise detail view in filemanager.
         */
        void slotRaiseDetailView();

        /*
         *  Raise thumbnail view in filemanager.
         */
        void slotRaiseThumbView();

        /*
         *  Invoked, when user clicked "Go" button in location toolbar.
         */
        void slotGo();

        /*
         *  Invoked, when user selected to filter for filemanager.
         */
        void slotSetFilter(int);

        /*
         *  Invoked, when user clicked "Go to tray".
         */
        void slotGotoTray();

        /*
         *  Next four slots will be called, when user selected
         *  subitem in 'Thumbnail size' menu.
         */
        void slotThumbsSmall();
        void slotThumbsMedium();
        void slotThumbsLarge();
        void slotThumbsHuge();

        /*
         *  Invoked, when SQ_LibraryListener found all libraries
         *  in /usr/lib/ksquirrel-libs. Now we can decode images.
         */
        void slotContinueLoading();

        /*
         *  Invoked, when user toggles "Separate image window" button.
         */
        void slotSeparateGL(bool);

        /*
         *  Invoked, when user clicked "Check for a newer version" button.
         */
        void slotCheckVersion();

        /*
         *  Invoked, when user clicked "Open file" button.
         */
        void slotOpenFile();

        /*
         *  Invoked, when user clicked "Open file #2" button.
         *  This slot will decode selected image, and change
         *  current directory.
         */
        void slotOpenFileAndSet();

        /*
         *  Invoked, when user clicked an animated logo in toolbar.
         *  This slot will open KSquirrel's homepage with default browser.
         */
        void slotAnimatedClicked();

        /*
         *  This slot will catch signals from image finder (if -t option specified)
         */
        void slotUDSEntries(KIO::Job*, const KIO::UDSEntryList&);

        /*
         *  Invoked, when image finder found all images (or didn't find any image)
         */
        void listResult(KIO::Job *);

        /*
         *  Continue loading, when all images are found
         */
        void slotContinueLoading2();

        /*
         *  Invokes a dialog with specific thumbnails actions:
         *  delete thumbnails on disk, show thumbnails on disk, ...
         */
        void slotTCMaster();

        /*
         *  Invoked, when user clicked "Slideshow through dialog".
         *  Creates SQ_SlideShow dialog.
         */
        void slotSlideShowDialog();

        /*
         *  Determine the next image, which should be shown in slideshow.
         */
        void slotSlideShowNextImage();

        /*
         *  Invoked, when user toggled slideshow mode with appropriate button.
         *  Will stop or run slideshow.
         */
        void slotSlideShowToggle(bool);

        /*
         *  Invoked, when user clicked "Plugins information" button.
         *  Creates a dialog with information on all found libraries
         */
        void slotPluginsInfo();

        /*
         *  Convinience slot.
         *  Reload libraries from disk.
         */
        void slotRescan();

    private:
        static KSquirrel     *m_instance;

        // main toolbar
        KToolBar    *tools;

        //main menu
        KMenuBar    *menubar;

        // different views (Icon view, List view ...)
        KRadioAction    *pARaiseListView, *pARaiseIconView, *pARaiseDetailView, *pARaiseThumbView;

        // popup menu with thumbnail sizes
        KActionMenu    *pAThumbs;

        // thumbnail sizes (small, normal, ...)
        KRadioAction    *pAThumb0, *pAThumb1, *pAThumb2, *pAThumb3;

        // toggle actions: show/hide url box, make image window
        // built-in/separate
        KToggleAction    *pAURL, *pASeparateGL;

        // popup menus: "File", "View" ...
        KPopupMenu    *pop_file, *pop_view, *pop_action, *pop_nav;

        // filters and views
        KPopupMenu    *actionFilterMenu, *actionViews;

        // menus with bookmarks
        KActionMenu     *bookmarks;
        KBookmarkMenu    *bookmarkMenu;

        // KIO::Job, which will find images (if -t option specified)
        KIO::ListJob    *job;

        // "Open file", "open file #2"
        KAction    *pAOpen, *pAOpenAndSet,

        // Open "SQ_ThumbnailCacheMaster"
        *pATCMaster,

        // Show image window
        *pAGLView,

        // Exit
        *pAExit,

        // Reload libraries from disk (from /usr/lib/ksquirrel-libs)
        *pARescan,

        // External tool and filters
        *pAExtTools, *pAFilters,

        // Goto tray
        *pAGotoTray,

        // Dialog for advanced slideshow
        *pASlideShowDialog,

        // Plugins information
        *pAPluginsInfo;

        // Main statusbar
        KStatusBar    *sbar;

        // contains paths of found libraries
        QStringList    strlibFound;

        // QWidgetStack, which contains
        // tree and filemanager at the first page
        // and SQ_GLView at the second
        QWidgetStack    *viewBrowser;

        // main QVBox, will contain menu, toolbar and 'viewBrowser'
        QVBox    *mainBox, *b2;

        // sizes for mainSplitter
        QValueList<int>    mainSizes;

        // libraries' filters
        QStringList    libFilters;

        // QMap, which contains pointers to statusbar widgets.
        // I can get a pointer by widget's name.
        //
        // For example:
        //
        // KSquirrel::app()->sbarWidget("fileName")->setText("Filename.txt");
        //
        QMap<QString, QLabel*>sbarwidgets;

        // QLabels for statusbar
        QLabel    *dirInfo, *fileIcon, *fileName;

        // QMap, which contains available DCOP
        // parameters (such as "image_next", "image_prev")
        // and its appropriate key sequences.
        //
        // For example, "image_next" is mapped to Qt::Key_PageDown
        // "image_prev" - to "Qt::Key_PageUp"
        QMap<QString, int>    messages;

        // combobox with history urls
        KHistoryCombo    *pCurrentURL;

        // filters' names and extensions
        QStringList    *sqFiltersName, *sqFiltersExt;

        // "Configure KSquirrel"
        KAction    *pAConfigure,

        // "Check for a newer version"
        *pACheck,

        // "Select group", "Deselect group", "Select all", "Deselect"
        // actions for filemanager
        *pASelectGroup, *pADeselectGroup, *pASelectAll, *pADeselectAll;

        // "Slideshow"
        KToggleAction    *pASlideShow;

        /*
         * Slideshow-specififc members
         */

         /*
          *  Is slideshow paused by user (with 'Pause') ?
          */
         bool slideShowPaused;

        // Directory for slideshow.
        // KSquirrel will use it to load file names.
        QString    slideShowDir;

        // Contains found files
        QDir    slideShowItems;

        // Delay, total files in selected directory and 
        // current file index
        int    slideShowIndex, slideShowDelay, slideShowTotal, slideShowRepeat;

        // current file name
        QString    slideShowName;

        // timer for slideshow
        QTimer    *slideShowTimer;

        // is slideshow stopped ?
        bool    slideShowStop;

        // url box
        SQ_LocationToolbar    *pTLocation;

        // used by image finder (if -t option specified)
        SQ_Dir    *dir;

        // our config file
        SQ_Config    *kconf;

        // widget stack containing different views (Icon view, List view ...)
        SQ_WidgetStack    *pWidgetStack;

        // our library handler
        SQ_LibraryHandler    *libhandler;

        // our library finder with dynamic loading/deleting support
        SQ_LibraryListener    *libl;

        // ou tray instance
        SQ_SystemTray    *tray;

        // external tools
        SQ_ExternalTool    *extool;

        // file tree
        SQ_TreeView    *ptree;

        // widget containing SQ_GLWidget and statusbar
        SQ_GLView    *gl_view;

        // archive handler, can unpack different archives
        SQ_ArchiveHandler    *ar;

        // object containing different useful information
        // on thumbnails (size, margin ...)
        SQ_ThumbnailSize    *thumbSize;

        // thumbnails' memory cache
        SQ_PixmapCache    *cache;

        // sidebar
        SQ_MultiBar       *sideBar;

        // not interesting ;)
        int     old_id;
        bool   first_time, old_disable, old_ext,
                 m_urlbox, m_sep, old_marks;
};

// Is slideshow running ?
inline
bool KSquirrel::slideShowRunning() const
{
    return !slideShowStop;
}

inline
KHistoryCombo* KSquirrel::historyCombo()
{
    return pCurrentURL;
}

inline
QStringList* KSquirrel::filtersNames() const
{
    return sqFiltersName;
}

inline
QStringList* KSquirrel::filtersExtensions() const
{
    return sqFiltersExt;
}

inline
KPopupMenu* KSquirrel::menuFilters()
{
    return actionFilterMenu;
}

inline
KPopupMenu* KSquirrel::menuViews()
{
    return actionViews;
}

#endif
