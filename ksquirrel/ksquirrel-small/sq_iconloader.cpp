/***************************************************************************
                          sq_iconloader.cpp  -  description
                             -------------------
    begin                : ??? Aug 20 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#include "sq_iconloader.h"
#include "sq_iconloader_pixmaps.h"

SQ_IconLoader* SQ_IconLoader::sing = NULL;

SQ_IconLoader::SQ_IconLoader() : KIconLoader("ksquirrel")
{
	sing = this;

	fillPixmaps();
}

SQ_IconLoader::~SQ_IconLoader()
{}

QPixmap SQ_IconLoader::loadIcon(const QString& name, KIcon::Group group, int size) const
{
	// try to load from installed icon theme
	QPixmap p = KIconLoader::loadIcon(name, group, size, KIcon::DefaultState, NULL, true);

	// requested pixmap not found, let's'
	// try to find it in our pixmaps
	if(p.isNull())
	{
		if(name == "move_task_up")
			p = pixmap_up;
		else if(name == "move_task_down")
			p = pixmap_down;
		else if(name == "display" && size == 32)
			p = pixmap_display;
		else if(name == "folder" && size == 32)
			p = pixmap_folder;
		else if(name == "images" && size == 32)
			p = pixmap_images;
		else if(name == "binary" && size == 32)
			p = pixmap_binary;
		else if(name == "edit" && size == 32)
			p = pixmap_edit;
		else
			p = KIconLoader::loadIcon("unknown", group, size);
	}

	return p;
}

/*
 *  Internal. Load all pixmaps.
 */
void SQ_IconLoader::fillPixmaps()
{
	pixmap_up      = QPixmap(xpm_up);
	pixmap_down    = QPixmap(xpm_down);
	pixmap_display = QPixmap(xpm_display);
	pixmap_folder  = QPixmap(xpm_folder);
	pixmap_images  = QPixmap(xpm_images);
	pixmap_binary  = QPixmap(xpm_binary);
	pixmap_edit    = QPixmap(xpm_edit);
}

SQ_IconLoader* SQ_IconLoader::instance()
{
	return sing;
}
