/*
 *  Copyright (C) 2006 Baryshev Dmitry, KSquirrel project
 *
 *  Originally based on browser_mnu.cpp by (C) Matthias Elter
 *  from kde-3.2.3
 */

/*****************************************************************

Copyright (c) 2001 Matthias Elter <elter@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qpixmap.h>
#include <qdir.h>

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kapplication.h>
#include <kprocess.h>
#include <kiconloader.h>
#include <ksimpleconfig.h>
#include <kdesktopfile.h>
#include <kmimetype.h>
#include <kglobalsettings.h>
#include <kio/global.h>
#include <krun.h>
#include <konq_operations.h>
#include <kfileitem.h>
#include <kdirwatch.h>
#include <kstringhandler.h>
#include <kurldrag.h>

#include "sq_dir.h"
#include "sq_categoriesview.h"
#include "sq_categorybrowsermenu.h"

#define CICON(a) (*_icons)[a]

QMap<QString, QPixmap> *SQ_CategoryBrowserMenu::_icons = 0;

SQ_CategoryBrowserMenu::SQ_CategoryBrowserMenu(QString path, QWidget *parent, const char *name, int startid)
    : KPanelMenu(path, parent, name)
    , _mimecheckTimer(0)
    , _startid(startid)
    , _dirty(false)
{
    _subMenus.setAutoDelete(true);
    _lastpress = QPoint(-1, -1);
    setAcceptDrops(true); // Should depend on permissions of path.

    // we are not interested for dirty events on files inside the
    // directory (see slotClearIfNeeded)
    connect( &_dirWatch, SIGNAL(dirty(const QString&)),
             this, SLOT(slotClearIfNeeded(const QString&)) );
    connect( &_dirWatch, SIGNAL(created(const QString&)),
             this, SLOT(slotClear()) );
    connect( &_dirWatch, SIGNAL(deleted(const QString&)),
             this, SLOT(slotClear()) );
}

SQ_CategoryBrowserMenu::~SQ_CategoryBrowserMenu()
{}

void SQ_CategoryBrowserMenu::slotClearIfNeeded(const QString& p)
{
    if (p == path())
        slotClear();
}


void SQ_CategoryBrowserMenu::initialize()
{
    _lastpress = QPoint(-1, -1);

    // don't change menu if already visible
    if (isVisible())
        return;

    if (_dirty) {
        // directory content changed while menu was visible
        slotClear();
        setInitialized(false);
        _dirty = false;
    }

    if (initialized()) return;
    setInitialized(true);

    // start watching if not already done
    if (!_dirWatch.contains(path()))
        _dirWatch.addDir( path() );

    // setup icon map
    initIconMap();

    // read configuration
    KConfig *c = KGlobal::config();
    c->setGroup("menus");
    _showhidden = c->readBoolEntry("ShowHiddenFiles", false);
    _maxentries = c->readNumEntry("MaxEntries2", 30);

    // clear maps
    _filemap.clear();
    _mimemap.clear();

    int filter = QDir::Dirs | QDir::Files;
    if(_showhidden)
        filter |= QDir::Hidden;

    QDir dir(path(), QString::null, QDir::DirsFirst | QDir::Name | QDir::IgnoreCase, filter);

    // does the directory exist?
    if (!dir.exists()) {
        insertItem(i18n("Failed to Read Folder"));
	return;
    }

    // get entry list
    const QFileInfoList *list = dir.entryInfoList();

    // no list -> read error
    if (!list) {
	insertItem(i18n("Failed to Read Folder"));
	return;
    }

    KURL url;
    url.setPath(path());
    if (!kapp->authorizeURLAction("list", KURL(), url))
    {
        insertItem(i18n("Not Authorized to Read Folder"));
        return;
    }

    // insert file manager and terminal entries
    // only the first part menu got them
    if(_startid == 0)
    {
       SQ_Dir dir(SQ_Dir::Categories);

       if(dir.root() != path())
       {
            insertTitle(path().right(path().length() - dir.root().length()));
            insertItem(CICON("bookmark_add"), i18n("Add here"), this, SLOT(slotAddToCategory()));
       }
    }

    bool first_entry = true;
    bool dirfile_separator = false;
    int item_count = 0;
    int run_id = _startid;

    // get list iterator
    QFileInfoListIterator it(*list);

    // jump to startid
    it += _startid;

    // iterate over entry list
    for (; it.current(); ++it)
    {
        // bump id
        run_id++;

        QFileInfo *fi = it.current();
        // handle directories
        if (fi->isDir())
        {
            QString name = fi->fileName();

            // ignore . and .. entries
            if (name == "." || name == "..") continue;

            QPixmap icon;
            QString path = fi->absFilePath();

            // parse .directory if it does exist
            if (QFile::exists(path + "/.directory")) {

                KSimpleConfig c(path + "/.directory", true);
                c.setDesktopGroup();
                icon = KGlobal::iconLoader()->loadIcon(c.readEntry("Icon"),
                                                       KIcon::Small, KIcon::SizeSmall,
                                                       KIcon::DefaultState, 0, true);
                if(icon.isNull())
                    icon = CICON("folder");
                name = c.readEntry("Name", name);
            }

            // use cached folder icon for directories without special icon
            if (icon.isNull())
                icon = CICON("folder");

            // insert separator if we are the first menu entry
            if(first_entry) {
                if (_startid == 0)
                    insertSeparator();
                first_entry = false;
            }

            // append menu entry
            append(icon, KStringHandler::cEmSqueeze( name, fontMetrics(), 20 ),
                   new SQ_CategoryBrowserMenu(path, this));

            // bump item count
            item_count++;

            dirfile_separator = true;
        }
/*
        // handle files
        else if(fi->isFile())
        {
            QString name = fi->fileName();
            QString title = KIO::decodeFileName(name);

            // ignore .directory and .order files
            if (name == ".directory" || name == ".order") continue;

            QPixmap icon;
            QString path = fi->absFilePath();

            bool mimecheck = false;

            // .desktop files
            if(KDesktopFile::isDesktopFile(path))
            {
                KSimpleConfig c(path, true);
                c.setDesktopGroup();
                title = c.readEntry("Name", title);

                QString s = c.readEntry("Icon");
                if(!_icons->contains(s)) {
                    icon  = KGlobal::iconLoader()->loadIcon(s, KIcon::Small, KIcon::SizeSmall,
                                                            KIcon::DefaultState, 0, true);

                    if(icon.isNull()) {
                        QString type = c.readEntry("Type", "Application");
                        if (type == "Directory")
                            icon = CICON("folder");
                        else if (type == "Mimetype")
                            icon = CICON("txt");
                        else if (type == "FSDevice")
                            icon = CICON("chardevice");
                        else
                            icon = CICON("exec");
                    }
                    else
                        _icons->insert(s, icon);
                }
                else
                    icon = CICON(s);
            }
            else {
                // set unknown icon
                icon = CICON("unknown");

                // mark for delayed mimetime check
                mimecheck = true;
            }

            // insert separator if we are the first menu entry
            if(first_entry) {
                if(_startid == 0)
                    insertSeparator();
                first_entry = false;
            }

            // insert separator if we we first file after at least one directory
            if (dirfile_separator) {
                insertSeparator();
                dirfile_separator = false;
            }

            // append file entry
            append(icon, title, name, mimecheck);

            // bump item count
            item_count++;
        }
*/
        if(item_count == _maxentries) {
            // Only insert a "More..." item if there are actually more items.
            ++it;
            if( it.current() ) {
                insertSeparator();
                append(CICON("folder_open"), i18n("More..."), new SQ_CategoryBrowserMenu(path(), this, 0, run_id));
            }
            break;
        }
    }

