/***************************************************************************
                          sq_diroperatorbase.h  -  description
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

#ifndef SQ_DIROPERATORBASE_H
#define SQ_DIROPERATORBASE_H

#include <kurl.h>
#include <kfileview.h>
#include <kdiroperator.h>

class QIconViewItem;
class QListViewItem;
class QTimer;

class KAction;

class SQ_FileIconView;
class SQ_FileDetailView;
class SQ_FileThumbView;

/*
 *  About KDirOperator (from kdelibs): 
 *  This widget works as a network transparent filebrowser. You specify a URL
 *  to display and this url will be loaded via KDirLister. The user can
 *  browse through directories, highlight and select files, delete or rename
 *  files.
 *
 *  SQ_DirOperatorBase is a base class for KSquirrel's filemanager. KSquirrel has two types of
 *  filemanagers - usual (used in SQ_WidgetStack), and simple (used in SQ_QuickBrowser).
 *  SQ_DirOperatorBase represents 'simple' version.
 */

class SQ_DirOperatorBase : public KDirOperator
{
    Q_OBJECT

    public:
        enum VV {TypeList = 0, TypeIcon, TypeDetail, TypeThumbs };

        /*
         *  Constructor. Creates diroperator with specified view type. SQ_DirOperatorBase
         *  reimplements createView() to create different custom views.
         *  View type is determined by 'type_'.
         */
        SQ_DirOperatorBase(const KURL &url = KURL(), VV type_ = SQ_DirOperatorBase::TypeList,
        QWidget *parent = 0, const char *name = 0);
        virtual ~SQ_DirOperatorBase();

        /*
         *  Deselect all items, set current item, select this item,
         *  and ensure it visible.
         */
        void setCurrentItem(KFileItem *item);

        /*
         *  Is current diropertor manages thumbnail view ?
         */
        bool isThumbView() const;

        /*
         *  SQ_DirOperatorBase can activate items by single or double click.
         *  This method will connect appropriate signals.
         */
        void reconnectClick(bool firstconnect = false);

    private:
        /*
         *  SQ_DirOperatorBase has context menu, derived from KDirOperator.
         *  This method will change this menu, insert new actions.
         */
        void setupActions();

    protected:
        /*
         *  Reimplement createView() to create custom views.
         */
        KFileView* createView(QWidget *parent, KFile::FileView view);

    public slots:
        /*
         *  Invoked, when user selected some external tool in menu.
         */
        void slotActivateExternalTool(int index);

    protected slots:
        /*
         *  Invoked, when user pressed 'Return' on item.
         */
        void slotReturnPressed(QIconViewItem *item);
        void slotReturnPressed(QListViewItem *item);

        /*
         *  Execute item. If current clicking policy is "Single click",
         *  single click will execute item, and double click otherwise.
         */
        void slotExecuted(QIconViewItem *item);
        void slotExecuted(QListViewItem *item);

        /*
         *  Item selected.
         */
        void slotSelected(QIconViewItem*);
        void slotSelected(QListViewItem*);

        /*
         *  URL entered.
         */
        void slotUrlEntered(const KURL&);

        /*
         *  If user clicked on item, and it is supported image type,
         *  start decoding with small delay.
         */
        void slotDelayedDecode();

    signals:
        /*
         *  If user clicked on item, and it is archive file,
         *  emit this signal. Normally SQ_WidgetStack will catch it.
         */
        void tryUnpack(KFileItem *item);

    public:
        /*
         *  Pointer to current view. All view types (such as icon view, list view ...)
         *  are derived from KFileView.
         */
        KFileView     *fileview;

        /*
         *  Views.
         */
        SQ_FileThumbView    *tv;
        SQ_FileIconView    *iv;
        SQ_FileDetailView    *dv;

        /*
         *  Single click to activate item ?
         */
        bool     sing;

        /*
         *  Some additional menus.
         */
        KActionMenu     *pADirOperatorMenu, *pAFileActions, *pAImageActions;

    protected:
        /*
         *  Action, which will run current item with default application.
         *  For eample, it can open film "f.avi" in Xine.
         */
        KAction    *pARunSeparately;

        /*
         *  View type.
         */
        VV     type;

        /*
         *  An url of activated item.
         */
        QString    tobeDecoded;

        /*
         *  Delay timer.
         */
        QTimer    *timer;
};

#endif
