/***************************************************************************
                          sq_iconloader.h  -  description
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

#ifndef SQ_ICONLOADER_H
#define SQ_ICONLOADER_H

#include <kiconloader.h>

/*
 *  SQ_IconLoader represents an icon loader. It tries to load pixmap from
 *  installed icon theme. If icon was not found, it uses internal pixmaps
 *  instead.
 */

class SQ_IconLoader : public KIconLoader
{
	public: 
		SQ_IconLoader();
		~SQ_IconLoader();

		QPixmap loadIcon(const QString& name, KIcon::Group group, int size) const;

	private:
		static SQ_IconLoader *sing;

		/*
		 *  Internal. Load all pixmaps.
		 */
		void fillPixmaps();

	public:
		static SQ_IconLoader* instance();

	private:
		QPixmap pixmap_up, pixmap_down,
			pixmap_display, pixmap_folder,
			pixmap_images, pixmap_binary,
			pixmap_edit;
};

#endif
