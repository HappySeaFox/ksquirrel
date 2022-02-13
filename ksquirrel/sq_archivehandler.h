/***************************************************************************
                          sq_archivehandler.h  -  description
                             -------------------
    begin                : ??? ??? 26 2004
    copyright            : (C) 2004 by Baryshev Dmitry
    email                : ksquirrel@tut.by
  ***************************************************************************/

/* Originally taken from ShowImg */

/***************************************************************************
                          extract.h  -  description
                             -------------------
    begin                : Sat Dec 1 2001
    copyright            : (C) 2001 by Richard Groult, 2003 OGINO Tomonori
    email                : rgroult@jalix.org ogino@nn.iij4u.or.jp
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_ARCHIVEHANDLER_H
#define SQ_ARCHIVEHANDLER_H

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>

// forward declarations
class KFileItem;

class SQ_Dir;

#define SQ_AH SQ_ArchiveHandler

/*
 *  Class for archive extracting.
 */

class SQ_ArchiveHandler : public QObject
{
	public: 
		SQ_ArchiveHandler(QObject * parent = 0, const char *name = 0);
		~SQ_ArchiveHandler();

		/*
		 *  Find protocol number by its name.
		 * 
		 *  For example findProtocolByName(""application/x-bzip"") will
		 *  return 0.
		 */
		int findProtocolByName(const QString &prot);

		/*
		 *  Find protocol number by KFileItem's mimetype.
		 */
		int findProtocolByFile(KFileItem *item);

		/*
		 *  Store fileitem, which we should be unpacked.
		 *  Also store full path to it.
		 */
		void setFile(KFileItem *);

		/*
		 *  Return fileitem's full path.
		 * 
		 *  For example, "/home/krasu/arc.zip"
		 */
		QString itemPath() const;

		/*
		 *  Get full path to extracted directory.
		 *
		 *  For example, after unpacking /home/krasu/arc.zip this method will return
		 *  "/home/krasu/.ksquirrel/extracts/home/krasu/arc.zip".
		 */
		QString itemExtractedPath() const;

		/*
		 *  Unpack an archive.
		 *
		 *  Use 'fullpath' as a path to archive.
		 */
		bool unpack();

		/*
		 *  Does temporary directory need to be cleaned ?
		 *
		 *  Temporary directory is ususally ~/.ksquirrel/extracts/.
		 */
		bool needClean();

		/*
		 *  Remove specified directory from temporary directory.
		 *
		 *  For example, clean("/home/krasu/arc.zip") will remove
		 *  /home/krasu/.ksquirrel/extracts/home/krasu/arc.zip.
		 */
		void clean(QString s);

		static SQ_ArchiveHandler* instance();

	private:
		QString	extracteddir, fullextracteddir;

		// full path to archive
		QString	fullpath;

		// Helper object for creating temporary directories
		SQ_Dir	*dir;

		/* Supported protocols.
		 * We use mimetype as key, and non-negative value as data.
		 *
		 * For example, 
		 * "application/x-bzip" => 0
		 * "application/x-bzip2" => 1
		 * ...
		 */
		QMap<QString, int> protocols;

		// Fileitem
		KFileItem *item;

		static SQ_ArchiveHandler *ar;
};

#endif
