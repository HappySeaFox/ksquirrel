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
#include <qwmatrix.h>
#include <qimage.h>

#include <kurl.h>

class KPopupMenu;

class SQ_Downloader;

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

        void load(const KURL &url);

        void ignore(bool ign);

        void loadPending();

        static SQ_PreviewWidget* instance() { return m_inst; }

        int delay() const;

        bool cancel() const;

        KURL url() const;

    private:
        void saveValues();

    signals:
        void next();
        void previous();
        void execute();

    private slots:
        void slotBackground();
        void slotText();
        void slotDownloadResult(const KURL &);

    protected:
        virtual void resizeEvent(QResizeEvent *);
        virtual void paintEvent(QPaintEvent *);
        virtual void mousePressEvent(QMouseEvent *e);
        virtual void keyPressEvent(QKeyEvent *e);
        virtual void wheelEvent(QWheelEvent *e);
        virtual void mouseDoubleClickEvent(QMouseEvent *e);

    private:
        bool fit();
        void fitAndConvert();

    private:
        QImage *all, *small;
        QPixmap pixmap;
        bool m_ignore, m_forceignore, m_cancel;
        QColor color, colorText;
        KPopupMenu *popup;
        KURL pending, m_url;
        int m_delay;
        QWMatrix matrix;
        SQ_Downloader *down;
        bool multi;
        QPixmap multi_pix;
        QString dimstring;
        bool dim;

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

inline
KURL SQ_PreviewWidget::url() const
{
    return m_url;
}

#endif
