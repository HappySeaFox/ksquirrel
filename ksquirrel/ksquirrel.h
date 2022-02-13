/***************************************************************************
                          ksquirrel.h  -  description
                             -------------------
    begin                : Dec 10 2003
    copyright            : (C) 2004 by Baryshev Dmitry
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

#include <kmainwindow.h>
#include <kio/job.h>

class KMenuBar;
class KIconLoader;
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
class QSplitter;
class QLabel;
class QWidgetStack;
class QHBox;
class QVBox;

class SQ_WidgetStack;
class SQ_SystemTray;
class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_Config;
class SQ_ExternalTool;
class SQ_TreeView;
class SQ_BookmarkOwner;
class SQ_HLOptions;
class SQ_ThumbnailSize;
class SQ_PixmapCache;
class SQ_LocationToolbar;
class SQ_GLView;
class SQ_GLWidget;
class SQ_ArchiveHandler;
class SQ_UpdateKsquirrelThread;
class SQ_Dir;
class SQ_Converter;
class SQ_Resizer;

class KSquirrel : public KMainWindow
{
	Q_OBJECT

	public:
		KSquirrel(SQ_HLOptions *, QWidget *parent = 0, const char *name = 0);
		~KSquirrel();

		void finalActions();
		void enableThumbsMenu(bool);
		void setCaption(const QString &cap);

		KPopupMenu* menuFilters();
		KPopupMenu* menuViews();

	protected:
		void closeEvent(QCloseEvent *e);
		void resizeEvent(QResizeEvent *e);

	private:
		void createLocationToolbar(SQ_LocationToolbar *);
		void createStatusBar(KStatusBar*);
		void createToolbar(KToolBar *);
		void createMenu(KMenuBar *);
		void createActions();
		void preCreate();
		void createPostSplash();

		void initFilterMenu();
		void initExternalTools();
		void initBookmarks();
		void tryCopyDesktops();

		void createWidgets(int);
		void handlePositionSize();
		void writeDefaultEntries(const QString &toConf);
		void applyDefaultSettings();
		void saveValues();
		void openFile(bool parseURL = false);

		void setFilter(const QString &f, const int id);

	signals:
		void thumbSizeChanged(const QString&);
		void continueLoading();

	public slots:
		void slotOptions();
		void slotFilters();
		void slotExtTools();
		void slotRaiseListView();
		void slotRaiseIconView();
		void slotRaiseDetailView();
		void slotRaiseThumbView();
		void slotGo();
		void slotSetFilter(int);
		void slotGotoTray();
		void slotCloseGLWidget();
		void slotFullScreen(bool full);
		void slotThumbsSmall();
		void slotThumbsMedium();
		void slotThumbsLarge();
		void slotThumbsHuge();
		void slotContinueLoading();
		void raiseGLWidget();
		void slotSetTreeShown(bool shown);
		void slotSeparateGL(bool);
		void slotOpenFile();
		void slotOpenFileAndSet();
		void slotNeedUpdate(const QString &ver);
		void slotShowUpdate();
		void slotAnimatedClicked();
		QString slotRescan();

	private slots:
		void slotUDSEntries(KIO::Job*, const KIO::UDSEntryList&);
		void listResult(KIO::Job*);
		void slotContinueLoading2();

	public:
		static KSquirrel 		*m_app;
		static KIconLoader		*iconL;

		static KIconLoader* loader();
		static KSquirrel* app();

		SQ_Config				*kconf;
		SQ_WidgetStack		*pWidgetStack;
		KHistoryCombo		*pCurrentURL;
		SQ_LibraryHandler		*libhandler;
		KStatusBar				*sbar;
		SQ_LibraryListener	*libl;
		SQ_SystemTray		*tray;
		QLabel					*dirInfo, *fileIcon, *fileName, *decodedStatus, *decodedIcon, *GLzoom, *GLangle, *GLcoord, *GLloaded, *GLcurrentFrame, *GLcurrentFile;
		QHBox					*decodedBox;
		SQ_ExternalTool		*extool;
		SQ_TreeView			*ptree;
		SQ_BookmarkOwner 	*bookmarkOwner;
		SQ_HLOptions			*highlevel;
		QStringList				*filters_name, *filters_ext;
		SQ_ThumbnailSize	*thumbSize;
		SQ_PixmapCache		*cache;
		KAction					*pAGLView, *pAConfigure, *pAExit, *pARescan, *pAExtTools, *pAFilters, *pAGotoTray;
		SQ_GLView			*gl_view;
		SQ_ArchiveHandler	*ar;
		QString				new_version;
		SQ_Converter			*conv;
		SQ_Resizer			*rsz;

	private:
		KToolBar				*tools;
		SQ_LocationToolbar	*pTLocation;
		KMenuBar				*menubar;
		KRadioAction			*pARaiseListView, *pARaiseIconView, *pARaiseDetailView, *pARaiseThumbView;
		KActionMenu			*pAThumbs;
		KRadioAction			*pAThumb0, *pAThumb1, *pAThumb2, *pAThumb3;
		KToggleAction			*pATree, *pAURL, *pASeparateGL;
		QSplitter				*mainSplitter;
		KPopupMenu			*pop_file, *pop_view, *pop_edit;
		KPopupMenu			*actionFilterMenu, *actionViews;
		KActionMenu 			*bookmarks;
		QStringList				strlibFound;
		int						old_id;
		QWidgetStack			*viewBrowser;
		bool 					first_time, hastree, old_disable, old_ext, m_urlbox, m_sep, old_marks;
		QVBox					*mainBox, *b2;
		KBookmarkMenu		*bookmarkMenu;
		QValueList<int>		mainSizes;
		QStringList				libFilters;
		KAction					*pAOpen, *pAOpenAndSet;
		SQ_UpdateKsquirrelThread	*updater;
		KIO::ListJob			*job;
		SQ_Dir					*dir;
		KActionMenu 			*pAImageActions;
};

#define	sqBookmarks		(KSquirrel::m_app->bookmarkOwner)
#define	sqHighLevel		(KSquirrel::m_app->highlevel)

#define	sqCurrentURL		(KSquirrel::m_app->pCurrentURL)

#define	sqSBdirInfo		(KSquirrel::m_app->dirInfo)
#define	sqSBfileIcon		(KSquirrel::m_app->fileIcon)
#define	sqSBfileName		(KSquirrel::m_app->fileName)
#define	sqSBDecoded		(KSquirrel::m_app->decodedStatus)
#define	sqSBDecodedI		(KSquirrel::m_app->decodedIcon)
#define	sqSBDecodedBox	(KSquirrel::m_app->decodedBox)
#define	sqSBGLZoom		(KSquirrel::m_app->GLzoom)
#define	sqSBGLAngle		(KSquirrel::m_app->GLangle)
#define	sqSBGLCoord		(KSquirrel::m_app->GLcoord)
#define	sqSBLoaded		(KSquirrel::m_app->GLloaded)
#define	sqSBFrame			(KSquirrel::m_app->GLcurrentFrame)
#define	sqSBFile			(KSquirrel::m_app->GLcurrentFile)

#define	sqFiltersName		(KSquirrel::m_app->filters_name)
#define	sqFiltersExt		(KSquirrel::m_app->filters_ext)

#endif
