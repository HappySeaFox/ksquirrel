/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/*
 *  SQ_PluginsInfo shows all found by SQ_LibraryListener libraries.
 */

void SQ_PluginsInfo::init()
{
    showLibs();
}

void SQ_PluginsInfo::showLibs()
{
    QString path;
    QPixmap pix;

    tableLib->clear();

    int cnt = SQ_LibraryHandler::instance()->count();

    textLibs->setText(SQ_KLIBS);
    textCount->setNum(cnt);

    if(!cnt) return;

    for(QValueVector<SQ_LIBRARY>::iterator it = SQ_LibraryHandler::instance()->begin();
            it != SQ_LibraryHandler::instance()->end();++it)
    {
        QListViewItem *item = new QListViewItem(tableLib, QString::null,
        QString((*it).quickinfo),
        QString((*it).version),
        QString((*it).filter),
        QString::fromLatin1("%1%2")
            .arg((*it).readable?"R":"")
            .arg((*it).writable?"W":""));

        if(pix.convertFromImage((*it).mime))
            item->setPixmap(0, pix);

        tableLib->insertItem(item);
    }
}
