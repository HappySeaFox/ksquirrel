/***************************************************************************
                          sq_glselectionpainter.h  -  description
                             -------------------
    begin                : Apr 4 2007
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

#ifndef SQ_GLSELECTIONPAINTER_H
#define SQ_GLSELECTIONPAINTER_H

#include <qwidget.h>
#include <qrect.h>

/*
 *  Selection itself.
 */
class SQ_GLSelection : public QWidget
{
    public:
        enum Type { Rectangle, Ellipse };

        SQ_GLSelection(QWidget *parent = 0, const char *name = 0, Type typ = Rectangle);
        ~SQ_GLSelection();

        void setType(Type tp);
        int type() const;

    protected:
        virtual void updateMask();

    private:
        Type m_type;
};

inline
void SQ_GLSelection::setType(Type tp)
{
    m_type = tp;
}

inline
int SQ_GLSelection::type() const
{
    return m_type;
}

/*
 *  This is a selection painter for SQ_GLWidget.
 *
 *  Selection can be shown as rectangle and ellipse.
 *
 *  Ellipsis selection mainly used in redeye filter.
 */

class SQ_GLSelectionPainter
{
    public:
        SQ_GLSelectionPainter(QWidget *widget);
        ~SQ_GLSelectionPainter();

        /*
         *  Set selection type - rectangle or ellipse
         */
        int type() const;

        /*
         *  when selection is drawn, it's valid.
         *  After end() it becomes invalid.
         */
        bool valid() const;

        void setVisible(bool vis);

        void begin(SQ_GLSelection::Type tp, int x, int y);
        void move(int x, int y);
        void setGeometry(const QRect &rc);
        void end();

        /*
         *  Selected rectangle geometry
         */
        QPoint pos() const;
        QSize size() const;

    private:
        QWidget *w;
        SQ_GLSelection *selection;

        int       xmoveold, ymoveold;
        bool      m_valid;
};

inline
QPoint SQ_GLSelectionPainter::pos() const
{
    return selection ? selection->pos() : QPoint();
}

inline
QSize SQ_GLSelectionPainter::size() const
{
    return selection ? selection->size() : QSize();
}

inline
int SQ_GLSelectionPainter::type() const
{
    return selection->type();
}

inline
bool SQ_GLSelectionPainter::valid() const
{
    return m_valid;
}

inline
void SQ_GLSelectionPainter::setGeometry(const QRect &rc)
{
    if(selection)
        selection->setGeometry(rc);
}

#endif
