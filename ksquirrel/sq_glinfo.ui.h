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

void SQ_GLInfo::init()
{    
    listGL->setSorting(-1);

    QListViewItem *item;
    QMap<QString, GLenum> v;

    v.insert("GL_VENDOR", GL_VENDOR);
    v.insert("GL_RENDERER", GL_RENDERER);
    v.insert("GL_VERSION", GL_VERSION);

    for(QMap<QString, GLenum>::ConstIterator it = v.constBegin(); it != v.constEnd(); ++it)
    {
        item = listGL->findItem(it.key().latin1(), 0);

        if(item)
        {
            const GLubyte *b = glGetString(it.data());

            if(b)
            {
                item->setText(1, (const char*)b);
            }   
        }
    }

    listGL->setCurrentItem(listGL->firstChild());
    listGL->clearSelection();
    listGL->setSelected(listGL->currentItem(), true);
}
