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

#include <kurl.h>
#include <kcombobox.h>
#include <kprocess.h>
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

class QSplitter;
class QLabel;
class QWidgetStack;

class SQ_WidgetStack;
class SQ_RunProcess;
class SQ_SystemTray;
class SQ_GLViewWidget;
class SQ_GLBase;
class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_FileviewFilter;
class SQ_Config;
class SQ_ExternalTool;
class SQ_TreeView;
class SQ_CacheHandler;
class SQ_BookmarkOwner;
class SQ_HLOptions;

template <class T> class QValueVector;
template <class T> class QPtrList;

class Squirrel : public KDockMainWindow, public DCOPObject
{
	Q_OBJECT

	public:
		static Squirrel *App;

		enum ViewType	{SQuirrel=0,Gqview,Kuickshow,WinViewer,Xnview,ShowImg,Browser};
		ViewType			curViewType;

	private:
		QValueVector<int>		*iconSizeList;
		KMenuBar			*menubar;
		KToolBar				*fileTools, *pTLocation;
		QPopupMenu			*pmLaunch;
		QPixmap				 fullIcon, unfullIcon;
		KDockWidget 			*mainDock;
		KAction				*pAGLView, *pAConfigure, *pAExit, *pANextFile, *pAPrevFile, *pARescan, *pAExtTools, *pAFilters, *pAGotoTray;
		KRadioAction			*pARaiseListView, *pARaiseIconView, *pARaiseDetailView;
		QSplitter				*mainSplitter;
		KPopupMenu			*pop_file, *pop_view, *pop_edit;
		KPopupMenu			*actionFilterMenu;
		KActionMenu 			*bookmarks;
	 	int	 				toolbarIconSize, createFirst;
		QStringList			strlibFound;

		SQ_RunProcess		*pMenuProc;
		QWidgetStack			*viewBrowser;

		void CreateLocationToolbar();
		void CreateStatusBar(KStatusBar*);
		void CreateToolbar(KToolBar*);
		void CreateMenu(KMenuBar*);
		void CreateActions();

		// init special QValueVector lists from config file.
		void InitFilterMenu();
		void InitRunMenu();
		void InitExternalTools();
		void InitBookmarks();

		void createWidgetsLikeSQuirrel();
		void createWidgetsLikeGqview();
		void createWidgetsLikeKuickshow();
		void createWidgetsLikeWinViewer();
		void createWidgetsLikeXnview();
		void createWidgetsLikeShowImg();
		void createWidgetsLikeBrowser();

		void handlePositionSize();
		void setSplitterSizes(int s1, int s2);

		int findProtocol(const QString &proc);

	public slots:
		void slotOptions();
		void slotFilters();
		void slotExtTools();
		void slotExecuteRunMenu();
		void slotRaiseListView();
		void slotRaiseIconView();
		void slotRaiseDetailView();
		void slotGo();
		void slotSetFilter(int);
		void slotGLView();
		void slotNextFile();
		void slotPreviousFile();
		void slotRunCommand(int);
		void slotGotoTray();
		void slotSetFile();
		void slotCloseGLWidget();

	public:
		Squirrel(SQ_HLOptions *, QWidget *parent = 0, const char *name = 0);
		~Squirrel();

		SQ_Config			*kconf;
		KIconLoader			*iconL;
		SQ_WidgetStack		*pWidgetStack;
		KHistoryCombo		*pCurrentURL;
		SQ_GLBase			*glBase;
		SQ_GLViewWidget		*glView;
		SQ_FileviewFilter		*filterList;
		SQ_LibraryHandler		*sqLibHandlerReal;
		KStatusBar			*sbar;
		SQ_LibraryListener		*libl;
		SQ_SystemTray		*tray;

		QLabel				*dirInfo, *curFileInfo, *fileIcon, *fileName, *decodedStatus, *GLreporter;
		SQ_ExternalTool		*extool;
		SQ_TreeView			*ptree;
		SQ_BookmarkOwner 	*bookmarkOwner;
		SQ_CacheHandler		*hcache;
		SQ_HLOptions			*highlevel;

		void raiseGLWidget();

	public slots:
		QCStringList functions();
		bool process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData);
		void control(const QString &str);

	protected:
		void closeEvent(QCloseEvent *e);
};

#define	sqApp			(Squirrel::App)
#define	sqConfig			(Squirrel::App->kconf)
#define	sqLibUpdater		(Squirrel::App->libl)
#define   sqLoader			(Squirrel::App->iconL)
#define   sqCache			(Squirrel::App->hcache)
#define	sqBookmarks		(Squirrel::App->bookmarkOwner)
#define	sqHighLevel		(Squirrel::App->highlevel)
#define	sqTray			(Squirrel::App->tray)

#define   sqStatus			(Squirrel::App->sbar)
#define	sqFilters			(Squirrel::App->filterList)
#define   sqProgress		(Squirrel::App->progress)
#define   sqWStack			(Squirrel::App->pWidgetStack)
#define   sqCurrentURL		(Squirrel::App->pCurrentURL)
#define	sqExternalTool		(Squirrel::App->extool)
#define	sqTree			(Squirrel::App->ptree)

#define   sqSBdirInfo		(Squirrel::App->dirInfo)
#define   sqSBcurFileInfo	(Squirrel::App->curFileInfo)
#define   sqSBfileIcon		(Squirrel::App->fileIcon)
#define   sqSBfileName		(Squirrel::App->fileName)
#define   sqSBDecoded		(Squirrel::App->decodedStatus)
#define	sqSBGLreport		(Squirrel::App->GLreporter)
#define	sqGLView			(Squirrel::App->glBase)
#define	sqLibHandler		(Squirrel::App->sqLibHandlerReal)
#define	sqViewType		(Squirrel::App->curViewType)

#endif
