/***************************************************************************
                          sq_widgetstack.h  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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

#ifndef SQ_WIDGETSTACK_H
#define SQ_WIDGETSTACK_H

#include <qwidgetstack.h>
#include <qstring.h>

#include <kurl.h>
#include <kfileitem.h>

template <class T>class QValueVector;

class	KAction;

class	SQ_ArchiveHandler;
class	SQ_DirOperator;

class SQ_WidgetStack : public QWidgetStack
{
	Q_OBJECT

	public:
		SQ_WidgetStack(QWidget *parent = 0);
		~SQ_WidgetStack();

		KURL getURL() const;
		void setURL(const QString &, bool);
		void setURL(const KURL &, bool);

		const KFileItemList* selectedItems() const;

		void emitSelected(const QString &file);
		void cleanUnpacked();
		void configureClickPolicy();
		int count() const;
		void selectFile(KFileItem *item, SQ_DirOperator *workAround = 0L);
		void updateGrid();

	public slots:
		void raiseWidget(int id);
		void raiseFirst(int id);
		void tryUnpack(const QString &fullpath);
		void setURL(const QString &);
		void setURL(const KURL &);
		void setURLfromtree(const KURL&);
		void slotFirstFile();
		void slotLastFile();
		bool slotPrevious(KFileItem *it = 0L);
		bool slotNext(KFileItem *it = 0L);
		void emitNextSelected();
		void emitPreviousSelected();
		void slotShowHidden(bool);
		void setNameFilter(const QString&);
		const QString getNameFilter() const;
		void thumbnailsUpdateEnded();
		void thumbnailUpdateStart(int);
		void thumbnailProcess();
		void setURLForCurrent(const QString &);
		void setURLForCurrent(const KURL &);

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

	public:
		KAction				*pABack, *pAForw, *pAUp, *pADelete, *pAHome, *pAProp, *pARefresh, *pAMkDir;

	private:
		SQ_DirOperator		*pDirOperatorList, *pDirOperatorIcon, *pDirOperatorDetail, *pDirOperatorThumb;
		QString				*path;
		int					ncount;
		SQ_ArchiveHandler	*ar;

};

#endif
