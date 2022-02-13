/***************************************************************************
                          sq_threaddirlister.h  -  description
                             -------------------
    begin                : Feb 10 2007
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


#ifndef SQ_THREADDIRLISTER_H
#define SQ_THREADDIRLISTER_H

#include <qthread.h>
#include <qevent.h> 
#include <qmutex.h>

#include <kurl.h>

class QObject;

class KConfig;

/********************************************************/

#define SQ_ItemsEventId (QEvent::User + 1)

class SQ_ItemsEvent : public QCustomEvent
{
    public:
        SQ_ItemsEvent(const KURL &parent, int c1, int c2)
            : QCustomEvent(SQ_ItemsEventId), m_files(c1), m_dirs(c2), m_url(parent)
        {}

        int files() const;
        int dirs() const;

        KURL url() const;

    private:
        int m_files, m_dirs;
        KURL m_url;
};

inline
KURL SQ_ItemsEvent::url() const
{
    return m_url;
}

inline
int SQ_ItemsEvent::files() const
{
    return m_files;
}

inline
int SQ_ItemsEvent::dirs() const
{
    return m_dirs;
}

/********************************************************/

/*
 *  This thread will read directory and determine
 *  number of files in it. Finally
 *  it will post an event to main thread with
 *  calculated count. This function is threaded
 *  due to perfomance reason.
 */
class SQ_ThreadDirLister : public QThread
{
    public:
        SQ_ThreadDirLister(QObject *o);
        ~SQ_ThreadDirLister();

        void appendURL(const KURL &url);
        void lock();
        void unlock();

    protected:
        virtual void run();

    private:
        // urls to read
        KURL::List todo;

        // this object will recieve our events
        QObject *obj;
        QMutex mutex;
        KConfig *cache;
};

inline
void SQ_ThreadDirLister::appendURL(const KURL &url)
{
    todo.append(url);
}

inline
void SQ_ThreadDirLister::lock()
{
//    mutex.lock();
}

inline
void SQ_ThreadDirLister::unlock()
{
//    mutex.unlock();
}

#endif
