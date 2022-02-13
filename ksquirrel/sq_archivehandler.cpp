/***************************************************************************
                          sq_archivehandler.cpp  -  description
                             -------------------
    begin                : ??? ??? 26 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kmessagebox.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kurl.h>

#include "sq_archivehandler.h"

SQ_ArchiveHandler * SQ_ArchiveHandler::m_instance = 0;

SQ_ArchiveHandler::SQ_ArchiveHandler(QObject * parent, const char *name)
    : QObject(parent, name), QMap<QString, QString>()
{
    m_instance = this;

    fillProtocols();
}

SQ_ArchiveHandler::~SQ_ArchiveHandler()
{}

void SQ_ArchiveHandler::fillProtocols()
{
    // search for tar protocol
    if(!KGlobal::dirs()->findResource("services", "tar.protocol").isEmpty())
    {
        insert("application/x-tgz",  "tar");
        insert("application/x-tar",  "tar");
        insert("application/x-tarz", "tar");
        insert("application/x-tbz",  "tar");
        insert("application/x-tzo",  "tar");
    }

    // search for rar protocol (kio_rar from
    // http://kde-apps.org/content/show.php/kio_rar?content=17527)
    if(!KGlobal::dirs()->findResource("services", "rar.protocol").isEmpty())
    {
        insert("application/x-rar", "rar");
        insert("application/x-rar-compressed", "rar");
    }

    // search for ar protocol
    if(!KGlobal::dirs()->findResource("services", "ar.protocol").isEmpty())
        insert("application/x-archive", "ar");

    // search for iso protocol
    if(!KGlobal::dirs()->findResource("services", "iso.protocol").isEmpty())
        insert("application/x-iso", "iso");

    // search for zip protocol
    if(!KGlobal::dirs()->findResource("services", "zip.protocol").isEmpty())
        insert("application/x-zip", "zip");

    // search for 7zip protocol (kio_p7zip
    // from http://kde-apps.org/content/show.php/kio_p7zip?content=17829)
    if(!KGlobal::dirs()->findResource("services", "p7zip.protocol").isEmpty())
        insert("application/x-7z", "p7zip");
}

/*
 *  Find protocol name by mimetype name.
 * 
 *  For example findProtocolByMime(""application/x-tgz"") will
 *  return "tar".
 */
QString SQ_ArchiveHandler::findProtocolByMime(const QString &mime)
{
    // find protocol
    QMap<QString, QString>::iterator it = find(mime);

    // return protocol number, if found, and -1 otherwise
    return (it == end() ? QString::null : it.data());
}

/*
 *  Find protocol name by KFileItem's mimetype.
 */
QString SQ_ArchiveHandler::findProtocolByFile(KFileItem *item)
{
    return item ? findProtocolByMime(item->mimetype()) : QString::null;
}

void SQ_ArchiveHandler::tryUnpack(KFileItem *item)
{
    if(item) tryUnpack(item->url(), item->mimetype());
}

void SQ_ArchiveHandler::tryUnpack(const KURL &url, const QString &mime)
{
    QString prot = findProtocolByMime(mime);

    if(!prot.isEmpty())
    {
        // just change protocol, KIO::get() will do all for us
        KURL _url = url;
        _url.setProtocol(prot);

        emit unpack(_url);
    }
}

#include "sq_archivehandler.moc"
