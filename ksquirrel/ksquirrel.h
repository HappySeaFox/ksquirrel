/***************************************************************************
                          ksquirrel.h  -  description
                             -------------------
    begin                : Dec 10 2003
    copyright            : (C) 2004 by ckult
    email                : squirrel-sf@yandex.ru
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

#include <kdockwidget.h>
#include <dcopobject.h>

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

template <class T> class QValueVector;
template <class T> class QPtrList;
class QSplitter;
class QLabel;
class QWidgetStack;
class QHBox;

class SQ_WidgetStack;
class SQ_SystemTray;
class SQ_GLView;
class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_Config;
class SQ_ExternalTool;
class SQ_TreeView;
class SQ_BookmarkOwner;
class SQ_HLOptions;
class SQ_ThumbnailSize;
class SQ_PixmapCache;

class Squirrel : public KDockMainWindow, public DCOPObject
{
	Q_OBJECT

	public:
		Squirrel(SQ_HLOptions *, QWidget *parent = 0, const char *name = 0);
		~Squirrel();

		void raiseGLWidget();

	protected:
		void closeEvent(QCloseEvent *e);

	private:
		void createLocationToolbar(KToolBar *);
		void createStatusBar(KStatusBar*);
		void createToolbar(KToolBar*);
		void createMenu(KMenuBar*);
		void createActions();

		void createPostSplash();

		// init special QValueVector lists from config file.
		void initFilterMenu();
		void initExternalTools();
		void initBookmarks();

		void createWidgetsLikeSQuirrel();
		void createWidgetsLikeGqview();
		void createWidgetsLikeKuickshow();
		void createWidgetsLikeNoComponents();
		void createWidgetsLikeXnview();
		void createWidgetsLikeShowImg();
		void createWidgetsLikeBrowser();

		void handlePositionSize();

		int findProtocol(const QString &proc);

		void writeDefaultEntries();
		void applyDefaultSettings();
		void saveValues();

	signals:
		void thumbSizeChanged(const QString&);

	public slots:
		QCStringList functions();
		bool process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData);
		void control(const QString &str);
		void slotOptions();
		void slotFilters();
		void slotExtTools();
		void slotRaiseListView();
		void slotRaiseIconView();
		void slotRaiseDetailView();
		void slotRaiseThumbView();
		void slotGo();
		void slotSetFilter(int);
		void slotGLView();
		void slotGotoTray();
		void slotSetFile();
		void slotCloseGLWidget();
		void slotFullScreen(bool full);
		QString slotRescan();
		void slotThumbsSmall();
		void slotThumbsMedium();
		void slotThumbsLarge();
		void slotThumbsHuge();

	public:
		static Squirrel *App;
		enum ViewType	{SQuirrel=0,Gqview,Kuickshow,NoComponents,Xnview,ShowImg,Browser};

		ViewType			curViewType;
		SQ_Config			*kconf;
		KIconLoader			*iconL;
		SQ_WidgetStack		*pWidgetStack;
		KHistoryCombo		*pCurrentURL;
		SQ_GLView			*gl;
		SQ_LibraryHandler		*sqLibHandlerReal;
		KStatusBar			*sbar;
		SQ_LibraryListener		*libl;
		SQ_SystemTray		*tray;
		QLabel				*dirInfo, *fileIcon, *fileName, *decodedStatus, *GLreporter;
		SQ_ExternalTool		*extool;
		SQ_TreeView			*ptree;
		SQ_BookmarkOwner 	*bookmarkOwner;
		SQ_HLOptions		*highlevel;
		QStringList			*filters_name, *filters_ext;
		SQ_ThumbnailSize	*thumbSize;
		SQ_PixmapCache		*cache;
		KAction				*pAGLView, *pAConfigure, *pAExit, *pARescan, *pAExtTools, *pAFilters, *pAGotoTray;

	private:
		KMenuBar			*menubar;
		KToolBar				*fileTools, *pTLocation;
		KDockWidget 			*mainDock;
		KRadioAction			*pARaiseListView, *pARaiseIconView, *pARaiseDetailView, *pARaiseThumbView;
		KActionMenu			*pAThumbs;
		KRadioAction			*pAThumb0, *pAThumb1, *pAThumb2, *pAThumb3;
		QSplitter				*mainSplitter, *mainSplitterV;
		KPopupMenu			*pop_file, *pop_view, *pop_edit;
		KPopupMenu			*actionFilterMenu;
		KActionMenu 			*bookmarks;
	 	int	 				createFirst;
		QStringList			strlibFound;
		int					old_id;
		QWidgetStack		*viewBrowser;
		QHBox				*progressBox;
		bool 				first_time;
};

#define	sqApp			(Squirrel::App)
#define	sqConfig			(Squirrel::App->kconf)
#define	sqLibUpdater		(Squirrel::App->libl)
#define	sqLoader			(Squirrel::App->iconL)
#define	sqBookmarks		(Squirrel::App->bookmarkOwner)
#define	sqHighLevel		(Squirrel::App->highlevel)
#define	sqTray			(Squirrel::App->tray)

#define	sqStatus			(Squirrel::App->sbar)
#define	sqWStack		(Squirrel::App->pWidgetStack)
#define	sqCurrentURL		(Squirrel::App->pCurrentURL)
#define	sqExternalTool	(Squirrel::App->extool)
#define	sqTree			(Squirrel::App->ptree)

#define	sqSBdirInfo		(Squirrel::App->dirInfo)
#define   sqSBfileIcon		(Squirrel::App->fileIcon)
#define   sqSBfileName		(Squirrel::App->fileName)
#define   sqSBDecoded		(Squirrel::App->decodedStatus)
#define	sqSBGLreport		(Squirrel::App->GLreporter)

#define	sqGLView		(Squirrel::App->gl)
#define	sqGLWidget		(Squirrel::App->gl->gl)
#define	sqQuickBrowser	(Squirrel::App->gl->gl->v)
#define	sqQuickOperator	(sqQuickBrowser->quick)
#define	sqLibHandler		(Squirrel::App->sqLibHandlerReal)
#define	sqViewType		(Squirrel::App->curViewType)

#define	sqFiltersName	(Squirrel::App->filters_name)
#define	sqFiltersExt		(Squirrel::App->filters_ext)

#define	sqThumbSize		(Squirrel::App->thumbSize)
#define	sqCache			(Squirrel::App->cache)

#endif
