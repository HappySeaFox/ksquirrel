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

#include "sq_diroperator.h"

template <class T> class QValueVector;

class SQ_WidgetStack : public QWidgetStack
{
	Q_OBJECT

	private:
		QValueVector<int>		*pIconSizeList;
		int					iCurrentListIndex, iCurrentIconIndex;
		SQ_DirOperator		*pDirOperatorList, *pDirOperatorIcon, *pDirOperatorDetail;
		QString				*path;
		int					ncount;

	public:
    		SQ_WidgetStack(QWidget *parent = 0);
		~SQ_WidgetStack();

		KURL getURL() const;
		void setURL(const QString &, bool);
		void setURL(const KURL &, bool);

		KAction			*pABack, *pAForw, *pAUp, *pADelete, *pAHome, *pAProp,
						*pARefresh, *pANewDir, *pAPrev, *pANext,
						*pAIconBigger, *pAIconSmaller;

		const KFileItemList* selectedItems() const;

		void emitNextSelected();
		void emitPreviousSelected();
		void reInitToolsMenu();
		int count() const;

	public slots:
		void raiseWidget(int id);
		void raiseFirst(int id);

		void setURL(const QString &);
		void setURL(const KURL &);
		void setURLfromtree(const KURL&);

		void slotSetIconBigger();
		void slotSetIconSmaller();

		void slotPrevious();
		void slotNext();

		void slotShowHidden(bool);

		void setNameFilter(const QString&);
		const QString getNameFilter() const;
};

#endif
