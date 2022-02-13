/***************************************************************************
                          sq_previewwidget.h  -  description
                             -------------------
    begin                : ??? Mar 13 2007
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

#ifndef SQ_PREVIEWWIDGET_H
#define SQ_PREVIEWWIDGET_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qcolor.h>

#include <kurl.h>

class KPopupMenu;

struct RGBA;

/**
  *@author Baryshev Dmitry
  */

class SQ_PreviewWidget : public QWidget
{
    Q_OBJECT

    public: 
	SQ_PreviewWidget(QWidget *parent = 0, const char *name = 0);
	~SQ_PreviewWidget();

        void rereadColor();

        void saveValues();

        void load(const KURL &url);

        void ignore(bool ign);

        void loadPending();

        static SQ_PreviewWidget* instance() { return m_inst; }

        int delay() const;

        bool cancel() const;

    private slots:
        void slotBackground();

    protected:
        virtual void resizeEvent(QResizeEvent *);
        virtual void paintEvent(QPaintEvent *);
        virtual void mousePressEvent(QMouseEvent *e);

    private:
        bool fit();

    private:
        RGBA *all, *small;
        QPixmap pixmap;
        int w, h, smallw, smallh;
        bool m_ignore, m_forceignore, m_cancel;
        QColor color;
        KPopupMenu *popup;
        KURL pending;
        int m_delay;

        static SQ_PreviewWidget *m_inst;
};

inline
void SQ_PreviewWidget::ignore(bool ign)
{
    m_ignore = ign;
}

inline
int SQ_PreviewWidget::delay() const
{
    return m_delay;
}

inline
bool SQ_PreviewWidget::cancel() const
{
    return m_cancel;
}

#endif
