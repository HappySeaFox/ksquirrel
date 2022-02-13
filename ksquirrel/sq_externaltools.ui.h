/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_ExternalTools::init()
{
    listTools->setSorting(-1);

    QListViewItem *itemafter = 0L, *item;

    for(QValueVector<Tool>::iterator it = SQ_ExternalTool::instance()->begin();it != SQ_ExternalTool::instance()->end();++it)
    {
        if(itemafter)
            item = new QListViewItem(listTools, itemafter, QString::null, (*it).name, (*it).command, (*it).icon);
        else
            item = new QListViewItem(listTools, QString::null, (*it).name, (*it).command, (*it).icon);

        item->setPixmap(0, SQ_IconLoader::instance()->loadIcon(item->text(3), KIcon::Desktop, 16));
        itemafter = item;

        item->setRenameEnabled(1, true);
        item->setRenameEnabled(2, true);
        item->setMultiLinesEnabled(false);
    
        listTools->insertItem(item);
    }

    pushToolUp->setPixmap(SQ_IconLoader::instance()->loadIcon("move_task_up", KIcon::Desktop, KIcon::SizeSmall));
    pushToolDown->setPixmap(SQ_IconLoader::instance()->loadIcon("move_task_down", KIcon::Desktop, KIcon::SizeSmall));
    pushNew->setPixmap(SQ_IconLoader::instance()->loadIcon("filenew", KIcon::Desktop, KIcon::SizeSmall));
    pushDelete->setPixmap(SQ_IconLoader::instance()->loadIcon("editdelete", KIcon::Desktop, KIcon::SizeSmall));
    pushClearAll->setPixmap(SQ_IconLoader::instance()->loadIcon("edittrash", KIcon::Desktop, KIcon::SizeSmall));
    pushHelp->setPixmap(SQ_IconLoader::instance()->loadIcon("help", KIcon::Desktop, KIcon::SizeSmall));

    listTools->setCurrentItem(listTools->firstChild());
    listTools->clearSelection();
    listTools->setSelected(listTools->currentItem(), true);
}

void SQ_ExternalTools::slotNewTool()
{
    QListViewItem *itemafter = listTools->lastItem(), *item;

    if(itemafter)
        item = new QListViewItem(listTools, itemafter, "", "Tool name", "tool_executable %f", "");
    else
        item = new QListViewItem(listTools,  "", "Tool name", "tool_executable %f", "");

    item->setRenameEnabled(1, true);
    item->setRenameEnabled(2, true);
    item->setMultiLinesEnabled(false);
    listTools->insertItem(item);
    item->startRename(1);
}

void SQ_ExternalTools::slotToolClear()
{
    QListViewItem *item = listTools->currentItem();
    
    if(!item) return;
    
    listTools->takeItem(item);
    
    item = listTools->currentItem();

    if(item)
        listTools->setSelected(item, true);
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
    
        SQ_ExternalTool::instance()->clear();

        for(;cur;cur = cur->itemBelow())
        {
            SQ_ExternalTool::instance()->append(Tool(cur->text(3), cur->text(1), cur->text(2)));
        }
    }
    
    return result;
}

void SQ_ExternalTools::slotToolRenameRequest( QListViewItem *item, const QPoint &, int pos )
{
    if(!item || pos == -1)
        return;
    
    if(pos > 0)
        item->startRename(pos);
    else
    {
        KIconDialog dialog(SQ_IconLoader::instance());
        dialog.setup(KIcon::Desktop, KIcon::Application, true, 16);
        QString result = dialog.openDialog();
    
        if(!result.isEmpty())
        {
            item->setPixmap(0, SQ_IconLoader::instance()->loadIcon(result, KIcon::Desktop, 16));
            item->setText(3, result);
        }
    }
}

void SQ_ExternalTools::slotHelp()
{
    QWhatsThis::display(tr2i18n("<b>Command</b> can contain <ul><li>%f: one file<li>%F: multiple files</ul>"));
}
