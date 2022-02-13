/***************************************************************************
                          sq_canvaswidget.cpp  -  description
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

#include <qpainter.h>
#include <qlabel.h>
#include <qcursor.h>

#include "sq_canvaswidget.h"

static const int move_factor = 5;
static const int move_margin = 2;
static const int sa = 3;

SRect::SRect(int _x, int _y, int _w, int _h) : x(_x), y(_y), width(_w), height(_h)
{}

SRect::SRect() : x(0), y(0), width(0), height(0)
{}

class ImageItem: public QCanvasRectangle
{
    public:
        ImageItem(QImage img, QCanvas *canvas);

    protected:
        void drawShape(QPainter &);

    private:
        QImage image;
        QPixmap pixmap;
};


ImageItem::ImageItem(QImage img, QCanvas *canvas) : QCanvasRectangle(canvas), image(img)
{
    setSize(image.width(), image.height());
    pixmap.convertFromImage(image, OrderedAlphaDither);
}


void ImageItem::drawShape(QPainter &p)
{
    p.drawPixmap( int(x()), int(y()), pixmap );
}

class SelectItem: public QCanvasRectangle
{
    public:
        SelectItem(const QColor &c, QCanvas *canvas);

    protected:
        void drawShape(QPainter &);

    private:
        QColor cc;
};

SelectItem::SelectItem(const QColor &c, QCanvas *canvas) : QCanvasRectangle(canvas), cc(c)
{
    setSize(10, 10);
}

void SelectItem::drawShape(QPainter &p)
{
    p.save();

    p.setPen(QPen(cc.dark(120), 1));

    p.drawRect((int)x(), (int)y(), width(), height());
    p.fillRect((int)x(), (int)y(), width(), height(), QBrush(cc, Qt::DiagCrossPattern));

    p.restore();
}


FigureEditor::FigureEditor(QCanvas *c, SQ_CanvasWidget *parent, const char *name, WFlags f)
    : QCanvasView(c,parent,name,f), cw(parent)
{
    mouseDraw = false;
    mouseMove = false;
    mouseMoveReal = false;
}

void FigureEditor::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();

    for (;it != list.end();++it)
    {
        if(*it)
            delete *it;
    }
}

void FigureEditor::contentsMousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton && !mouseMove)
    {
        xmoveold = e->x();
        ymoveold = e->y();

        if(xmoveold > canvas()->width()-1 || ymoveold > canvas()->height()-1)
            return;

        mouseDraw = true;

        canvasW()->selection()->setSize(sa,sa);
        canvasW()->selection()->move(xmoveold, ymoveold);
        canvasW()->selection()->show();
        canvas()->update();
    }
    else if(e->button() == Qt::LeftButton && mouseMove)
    {
        mouseMoveReal = true;   
        xmoveold = (int)canvasW()->selection()->x();
        ymoveold = (int)canvasW()->selection()->y();
        wold = canvasW()->selection()->width();
        hold = canvasW()->selection()->height();
        canvasW()->selection()->show();
        canvas()->update();
    }
}

void FigureEditor::contentsMouseMoveEvent(QMouseEvent *e)
{
    xmove = e->x();
    ymove = e->y();

    if(xmove < 0) xmove = 0;
    if(ymove < 0) ymove = 0;
    if(xmove > canvas()->width()-1) xmove = canvas()->width()-1;
    if(ymove > canvas()->height()-1) ymove = canvas()->height()-1;

    emit mouse(xmove, ymove);

    if(e->state() == Qt::LeftButton && !mouseMoveReal && mouseDraw)
    {
        int X, Y, Xmin, Ymin;

        X = QMAX(xmove, xmoveold);
        Y = QMAX(ymove, ymoveold);
        Xmin = QMIN(xmove, xmoveold);
        Ymin = QMIN(ymove, ymoveold);

        canvasW()->selection()->move(Xmin, Ymin);
        canvasW()->selection()->setSize(X-Xmin, Y-Ymin);
        canvas()->update();

        SRect rct(Xmin, Ymin, X-Xmin, Y-Ymin);

        emit rectSelected(rct);
    }
    else if(e->state() == Qt::LeftButton && mouseMoveReal)
    {
        int X, Y, Xmin, Ymin, ss;

        X = QMAX(xmove, xmoveold);
        Y = QMAX(ymove, ymoveold);
        Xmin = QMIN(xmove, xmoveold);
        Ymin = QMIN(ymove, ymoveold);

        if(moveType == moveXL)
        {
            ss = wold - xmove + xmoveold;

            if(ss > sa)
            {
                canvasW()->selection()->move(xmove, canvasW()->selection()->y());
                canvasW()->selection()->setSize(ss, canvasW()->selection()->height());
            }
        }
        else if(moveType == moveXR)
        {
            ss = xmove - xmoveold;

            if(ss > sa)
            {
                canvasW()->selection()->setSize(ss, canvasW()->selection()->height());
            }
        }
        else if(moveType == moveYU)
        {
            ss = hold - ymove + ymoveold;

            if(ss > sa)
            {
                canvasW()->selection()->move(canvasW()->selection()->x(), ymove);
                canvasW()->selection()->setSize(canvasW()->selection()->width(), ss);
            }
        }
        else if(moveType == moveYD)
        {
            ss = ymove - ymoveold;

            if(ss > sa)
            {
                canvasW()->selection()->setSize(canvasW()->selection()->width(), ss);
            }
        }

        canvas()->update();
        emit rectSelected(SRect((int)canvasW()->selection()->x(), (int)canvasW()->selection()->y(),
                                    (int)canvasW()->selection()->width(), (int)canvasW()->selection()->height()));
    }
    else if(e->state() == Qt::NoButton)
    {
        SelectItem *g = canvasW()->selection();

        if(xmove < g->x()+move_margin && xmove > g->x()-move_margin && ymove > g->y() && ymove < g->y()+g->height())
        {
            setCursor(Qt::SizeHorCursor);
            mouseMove = true;
            moveType = moveXL;
        }
        else if(xmove < g->x()+g->width()+move_margin && xmove > g->x()+g->width()-move_margin && ymove > g->y() && ymove < g->y()+g->height())
        {
            setCursor(Qt::SizeHorCursor);
            mouseMove = true;
            moveType = moveXR;
        }
        else if(xmove > g->x() && xmove < g->x()+g->width() && ymove > g->y()-move_margin && ymove < g->y()+move_margin)
        {
            setCursor(Qt::SizeVerCursor);
            moveType = moveYU;
            mouseMove = true;
        }
        else if(xmove > g->x() && xmove < g->x()+g->width() && ymove > g->y()+g->height()-move_margin && ymove < g->y()+g->height()+move_margin)
        {
            setCursor(Qt::SizeVerCursor);
            moveType = moveYD;
            mouseMove = true;
        }
        else
        {
            unsetCursor();
            mouseMove = false;
        }
    }
}

void FigureEditor::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if(e->state() == Qt::RightButton)
    {
        canvasW()->selection()->hide();
        canvas()->update();
    }

    mouseDraw = false;
    mouseMoveReal = mouseMove = false;
}

SQ_CanvasWidget::SQ_CanvasWidget(QWidget* parent, const char* name) : QVBox(parent,name)
{
    canvas = new QCanvas;
    canvas->setAdvancePeriod(86400000);

    m_editor = new FigureEditor(canvas, this);

    m_editor->viewport()->setMouseTracking(true);

    m_editor->viewport()->setMouseTracking(true);

    canvas->setDoubleBuffering(true);

    g = new SelectItem(QColor(230,0,103), canvas);

    g->setZ(1);
}

SQ_CanvasWidget::~SQ_CanvasWidget()
{}

void SQ_CanvasWidget::zoomIn()
{
    QWMatrix m = m_editor->worldMatrix();
    m.scale( 2.0, 2.0 );
    m_editor->setWorldMatrix(m);
}

void SQ_CanvasWidget::zoomOut()
{
    QWMatrix m = m_editor->worldMatrix();
    m.scale( 0.5, 0.5 );
    m_editor->setWorldMatrix(m);
}

void SQ_CanvasWidget::moveL()
{
    m_editor->scrollBy(-move_factor, 0);
}

void SQ_CanvasWidget::moveR()
{
    m_editor->scrollBy(move_factor, 0);
}

void SQ_CanvasWidget::moveU()
{
    m_editor->scrollBy(0, -move_factor);
}

void SQ_CanvasWidget::moveD()
{
    m_editor->scrollBy(0, move_factor);
}

void SQ_CanvasWidget::setImage(const QImage &im)
{
    canvas->resize(im.width(), im.height());

    QCanvasPolygonalItem* i = new ImageItem(im.swapRGB(), canvas);

    i->move(0,0);
    i->show();
    i->setZ(0);
}

void SQ_CanvasWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Left)
        moveL();
    else if(e->key() == Qt::Key_Right)
        moveR();
    else if(e->key() == Qt::Key_Up)
        moveU();
    else if(e->key() == Qt::Key_Down)
        moveD();
    else if(e->key() == Qt::Key_Plus)
        zoomIn();
    else if(e->key() == Qt::Key_Minus)
        zoomOut();

    e->accept();
}
