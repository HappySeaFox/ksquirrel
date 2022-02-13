/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include <kiconloader.h>

void SQ_ExternalTools::init()
{
     listTools->setSorting(-1);

    	int count = sqExternalTool->count();
	QListViewItem *itemafter = 0L, *item;

	for(int i = 0;i < count;i++)
	{
		if(itemafter)
		    item = new QListViewItem(listTools, itemafter, sqExternalTool->getToolName(i), sqExternalTool->getToolCommand(i));
		else
		item = new QListViewItem(listTools, sqExternalTool->getToolName(i), sqExternalTool->getToolCommand(i));

	itemafter = item;

	    item->setRenameEnabled(0, true);
	    item->setRenameEnabled(1, true);
	    item->setMultiLinesEnabled(false);
	    
	    listTools->insertItem(item);
	}

	pushToolUp->setPixmap(sqLoader->loadIcon("up", KIcon::Desktop, KIcon::SizeSmall));
	pushToolDown->setPixmap(sqLoader->loadIcon("down", KIcon::Desktop, KIcon::SizeSmall));
	
	listTools->setCurrentItem(listTools->firstChild());
	listTools->clearSelection();
	listTools->setSelected(listTools->currentItem(), true);
   
}

void SQ_ExternalTools::slotNewTool()
{
	QListViewItem *itemafter = listTools->lastItem(), *item;

	if(itemafter)
		item = new QListViewItem(listTools, itemafter, "Tool name", "tool_executable %s");
	else
		item = new QListViewItem(listTools,  "Tool name", "tool_executable %s");

	item->setRenameEnabled(0, true);
	item->setRenameEnabled(1, true);
	item->setMultiLinesEnabled(false);
	listTools->insertItem(item);
	item->startRename(0);
}

void SQ_ExternalTools::slotToolClear()
{
	QListViewItem *item = listTools->currentItem();
    
	if(!item) return;
    
	listTools->takeItem(item);
}

void SQ_ExternalTools::slotToolClearAll()
{
listTools->clear();
}

void SQ_ExternalTools::slotToolUp()
{
    QListViewItem *item = listTools->currentItem();
    
    if(!item) return;
 
    QListViewItem *itemafter = item->itemAbove();
    
    if(!itemafter) return;
 
    itemafter->moveItem(item);

}

void SQ_ExternalTools::slotToolDown()
{
    QListViewItem *item = listTools->currentItem();
    
    if(!item) return;
 
    QListViewItem *itemafter = item->itemBelow();
    
    if(!itemafter) return;
 
    item->moveItem(itemafter);

}

int SQ_ExternalTools::start()
{
     int result = exec();
    
    if(result == QDialog::Accepted)
    {
	QListViewItem *cur = listTools->firstChild();
	sqExternalTool->clear();

	for(;cur;cur = cur->itemBelow())
	{
		SQ_EXT_TOOL tf = {cur->text(0), cur->text(1)};
		sqExternalTool->addTool(tf);
	}
    }
    
    return result;
}


void SQ_ExternalTools::slotToolRenameRequest( QListViewItem *item, const QPoint &point, int pos )
{
    if(item)
	item->startRename(((pos>=0)?pos:0));
}
