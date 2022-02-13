/***************************************************************************
                          sq_diroperatorbase.cpp  -  description
                             -------------------
    begin                : Sep 8 2004
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qlabel.h>
#include <qtimer.h>

#include <kapplication.h>
#include <kaction.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kstdaccel.h>
#include <kdebug.h>

#include "sq_treeview.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_filethumbview.h"
#include "sq_filedetailview.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_libraryhandler.h"
#include "sq_widgetstack.h"
#include "sq_config.h"
#include "sq_archivehandler.h"
#include "sq_externaltool.h"
#include "sq_categoriesview.h"
#include "sq_popupmenu.h"
#include "sq_categorybrowsermenu.h"

#define SQ_MAX_WORD_LENGTH 50

static const int timer_value = 10;

SQ_DirOperatorBase::SQ_DirOperatorBase(const KURL &url, ViewT type_, QWidget *parent, const char *name) :
    KDirOperator(url, parent, name)
{
    kdDebug() << "+SQ_DirOperatorBase" << endl;

    type = type_;

    // create and insert new actions in context menu
    setupActions();

    connect(this, SIGNAL(urlEntered(const KURL&)), this, SLOT(slotUrlEntered(const KURL&)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotDelayedDecode()));

    fileview = NULL;
}

SQ_DirOperatorBase::~SQ_DirOperatorBase()
{
    kdDebug() << "-SQ_DirOperatorBase" << endl;
}

KFileView* SQ_DirOperatorBase::createView(QWidget *, KFile::FileView)
{
    return fileview;
}

void SQ_DirOperatorBase::slotReturnPressed(QIconViewItem *item)
{
    if(!item) return;

    KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item);

    if(!f)
        return;

    KFileItem *fi = f->fileInfo();

    if(!fi)
        return;

    if(fi->isDir())
        setURL(fi->url(), true);
    else
        slotExecuted(item);
}

void SQ_DirOperatorBase::slotReturnPressed(QListViewItem *item)
{
    if(!item) return;

    KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item);

    if(!f)
        return;

    KFileItem *fi = f->fileInfo();

    if(!fi)
        return;

    if(fi->isDir())
        setURL(fi->url(), true);
    else
        slotExecuted(item);
}

void SQ_DirOperatorBase::slotExecuted(QIconViewItem *item)
{
    if(!item) return;

    KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item);

    if(!f)
        return;

    KFileItem *fi = f->fileInfo();

    execute(fi);
}

void SQ_DirOperatorBase::slotExecuted(QListViewItem *item)
{
    if(!item) return;

    KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item);

    if(!f)
        return;

    KFileItem *fi = f->fileInfo();

    execute(fi);
}

/*
 *  SQ_DirOperatorBase can activate items by single or double click.
 *  This method will connect appropriate signals.
 */
