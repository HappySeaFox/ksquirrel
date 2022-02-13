/***************************************************************************
                          sq_canvaswidget.h  -  description
                             -------------------
    begin                : Fri Oct 6 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

/*
 *  Originally based on canvas example from Qt sources.
 */

#ifndef SQ_CANVASWIDGET_H
#define SQ_CANVASWIDGET_H

#include <qvbox.h>
#include <qcanvas.h>
#include <qimage.h>
#include <qrect.h>

class QPainter;

class SelectItem;

class SQ_CanvasWidget;

class SRect
{
    public:
        SRect(int,int,int,int);
        SRect();

        int x, y;
        int width, height;
};

class FigureEditor : public QCanvasView
{
    Q_OBJECT

    public:
        enum moveTT { moveXL, moveXR, moveYU, moveYD };

        FigureEditor(QCanvas *, SQ_CanvasWidget *parent = 0,
                        const char *name = 0, WFlags f = 0);
        void clear();

        SQ_CanvasWidget* canvasW() const;

    protected:
        virtual void contentsMousePressEvent(QMouseEvent *e);
        virtual void contentsMouseMoveEvent(QMouseEvent *e);
        virtual void contentsMouseReleaseEvent(QMouseEvent *e);

    signals:
        void mouse(int, int);
        void rectSelected(const SRect &);

    private:
        bool        mouseDraw, mouseMove, mouseMoveReal;
        int         xmoveold, ymoveold, xmove, ymove, moveType, wold, hold;
        SQ_CanvasWidget *cw;
};

class SQ_CanvasWidget : public QVBox
{
    Q_OBJECT

    public:
        SQ_CanvasWidget(QWidget* parent = 0, const char* name = 0);
        ~SQ_CanvasWidget();

        void setImage(const QImage &im);

        FigureEditor* editor() const;

        SelectItem* selection() const;

    protected:
        virtual void keyPressEvent(QKeyEvent *e);

    private slots:
        void zoomIn();
        void zoomOut();
        void moveL();
        void moveR();
        void moveU();
        void moveD();

    private:
        QCanvas *canvas;
        FigureEditor *m_editor;
        QImage butterflyimg;
        SelectItem *g;
};

inline
SQ_CanvasWidget* FigureEditor::canvasW() const
{
    return cw;
}

inline
FigureEditor* SQ_CanvasWidget::editor() const
{
    return m_editor;
}

inline
SelectItem* SQ_CanvasWidget::selection() const
{
    return g;
}

#endif
