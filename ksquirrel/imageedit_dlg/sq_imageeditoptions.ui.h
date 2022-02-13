/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_ImageEditOptions::init()
{
    pushPut->setPixmap(KSquirrel::loader()->loadIcon("fileopen", KIcon::Desktop, 16));
    
    KURLCompletion *pURLCompletion = new KURLCompletion(KURLCompletion::DirCompletion);
    pURLCompletion->setDir("/");

    linePutTo->setCompletionObject(pURLCompletion);
}

void SQ_ImageEditOptions::slotPutConverted()
{
    putto = KFileDialog::getExistingDirectory(linePutTo->text(), this);    

    if(!putto.isNull() && !putto.isEmpty())
	linePutTo->setText(putto);
}

int SQ_ImageEditOptions::exec(SQ_ImageOptions *o)
{
    linePrefix->setText(o->prefix);
    linePutTo->setText(o->putto);
    QButton *b = buttonGroup1->find(o->where_to_put);

    if(b)
	b->animateClick();
    else
    {
	qWarning("SQ_ImageEditOptions::exec: wrong button Id");
	radioCurrentDir->animateClick();
    }

    checkClose->setChecked(o->close);

    int result = QDialog::exec();

    if(result == QDialog::Accepted)
    {
	o->putto = (linePutTo->isEnabled()) ? linePutTo->text() : QString::null;
	o->prefix = linePrefix->text();
	o->where_to_put = buttonGroup1->selectedId();
	o->close = checkClose->isChecked();
    }

    return result;
}
