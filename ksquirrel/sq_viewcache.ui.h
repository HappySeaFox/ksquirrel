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

    for(SQ_PixmapCache::iterator it = SQ_PixmapCache::instance()->begin();it != itEnd;++it)
    {
        if(itemafter)
            item = new QListViewItem(listCache, itemafter, it.key(), "", it.data().info.dimensions + "x" + it.data().info.bpp);
        else
            item = new QListViewItem(listCache, it.key(), "", it.data().info.dimensions + "x" + it.data().info.bpp);

        item->setPixmap(1, it.data().info.mime);

        itemafter = item;

        listCache->insertItem(item);
    }
}
