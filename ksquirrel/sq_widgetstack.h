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
#include <qvaluelist.h>

#include "sq_diroperator.h"
#include "ksquirrel.h"

class SQ_WidgetStack : public QWidgetStack
{
	Q_OBJECT

	private:
		QValueList<int>		*pIconSizeList;
		int				iCurrentListIndex, iCurrentIconIndex;
		SQ_DirOperator		*pDirOperatorList, *pDirOperatorIcon, *pDirOperatorDetail;

	public:
    		SQ_WidgetStack(QWidget *parent = 0);
		~SQ_WidgetStack();

		KURL getURL() const;
		void setURL(const QString &, bool);
		void setURL(const KURL &, bool);

		void setURLfromtree(const KURL&);

		KAction			*pABack, *pAForw, *pAUp, *pADelete, *pAHome, *pAProp,
						*pARefresh, *pANewDir, *pAPrev, *pANext,
						*pAIconBigger, *pAIconSmaller;


	public slots:
		void raiseWidget(int id);

		void setURL(const QString &);
		void setURL(const KURL &);

		void slotSetIconBigger();
		void slotSetIconSmaller();

		void slotPrevious();
		void slotNext();

		void slotShowHidden(bool);

		void setNameFilter(const QString&);
};

#endif