void SQ_DirOperatorBase::reconnectClick(bool firstconnect)
{
    bool old_sing = sing;

    // determine clicking policy
    SQ_Config::instance()->setGroup("Fileview");

    switch(SQ_Config::instance()->readNumEntry("click policy", 0))
    {
        case 0: sing = KGlobalSettings::singleClick(); break;
        case 1: sing = true; break;
        case 2: sing = false; break;

        default:
            sing = true;
    }

    if(!firstconnect)
        if(sing == old_sing)
            return;

    // finally, connect signals
    switch(type)
    {
        case SQ_DirOperatorBase::TypeIcons:
        case SQ_DirOperatorBase::TypeList:
        {
            SQ_FileIconView *iv = dynamic_cast<SQ_FileIconView *>(fileview);

            if(sing)
            {
                disconnect(iv, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
                disconnect(iv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), iv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
                disconnect(iv, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
                connect(iv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), iv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
                connect(iv, SIGNAL(clicked(QIconViewItem*)), SLOT(slotExecuted(QIconViewItem*)));
            }
            else
            {
                disconnect(iv, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
                disconnect(iv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), iv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
                disconnect(iv, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
                connect(iv, SIGNAL(doubleClicked(QIconViewItem*)), SLOT(slotExecuted(QIconViewItem*)));
            }
        }
        break;

        case SQ_DirOperatorBase::TypeThumbs:
        {
            SQ_FileThumbView *tv = dynamic_cast<SQ_FileThumbView *>(fileview);

            if(sing)
            {
                disconnect(tv, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
                disconnect(tv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), tv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
                disconnect(tv, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
                connect(tv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), tv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
                connect(tv, SIGNAL(clicked(QIconViewItem*)), SLOT(slotExecuted(QIconViewItem*)));
            }
            else
            {
                disconnect(tv, SIGNAL(doubleClicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
                disconnect(tv, SIGNAL(clicked(QIconViewItem*, const QPoint&)), tv, SLOT(slotSelected(QIconViewItem*, const QPoint&)));
                disconnect(tv, SIGNAL(clicked(QIconViewItem*)), this, SLOT(slotExecuted(QIconViewItem*)));
                connect(tv, SIGNAL(doubleClicked(QIconViewItem*)), SLOT(slotExecuted(QIconViewItem*)));
            }
        }
        break;

        case SQ_DirOperatorBase::TypeDetailed:
        {
            SQ_FileDetailView *dv = dynamic_cast<SQ_FileDetailView *>(fileview);

            if(sing)
            {
                disconnect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, 0);
                disconnect(dv, SIGNAL(clicked(QListViewItem*)), this, 0);
                disconnect(dv, SIGNAL(doubleClicked(QListViewItem*)), this, 0);
                connect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)));
                connect(dv, SIGNAL(clicked(QListViewItem*)), SLOT(slotExecuted(QListViewItem*)));
            }
            else
            {
                disconnect(dv, SIGNAL(doubleClicked(QListViewItem*)), this, 0);
                disconnect(dv, SIGNAL(clicked(QListViewItem*, const QPoint&, int)), dv, 0);
                disconnect(dv, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));
                connect(dv, SIGNAL(doubleClicked(QListViewItem*)), SLOT(slotExecuted(QListViewItem*)));
            }
        }
        break;
    }
}

/*
 *  Deselect all items, set current item, select this item,
 *  and ensure it visible.
 */
void SQ_DirOperatorBase::setCurrentItem(KFileItem *item)
{
    // for safety...
    if(!item)
        return;

    // clear selection
    fileview->clearSelection();

    // set current item and select it
    fileview->setCurrentItem(item);
    fileview->setSelected(item, true);

    // make this item visible
    fileview->ensureItemVisible(item);
}

void SQ_DirOperatorBase::slotUrlEntered(const KURL &url)
{
    SQ_WidgetStack::instance()->setURLForCurrent(url);
}

void SQ_DirOperatorBase::slotSelected(QIconViewItem *item)
{
    if(!item) return;

    QString str;
    KFileItem *fi;

    if(KFileIconViewItem* f = dynamic_cast<KFileIconViewItem*>(item))
    {
        fi = f->fileInfo();

        // let SQ_WidgetStack to select this file in all other views
        SQ_WidgetStack::instance()->selectFile(fi, this);
    }
}

void SQ_DirOperatorBase::slotSelected(QListViewItem *item)
{
    if(!item) return;

    QString str;
    KFileItem *fi;

    if(KFileListViewItem* f = dynamic_cast<KFileListViewItem*>(item))
    {
        fi = f->fileInfo();

        // let SQ_WidgetStack to select this file in all other views
        SQ_WidgetStack::instance()->selectFile(fi, this);
    }
}

void SQ_DirOperatorBase::slotDelayedDecode()
{
    SQ_GLWidget::window()->slotStartDecoding(tobeDecoded, true);
}

// Insert new actions in context menu.
void SQ_DirOperatorBase::setupActions()
{
    actionCollection()->action("mkdir")->setShortcut(KShortcut(CTRL+Qt::Key_N));

    // remove "View" submenu, since we will insert our's one.
    setupMenu(KDirOperator::SortActions | KDirOperator::NavActions | KDirOperator::FileActions);

    KActionMenu *pADirOperatorMenu = dynamic_cast<KActionMenu *>(actionCollection()->action("popupMenu"));

    pADirOperatorMenu->popupMenu()->insertItem(i18n("&External Tools"), SQ_ExternalTool::instance()->constPopupMenu());
    pADirOperatorMenu->popupMenu()->insertItem(i18n("Add To &Category"), SQ_CategoriesBox::instance()->popupMenu());

    connect(actionCollection()->action("properties"), SIGNAL(enabled(bool)), 
        SQ_ExternalTool::instance()->constPopupMenu(), SLOT(setEnabled(bool)));

    connect(actionCollection()->action("properties"), SIGNAL(enabled(bool)), 
        SQ_CategoriesBox::instance()->popupMenu(), SLOT(setEnabled(bool)));

    new KAction(QString::null, 0, CTRL+Key_J, SQ_WidgetStack::instance(), SLOT(slotRunSeparately()), actionCollection(), "dirop_runsepar");
    new KAction(QString::null, 0, KStdAccel::copy(), SQ_WidgetStack::instance(), SLOT(slotFileCopy()), actionCollection(), "dirop_copy");
    new KAction(QString::null, 0, KStdAccel::cut(), SQ_WidgetStack::instance(), SLOT(slotFileCut()), actionCollection(), "dirop_cut");
    new KAction(QString::null, 0, KStdAccel::paste(), SQ_WidgetStack::instance(), SLOT(slotFilePaste()), actionCollection(), "dirop_paste");
    new KAction(QString::null, 0, 0, SQ_WidgetStack::instance(), SLOT(slotFileCopyTo()), actionCollection(), "dirop_copyto");
    new KAction(QString::null, 0, 0, SQ_WidgetStack::instance(), SLOT(slotFileMoveTo()),actionCollection(), "dirop_cutto");
    new KAction(QString::null, 0, 0, SQ_WidgetStack::instance(), SLOT(slotFileLinkTo()), actionCollection(), "dirop_linkto");
    new KAction(i18n("Recreate selected thumbnails"), "reload", CTRL+Key_T, SQ_WidgetStack::instance(),
        SLOT(slotRecreateThumbnail()), actionCollection(), "dirop_recreate_thumbnails");
}

// Internal method
void SQ_DirOperatorBase::execute(KFileItem *fi)
{
    if(!fi)
        return;

    QString fullpath = fi->url().path();
    QFileInfo fm(fullpath);

    if(!fi->isDir())
    {
        tobeDecoded = fi->url().path();

        // Ok, this file is image file. Let's try to load it
        if(SQ_LibraryHandler::instance()->supports(tobeDecoded))
            timer->start(timer_value, true);
        else
        {
            SQ_GLView::window()->sbarWidget("SBDecoded")->setText(i18n("Format \"%1\" not supported").arg(fm.extension(false)));

            SQ_Config::instance()->setGroup("Fileview");

            // archive ?
            if(SQ_ArchiveHandler::instance()->findProtocolByFile(fi) != -1)
                if(SQ_Config::instance()->readBoolEntry("archives", true))
                    emit tryUnpack(fi);
                else;
            // not image, not archive. Read settings and run this file separately
            // with default application (if needed).
            else if(SQ_Config::instance()->readBoolEntry("run unknown", false))
                emit runSeparately(fi);
        }
    }
}

void SQ_DirOperatorBase::prepareView(ViewT t)
{
    type = t;

    switch(type)
    {
        case  SQ_DirOperatorBase::TypeDetailed:
        {
            SQ_FileDetailView *dv;
            fileview = dv = new SQ_FileDetailView(this, "detail view");

            reconnectClick(true);

            disconnect(dv, SIGNAL(returnPressed(QListViewItem*)), dv, 0);
            connect(dv, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotReturnPressed(QListViewItem*)));
            connect(dv, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(slotSelected(QListViewItem*)));
        }
        break;

        case  SQ_DirOperatorBase::TypeList:
        case  SQ_DirOperatorBase::TypeIcons:
        {
            SQ_FileIconView *iv;
            fileview = iv = new SQ_FileIconView(this, (type == SQ_DirOperatorBase::TypeIcons) ? "icon view":"list view");

            reconnectClick(true);

            disconnect(iv, SIGNAL(returnPressed(QIconViewItem*)), iv, 0);
            connect(iv, SIGNAL(returnPressed(QIconViewItem*)), this, SLOT(slotReturnPressed(QIconViewItem*)));
            connect(iv, SIGNAL(currentChanged(QIconViewItem*)), this, SLOT(slotSelected(QIconViewItem*)));
        }
        break;

        case  SQ_DirOperatorBase::TypeThumbs:
        {
            SQ_FileThumbView *tv;
            fileview = tv = new SQ_FileThumbView(this, "thumb view");

            reconnectClick(true);

            disconnect(tv, SIGNAL(returnPressed(QIconViewItem*)), tv, 0);
            connect(tv, SIGNAL(returnPressed(QIconViewItem*)), this, SLOT(slotReturnPressed(QIconViewItem*)));
            connect(dirLister(), SIGNAL(canceled()), tv, SLOT(stopThumbnailUpdate()));
            connect(tv, SIGNAL(currentChanged(QIconViewItem*)), this, SLOT(slotSelected(QIconViewItem*)));
        }
        break;
    }
}

void SQ_DirOperatorBase::setPreparedView()
{
    setView(fileview);
}

#include "sq_diroperatorbase.moc"
