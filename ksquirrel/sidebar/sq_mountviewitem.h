/***************************************************************************
                          sq_mountviewitem.h  -  description
                             -------------------
    begin                : ??? Feb 24 2007
    copyright            : (C) 2007 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_MOUNTVIEWITEM_H
#define SQ_MOUNTVIEWITEM_H

#include <klistview.h>

class SQ_MountViewItem : public KListViewItem
{
    public:
        SQ_MountViewItem(KListView *parent, const QString &mpoint);
        ~SQ_MountViewItem();

        bool mounted() const;
        void setMounted(bool b);

        QString device() const;
        void setDevice(const QString &dev);

    private:
        bool m_mounted;
        QString m_device;
};

inline
bool SQ_MountViewItem::mounted() const
{
    return m_mounted;
}

inline
QString SQ_MountViewItem::device() const
{
    return m_device;
}

inline
void SQ_MountViewItem::setDevice(const QString &dev)
{
    m_device = dev;
}

#endif
