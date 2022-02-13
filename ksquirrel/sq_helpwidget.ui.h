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

void SQ_HelpWidget::init()
{
        setPalette(QToolTip::palette());
}

bool SQ_HelpWidget::event(QEvent *e)
{
        if(e->type() == QEvent::WindowDeactivate 
                        || e->type() == QEvent::MouseButtonPress 
                        || e->type() == QEvent::KeyPress)
        {
                reject();
        }

        return QDialog::event(e);
}
