/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/*
 *  SQ_ThumbnailCacheMaster manipulates thumbnail cache
 *  in memory and on disk. It can show current cache size,
 *  delete cache on disk, clear cache etc.
 */

void SQ_ThumbnailCacheMaster::init()
{
    pushCache->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_disk.png")));
    pushCacheBrowse->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_disk_browse.png")));
    pushCacheMemory->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_mem.png")));
    pushClearCache->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_disk_clear.png")));
    pushClearCacheMemory->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_mem_clear.png")));
    pushShowCache->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_mem_view.png")));
    pushSyncCache->setPixmap(QPixmap::fromMimeSource(locate("appdata", "images/cache_mem_sync.png")));
}

void SQ_ThumbnailCacheMaster::slotCalcCache()
{
    KURL url;
    SQ_Dir tmp(SQ_Dir::Thumbnails);

    url.setPath(tmp.root());

    int size = KDirSize::dirSize(url);

    QString s = KIO::convertSize(size);

    textThumbSize->setText(s);
}

void SQ_ThumbnailCacheMaster::slotClearCache()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    
    SQ_Dir tmp(SQ_Dir::Thumbnails);
    
    KURL url = tmp.root();

    KIO::DeleteJob *job = KIO::del(url);
    connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotClearFinished(KIO::Job*)));
}

void SQ_ThumbnailCacheMaster::slotClearFinished( KIO::Job * )
{
    QApplication::restoreOverrideCursor();
    slotCalcCache();
}

void SQ_ThumbnailCacheMaster::slotClearMemoryCache()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    SQ_PixmapCache::instance()->clear();
    slotCalcCacheMemory();

    QApplication::restoreOverrideCursor();
}

void SQ_ThumbnailCacheMaster::slotCalcCacheMemory()
{
    int size = SQ_PixmapCache::instance()->totalSize();

    QString s = KIO::convertSize(size);

    textCacheMemSize->setText(s);
}

void SQ_ThumbnailCacheMaster::slotShowDiskCache()
{
    SQ_ViewCache m_view(this);
    m_view.exec();
}

void SQ_ThumbnailCacheMaster::slotSyncCache()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    SQ_PixmapCache::instance()->sync();

    slotCalcCacheMemory();
    slotCalcCache();

    QApplication::restoreOverrideCursor();
}

void SQ_ThumbnailCacheMaster::slotBrowseDiskCache()
{
    SQ_Dir tmp(SQ_Dir::Thumbnails);

    kapp->invokeBrowser(tmp.root());
}
