/***************************************************************************
                          sq_diroperator.h  -  description
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

#ifndef SQ_DIROPERATOR_H
#define SQ_DIROPERATOR_H

#include <qmap.h>

#include <kdiroperator.h>

class QTimer;

class KDirLister;
class KFileIconView;
class KFileItem;

class SQ_Downloader;

class SQ_DirOperator : public KDirOperator
{
    Q_OBJECT

    public:
        /*
         *  View type.
         *
         *  - list view with small icons
         *  - icon view with large icons
         *  - detailed view with file details
         *  - thumbnail view with image thumbnails
         */
        enum ViewT {TypeList = 0, TypeIcons, TypeDetailed, TypeThumbs };

        /*
         *  Constructor. Creates diroperator with specified view type. SQ_DirOperatorBase
         *  reimplements createView() to create different custom views.
         *  View type is determined by 'type_'.
         */
        SQ_DirOperator(const KURL &url = KURL(), ViewT type_ = SQ_DirOperator::TypeList,
                                    QWidget *parent = 0, const char *name = 0);

        ~SQ_DirOperator();

        /*
         *  Deselect all items, set current item, select this item,
         *  and ensure it visible.
         */
        void setCurrentItem(KFileItem *item);

        /*
         *  Save new view type for future use. It means that SQ_WidgetStack
         *  wants to change view type (for example "list view" => "thumbnail view"),
         *  and will activate an SQ_DirOperator's action, which will change view type.
         *
         *  See SQ_WidgetStack::raiseWidget() for more.
         */
        void prepareView(ViewT);

        KFileView* preparedView();

        void setPreparedView();

        void startOrNotThumbnailUpdate();

        /*
         *  Is current diropertor manages thumbnail view ?
         */
        bool isThumbView() const;

        int viewType() const;

        /*
         *  Smart update. Store all file items, reset view,
         *  and transfer all items back.
         */
        void smartUpdate();

        /*
         *  Remove ".." item from current view
         */
        void removeCdUpItem();

        void stopPreview();

        void setPendingFile(const QString &p);

        void execute(KFileItem *item);

    protected:
        /*
         *  Reimplement createView() to create custom views.
         */
        virtual KFileView* createView(QWidget *parent, KFile::FileView view);

    private:
        void itemKill(KFileItem *);

        void executePrivate(KFileItem *);

        void disableSpecificActions(KFileIconView *);

        void fireDiskSize(const KURL &url);

        /*
         *  SQ_DirOperator has context menu, derived from KDirOperator.
         *  This method will change this menu, insert new actions.
         */
        void setupActionsMy();

        void clearListers();

    signals:
        /*
         *  If user clicked on item, and it is archive file,
         *  emit this signal. Normally SQ_WidgetStack will catch it.
         */
        void tryUnpack(KFileItem *item);

        /*
         *  Run selected file separately (with default application)
         */
        void runSeparately(KFileItem *item);

    public slots:
        void urlAdded(const KURL &);
        void urlRemoved(const KURL &);

        void stopThumbnailUpdate();

        /*
         *  Invoked, when current directory has been loaded.
         */
        void slotFinishedLoading();

        /*
         *  Change thumbnail size.
         */
        void slotSetThumbSize(const QString&);

        /*
         *  Invoked, when user selected some external tool in menu.
         */
        void slotActivateExternalTool(int index);

    private slots:

        /*
         *  Since KDE 3.4 (or 3.5 ?) it is neccessary to reimplement this slot
         *  to insert our own actions in context menu.
         */
        void activatedMenu(const KFileItem *, const QPoint &pos);

        void slotDownloaderResult(const KURL &);

        void slotSetURL(const KURL &);

        /*
         *  Connected to dirLister()
         */
        void slotNewItems(const KFileItemList &);
        void slotRefreshItems(const KFileItemList &);

        void slotDelayedFinishedLoading();

        void slotFoundMountPoint(const unsigned long&,
                    const unsigned long&,
                    const unsigned long&,
                    const QString&);

        /*
         *  Edit current item's mimetype (Konqueror-related action).
         */
        void slotEditMime();

        void slotPreview();

        void slotDropped(const KFileItem *, QDropEvent*, const KURL::List&);
        void slotAddToBasket();

        /*
         *  Execute item. If current clicking policy is "Single click",
         *  single click will execute item, and double click otherwise.
         */
        void slotExecutedConst(const KFileItem *item);

        /*
         *  Item selected.
         */
        void slotSelected(KFileItem *);

        /*
         *  URL entered.
         */
        void slotUrlEntered(const KURL&);

        /*
         *  Invoked, when some item has been deleted. We should
         *  remove appropriate thumbnail from pixmap cache.
         */
        void slotItemDeleted(KFileItem *);

        void slotUpdateInformation(int,int);

        void slotSelectionChanged();

    private:
        typedef QMap<KURL, KDirLister *> SQ_Listers;
        SQ_Listers listers;
        /*
         *  Pointer to current view. All view types (such as icon view, list view ...)
         *  are derived from KFileView.
         */
        KFileView     *fileview;

        /*
         *  Some additional menus.
         */
        KActionMenu     *pADirOperatorMenu, *pAFileActions, *pAImageActions;

        ViewT     type;
        QTimer    *timer_preview;
        KURL lasturl;
        bool usenew;
        QString m_pending;
        KIO::filesize_t totalSize;
        SQ_Downloader *down;
};

inline
int SQ_DirOperator::viewType() const
{
    return static_cast<int>(type);
}

inline
bool SQ_DirOperator::isThumbView() const
{
    return (type == SQ_DirOperator::TypeThumbs);
}

inline
KFileView* SQ_DirOperator::preparedView()
{
    return fileview;
}

inline
KFileView* SQ_DirOperator::createView(QWidget *, KFile::FileView)
{
    return fileview;
}

inline
void SQ_DirOperator::setPendingFile(const QString &p)
{
    m_pending = p;
}

#endif
