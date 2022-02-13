/*
    copyright            : (C) 2004 by Baryshev Dmitry
    KSquirrel - image viewer for KDE
*/

/*
Gwenview - A simple image viewer for KDE
Copyright 2000-2003 Aur?lien G?teau

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sq_thumbnailsize.h"

SQ_ThumbnailSize * SQ_ThumbnailSize::m_instance = 0;

SQ_ThumbnailSize::SQ_ThumbnailSize(QObject *parent, Size value) 
    : QObject(parent), mValue(value), m_extended(false), m_margin(16)
{
    m_instance = this;
}

SQ_ThumbnailSize::SQ_ThumbnailSize(QObject *parent, const QString& str) 
    : QObject(parent), m_extended(false), m_margin(16)
{
    QString low = str.lower();

    if(low == "medium")
        mValue = Medium;
    else if(low == "large")
        mValue = Large;
    else
        mValue = Huge;
}

void SQ_ThumbnailSize::setPixelSize(const QString &size)
{
    QString low = size.lower();

    if(low == "medium")
        mValue = Medium;
    else if(low == "large")
        mValue = Large;
    else
        mValue = Huge;
}

QString SQ_ThumbnailSize::stringValue() const
{
    static QString sizeStr[3] = {"64","96","128"};

    return sizeStr[int(mValue)];
}
