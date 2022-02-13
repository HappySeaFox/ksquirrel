/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void SQ_InterfaceType::init()
{
    listType->insertItem(QPixmap::fromMimeSource(locate("appdata", "images/like/like_classic.png")), tr2i18n("KSquirrel Classic"), 0);
    listType->insertItem(QPixmap::fromMimeSource(locate("appdata", "images/like/like_builtin.png")), tr2i18n("Built-in"), 1);
    listType->insertItem(QPixmap::fromMimeSource(locate("appdata", "images/like/like_gqview.png")), tr2i18n("Like GQview"), 2);
}

int SQ_InterfaceType::start()
{
    index = KSquirrel::app()->interfaceType();
    listType->setCurrentItem(index);
    listType->setSelected(index, true);

    int result = exec();

    if(result == QDialog::Accepted && index != KSquirrel::app()->interfaceType())
	KSquirrel::app()->rebuildInterface(index);

    return result;
}

void SQ_InterfaceType::slotSelected(int ind)
{
    index = ind;
}

void SQ_InterfaceType::slotApply()
{
    if(index != KSquirrel::app()->interfaceType())
	KSquirrel::app()->rebuildInterface(index);
}
