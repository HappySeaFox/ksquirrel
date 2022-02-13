/***************************************************************************
                          sq_kipiinterface.h  -  description
                             -------------------
    begin                :  Feb 5 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#ifndef SQ_KIPIINTERFACE_H
#define SQ_KIPIINTERFACE_H	

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef SQ_HAVE_KIPI

#include <libkipi/interface.h>

class SQ_KIPIInterfacePrivate;

class SQ_KIPIInterface : public KIPI::Interface
{
    Q_OBJECT

    public:
        SQ_KIPIInterface(QWidget *parent);
        ~SQ_KIPIInterface();

        KIPI::ImageCollection currentAlbum();
        KIPI::ImageCollection currentSelection();
        KIPI::ImageCollection currentBasket();

        QValueList<KIPI::ImageCollection> allAlbums();
        KIPI::ImageInfo info(const KURL &);
        int features() const;
        bool addImage(const KURL &, QString &);

    private slots:
        void slotSelectionChanged();
        void slotDirectoryChanged();
};

inline
bool SQ_KIPIInterface::addImage(const KURL &, QString &)
{
    return true;
}

#endif

#endif
