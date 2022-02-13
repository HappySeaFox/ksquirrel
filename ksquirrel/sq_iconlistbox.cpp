/***************************************************************************
                          sq_iconlistbox.cpp  -  description
                             -------------------
    begin                : ??? ??? 19 2004
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

#include "sq_iconlistbox.h"
#include "sq_iconlistitem.h"

SQ_IconListBox::SQ_IconListBox(QWidget *parent, const char *name, WFlags f) : KListBox(parent, name, f),
		mHeightValid(false), mWidthValid(false)
{
    setHScrollBarMode(QScrollView::AlwaysOff);
}

void SQ_IconListBox::updateWidth()
{
	if(!mWidthValid)
	{
		int maxWidth = 10, w;

		for(QListBoxItem *i = item(0); i != 0; i = i->next())
		{
			w = ((SQ_IconListItem *)i)->width(this);
			maxWidth = QMAX(w, maxWidth);
		}

		for(QListBoxItem *i = item(0); i != 0; i = i->next())
			((SQ_IconListItem *)i)->expandMinimumWidth(maxWidth + 30);

		if(verticalScrollBar()->isVisible())
      			maxWidth += verticalScrollBar()->sizeHint().width();

		setFixedWidth(maxWidth + frameWidth()*2 + 44);
		mWidthValid = true;
	}
}

void SQ_IconListBox::invalidateHeight()
{
	mHeightValid = false;
}

void SQ_IconListBox::invalidateWidth()
{
	mWidthValid = false;
}

void SQ_IconListBox::updateAndInstall(QObject *o)
{
	invalidateHeight();
	invalidateWidth();
	updateWidth();

	QFont listFont(font());

	listFont.setBold(true);
	setFont(listFont);
	verticalScrollBar()->installEventFilter(o);
	setCurrentItem(0);
	setSelected(0, true);
}
