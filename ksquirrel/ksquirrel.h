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

#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qtabwidget.h>
#include <qdockwindow.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>
#include <qstringlist.h>

#include <ktoolbar.h>
#include <kstatusbar.h>
#include <ksystemtray.h>
#include <kiconloader.h>
#include <kurl.h>
#include <kdockwidget.h>
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kcombobox.h>
#include <kaction.h>
#include <kprocess.h>
#include <dcopobject.h>

class SQ_WidgetStack;
class SQ_RunProcess;
class SQ_SystemTray;
class SQ_GLViewWidget;
class SQ_Bookmarks;
class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_FileviewFilter;

class KConfig;

template <class T> class QValueVector;

class Squirrel : public KDockMainWindow, public DCOPObject
{
	Q_OBJECT

	public:
		static Squirrel *App;

		enum ViewType{SQuirrel=0,Gqview,Kuickshow,WinViewer,Xnview,Browser};
		ViewType			curViewType;

	private:
		QValueVector<int>		*iconSizeList;
		SQ_SystemTray		*tray;
		QToolButton			*pTBAbstractButton, *pTBBookmarksButton;
		KMenuBar			*menubar;
		KToolBar				*fileTools, *pTLocation;
		QPopupMenu			*pmLaunch;
		QPixmap				 fullIcon, unfullIcon;

		KDockWidget 			*mainDock;

		KAction				*pAGLView, *pAConfigure, *pAExit, *pANextFile, *pAPrevFile, *pARescan, *pAExtTools;
		KRadioAction			*pARaiseListView, *pARaiseIconView, *pARaiseDetailView;

		QSplitter				*mainSplitter;

		SQ_RunProcess		*pMenuProc;

		KPopupMenu			*pop_file, *pop_view, *pop_edit;
		KPopupMenu			*actionFilterMenu;
	 	int	 				toolbarIconSize, createFirst;
		QStringList			strlibFound;

		void InitRunMenu();
		void CreateLocationToolbar();
		void CreateStatusBar();
		void CreateToolbar(KToolBar*);
		void CreateMenu(KMenuBar*);
		void CreateActions();
		void InitFilterMenu();

		void createWidgetsLikeSQuirrel();
		void createWidgetsLikeGqview();
		void createWidgetsLikeKuickshow();
		void createWidgetsLikeWinViewer();
		void createWidgetsLikeXnview();
		void createWidgetsLikeBrowser();

	public slots:
		void slotOptions();
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

		void slotDoNothing() {}

	public:
		Squirrel(QWidget *parent = 0, const char *name = 0);
		~Squirrel();

		KConfig				*kconf;
		KIconLoader			*iconL;
		SQ_WidgetStack		*pWidgetStack;
		KHistoryCombo		*pCurrentURL;
		SQ_GLViewWidget		*glView;
		SQ_FileviewFilter		*filterList;
		SQ_LibraryHandler		*sqLibHandlerReal;
		KStatusBar			*sbar;
		SQ_LibraryListener		*libl;

		QLabel				*dirInfo, *curFileInfo, *fileIcon, *fileName, *decodedStatus;
		QString				libPrefix;
		QColor				GLBkGroundColor;

	public slots:
		QCStringList functions();
		bool process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData);
		void control(const QString &str);

	protected:
		void closeEvent(QCloseEvent *e);
};

#define	sqApp			(Squirrel::App)
#define	sqFilters			(Squirrel::App->filterList)
#define	sqConfig			(Squirrel::App->kconf)
#define	sqLibUpdater		(Squirrel::App->libl)
#define   sqLoader			(Squirrel::App->iconL)
#define   sqStatus			(Squirrel::App->sbar)
#define   sqProgress		(Squirrel::App->progress)
#define   sqBookmarks		(Squirrel::App->bookmarks)
#define   sqWStack			(Squirrel::App->pWidgetStack)
#define   sqCurrentURL		(Squirrel::App->pCurrentURL)

#define   sqSBdirInfo		(Squirrel::App->dirInfo)
#define   sqSBcurFileInfo	(Squirrel::App->curFileInfo)
#define   sqSBfileIcon		(Squirrel::App->fileIcon)
#define   sqSBfileName		(Squirrel::App->fileName)
#define   sqSBDecoded		(Squirrel::App->decodedStatus)

#define	sqGLView			(Squirrel::App->glView)
#define	sqGLViewBGColor	(Squirrel::App->GLBkGroundColor)

#define	sqLibHandler		(Squirrel::App->sqLibHandlerReal)
#define	sqLibPrefix		(Squirrel::App->libPrefix)

#define	sqViewType		(Squirrel::App->curViewType)
#endif
