/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void SQ_ConvertWidget::init()
{
    selected = (KFileItemList *)sqWStack->selectedItems();
    
    QPtrListIterator<KFileItem> it(*selected);
  
    for(; it.current(); ++it)
	if(it.current()->isFile())
	    listFiles->insertItem(it.current()->name());
    
    listFiles->sort();
}

void SQ_ConvertWidget::slotOpenDir()
{
    QString s = KFileDialog::getExistingDirectory("/", this, "Choose a directory");
    linePutHere->setText(s);
}
