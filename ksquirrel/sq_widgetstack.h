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

		enum Direction { Next = 0, Previous = 1 };
		enum FileAction { Copy = 0, Cut, Paste, Unknown };
		enum moveToError { moveSuccess = 0, moveFailed } ;

		KURL getURL() const;
		void setURL(const QString &, bool, bool = true);
		void setURL(const KURL &, bool, bool = true);

		const KFileItemList* selectedItems() const;
		const KFileItemList* items() const;

		void configureClickPolicy();
		int count() const;
		void selectFile(KFileItem *item, SQ_DirOperatorBase *workAround = 0L);
		void updateGrid(bool arrange);
		void updateView();
		SQ_DirOperator* visibleWidget() const;
		SQ_DirOperator* widget(int id) const;

		static SQ_WidgetStack* instance();

	private:
		void setupDirOperator(SQ_DirOperator *op, const QString &filter);
		bool copyOrCut();

	public slots:
		void raiseWidget(int id);
		void raiseFirst(int id);
		void tryUnpack(KFileItem *);
		void setURLfromtree(const KURL&);
		void slotFirstFile();
		void slotLastFile();
		void emitNextSelected();
		void emitPreviousSelected();
		void slotShowHidden(bool);
		void setNameFilter(const QString&);
		void thumbnailsUpdateEnded();
		void thumbnailUpdateStart(int);
		void thumbnailProcess();
		void setURLForCurrent(const QString &);
		void setURLForCurrent(const KURL &);
		void slotRunSeparately();
		void slotDelayedShowProgress();

		int moveTo(Direction direction, KFileItem *it = 0L);

		const QString getNameFilter() const;

		void slotFileCopy();
		void slotFileCut();
		void slotFilePaste();
		void slotFileCopyTo();
		void slotFileMoveTo();
		void slotFileLinkTo();

	private slots:
		void slotDelayedSetExtractURL();
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

	public:
		KAction				*pABack, *pAForw, *pAUp, *pADelete, *pAHome, *pAProp, *pARefresh, *pAMkDir;
		KToggleAction		*pAHidden;

	private:
		SQ_DirOperator		*pDirOperatorList, *pDirOperatorIcon, *pDirOperatorDetail, *pDirOperatorThumb;
		QString				*path;
		int					ncount;
		KActionCollection	*ac;
		QTimer 				*timerShowProgress;
		KURL::List			files;
		FileAction			fileaction;

		static SQ_WidgetStack	*inst;
};

#endif
