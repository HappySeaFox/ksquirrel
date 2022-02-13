/***************************************************************************
                          sq_widgetstack.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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

#ifndef SQ_WIDGETSTACK_H
#define SQ_WIDGETSTACK_H

#include <qwidgetstack.h>
#include <qstring.h>

#include <kurl.h>
#include <kfileitem.h>

class	QTimer;

class	KAction;
class	KToggleAction;
class	KActionCollection;

class	SQ_DirOperatorBase;
class	SQ_DirOperator;

class SQ_WidgetStack : public QWidgetStack
{
	Q_OBJECT

	public:
		SQ_WidgetStack(QWidget *parent = 0);
		~SQ_WidgetStack();

		/*
		 *  Direction for moveTo(). 
		 */
		enum Direction { Next = 0, Previous = 1 };

		enum FileAction { Copy = 0, Cut, Paste, Unknown };

		enum moveToError { moveSuccess = 0, moveFailed } ;		

		/*
		 *  Get current url of visible diroperator.
		 */
		KURL url() const;

		/*
		 *  Set current url for all _other_ widgets and objects (bookmarks,
		 *  tree, SQ_QuickOperator etc.)
		 */
		void setURL(const QString &, bool, bool = true);
		void setURL(const KURL &, bool, bool = true);

		/*
		 *  Selected items in currently visible diroperator.
		 */
		const KFileItemList* selectedItems() const;

		/*
		 *  All items in currently visible diroperator.
		 */
		const KFileItemList* items() const;

		/*
		 *  Reconfigure clicking policy.
		 */
		void configureClickPolicy();

		/*
		 *  Items count.
		 */
		int count() const;

		/*
		 *  Set current item to 'item', select it, and synchronize with
		 *  SQ_QuickBrowser.
		 *
		 *  TODO: remove workAround?
		 */
		void selectFile(KFileItem *item, SQ_DirOperatorBase *workAround = 0L);

		/*
		 *  Update grid for thumbnail view. New grid is calcalated from item
		 *  margins (from Options).
		 */
		void updateGrid(bool arrange);

		/*
		 *  Reread current directory.
		 */
		void updateView();

		/*
		 *  Currently visible SQ_DirOperator.
		 */
		SQ_DirOperator* visibleWidget() const;

		/*
		 *  Get approrpiate SQ_DirOperator from stack.
		 */
		SQ_DirOperator* widget(int id) const;

		static SQ_WidgetStack* instance();

	private:
		/*
		 *  Setup KDirLister...
		 */
		void setupDirOperator(SQ_DirOperator *op, const QString &filter);

		/*
		 *  Save currently selected items' paths, if any.
		 *
		 *  Return true, if it was saved at least one url.
		 */
		bool prepare();

		/*
		 *  Execute SQ_SelectDeselectGroup dialog, and select or
		 *  deselect files after it has been closed.
		 */
		void selectDeselectGroup(bool select);

	public slots:

		/*
		 *  Wrapper for QWidgetStack::raiseWidget(). It will create
		 *  needed SQ_DirOperators on call (at startup time only one
		 *  SQ_DirOperator is created). Also set name filter and url.
		 */
		void raiseWidget(int id);

		/*
		 *  Raise widget for the first time. Called only once by KSquirrel,
		 *  when SQ_widgetStack is been created.
		 */
		void raiseFirst(int id);

		/*
		 *  Try to unpack an archive referenced by given KFileItem.
		 *  Used by SQ_DirOperatorBase, when user clicked on item in filemanager.
		 */
		void tryUnpack(KFileItem *);

		/*
		 *  User selected some url in file tree. Let's set it for
		 *  current diroperator and other widgets (with setURL()).
		 */
		void setURLfromtree(const KURL&);

		/*
		 *  Select first supported image in current directory.
		 *  Used by SQ_GLWidget.
		 */
		void slotFirstFile();

		/*
		 *  Select last supported image in current directory.
		 *  Used by SQ_GLWidget.
		 */
		void slotLastFile();
		void emitNextSelected();
		void emitPreviousSelected();

		/*
		 *  Show/hide hidden files.
		 */
		void slotShowHidden(bool);

		/*
		 *  Set filter.
		 */
		void setNameFilter(const QString&);

		/*
		 *  Used by SQ_FileThumbView to manipulate thumbnail progress.
		 *
		 *  TODO: move to SQ_FileThumbView ?
		 */
		void thumbnailsUpdateEnded();
		void thumbnailUpdateStart(int);
		void thumbnailProcess();
		void slotDelayedShowProgress();

		/*
		 *  Set url
		 */
		void setURLForCurrent(const QString &);
		void setURLForCurrent(const KURL &);
		void slotRunSeparately();

		/*
		 *  Select next/prevous supported image in filemanager.
		 *  Do nothing, if no more supported images found in given
		 *  direction.
		 */
		int moveTo(Direction direction, KFileItem *it = 0L);

		/*
		 *  Get current filter.
		 */
		QString nameFilter() const;

		/*
		 *  Slots for file actions: copy, move, cut...
		 */
		void slotFileCopy();
		void slotFileCut();
		void slotFilePaste();
		void slotFileCopyTo();
		void slotFileMoveTo();
		void slotFileLinkTo();

	private slots:

		/*
		 *  Go to unpacked archive.
		 */
		void slotDelayedSetExtractURL();

		/*
		 *  Wrappers for KDirOperator's file actions:
		 *  "up", "back", "home", etc.
		 */
		void slotUp();
		void slotBack();
		void slotForward();
		void slotReload();
		void slotHome();
		void slotMkDir();
		void slotProperties();
		void slotDelete();

		void slotRecreateThumbnail();
		void slotDelayedRecreateThumbnail();

		/*
		 *  User wants to select to deselect some files.
		 */
		void slotSelectGroup();
		void slotDeselectGroup();

		/*
		 *  Deselect all files.
		 */
		void slotDeselectAll();

		/*
		 *  Select all files.
		 */
		void slotSelectAll();

	public:
		KAction				*pABack, *pAForw, *pAUp, *pADelete, *pAHome, *pAProp, *pARefresh, *pAMkDir;
		KToggleAction		*pAHidden;

	private:
		SQ_DirOperator		*pDirOperatorList, *pDirOperatorIcon, *pDirOperatorDetail, *pDirOperatorThumb;
		QString				path;
		int					ncount;
		KActionCollection	*ac;
		QTimer 				*timerShowProgress;
		KURL::List			files;
		FileAction			fileaction;

		static SQ_WidgetStack	*sing;
};

#endif
