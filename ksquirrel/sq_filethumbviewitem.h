/***************************************************************************
                          sq_filethumbviewitem.h  -  description
                             -------------------
    begin                : ??? ??? 12 2004
    copyright            : (C) 2004 by Baryshev Dmitry
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

#ifndef SQ_FILETHUMBVIEWITEM_H
#define SQ_FILETHUMBVIEWITEM_H

#include <qpixmap.h>

#include <kfileiconview.h>

/*
 *  SQ_FileThumbViewItem represents a thumbnail item in SQ_FileThumbView.
 *  Additianally it stores some useful info.
 */

class SQ_FileThumbViewItem : public KFileIconViewItem
{
    public: 
        SQ_FileThumbViewItem(QIconView *parent, const QString &text, const QPixmap &pixmap, KFileItem *fi);
        ~SQ_FileThumbViewItem();

        bool listed() const;

        void setListed(bool l);

    protected:
        /*
         *  Painting routines.
         */
        virtual void paintItem(QPainter *p, const QColorGroup &cg);
        virtual void paintFocus(QPainter *p, const QColorGroup &cg);
        void paintText(QPainter *p, const QColorGroup &c);

    private:
        bool m_listed;
};

inline
bool SQ_FileThumbViewItem::listed() const
{
    return m_listed;
}

inline
void SQ_FileThumbViewItem::setListed(bool l)
{
    m_listed = l;
}

#endif