#if 0
    // WABA: tear off handles don't work together with dynamically updated
    // menus. We can't update the menu while torn off, and we don't know
    // when it is torn off.
    if(KGlobalSettings::insertTearOffHandle() && item_count > 0)
        insertTearOffHandle();
#endif

    adjustSize();

    QString dirname = path();

    int maxlen = contentsRect().width() - 40;
    if(item_count == 0)
        maxlen = fontMetrics().width(dirname);

    if (fontMetrics().width(dirname) > maxlen) {
        while ((!dirname.isEmpty()) && (fontMetrics().width(dirname) > (maxlen - fontMetrics().width("..."))))
            dirname = dirname.remove(0, 1);
        dirname.prepend("...");
    }
    setCaption(dirname);

    // setup and start delayed mimetype check timer
    if(_mimemap.count() > 0) {

        if(!_mimecheckTimer)
            _mimecheckTimer = new QTimer(this);

        connect(_mimecheckTimer, SIGNAL(timeout()), SLOT(slotMimeCheck()));
        _mimecheckTimer->start(0);
    }
}

void SQ_CategoryBrowserMenu::append(const QPixmap &pixmap, const QString &title, const QString &file, bool mimecheck)
{
    // avoid &'s being converted to accelerators
    QString newTitle = title;
    newTitle.replace("&", "&&");
    newTitle = KStringHandler::cEmSqueeze( newTitle, fontMetrics(), 20 );

    // insert menu item
    int id = insertItem(pixmap, newTitle);

    // insert into file map
    _filemap.insert(id, file);

    // insert into mimetype check map
    if(mimecheck)
        _mimemap.insert(id, true);
}

void SQ_CategoryBrowserMenu::append(const QPixmap &pixmap, const QString &title, SQ_CategoryBrowserMenu *subMenu)
{
    // avoid &'s being converted to accelerators
    QString newTitle = title;
    newTitle.replace("&", "&&");
    newTitle = KStringHandler::cEmSqueeze( newTitle, fontMetrics(), 20 );

    // insert submenu
    insertItem(pixmap, newTitle, subMenu);
    // remember submenu for later deletion
    _subMenus.append(subMenu);
}

