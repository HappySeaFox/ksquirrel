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
    KFile::Mode mode = static_cast<KFile::Mode>(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);
    kurl->setMode(mode);
}

int SQ_ImageEditOptions::exec(SQ_ImageOptions *o)
{
    kurl->setURL(o->putto);

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
        o->putto = (kurl->isEnabled()) ? kurl->url() : QString::null;
        o->where_to_put = buttonGroup1->selectedId();
        o->close = checkClose->isChecked();

        SQ_Config::instance()->setGroup("Image edit options");
        SQ_Config::instance()->writeEntry(prefix + "_putto", o->putto);
        SQ_Config::instance()->writeEntry(prefix + "_where_to_put", o->where_to_put);
        SQ_Config::instance()->writeEntry(prefix + "_close", o->close);
    }

    return result;
}

void SQ_ImageEditOptions::setConfigPrefix( const QString &pr )
{
    prefix = pr;
}
