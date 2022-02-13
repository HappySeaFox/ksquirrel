/*
    copyright : (C) 2004 by Baryshev Dmitry
    KSQuirrel - image viewer for KDE
*/

/*
Gwenview - A simple image viewer for KDE
Copyright (c) 2000-2003 Aur?lien G?teau

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
#ifndef SQ_THUMBNAILSIZE_H
#define SQ_THUMBNAILSIZE_H

#include <qstring.h>

/**
 * This is an "enum-like" class : an enum with cast operators to convert
 * from/to string and a few other methods
 */
class SQ_ThumbnailSize
{
    public:
        enum Size { Small = 0, Medium, Large, Huge };

        SQ_ThumbnailSize(Size value = Huge);
        SQ_ThumbnailSize(const QString& str);

        void setExtended(bool);
        bool extended() const;

        void setMargin(int);
        int margin() const;

        void setPixelSize(const QString &size);

        int pixelSize() const;
        int extendedPixelSize() const;

        bool operator== (const SQ_ThumbnailSize& size) const { return mValue == size.mValue; }
        operator const QString&() const;
        operator Size() const { return mValue; }

        int value() const;
        QString stringValue() const;

        static int biggest() { return 128; }

        static SQ_ThumbnailSize* instance();

    private:
        Size mValue;
        bool m_extended;
        int m_margin;

        static SQ_ThumbnailSize *thumb;
};

inline
int SQ_ThumbnailSize::value() const
{
    return (int)mValue;
}

inline
void SQ_ThumbnailSize::setExtended(bool ex)
{
    m_extended = ex;
}

inline
bool SQ_ThumbnailSize::extended() const
{
    return m_extended;
}

inline
void SQ_ThumbnailSize::setMargin(int m)
{
    m_margin = m;
}

inline
int SQ_ThumbnailSize::margin() const
{
    return m_margin;
}

#endif
