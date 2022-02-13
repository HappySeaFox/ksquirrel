/***************************************************************************
                          sq_dragprovider.h  -  description
                             -------------------
    begin                : ??? Sep 17 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#ifndef SQ_DRAGPROVIDER_H
#define SQ_DRAGPROVIDER_H

#include <qobject.h>

#include <kfileitem.h>

class QWidget;

/*
 *  Class for making drag operations easier.
 *
 *  All fileview types (Details, Thumbnails...) use an instance of
 *  this class in startDrag() method.
 */

class SQ_DragProvider : public QObject
{
    public:
        enum SourceType { Icons, Thumbnails };

        SQ_DragProvider(QObject *parent);
        ~SQ_DragProvider();

        /*
         *  Set drag source (fileview), file list and fileview type.
         */
        void setParams(QWidget *_source, const KFileItemList &_files, SourceType tp);

        /*
         *  Start drag operation.
         */
        void start();

        static SQ_DragProvider* instance() { return m_inst; }

    private:
        QWidget *source;
        KFileItemList files;
        SourceType type;

        static SQ_DragProvider *m_inst;
};

#endif
