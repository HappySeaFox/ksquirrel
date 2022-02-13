/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void SQ_PluginsInfo::init()
{
    QPixmap pixNF = QPixmap::fromMimeSource(locate("appdata", "images/libs_notfound.png"));
    pixmapNotFound->setPixmap(pixNF);

    showLibs();
}

void SQ_PluginsInfo::showLibs()
{
    QString path;
    QPixmap pix;

    tableLib->clear();

    int cnt = SQ_LibraryHandler::instance()->count();

    WST->raiseWidget(cnt ? 0 : 1);

    textTotal->setNum(cnt);

    if(!cnt) return;

    QValueVector<SQ_LIBRARY>::iterator   BEGIN = SQ_LibraryHandler::instance()->begin();
    QValueVector<SQ_LIBRARY>::iterator      END = SQ_LibraryHandler::instance()->end();

    for(QValueVector<SQ_LIBRARY>::iterator it = BEGIN;it != END;++it)
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
