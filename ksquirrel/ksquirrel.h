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

class SQ_WidgetStack;
class SQ_RunProcess;
class SQ_SystemTray;
class SQ_GLViewWidget;
class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_FileviewFilter;
class SQ_Config;
class SQ_ExternalTool;
class SQ_TreeView;
class SQ_CacheHandler;
class SQ_BookmarkOwner;

template <class T> class QValueVector;
template <class T> class QPtrList;

class Squirrel : public KDockMainWindow, public DCOPObject
{
	Q_OBJECT

	public:
		static Squirrel *App;

		enum ViewType	{SQuirrel=0,Gqview,Kuickshow,WinViewer,Xnview,Browser};
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
		SQ_SystemTray		*tray;

		void CreateLocationToolbar();
		void CreateStatusBar();
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
		void createWidgetsLikeBrowser();

		void handlePositionSize();

	public slots:
		void slotOptions();
		void slotFilters();
		void slotExtTools();
		void slotExecuteRunMenu();
		void slotRaiseListView();
		void slotRaiseIconView();
		void slotRaiseDetailView();
		void slotClearAdress();
		void slotGo();
		void slotSetFilter(int);
		void slotGLView();
		void slotNextFile();
		void slotPreviousFile();
		void slotRunCommand(int);
		void slotGotoTray();

	public:
		Squirrel(QWidget *parent = 0, const char *name = 0);
		~Squirrel();

		SQ_Config			*kconf;
		KIconLoader			*iconL;
		SQ_WidgetStack		*pWidgetStack;
		KHistoryCombo		*pCurrentURL;
		SQ_GLViewWidget		*glView;
		SQ_FileviewFilter		*filterList;
		SQ_LibraryHandler		*sqLibHandlerReal;
		KStatusBar			*sbar;
		SQ_LibraryListener		*libl;

		QLabel				*dirInfo, *curFileInfo, *fileIcon, *fileName, *decodedStatus, *GLreporter;
		QString				libPrefix;
		SQ_ExternalTool		*extool;
		SQ_TreeView			*ptree;
		SQ_BookmarkOwner 	*bookmarkOwner;
		SQ_CacheHandler		*hcache;

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

#define	sqGLView			(Squirrel::App->glView)
#define	sqLibHandler		(Squirrel::App->sqLibHandlerReal)
#define	sqLibPrefix		(Squirrel::App->libPrefix)

#define	sqViewType		(Squirrel::App->curViewType)
#endif
