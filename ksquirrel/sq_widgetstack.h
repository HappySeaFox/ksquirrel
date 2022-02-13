/***************************************************************************
                          sq_widgetstack.h  -  description
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

#ifndef SQ_WIDGETSTACK_H
#define SQ_WIDGETSTACK_H

#include <qobject.h>
#include <qstring.h>

#include <kurl.h>
#include <kfileitem.h>

#include "sq_diroperator.h"

class QTimer;

class KAction;
class KToggleAction;
class KFileView;

class SQ_Downloader;

/*
 *  SQ_WidgetStack is a simple wrapper for file manager (navigator).
 *
 *  It serves all file actions (Copy, Move, Link to...), thumbnail
 *  updates, selecting/deselecting files with +/-  etc.
 */

class SQ_WidgetStack : public QObject
{
    Q_OBJECT

    public:
        /*
         *  Create navigator.
         *  id ::=
         *    0 = List view
         *    1 = Icon view
         *    2 = Detail view
         *    3 = Thumbnail view
         */
        SQ_WidgetStack(QWidget *parent, const int id);

        ~SQ_WidgetStack();

        /*
         *  Direction for moveTo(). 
         */
        enum Direction { Next = 0, Previous = 1 };

        enum FileAction { Copy = 0, Cut, Paste, Unknown };

        enum moveToError { moveSuccess = 0, moveFailed } ;

        SQ_DirOperator* diroperator() const;

        bool updateRunning() const;

        /*
         *  Get current url. Just calls SQ_DirOperator::url().
         */
        KURL url() const;

        /*
         *  Selected items in currently visible diroperator.
         */
        const KFileItemList* selectedItems() const;

        /*
         *  All items in currently visible diroperator.
         */
        const KFileItemList* items() const;

        /*
         *  Update grid for thumbnail view. New grid is calcalated from item
         *  margins (from Options).
         */
        void updateGrid(bool arrange);

        /*
         *  Reread current directory.
         */
        void updateView();

        /*
         *  Quick access to SQ_DirOperator::actionCollection::action()
         */
        KAction *action(const QString &name);

        static SQ_WidgetStack* instance() { return m_instance; }

    private:

        void moveToFirstLast(Direction d);
        /*
         *  Save currently selected items' paths, if any.
         *
         *  Return true, if at least one url was saved.
         */
        bool prepare();

        /*
         *  Execute SQ_SelectDeselectGroup dialog, and select or
         *  deselect files after it has been closed.
         */
        void selectDeselectGroup(bool select);

        /*
         *  Set current url for all _other_ widgets and objects (bookmarks,
         *  tree, SQ_QuickOperator etc.)
         */
        void setURL(const KURL &, bool = true);

    public slots:
        /*
         *  Change view type. See SQ_DirOperator::ViewT for more.
         */
        void raiseWidget(SQ_DirOperator::ViewT, bool doUpdate = true);

        /*
         *  Select first supported image in current directory.
         *  Used by SQ_GLWidget.
         */
        void slotFirstFile();

        /*
         *  Select last supported image in current directory.
         *  Used by SQ_GLWidget.
         */
        void slotLastFile();
        void emitNextSelected();
        void emitPreviousSelected();

        /*
         *  Set filter.
         */
        void setNameFilter(const QString&);

        /*
         *  Used by SQ_FileThumbView to manipulate thumbnail progress.
         */
        void thumbnailsUpdateEnded();
        void thumbnailUpdateStart(int);
        void thumbnailProcess();
        void slotDelayedShowProgress();

        /*
         *  Set url
         */
        void setURLForCurrent(const QString &, bool parseTree = true);
        void setURLForCurrent(const KURL &, bool parseTree = true);
        void slotRunSeparately();

        /*
         *  Select next/prevous supported image in filemanager.
         *  Do nothing, if no more supported images found in given
         *  direction.
         */
        int moveTo(Direction direction, KFileItem *it = 0, bool useSupported = true);

        /*
         *  Get current filter.
         */
        QString nameFilter() const;

        /*
         *  Slots for file actions: copy, move, cut...
         */
        void slotFileCopy();
        void slotFileCut();
        void slotFilePaste();
        void slotFileCopyTo();
        void slotFileMoveTo();
        void slotFileLinkTo();

    private slots:
        void slotRecreateThumbnail();
        void slotDelayedRecreateThumbnail();

        /*
         *  User wants to select to deselect some files.
         */
        void slotSelectGroup();
        void slotDeselectGroup();

        /*
         *  Deselect all files.
         */
        void slotDeselectAll();

        /*
         *  Select all files.
         */
        void slotSelectAll();

    private:
        /*
         *  File manager itself
         */
        SQ_DirOperator    *dirop;

        QTimer     *timerShowProgress;
        KURL::List    files; // files to copy, move or link
        FileAction    fileaction;
        SQ_Downloader *down;

        static SQ_WidgetStack    *m_instance;
};

inline
SQ_DirOperator* SQ_WidgetStack::diroperator() const
{
    return dirop;
}

#endif
