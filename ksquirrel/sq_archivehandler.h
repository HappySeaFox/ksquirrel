/***************************************************************************
                          sq_archivehandler.h  -  description
                             -------------------
    begin                : ??? ??? 26 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
  ***************************************************************************/

#ifndef SQ_ARCHIVEHANDLER_H
#define SQ_ARCHIVEHANDLER_H

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>

// forward declarations
class KFileItem;
class KURL;

#define SQ_AH SQ_ArchiveHandler

/*
 *  Helper class for archive extracting via kio-slaves (since 0.7.1).
 */

class SQ_ArchiveHandler : public QObject, QMap<QString, QString>
{
    Q_OBJECT

    public: 
        SQ_ArchiveHandler(QObject *parent = 0, const char *name = 0);
        ~SQ_ArchiveHandler();

        /*
         *  Find protocol name by mimetype name.
         * 
         *  For example findProtocolByMime(""application/x-tgz"") will
         *  return "tar".
         */
        QString findProtocolByMime(const QString &mime);

        /*
         *  Find protocol name by KFileItem's mimetype.
         */
        QString findProtocolByFile(KFileItem *item);

        /*
         *  Determine mimetype and emit unpack() signal
         */
        void tryUnpack(KFileItem *);

        void tryUnpack(const KURL &, const QString &mime);

        static SQ_ArchiveHandler* instance() { return m_instance; }

    private:
        void fillProtocols();

    signals:
        void unpack(const KURL &);

    private:
        static SQ_ArchiveHandler *m_instance;
};

#endif
