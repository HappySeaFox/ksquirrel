/***************************************************************************
                          sq_kipiinterface.cpp  -  description
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

#include "sq_kipiinterface.h"

#ifdef SQ_HAVE_KIPI

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kfileitem.h>
#include <klocale.h>
#include <kurl.h>
#include <kfileview.h>
#include <kfileiconview.h>
#include <kdirlister.h>

#include <libkipi/imagecollectionshared.h>
#include <libkipi/imageinfoshared.h>

#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_imagebasket.h"

class SQ_KIPIImageCollection : public KIPI::ImageCollectionShared
{
    public:
	SQ_KIPIImageCollection(const QString& name, const KURL::List& images)
	    : KIPI::ImageCollectionShared(), mName(name), mImages(images)
        {}

        QString name()
        {
            return mName;
        }

        QString comment()
        {
            return QString::null;
        }

	KURL::List images()
        {
            return mImages;
        }

	// FIXME: Return current URL instead
	KURL uploadPath()
        {
            KURL url;
            url.setPath("/");
            return url;
	}

        QString uploadRootName()
        {
            return i18n("Root directory");
        }

    private:
        QString mName;
        KURL::List mImages;
};

/***************************************************************/

class SQ_KIPIImageInfo : public KIPI::ImageInfoShared
{
    public:
        SQ_KIPIImageInfo(KIPI::Interface *interface, const KURL &url)
            : KIPI::ImageInfoShared(interface, url)
        {}

        QString title()
        {
            return _url.fileName();
	}
	
        QString description()
        {
            return QString::null;
        }

        void setDescription(const QString&)
        {}

        QMap<QString,QVariant> attributes()
        {
            return QMap<QString,QVariant>();
        }

        void clearAttributes()
        {}

        void addAttributes(const QMap<QString, QVariant>&)
        {}
};


/***************************************************************/

SQ_KIPIInterface::SQ_KIPIInterface(QWidget *parent)
 : KIPI::Interface(parent, "KSquirrel KIPI Interface")
{
    connect(dynamic_cast<KFileIconView *>(SQ_WidgetStack::instance()->diroperator()->view()), SIGNAL(selectionChanged()),
        this, SLOT(slotSelectionChanged()));

    connect(SQ_WidgetStack::instance()->diroperator()->dirLister(), SIGNAL(completed(const KURL&)),
        this, SLOT(slotDirectoryChanged()));
}

SQ_KIPIInterface::~SQ_KIPIInterface()
{}

KIPI::ImageCollection SQ_KIPIInterface::currentAlbum()
{
    KURL::List list;

    const KFileItemList *l = SQ_WidgetStack::instance()->diroperator()->view()->items();

    if(l)
    {
        KFileItemListIterator it(*l);

        for( ; it.current(); ++it)
            list.append(it.current()->url());
    }

    return KIPI::ImageCollection(new SQ_KIPIImageCollection(i18n("Folder content"), list)); 
}

KIPI::ImageCollection SQ_KIPIInterface::currentSelection()
{
    KURL::List list;

    const KFileItemList *l = SQ_WidgetStack::instance()->diroperator()->selectedItems();

    if(l)
    {
        KFileItemListIterator it(*l);

        for( ; it.current(); ++it)
            list.append(it.current()->url());
    }

    return KIPI::ImageCollection(new SQ_KIPIImageCollection(i18n("Selected images"), list)); 
}

KIPI::ImageCollection SQ_KIPIInterface::currentBasket()
{
    KURL::List list;

    KFileItemList l = SQ_ImageBasket::instance()->realItems();

    KFileItemListIterator it(l);

    for( ; it.current(); ++it)
        list.append(it.current()->url());

    return KIPI::ImageCollection(new SQ_KIPIImageCollection(i18n("Image basket"), list)); 
}

QValueList<KIPI::ImageCollection> SQ_KIPIInterface::allAlbums()
{
    QValueList<KIPI::ImageCollection> list;

    list << currentAlbum() << currentSelection() << currentBasket();

    return list;
}

KIPI::ImageInfo SQ_KIPIInterface::info(const KURL& url)
{
    return KIPI::ImageInfo(new SQ_KIPIImageInfo(this, url));
}

int SQ_KIPIInterface::features() const
{
    return KIPI::AcceptNewImages; 
}

void SQ_KIPIInterface::slotSelectionChanged()
{
    emit selectionChanged(SQ_WidgetStack::instance()->diroperator()->selectedItems()->count() > 0);
}

void SQ_KIPIInterface::slotDirectoryChanged()
{
    emit currentAlbumChanged(SQ_WidgetStack::instance()->diroperator()->numFiles() > 0);
}

#include "sq_kipiinterface.moc"

#endif
