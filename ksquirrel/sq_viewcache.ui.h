/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_ViewCache::init()
{
    textTotal->setText(QString("<b>%1</b>").arg((int)SQ_PixmapCache::instance()->count()));

    if(SQ_PixmapCache::instance()->isEmpty())
        return;

    listCache->setSorting(-1);

    QListViewItem *itemafter = 0, *item;

    SQ_PixmapCache::iterator itEnd = SQ_PixmapCache::instance()->end();
    SQ_Thumbnail th;
    KURL url;
    QString s;

    for(SQ_PixmapCache::iterator it = SQ_PixmapCache::instance()->begin();it != itEnd;++it)
    {
        th = it.data();
        url = it.key();
        s = url.isLocalFile() ? url.path() : url.prettyURL();

        if(itemafter)
            item = new QListViewItem(listCache, itemafter, s,
                            QString::null, QString::fromLatin1("%1x%2").arg(th.w).arg(th.h));
        else
            item = new QListViewItem(listCache, s,
                            QString::null, QString::fromLatin1("%1x%2").arg(th.w).arg(th.h));

        item->setPixmap(1, it.data().mime);

        itemafter = item;

        listCache->insertItem(item);
    }
}
