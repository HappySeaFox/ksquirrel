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

#ifndef SPLITTER_MAIN_WIDGET
#define SPLITTER_MAIN_WIDGET

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
#include <kconfig.h>
#include <kiconloader.h>
#include <kurl.h>
#include <kdockwidget.h>
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kcombobox.h>
#include <kaction.h>
#include <kprocess.h>

class SQ_Splash;
class SQ_WidgetStack;
class SQ_RunProcess;
class SQ_SystemTray;
class SQ_GLViewWidget;
class SQ_Bookmarks;
class SQ_SquirrelOptions;
class SQ_LibraryListener;
class SQ_LibraryHandler;

template <class T> class QValueVector;

typedef struct
{
	QString name;
	QString filter;
}FILTER;


class Squirrel : public KDockMainWindow
{
    Q_OBJECT

    public:
		enum ViewType{SQuirrel=0,Gqview,Kuickshow,WinViewer,Xnview};
		ViewType			curViewType;

    private:
		QValueVector<FILTER>*filterList;
		QValueVector<int>		*iconSizeList;
		SQ_SystemTray		*tray;
		QToolButton			*pTBAbstractButton, *pTBBookmarksButton;
		KMenuBar			*menubar;
		KToolBar				*fileTools, *pTLocation;
		QPopupMenu			*pmLaunch;
		QPixmap				 fullIcon, unfullIcon;

		SQ_Splash			*splash;

		KDockWidget 			*mainDock;

		KAction				*pAGLView, *pAConfigure, *pAExit, *pAConvert, *pANextFile, *pAPrevFile, *pARescan;
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

    public slots:
		void slotConvert();
		void slotOptions();
		void slotExecuteRunMenu();
		void slotSplashClose();

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

    protected:
		void closeEvent(QCloseEvent *e);

    public:
		static Squirrel *App;

		KConfig				*kconf;
		KIconLoader			*iconL;
		SQ_WidgetStack		*pWidgetStack;
		KHistoryCombo		*pCurrentURL;
		SQ_GLViewWidget		*glView;

		SQ_LibraryHandler		*sqLibHandlerReal;

		KStatusBar			*sbar;
		QLabel				*dirInfo, *curFileInfo, *fileIcon, *fileName, *decodedStatus;
		QString				libPrefix;
		SQ_SquirrelOptions	*options;
		SQ_LibraryListener		*libl;

		QColor				GLBkGroundColor;
};

#define	sqApp			(Squirrel::App)
#define	sqOptions		(Squirrel::App->options)
#define	sqConfig			(Squirrel::App->kconf)
#define	sqLibUpdater		(Squirrel::App->libl)
#define   sqLoader			(Squirrel::App->iconL)
#define   sqStatus			(Squirrel::App->sbar)
#define   sqProgress		(Squirrel::App->progress)
#define   sqBookmarks		(Squirrel::App->bookmarks)
#define   sqWStack		(Squirrel::App->pWidgetStack)
#define   sqCurrentURL		(Squirrel::App->pCurrentURL)

#define   sqSBdirInfo		(Squirrel::App->dirInfo)
#define   sqSBcurFileInfo	(Squirrel::App->curFileInfo)
#define   sqSBfileIcon		(Squirrel::App->fileIcon)
#define   sqSBfileName		(Squirrel::App->fileName)
#define   sqSBDecoded		(Squirrel::App->decodedStatus)

#define	sqGLView		(Squirrel::App->glView)
#define	sqGLViewBGColor	(Squirrel::App->GLBkGroundColor)

#define	sqLibHandler		(Squirrel::App->sqLibHandlerReal)
#define	sqLibPrefix		(Squirrel::App->libPrefix)

#define	sqViewType		(Squirrel::App->curViewType)
#endif
