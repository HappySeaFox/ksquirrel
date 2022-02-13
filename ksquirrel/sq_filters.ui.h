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

    QListViewItem *itemafter = 0L, *item;

    QValueList<QString>::iterator nEND = KSquirrel::app()->filtersNames()->end();
    QValueList<QString>::iterator it_name = KSquirrel::app()->filtersNames()->begin();
    QValueList<QString>::iterator it_ext = KSquirrel::app()->filtersExtensions()->begin();

    for(;it_name != nEND;++it_name,++it_ext)
    {
        if(itemafter)
            item = new QListViewItem(listFilters, itemafter, *it_name, *it_ext);
        else
            item = new QListViewItem(listFilters, *it_name, *it_ext);

        itemafter = item;

        item->setRenameEnabled(0, true);
        item->setRenameEnabled(1, true);
        item->setMultiLinesEnabled(false);
    
        listFilters->insertItem(item);
    }

    pushFilterUp->setPixmap(SQ_IconLoader::instance()->loadIcon("move_task_up", KIcon::Desktop, KIcon::SizeSmall));
    pushFilterDown->setPixmap(SQ_IconLoader::instance()->loadIcon("move_task_down", KIcon::Desktop, KIcon::SizeSmall));
    pushNew->setPixmap(SQ_IconLoader::instance()->loadIcon("filenew", KIcon::Desktop, KIcon::SizeSmall));
    pushDelete->setPixmap(SQ_IconLoader::instance()->loadIcon("editdelete", KIcon::Desktop, KIcon::SizeSmall));
    pushClearAll->setPixmap(SQ_IconLoader::instance()->loadIcon("edittrash", KIcon::Desktop, KIcon::SizeSmall));
    
    listFilters->setCurrentItem(listFilters->firstChild());
    listFilters->clearSelection();
    listFilters->setSelected(listFilters->currentItem(), true);

    SQ_Config::instance()->setGroup("Filters");
 
    checkBoth->setChecked(SQ_Config::instance()->readBoolEntry("menuitem both", true));
}

void SQ_Filters::slotNewFilter()
{
    QListViewItem *itemafter = listFilters->lastItem(), *item;

    if(itemafter)
        item = new QListViewItem(listFilters, itemafter, i18n("Name"), "*.");
    else
        item = new QListViewItem(listFilters,  i18n("Name"), "*.");

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

    item = listFilters->currentItem();
    
    if(item)
        listFilters->setSelected(item, true);
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

        KSquirrel::app()->filtersNames()->clear();
        KSquirrel::app()->filtersExtensions()->clear();

        for(;cur;cur = cur->itemBelow())
        {
            KSquirrel::app()->filtersNames()->append(cur->text(0));
            KSquirrel::app()->filtersExtensions()->append(cur->text(1));
        }

        SQ_Config::instance()->setGroup("Filters");
        SQ_Config::instance()->writeEntry("menuitem both", checkBoth->isChecked());
    }
    
    return result;
}


void SQ_Filters::slotFilterRenameRequest( QListViewItem *item, const QPoint &, int pos )
{
    if(item)
        item->startRename(((pos>=0)?pos:0));
}