void SQ_CategoryBrowserMenu::mousePressEvent(QMouseEvent *e)
{
    QPopupMenu::mousePressEvent(e);
    _lastpress = e->pos();
}

void SQ_CategoryBrowserMenu::mouseMoveEvent(QMouseEvent *e)
{
    QPopupMenu::mouseMoveEvent(e);

    if (!(e->state() & LeftButton)) return;
    if(_lastpress == QPoint(-1, -1)) return;

    // DND delay
    if((_lastpress - e->pos()).manhattanLength() < 12) return;

    // get id
    int id = idAt(_lastpress);
    if(!_filemap.contains(id)) return;

    // reset _lastpress
    _lastpress = QPoint(-1, -1);

    // start drag
    KURL url;
    url.setPath(path() + "/" + _filemap[id]);
    KURL::List files(url);
    KURLDrag *d = new KURLDrag(files, this);
    d->setPixmap(iconSet(id)->pixmap());
    d->drag();
}

void SQ_CategoryBrowserMenu::dragEnterEvent( QDragEnterEvent *ev )
{
    if (KURLDrag::canDecode(ev))
        ev->accept(rect());
    KPanelMenu::dragEnterEvent(ev);
}

void SQ_CategoryBrowserMenu::dropEvent( QDropEvent *ev )
{
    KFileItem item( path(), QString::fromLatin1( "inode/directory" ),  KFileItem::Unknown );
    KonqOperations::doDrop( &item, path(), ev, this );
    KPanelMenu::dropEvent(ev);
    // ### TODO: Update list
}

void SQ_CategoryBrowserMenu::slotExec(int id)
{
    kapp->propagateSessionManager();

    if(!_filemap.contains(id)) return;

    KURL url;
    url.setPath(path() + "/" + _filemap[id]);
    new KRun(url, 0, true); // will delete itself
    _lastpress = QPoint(-1, -1);
}

void SQ_CategoryBrowserMenu::slotAddToCategory()
{
    SQ_CategoriesBox::instance()->addToCategory(path());
}

void SQ_CategoryBrowserMenu::slotMimeCheck()
{
    // get the first map entry
    QMap<int, bool>::Iterator it = _mimemap.begin();

    // no mime types left to check -> stop timer
    if(it == _mimemap.end()) {
        _mimecheckTimer->stop();
        return;
    }

    int id = it.key();
    QString file = _filemap[id];

    _mimemap.remove(it);

    KURL url;
    url.setPath( path() + '/' + file );

//    KMimeType::Ptr mt = KMimeType::findByURL(url, 0, true, false);
//    QString icon(mt->icon(url, true));
    QString icon = KMimeType::iconForURL( url );
//    kdDebug() << url.url() << ": " << icon << endl;

    file = KStringHandler::cEmSqueeze( file, fontMetrics(), 20 );

    file.replace("&", "&&");
    if(!_icons->contains(icon)) {
        QPixmap pm = SmallIcon(icon);
#if ( QT_VERSION >= 0x030200 )
        if( pm.height() > 16 )
        {
            QPixmap cropped( 16, 16 );
            copyBlt( &cropped, 0, 0, &pm, 0, 0, 16, 16 );
            pm = cropped;
        }
#endif
        _icons->insert(icon, pm);
        changeItem(id, pm, file);
    }
    else
        changeItem(id, CICON(icon), file);
}

void SQ_CategoryBrowserMenu::slotClear()
{
    // no need to watch any further
    if (_dirWatch.contains(path()))
        _dirWatch.removeDir( path() );

    // don't change menu if already visible
    if (isVisible()) {
        _dirty = true;
        return;
    }
    KPanelMenu::slotClear();
    _subMenus.clear(); // deletes submenus
}

void SQ_CategoryBrowserMenu::initIconMap()
{
    if(_icons) return;

//    kdDebug() << "SQ_CategoryBrowserMenu::initIconMap" << endl;

    _icons = new QMap<QString, QPixmap>;

    _icons->insert("folder", SmallIcon("folder"));
    _icons->insert("unknown", SmallIcon("mime_empty"));
    _icons->insert("folder_open", SmallIcon("folder_open"));
    _icons->insert("kdisknav", SmallIcon("kdisknav"));
    _icons->insert("kfm", SmallIcon("kfm"));
    _icons->insert("terminal", SmallIcon("terminal"));
    _icons->insert("txt", SmallIcon("txt"));
    _icons->insert("exec", SmallIcon("exec"));
    _icons->insert("chardevice", SmallIcon("chardevice"));
}

#include "sq_categorybrowsermenu.moc"
