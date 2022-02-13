/***************************************************************************
                          sq_filedetailview.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qheader.h>
#include <qcursor.h>

#include <kapplication.h>
#include <kpopupmenu.h>
#include <kdebug.h>

#include "sq_config.h"
#include "sq_iconloader.h"
#include "sq_filedetailview.h"
#include "sq_widgetstack.h"
#include "sq_diroperator.h"
#include "sq_navigatordropmenu.h"

SQ_FileListViewItem::SQ_FileListViewItem(QListView *parent, KFileItem *fi) : KFileListViewItem(parent, fi)
{}

SQ_FileListViewItem::SQ_FileListViewItem(QListView *parent, const QString &text,
     const QPixmap &icon, KFileItem *fi)
    : KFileListViewItem(parent, text, icon, fi)
{}

SQ_FileListViewItem::~SQ_FileListViewItem()
{}

/*
 *  Reimplement paintFocus() to ignore painting focus.
 */
void SQ_FileListViewItem::paintFocus(QPainter *, const QColorGroup &, const QRect &)
{}

SQ_FileDetailView::SQ_FileDetailView(QWidget* parent, const char* name)
    : KFileDetailView(parent, name)
{
    kdDebug() << "+SQ_FileDetailView" << endl;

    // drag & drop support
    setAcceptDrops(true);
    connect(this, SIGNAL(dropped(QDropEvent *, const KURL::List &, const KURL &)), this,
    SLOT(slotDropped(QDropEvent *, const KURL::List &, const KURL &)));

    // pixmap for directory item
    dirPix = SQ_IconLoader::instance()->loadIcon("folder", KIcon::Desktop, KIcon::SizeSmall);
}

SQ_FileDetailView::~SQ_FileDetailView()
{
    kdDebug() << "-SQ_FileDetailView" << endl;
}

/*
 *  Reimplement insertItem() to enable/disable inserting
 *  directories (depends on current settings).
 */
void SQ_FileDetailView::insertItem(KFileItem *i)
{
    SQ_Config::instance()->setGroup("Fileview");

    // directores disabled ?
    if(i->isDir() && SQ_Config::instance()->readBoolEntry("disable_dirs", false))
        return;

    // add new item
    SQ_FileListViewItem *item = new SQ_FileListViewItem(this, i);

    initItem(item, i);

    i->setExtraData(this, item);
}

/*
 *  Internal. Set item's sorting key.
 */
void SQ_FileDetailView::initItem(SQ_FileListViewItem *item, const KFileItem *i)
{
    // determine current sorting type
    QDir::SortSpec spec = KFileView::sorting();

    if(spec & QDir::Time)
        item->setKey(sortingKey(i->time(KIO::UDS_MODIFICATION_TIME), i->isDir(), spec));
    else if(spec & QDir::Size)
        item->setKey(sortingKey(i->size(), i->isDir(), spec));
    else
        item->setKey(sortingKey(i->text(), i->isDir(), spec));
}

/*
 *  On double click execute item or
 *  invoke default browser in current url.
 */
void SQ_FileDetailView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    QPoint vp = contentsToViewport(e->pos());

    QListViewItem *item = itemAt(vp);

    emit QListView::doubleClicked(item);

    int col = item ? header()->mapToLogical(header()->cellAt(vp.x())) : -1;

    // double click on item
    if(item)
    {
        if(e->button() == Qt::LeftButton && !SQ_WidgetStack::instance()->diroperator()->singleClick())
            emitExecute(item, e->globalPos(), col);

        emit doubleClicked(item, e->globalPos(), col);
    }
    // double click was in viewport, let's invoke browser
    else
    {
        kapp->invokeBrowser(SQ_WidgetStack::instance()->url().path());
    }
}

/*
 *  Somebody dropped urls in viewport. Let's execute popup menu with
 *  file actions.
 */
void SQ_FileDetailView::slotDropped(QDropEvent *, const KURL::List &urls, const KURL &_url)
{
    KURL url = (_url.isEmpty()) ? SQ_WidgetStack::instance()->url() : _url;

    // setup and execute menu with file actions
    SQ_NavigatorDropMenu::instance()->setupFiles(urls, url);
    SQ_NavigatorDropMenu::instance()->exec(QCursor::pos());
}

// Accept dragging
void SQ_FileDetailView::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(true);
}

/*
 *  Insert ".." item.
 */
void SQ_FileDetailView::insertCdUpItem(const KURL &base)
{
    KFileItem *fi = new KFileItem(base.upURL(), QString::null, KFileItem::Unknown);

    SQ_FileListViewItem *item = new SQ_FileListViewItem(this, QString::fromLatin1(".."), dirPix, fi);

    item->setSelectable(false);
    item->setKey(sortingKey("..", true, QDir::Name|QDir::DirsFirst));

    fi->setExtraData(this, item);
}

/*
 *  Clear current view and insert "..".
 */
void SQ_FileDetailView::clearView()
{
    // call default clearing method
    KFileDetailView::clearView();

    // insert ".."
    insertCdUpItem(SQ_WidgetStack::instance()->url());
}

#include "sq_filedetailview.moc"
