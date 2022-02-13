/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void SQ_Filters::init()
{
     listFilters->setSorting(-1);

    	int count = sqFilters->count();
	QListViewItem *itemafter = 0L, *item;

	for(int i = 0;i < count;i++)
	{
		if(itemafter)
		    item = new QListViewItem(listFilters, itemafter, sqFilters->getFilterName(i), sqFilters->getFilterExt(i));
		else
		item = new QListViewItem(listFilters, sqFilters->getFilterName(i), sqFilters->getFilterExt(i));

		itemafter = item;

	    item->setRenameEnabled(0, true);
	    item->setRenameEnabled(1, true);
	    item->setMultiLinesEnabled(false);
	    
	    listFilters->insertItem(item);
	}

	pushFilterUp->setPixmap(sqLoader->loadIcon("up", KIcon::Desktop, KIcon::SizeSmall));
	pushFilterDown->setPixmap(sqLoader->loadIcon("down", KIcon::Desktop, KIcon::SizeSmall));
	
	listFilters->setCurrentItem(listFilters->firstChild());
	listFilters->clearSelection();
	listFilters->setSelected(listFilters->currentItem(), true);
   
}

void SQ_Filters::slotNewFilter()
{
	QListViewItem *itemafter = listFilters->lastItem(), *item;

	if(itemafter)
		item = new QListViewItem(listFilters, itemafter, "Name", "*.");
	else
		item = new QListViewItem(listFilters,  "Name", "*.");

	item->setRenameEnabled(0, true);
	item->setRenameEnabled(1, true);
	item->setMultiLinesEnabled(false);
	listFilters->insertItem(item);
	item->startRename(0);

}

void SQ_Filters::slotFilterClear()
{
	QListViewItem *item = listFilters->currentItem();
    
	if(!item) return;
    
	listFilters->takeItem(item);
}

void SQ_Filters::slotFilterClearAll()
{

}

void SQ_Filters::slotFilterUp()
{
    QListViewItem *item = listFilters->currentItem();
    
    if(!item) return;
 
    QListViewItem *itemafter = item->itemAbove();
    
    if(!itemafter) return;
 
    itemafter->moveItem(item);

}

void SQ_Filters::slotFilterDown()
{
    QListViewItem *item = listFilters->currentItem();
    
    if(!item) return;
 
    QListViewItem *itemafter = item->itemBelow();
    
    if(!itemafter) return;
 
    item->moveItem(itemafter);

}

int SQ_Filters::start()
{
     int result = exec();
    
    if(result == QDialog::Accepted)
    {
	QListViewItem *cur = listFilters->firstChild();
	sqFilters->clear();

	for(;cur;cur = cur->itemBelow())
	{
		FILTER tf = {cur->text(0), cur->text(1)};
		sqFilters->addFilter(tf);
	}
    }
    
    return result;
}


void SQ_Filters::slotFilterRenameRequest( QListViewItem *item, const QPoint &point, int pos )
{
    if(item)
	item->startRename(((pos>=0)?pos:0));
}
