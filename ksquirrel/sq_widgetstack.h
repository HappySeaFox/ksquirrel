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

	private:
		void setupDirOperator(SQ_DirOperator *op, const QString &filter);

	public slots:
		void raiseWidget(int id);
		void raiseFirst(int id);
		void tryUnpack(KFileItem *);
		void setURLfromtree(const KURL&);
		void slotFirstFile();
		void slotLastFile();
		bool moveTo(Direction direction, KFileItem *it = 0L);
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

		const QString getNameFilter() const;

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
		KAction				*pABack, *pAForw, *pAUp, *pADelete, *pAHome, *pAProp, *pARefresh, *pAMkDir, *pARecreate;

	private:
		SQ_DirOperator	*pDirOperatorList, *pDirOperatorIcon, *pDirOperatorDetail, *pDirOperatorThumb;
		QString			*path;
		int					ncount;
		KActionCollection	*ac;
		QTimer 			*timerShowProgress;
};

#endif
