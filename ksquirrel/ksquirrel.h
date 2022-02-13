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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qstringlist.h>
#include <qmap.h>

#include <kmainwindow.h>
#include <kio/job.h>
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
class SQ_ThumbnailSize;
class SQ_PixmapCache;
class SQ_LocationToolbar;
class SQ_GLView;
class SQ_GLWidget;
class SQ_ArchiveHandler;
class SQ_Dir;

class KSquirrel : public KMainWindow, public DCOPObject
{
	Q_OBJECT

	public:
		KSquirrel(QWidget *parent = 0, const char *name = 0);
		~KSquirrel();
		
		void finalActions();
		void enableThumbsMenu(bool);
		void setCaption(const QString &cap);

		KPopupMenu* menuFilters();
		KPopupMenu* menuViews();
		QLabel*		sbarWidget(const QString &name);

		// dcop methods
	        QCStringList functions();
	        bool process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData);

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

		void fillMessages();

		// dcop method
	        void control(const QString &str);

	signals:
		void thumbSizeChanged(const QString&);
		void continueLoading();

	public slots:
		void slotCloseGLWidget();
		void raiseGLWidget();

	private slots:
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
		void slotFullScreen(bool full);
		void slotThumbsSmall();
		void slotThumbsMedium();
		void slotThumbsLarge();
		void slotThumbsHuge();
		void slotContinueLoading();
		void slotSetTreeShown(bool shown);
		void slotSeparateGL(bool);
		void slotCheckVersion();
		void slotOpenFile();
		void slotOpenFileAndSet();
		void slotAnimatedClicked();
		void slotUDSEntries(KIO::Job*, const KIO::UDSEntryList&);
		void listResult(KIO::Job *);
		void slotContinueLoading2();
		void slotShowImageEditActions();
		void slotTCMaster();
		QString slotRescan();

	public:
		static KSquirrel 			*sing;
		static KIconLoader		*iconL;

		static KIconLoader* 		loader();
		static KSquirrel* 		app();

		KHistoryCombo			*pCurrentURL;
		QStringList				*sqFiltersName, *sqFiltersExt;
		KAction					*pAImageConvert, *pAImageResize, *pAImageBCG, *pAImageRotate,
								*pAImageToolbar, *pAImageFilter, *pAPrintImages, *pAConfigure,
								*pACheck;

	private:
		KToolBar				*tools;
		KMenuBar				*menubar;
		KRadioAction			*pARaiseListView, *pARaiseIconView, *pARaiseDetailView, *pARaiseThumbView;
		KActionMenu			*pAThumbs;
		KRadioAction			*pAThumb0, *pAThumb1, *pAThumb2, *pAThumb3;
		KToggleAction			*pATree, *pAURL, *pASeparateGL;
		KPopupMenu			*pop_file, *pop_view, *pop_edit;
		KPopupMenu			*actionFilterMenu, *actionViews;
		KActionMenu 			*bookmarks;
		KBookmarkMenu			*bookmarkMenu;
		KIO::ListJob				*job;
		KAction					*pAOpen, *pAOpenAndSet, *pATCMaster, *pAGLView,
								*pAExit, *pARescan, *pAExtTools, *pAFilters, *pAGotoTray;
		KStatusBar				*sbar;

		QSplitter				*mainSplitter;
		QStringList				strlibFound;
		QWidgetStack			*viewBrowser;
		QVBox					*mainBox, *b2;
		QValueList<int>			mainSizes;
		QStringList				libFilters;
		QMap<QString, QLabel*>sbarwidgets;
		QLabel					*dirInfo, *fileIcon, *fileName;
		QString					new_version;
		QMap<QString, int>		messages;

		SQ_LocationToolbar		*pTLocation;
		SQ_Dir					*dir;
		SQ_Config				*kconf;
		SQ_WidgetStack			*pWidgetStack;
		SQ_LibraryHandler		*libhandler;
		SQ_LibraryListener		*libl;
		SQ_SystemTray			*tray;
		SQ_ExternalTool			*extool;
		SQ_TreeView			*ptree;
		SQ_GLView				*gl_view;
		SQ_ArchiveHandler		*ar;
		SQ_ThumbnailSize		*thumbSize;
		SQ_PixmapCache		*cache;

		int						old_id;
		bool 					first_time, hastree, old_disable, old_ext,
								m_urlbox, m_sep, old_marks;
};

#endif
