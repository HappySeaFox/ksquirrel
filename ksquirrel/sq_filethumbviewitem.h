/***************************************************************************
                          sq_filethumbviewitem.h  -  description
                             -------------------
    begin                : ??? ??? 12 2004
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

#ifndef SQ_FILETHUMBVIEWITEM_H
#define SQ_FILETHUMBVIEWITEM_H

#include <kfileiconview.h>

#include "sq_thumbnailinfo.h"

/**
  *@author CKulT
  */

class SQ_FileThumbViewItem : public KFileIconViewItem
{
	public: 
		SQ_FileThumbViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi);
		~SQ_FileThumbViewItem();

		QString getFullInfo() const;
		void setInfo(const SQ_Thumbnail &t);

	protected:
		virtual void paintItem(QPainter *p, const QColorGroup &cg);
		virtual void paintFocus(QPainter *p, const QColorGroup &cg);

	private:
		SQ_Thumbnail info;
};

#endif
