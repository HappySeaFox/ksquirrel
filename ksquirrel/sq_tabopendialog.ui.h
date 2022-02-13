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

void SQ_TabOpenDialog::init()
{
    res = -1;
}

void SQ_TabOpenDialog::slotPush1()
{
    res = 0;
    accept();
}


void SQ_TabOpenDialog::slotPush2()
{
    res = 1;
    accept();
}

void SQ_TabOpenDialog::slotPush3()
{
    res = 2;
    accept();
}

int SQ_TabOpenDialog::result()
{
    return res;
}
