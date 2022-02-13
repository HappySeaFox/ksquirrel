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
	textTotal->setText(QString(" %1 ").arg((int)sqCache->count()));
	
	if(sqCache->isEmpty())
		return;

	QPixmap pix;

	SQ_PixmapCache::cache_iterator BEGIN = sqCache->begin();
	SQ_PixmapCache::cache_iterator END = sqCache->end();
    
	listCache->setSorting(-1);

	QListViewItem *itemafter = 0L, *item;
   
	for(SQ_PixmapCache::cache_iterator it = BEGIN;it != END;it++)
	{
		if(itemafter)
			item = new QListViewItem(listCache, itemafter, it.key(), "", it.data().info.dimensions + "x" + it.data().info.bpp);
		else
			item = new QListViewItem(listCache, it.key(), "", it.data().info.dimensions + "x" + it.data().info.bpp);
	
		if(pix.convertFromImage(it.data().info.mime))
		    item->setPixmap(1, pix);

		itemafter = item;

		listCache->insertItem(item);
	}
}
