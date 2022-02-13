/***************************************************************************
                          sq_glwidget.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qeventloop.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qslider.h>
#include <qlabel.h>

#include <kapplication.h>
#include <kaction.h>
#include <kcursor.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstringhandler.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kio/job.h>
#include <ktempfile.h>
#include <kwin.h>
#include <ktabbar.h>

#include <cmath>
#include <cstdlib>

#include "ksquirrel.h"
#include "sq_widgetstack.h"
#include "sq_fileiconview.h"
#include "sq_diroperator.h"
#include "sq_popupmenu.h"
#include "sq_libraryhandler.h"
#include "sq_config.h"
#include "sq_glwidget_helpers.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_glu.h"
#include "sq_errorstring.h"
#include "sq_codecsettings.h"
#include "sq_previewwidget.h"
#include "sq_glselectionpainter.h"
#include "sq_thumbnailloadjob.h"
#include "sq_thumbnailsize.h"
#include "sq_hloptions.h"
#include "sq_utils.h"
#include "sq_tabopendialog.h"

#include "fmt_filters.h"

#include <ksquirrel-libs/fileio.h>
#include <ksquirrel-libs/fmt_codec_base.h>
#include <ksquirrel-libs/fmt_utils.h>
#include <ksquirrel-libs/error.h>

#ifdef SQ_HAVE_KEXIF
#include <libkexif/kexifdata.h>
#endif

// is it enough ?
#define SQ_FLOAT_SIGMA 1e-5

#define ISFLOAT(a,b) (fabs(a - b) < SQ_FLOAT_SIGMA)
#define ISFLOAT1(a) ISFLOAT(a, 1.0)
#define ISFLOAT0(a) ISFLOAT(a, 0.0)

#define SQ_TAB_TEXT_LENGTH 30

/* ***************************************************************************************** */

SQ_GLWidget * SQ_GLWidget::m_instance = 0;

static const int timer_delay_file = 5;
static const double rad_const = 3.14159265358979323846 / 180.0;

static const float SQ_WINDOW_BACKGROUND_POS = -1000.0;
static const float SQ_IMAGE_CHECKER_POS =     -999.0;
static const float SQ_FIRST_FRAME_POS =       -998.0;
static const float SQ_MARKS_POS =             -997.0;
static const float SQ_FIRST_TILE_LAYER =      -995.0;
static const float SQ_ONSCREEN_LAYER =      10000.0;

/* ***************************************************************************************** */

SQ_GLWidget::SQ_GLWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
    kdDebug() << "+SQ_GLWidget" << endl;

    // init all variables...
    m_instance = this;
    ac = new KActionCollection(this, this, "GLWidget actionCollection");
    changed = blocked = decoded = reset_mode = false;
    movetype = -1;
    buffer = new RGBA [512 * 512];
    zoomFactor = 1.0;
    old_id = -1;
    menu = new QPopupMenu(this);
    hackResizeGL = false;
    lastCopy = KURL::fromPathOrURL("/");
    oldZoom = -1;

    percentsLabel = new QLabel(this);
    percentsLabel->move(4, 4);
    percentsLabel->hide();

    tabold = tab = &taborig;

    tmp = new KTempFile;
    tmp->setAutoDelete(true);
    tmp->close();

    SQ_Config::instance()->setGroup("GL view");
    zoom_type = SQ_Config::instance()->readNumEntry("zoom type", 2);
    linear = SQ_Config::instance()->readBoolEntry("zoom_nice", true);

    // load background for transparent image
    BGquads = QImage(locate("appdata", "images/checker.png"));

    if(BGquads.isNull())
    {
        BGquads = QImage(32, 32, 32).copy();
        BGquads.setAlphaBuffer(true);
        BGquads.fill(0);
    }

    zoomfactor = SQ_Config::instance()->readNumEntry("zoom", 25);
    movefactor = SQ_Config::instance()->readNumEntry("move", 5);
    rotatefactor = SQ_Config::instance()->readNumEntry("angle", 90);

    // load cursors
    setCursor(KCursor::arrowCursor());

    setFocusPolicy(QWidget::WheelFocus);
    setAcceptDrops(true);

    // popup menu with image pages
    images = new KPopupMenu;
    images->setCheckable(true);

    // create actions
    createActions();

    // create toolbars
    createToolbar();

    // create tickmarks
    createMarks();

    initAccelsAndMenu();

    enableActions(false);

    KCursor::setAutoHideCursor(this, true);
    KCursor::setHideCursorDelay(2500);

    timer_prev = new QTimer(this);
    timer_next = new QTimer(this);

    Q_CHECK_PTR(timer_prev);
    Q_CHECK_PTR(timer_next);

    connect(timer_prev, SIGNAL(timeout()), SQ_WidgetStack::instance(), SLOT(emitPreviousSelected()));
    connect(timer_next, SIGNAL(timeout()), SQ_WidgetStack::instance(), SLOT(emitNextSelected()));

    timer_anim = new QTimer(this);

    Q_CHECK_PTR(timer_anim);

    connect(timer_anim, SIGNAL(timeout()), this, SLOT(slotAnimateNext()));

    connect(images, SIGNAL(activated(int)), this, SLOT(slotSetCurrentImage(int)));
    connect(images, SIGNAL(aboutToHide()), this, SLOT(slotImagesHidden()));
    connect(images, SIGNAL(aboutToShow()), this, SLOT(slotImagesShown()));

    gls = new SQ_GLSelectionPainter(this);
}

SQ_GLWidget::~SQ_GLWidget()
{
    kdDebug() << "-SQ_GLWidget" << endl;

    delete gls;

    delete parts_broken;

    removeCurrentTabs();

    delete selectionMenu;
    delete zoom;
    delete images;
    delete buffer;
    delete tmp;
}

// Initialize OpenGL context. Used internally by QGLWidget.
void SQ_GLWidget::initializeGL()
{
    setClearColor();
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_FLAT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    // finally, clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // create "broken" image
    initBrokenImage();

    initMarks();
}

// Resize OpenGL context. Used internally by QGLWidget.
void SQ_GLWidget::resizeGL(int width, int height)
{
    gls->setSourceSize(width, height);

    // set new viewport
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // O(0,0) will be in the center of the window
    glOrtho(-width/2, width/2, -height/2, height/2, 0.1f, 10000.0);

    // camera setup
    SQ_GLU::gluLookAt(0,0,1, 0,0,0, 0,1,0);
    glMatrixMode(GL_MODELVIEW);

    if(decoded && !hackResizeGL)
        slotZoomIfLess();

    hackResizeGL = false;
}

/*
 *  Fill a w x h region with texture. Generate texture if needed.
 */
void SQ_GLWidget::draw_background(void *bits, unsigned int *tex, int dim, GLfloat w, GLfloat h, bool &bind, bool deleteOld)
{
    float half_w, half_h;

    half_w = w / 2.0;
    half_h = h / 2.0;

    // bind texture ?
    if(bind)
    {
        // delete old texture before binding ?
        if(deleteOld)
            glDeleteTextures(1, tex);

        // generate and bind texture
        glGenTextures(1, tex);
        glBindTexture(GL_TEXTURE_2D, *tex);

        // setup parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // assign image to texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

        bind = false;
    }
    else
        glBindTexture(GL_TEXTURE_2D, *tex);

    // draw
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);                          glVertex2f(-half_w, half_h);
        glTexCoord2f(w/(GLfloat)dim, 0.0);               glVertex2f(half_w, half_h);
        glTexCoord2f(w/(GLfloat)dim, h/(GLfloat)dim);    glVertex2f(half_w, -half_h);
        glTexCoord2f(0.0, h/(GLfloat)dim);               glVertex2f(-half_w, -half_h);
    glEnd();
}

// Redraw OpenGL context. Used internally by QGLWidget.
void SQ_GLWidget::paintGL()
{
    int z;

    // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(gls->valid())
    {
        matrix_push();
        matrix_pure_reset();
        QPoint p = gls->center();

        // move to selection center
        MATRIX_X = p.x();
        MATRIX_Y = p.y();
        write_gl_matrix();

        gls->draw();

        matrix_pop();
        write_gl_matrix();
    }

    glEnable(GL_TEXTURE_2D);

    // draw window background ?
    SQ_Config::instance()->setGroup("GL view");
    if(SQ_Config::instance()->readNumEntry("GL view background type", 1) == 2)
    {
        static bool del = false;
        matrix_push();
        matrix_pure_reset();
        matrix_move_z(SQ_WINDOW_BACKGROUND_POS);
        draw_background(BGpixmap.bits(), &texPixmap, BGpixmap.width(), width(), height(), changed, del);
        del = true;
        matrix_pop();
        write_gl_matrix();
    }

    // draw image
    if(!reset_mode && decoded)
    {
        SQ_Config::instance()->setGroup("GL view");

        fmt_image *im = &tab->finfo.image[tab->current];

        // if the image is transparent, and we should draw background for image
        if(im->hasalpha && SQ_Config::instance()->readBoolEntry("alpha_bkgr", true))
        {
            static bool t = true;
            GLfloat w = (float)im->w / 2.0, h = (float)im->h / 2.0;

            static GLdouble eq[4][4] =
            {
                {0.0, 1.0, 0.0, 0.0},
                {1.0, 0.0, 0.0, 0.0},
                {0.0, -1.0, 0.0, 0.0},
                {-1.0, 0.0, 0.0, 0.0}
            };

            // we will draw background for transparent image (quads) within
            // entire window, and cut off useless regions with clip planes
            glPushMatrix();
            glTranslatef(-w, -h, 0.0);
            glClipPlane(GL_CLIP_PLANE0, eq[0]);
            glClipPlane(GL_CLIP_PLANE1, eq[1]);
            glEnable(GL_CLIP_PLANE0);
            glEnable(GL_CLIP_PLANE1);
            glPopMatrix();
            glPushMatrix();
            glTranslatef(w, h, 0.0);
            glClipPlane(GL_CLIP_PLANE2, eq[2]);
            glClipPlane(GL_CLIP_PLANE3, eq[3]);
            glEnable(GL_CLIP_PLANE2);
            glEnable(GL_CLIP_PLANE3);
            glPopMatrix();

            // draw background
            matrix_push();
            matrix_pure_reset();
            matrix_move_z(SQ_IMAGE_CHECKER_POS);
            draw_background(BGquads.bits(), &texQuads, 32, width(), height(), t, !t);
            matrix_pop();
            write_gl_matrix();

            // don't need planes any more...
            glDisable(GL_CLIP_PLANE3);
            glDisable(GL_CLIP_PLANE2);
            glDisable(GL_CLIP_PLANE1);
            glDisable(GL_CLIP_PLANE0);
        }

        matrix_move_z(SQ_FIRST_FRAME_POS);

        Parts *pt = tab->broken ? parts_broken : &tab->parts[tab->current];

        // draw current image
        int toy = pt->tilesy.size();
        int tox = pt->tilesx.size();
        for(z = 0;z < toy;z++)
            if(glIsList(pt->m_parts[z * tox].list))
                glCallList(pt->m_parts[z * tox].list);

        // draw tickmarks ("broken" image won't have tickmarks)
        if(!tab->broken && marks && SQ_Config::instance()->readBoolEntry("marks", true))
        {
            GLfloat zum = getZoom();
            GLfloat x = fabsf(pt->m_parts[0].x1) * zum, y = pt->m_parts[0].y1 * zum;
            GLfloat X = MATRIX_X, Y = MATRIX_Y;

            if(x < 0.0)
                x = -x;

            const GLfloat ly = y+16, ry = -y-16;
            const GLfloat lx = x+16, rx = -x-16;

            matrix_push();
            matrix_pure_reset();
            MATRIX_X = X;
            MATRIX_Y = Y;
            matrix_rotate2(tab->curangle);
            matrix_move_z(SQ_MARKS_POS);

            GLfloat coords[4][8] =
            {
                {rx, ly,      -x, ly,      -x, y,       rx, y},
                {x, ly,       lx, ly,      lx, y,       x, y},
                {x, -y,       lx, -y,      lx, ry,      x, ry},
                {rx, -y,      -x, -y,      -x, ry,      rx, ry}
            };

            for(z = 0;z < 4;z++)
            {
                glBindTexture(GL_TEXTURE_2D, mark[z]);

                glBegin(GL_QUADS);
                    glTexCoord2f(0.0, 0.0);    glVertex2f(coords[z][0], coords[z][1]);
                    glTexCoord2f(1.0, 0.0);    glVertex2f(coords[z][2], coords[z][3]);
                    glTexCoord2f(1.0, 1.0);    glVertex2f(coords[z][4], coords[z][5]);
                    glTexCoord2f(0.0, 1.0);    glVertex2f(coords[z][6], coords[z][7]);
                glEnd();
            }

            matrix_pop();
            write_gl_matrix();
        }
    }

    glDisable(GL_TEXTURE_2D);

    matrixChanged();

    if(!tab->broken && tab->total > 1)
        frameChanged();
}

/*
 *  Change statusbar info according with
 *  current matrix (it shows current zoom & angle values).
 */
void SQ_GLWidget::matrixChanged()
{
    QString str;

    float m = getZoom();
    float zoom = m * 100.0;
    float z = (m < 1.0) ? 1.0/m : m;

    // construct zoom
    str = QString::fromLatin1("%1% [%2:%3]")
            .arg(zoom, 0, 'f', 1)
            .arg((m < 1.0)?1.0:z, 0, 'f', 1)
            .arg((m > 1.0)?1.0:z, 0, 'f', 1);

    SQ_GLView::window()->sbarWidget("SBGLZoom")->setText(str);

    // construct rotation angle
    str = QString::fromLatin1("%1%2 %3 deg")
            .arg((tab->isflippedV)?"V":"")
            .arg((tab->isflippedH)?"H":"")
            .arg(tab->curangle, 0, 'f', 1);

    SQ_GLView::window()->sbarWidget("SBGLAngle")->setText(str);
}

/*
 *  Mouse wheel event. Let's load next/previous image, or
 *  zoom in/zoom out (depends on settings).
 */
void SQ_GLWidget::wheelEvent(QWheelEvent *e)
{
    if(e->delta() < 0 && e->state() == Qt::NoButton)
    {
        SQ_Config::instance()->setGroup("GL view");

        // load next file in current directory
        if(SQ_Config::instance()->readNumEntry("scroll", 0))
            slotNext();
        else
            // zoom image otherwise
            slotZoomPlus();
    }
    else if(e->delta() > 0 && e->state() == Qt::NoButton)
    {
        SQ_Config::instance()->setGroup("GL view");

        if(SQ_Config::instance()->readNumEntry("scroll", 0))
            slotPrev();
        else
            slotZoomMinus();
    }
    // some special bindings:
    // if CTRL key is pressed, zoom 2x or 0.5x
    else if(e->delta() < 0 && e->state() == Qt::ControlButton)
        matrix_zoom(2.0);
    else if(e->delta() > 0 && e->state() == Qt::ControlButton)
        matrix_zoom(0.5f);
    else if(e->delta() < 0 && e->state() == Qt::ShiftButton)
        slotZoomPlus();
    else if(e->delta() > 0 && e->state() == Qt::ShiftButton)
        slotZoomMinus();
}

// User pressed mouse button down.
void SQ_GLWidget::mousePressEvent(QMouseEvent *e)
{
    setFocus();

    // left button, update cursor
    if(e->button() == Qt::LeftButton && e->state() == Qt::NoButton && tab->glselection == -1)
    {
        QTime t = QTime::currentTime();

        SQ_Config::instance()->setGroup("GL view");
        int dc = SQ_Config::instance()->readNumEntry("double_click", 0);

        if(dc && clickTime.isValid() && clickTime.msecsTo(t) <= KApplication::doubleClickInterval())
        {
            if(dc == 1)
                KSquirrel::app()->closeGLWidget();
            else
                toggleFullScreen();

            return;
        }
        else
            clickTime = t;

        setCursor(KCursor::sizeAllCursor());

        xmoveold = e->x();
        ymoveold = e->y();

        movetype = 1;
    }
    // left button + SHIFT, let's start drawing zoom frame
    else if(e->button() == Qt::LeftButton && (e->state() == Qt::ShiftButton || tab->glselection != -1))
    {
        // stop animation!
        stopAnimation();

        // update cursor to crosshair
        setCursor(KCursor::crossCursor());

        if(tab->glselection == SQ_GLSelectionPainter::Rectangle || tab->glselection == SQ_GLSelectionPainter::Ellipse)
            gls->begin(static_cast<SQ_GLSelectionPainter::Type>(tab->glselection), e->x(), e->y());
        else
            gls->begin(SQ_GLSelectionPainter::Rectangle, e->x(), e->y());

        movetype = 2;
    }
    // right button - show context menu
    else if(e->button() == Qt::RightButton)
        menu->popup(QCursor::pos());
    // middle button - toggle fullscreen state
    else if(e->button() == Qt::MidButton)
        toggleFullScreen();
}

// User moved mouse.
void SQ_GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    // user didn't press any mouse button before ?
    if(movetype == -1 && fullscreen())
    {
#if 0
//        bool tvis = SQ_GLView::window()->toolbar()->isShown();
//        bool svis = SQ_GLView::window()->statusbar()->isShown();

        int h = SQ_GLView::window()->toolbar()->height() +
                SQ_GLView::window()->tabbar()->height();

#warning FIXME
        hackResizeGL = true;
        SQ_GLView::window()->boxBar()->setShown((/*tvis ? false:*/(e->y() < h)));
        SQ_GLView::window()->statusbar()->setShown((/*svis ? false:*/(e->y() >
                        height()-SQ_GLView::window()->statusbar()->height())));
#endif
        return;
    }

    // left mouse button, lets move image (or matrix :-) )
    if(movetype == 1)
    {
        xmove = e->x();
        ymove = e->y();

        matrix_move(xmove-xmoveold, -ymove+ymoveold);

        xmoveold = e->x();
        ymoveold = e->y();
    }
    // left + SHIFT
    else if(movetype == 2)
        gls->move(e->x(), e->y());
}

// User released some mouse button.
void SQ_GLWidget::mouseReleaseEvent(QMouseEvent *)
{
    if(movetype == -1)
        return;

    // left button - restore cursor
    if(movetype == 1 || (movetype == 2 && tab->glselection != -1)) // permanent selection
        setCursor(KCursor::arrowCursor());
    // left button + SHIFT - zoom to selected rectangle (if needed)
    else if(movetype == 2 && tab->glselection == -1)
    {
        setCursor(KCursor::arrowCursor());

        QSize sz = gls->size();
        QPoint pt = gls->pos();
        QRect lastRect(pt.x(), pt.y(), sz.width(), sz.height());

        gls->end();

        QPoint lastC = lastRect.center();
        QPoint O(width() / 2, height() / 2);

        if(lastRect.width() > 2 && lastRect.height() > 2)
        {
            bool lastReset = reset_mode;
            reset_mode = true;
            float X = MATRIX_X, Y = MATRIX_Y;
            matrix_move(O.x() - lastC.x(), lastC.y() - O.y());
            reset_mode = lastReset;

            // try to zoom
            bool zoomed = tab->broken ? false : zoomRect(lastRect);

            // not zoomed ? (zoom > maximum zoom)
            if(!zoomed)
            {
                MATRIX_X = X;
                MATRIX_Y = Y;
                write_gl_matrix();
            }
        }

        updateGL();

        // start animation, if needed
        if(!manualBlocked())
            startAnimation();
    }

    movetype = -1;
}

/*
 *  Zoom to 'r'. Will be called after somebody used right mouse button
 *  to select zoom region.
 */
bool SQ_GLWidget::zoomRect(const QRect &r)
{
    // calculate zoom factor
    float factor = 1.0;
    float w = (float)width(), h = (float)height();
    float factorw = w / (float)r.width();
    float factorh = h / (float)r.height();
    float t = w / h;

    if(t > (float)r.width() / (float)r.height())
        factor = factorh;
    else
        factor = factorw;

    // try to zoom
    return matrix_zoom(factor);
}

// Fit width.
void SQ_GLWidget::slotZoomW()
{
    zoom_type = 0;
    pAZoomW->setChecked(true);

    // no image decoded
    if(tab->broken || tab->finfo.image.empty()) return;

    // calculate zoom factor
    float factor = (float)width() / (tab->rotate ? (float)tab->finfo.image[tab->current].h : (float)tab->finfo.image[tab->current].w);

    // "Ignore, if the image is less than window" - restore zoom factor to 1.0
    if(pAIfLess->isChecked() && (tab->finfo.image[tab->current].w < width() && tab->finfo.image[tab->current].h < height()))
        factor = 1.0;

    // zoom...
    internalZoom(factor);
}

// Fit height.
void SQ_GLWidget::slotZoomH()
{
    zoom_type = 1;
    pAZoomH->setChecked(true);

    if(tab->broken || tab->finfo.image.empty()) return;

    float factor = (float)height() / (tab->rotate ? (float)tab->finfo.image[tab->current].w : (float)tab->finfo.image[tab->current].h);

    if(pAIfLess->isChecked() && (tab->finfo.image[tab->current].w < width() && tab->finfo.image[tab->current].h < height()))
        factor = 1.0;

    internalZoom(factor);
}

// Fit image (e.g. width and height).
void SQ_GLWidget::slotZoomWH()
{
    zoom_type = 2;
    pAZoomWH->setChecked(true);

    if(tab->broken || tab->finfo.image.empty()) return;

    float factor = 1.0;
    float w = (float)width(), h = (float)height();
    float factorw = w / (tab->rotate  ? (float)tab->finfo.image[tab->current].h : (float)tab->finfo.image[tab->current].w);
    float factorh = h / (tab->rotate ? (float)tab->finfo.image[tab->current].w : (float)tab->finfo.image[tab->current].h);
    float t = w / h;
    float F = tab->rotate ? ((float)tab->finfo.image[tab->current].h / (float)tab->finfo.image[tab->current].w) : ((float)tab->finfo.image[tab->current].w / (float)tab->finfo.image[tab->current].h);

    if(t > F)
        factor = factorh;
    else
        factor = factorw;

    if(pAIfLess->isChecked() && (tab->finfo.image[tab->current].w < width() && tab->finfo.image[tab->current].h < height()))
        factor = 1.0;

    internalZoom(factor);
}

// Previous zoom.
void SQ_GLWidget::slotZoomLast()
{
    zoom_type = 4;
    pAZoomLast->setChecked(true);

    if(tab->broken || tab->finfo.image.empty()) return;

    internalZoom(zoomFactor);
}

// Zoom 100%.
void SQ_GLWidget::slotZoom100()
{
    zoom_type = 3;
    pAZoom100->setChecked(true);

    if(tab->broken || tab->finfo.image.empty()) return;

    internalZoom(1.0);
}

// "Ignore, if the image is less than window"
void SQ_GLWidget::slotZoomIfLess()
{
    if(tab->broken || tab->finfo.image.empty()) return;

    switch(zoom_type)
    {
        case 0: slotZoomW(); break;
        case 1: slotZoomH(); break;
        case 2: slotZoomWH(); break;
        case 3: break;

        default:
            slotZoomLast();
    }
}

// Zoom+
void SQ_GLWidget::slotZoomPlus()
{
    matrix_zoom(1.0+zoomfactor/100.0);
}

// Zoom-
void SQ_GLWidget::slotZoomMinus()
{
    matrix_zoom(1.0/(1.0+zoomfactor/100.0));
}

// Rotate left.
void SQ_GLWidget::slotRotateLeft()
{
    matrix_rotate(-rotatefactor);
}

// Rotate right.
void SQ_GLWidget::slotRotateRight()
{
    matrix_rotate(rotatefactor);
}

/*
     operations with matrices are taken from GLiv =)

     thanks to Guillaume Chazarian.
*/
void SQ_GLWidget::flip(int id, bool U)
{
    GLfloat x = MATRIX_X, y = MATRIX_Y;
    MATRIX_X = 0;
    MATRIX_Y = 0;

    tab->matrix[id] *= -1.0;
    tab->matrix[id + 1] *= -1.0;
    tab->matrix[id + 3] *= -1.0;

    MATRIX_X = x;
    MATRIX_Y = y;

    write_gl_matrix();

    if(!reset_mode && U)
        updateGL();
}

void SQ_GLWidget::slotFlipH()
{
    if(!tab->broken)
    {
        tab->isflippedH = !tab->isflippedH;
        flip(0);
    }
}

void SQ_GLWidget::slotFlipV()
{
    if(!tab->broken)
    {
        tab->isflippedV = !tab->isflippedV;
        flip(4);
    }
}

void SQ_GLWidget::slotMatrixReset()
{
    if(!tab->broken)
    {
        oldZoom = getZoom();
        matrix_reset(false);
        matrix_zoom(1.0);
    }
}

void SQ_GLWidget::write_gl_matrix()
{
    GLfloat transposed[16] =
    {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    transposed[0] = MATRIX_C1;
    transposed[5] = MATRIX_C2;
    transposed[4] = MATRIX_S1;
    transposed[1] = MATRIX_S2;
    transposed[12] = MATRIX_X;
    transposed[13] = MATRIX_Y;
    transposed[14] = MATRIX_Z;

    glLoadMatrixf(transposed);
}

void SQ_GLWidget::matrix_move(GLfloat x, GLfloat y)
{
    if(tab->broken) return;

    MATRIX_X += x;
    MATRIX_Y += y;

    write_gl_matrix();

    if(!reset_mode)
        updateGL();
}

void SQ_GLWidget::matrix_move_z(GLfloat z)
{
    MATRIX_Z = z;

    write_gl_matrix();
}

void SQ_GLWidget::matrix_push()
{
    memcpy(saved, tab->matrix, sizeof(tab->matrix));
}

void SQ_GLWidget::matrix_pop()
{
    memcpy(tab->matrix, saved, sizeof(tab->matrix));
}

void SQ_GLWidget::matrix_reset(bool U)
{
    tab->nullMatrix();

    tab->curangle = 0.0;
    tab->isflippedH = tab->isflippedV = false;

    if(decoded)
        exifRotate(U);
}

void SQ_GLWidget::matrix_pure_reset()
{
    tab->nullMatrix();
    write_gl_matrix();
}

bool SQ_GLWidget::matrix_zoom(GLfloat ratio)
{
    if(tab->broken) return false;

    SQ_Config::instance()->setGroup("GL view");

    int zoom_lim = SQ_Config::instance()->readNumEntry("zoom limit", 1);
    GLfloat zoom_min, zoom_max, zoom_tobe;

    zoom_tobe = hypot(MATRIX_C1 * ratio, MATRIX_S1 * ratio) * 100.0;

    switch(zoom_lim)
    {
        case 2:
            zoom_min = (float)SQ_Config::instance()->readNumEntry("zoom_min", 1);
            zoom_max = (float)SQ_Config::instance()->readNumEntry("zoom_max", 10000);
        break;

        default: // "case 1:" too
            zoom_min = 1.0;
            zoom_max = 10000.0;
    }

    if(zoom_lim)
    {
        float z = getZoomPercents();

        // zoom limit exceeded - do nothing
        if((z >= zoom_max && ratio > 1.0) || (z <= zoom_min && ratio < 1.0))
            return false;

        // if the new zoom will be greater (smaller) than maximum
        // (minimum) zoom - scale it.
        if(ratio < 1.0 && zoom_min >= zoom_tobe)
            ratio = ratio * zoom_min / zoom_tobe;
        else if(ratio > 1.0 && zoom_max <= zoom_tobe)
            ratio = ratio * zoom_max / zoom_tobe;
    }

    GLfloat oldz = (oldZoom == -1 ? getZoom() : oldZoom);

    MATRIX_C1 *= ratio;
    MATRIX_S1 *= ratio;
    MATRIX_X  *= ratio;
    MATRIX_S2 *= ratio;
    MATRIX_C2 *= ratio;
    MATRIX_Y  *= ratio;

    hackMatrix();

    GLfloat z = getZoom();
    int filter = -1;

    if(ISFLOAT1(oldz) && !ISFLOAT1(z))
        filter = linear ? GL_LINEAR : GL_NEAREST;
    else if(ISFLOAT1(z))
        filter = GL_NEAREST;

    // update all textures
    if(filter != -1)
    {
        for(int i = 0;i < tab->total;i++)
        {
            int toxy = tab->parts[i].m_parts.size();

            for(int j = 0;j < toxy;j++)
            {
                glBindTexture(GL_TEXTURE_2D, tab->parts[i].m_parts[j].tex);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            }
        }
    }

    oldZoom = -1;
    write_gl_matrix();

    changeSlider(z);

    if(!reset_mode)
        updateGL();

    return true;
}

/*
 *  Floating point operations are not 100% exact.
 *  We should correct matrix values.
 */
void SQ_GLWidget::hackMatrix()
{
    if(ISFLOAT1(MATRIX_C1)) MATRIX_C1 = MATRIX_C1 < 0 ? -1.0 : 1.0;
    else if(ISFLOAT0(MATRIX_C1)) MATRIX_C1 = 0.0;

    if(ISFLOAT1(MATRIX_C2)) MATRIX_C2 = MATRIX_C2 < 0 ? -1.0 : 1.0;
    else if(ISFLOAT0(MATRIX_C2)) MATRIX_C2 = 0.0;

    if(ISFLOAT1(MATRIX_S1)) MATRIX_S1 = MATRIX_S1 < 0 ? -1.0 : 1.0;
    else if(ISFLOAT0(MATRIX_S1)) MATRIX_S1 = 0.0;

    if(ISFLOAT1(MATRIX_S2)) MATRIX_S2 = MATRIX_S2 < 0 ? -1.0 : 1.0;
    else if(ISFLOAT0(MATRIX_S2)) MATRIX_S2 = 0.0;

    if(ISFLOAT0(MATRIX_X)) MATRIX_X = 0.0;
    if(ISFLOAT0(MATRIX_Y)) MATRIX_Y = 0.0;
}

GLfloat SQ_GLWidget::getZoom() const
{
    return hypot(MATRIX_C1, MATRIX_S1);
}

GLfloat SQ_GLWidget::getZoomPercents() const
{
    return getZoom() * 100.0;
}

void SQ_GLWidget::matrix_rotate(GLfloat angle, bool U)
{
    if(tab->broken) return;

    GLfloat c1 = MATRIX_C1, c2 = MATRIX_C2, s1 = MATRIX_S1, s2 = MATRIX_S2;

    double rad = angle * rad_const;
    double cosine = cos(rad);
    double sine = sin(rad);

    MATRIX_C1 = c1  * cosine + s2 * sine;
    MATRIX_S1 = s1  * cosine + c2 * sine;
    MATRIX_S2 = -c1 * sine   + s2 * cosine;
    MATRIX_C2 = -s1 * sine   + c2 * cosine;

    hackMatrix();

    tab->curangle += angle;

    if(tab->curangle == 360.0 || tab->curangle == -360.0)
        tab->curangle = 0.0;
    else if(tab->curangle > 360.0)
        tab->curangle -= 360.0;
    else if(tab->curangle < -360.0)
        tab->curangle += 360.0;

    write_gl_matrix();

    if(U)
        updateGL();
}

void SQ_GLWidget::matrix_rotate2(GLfloat angle)
{
    GLfloat c1 = MATRIX_C1, c2 = MATRIX_C2, s1 = MATRIX_S1, s2 = MATRIX_S2;

    double rad = angle * rad_const;
    double cosine = cos(rad);
    double sine = sin(rad);

    MATRIX_C1 = c1  * cosine + s2 * sine;
    MATRIX_S1 = s1  * cosine + c2 * sine;
    MATRIX_S2 = -c1 * sine   + s2 * cosine;
    MATRIX_C2 = -s1 * sine   + c2 * cosine;

    write_gl_matrix();
}

/*
 *  Bind textures, draw them and create GL lists.
 *  Also show textures by executing GL lists, if 'swap' = true.
 */
bool SQ_GLWidget::showFrames(int i, Parts *p, bool swap)
{
    int z, k = 0;
    const int a = p->tilesx.size() * i, b = a + p->tilesx.size();
    int filter = linear ? GL_LINEAR : GL_NEAREST;

    // for safety...
    makeCurrent();

    glEnable(GL_TEXTURE_2D);

    float zm = getZoom();

    for(z = a;z < b;z++)
    {
        glBindTexture(GL_TEXTURE_2D, p->m_parts[z].tex);

        // setup texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ISFLOAT1(zm) ? GL_NEAREST : filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ISFLOAT1(zm) ? GL_NEAREST : filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        setupBits(p, buffer, i, k);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p->tilesx[k], p->tilesy[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

        k++;
    }

    // create new display list
    glNewList(p->m_parts[a].list, swap ? GL_COMPILE_AND_EXECUTE : GL_COMPILE);

    // bind & draw textures (if needed)
    for(z = a;z < b;z++)
    {
        glBindTexture(GL_TEXTURE_2D, p->m_parts[z].tex);

        glBegin(GL_QUADS);
            glTexCoord2f(p->m_parts[z].tx1, p->m_parts[z].ty1); glVertex2f(p->m_parts[z].x1, p->m_parts[z].y1);
            glTexCoord2f(p->m_parts[z].tx2, p->m_parts[z].ty1); glVertex2f(p->m_parts[z].x2, p->m_parts[z].y1);
            glTexCoord2f(p->m_parts[z].tx2, p->m_parts[z].ty2); glVertex2f(p->m_parts[z].x2, p->m_parts[z].y2);
            glTexCoord2f(p->m_parts[z].tx1, p->m_parts[z].ty2); glVertex2f(p->m_parts[z].x1, p->m_parts[z].y2);
        glEnd();
    }

    glEndList();

    glDisable(GL_TEXTURE_2D);

    // swap buffers...
    if(swap)
        swapBuffers();

    return GL_TRUE;
}

void SQ_GLWidget::setupBits(Parts *p, RGBA *_buffer, int y, int x)
{
    QPair<int, int> pair = SQ_GLWidget::calcRealDimensions(*p, y, x);

    int offs = p->realw * pair.second + pair.first;
    RGBA *orig = p->buffer->data() + offs;

    int toy = p->tilesy[y];
    int tox = p->tilesx[x];

    for(int j = 0;j < toy;j++)
        memcpy(_buffer + tox*j, orig + p->realw*j, tox*sizeof(RGBA));
}

/*
 *  Start decoding given image. We can call it from anywhere.
 */
void SQ_GLWidget::startDecoding(const QString &file)
{
    if(SQ_PreviewWidget::instance()->cancel())
        SQ_WidgetStack::instance()->diroperator()->stopPreview();

    if(reset_mode)
        return;

    tabold = tab;
    tmptab.empty();
    tab = &tmptab;
    reset_mode = true;

    timer_anim->stop();
    images->clear();

    tab->m_File = file;                         // original name
    tab->File = QFile::encodeName(tab->m_File); // translated name
    tab->m_original = m_original;

    QFileInfo fm(file);
    tab->fmt_ext = fm.extension(false);
    tab->fmt_size = fm.size();

    // show window with image
    KSquirrel::app()->raiseGLWidget();

    if(m_expected.isEmpty())
        KApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput | QEventLoop::ExcludeSocketNotifiers);

    decode();
}

/*
 *  Prepare decoding. It will find proper library for decoding,
 *  clear old memory buffers, etc.
 */
bool SQ_GLWidget::prepare()
{
    QString status;

    tab = &tmptab;

    // get library from cache
    SQ_LIBRARY *m_lib = SQ_LibraryHandler::instance()->libraryForFile(tab->m_File);

    if(!m_lib)
    {
        KMessageBox::error(this, i18n("Codec for %1 format not found").arg(tab->fmt_ext));
        reset_mode = false;
        tab = tabold;
        return false;
    }
    else
        tmptab.lib = m_lib;

    enableActions(true);

    int already = -1, result = -1, i = 0;
    SQ_Config::instance()->setGroup("GL view");
    bool use_tabs = SQ_GLView::window()->tabs();
    bool useCurrent = false;

    if(!use_tabs)
    {
        removeCurrentTabs();
        SQ_GLView::window()->removeTabs();
    }
    else
    {
        if(m_expected != m_original)
        {
            std::vector<Tab>::iterator it = tabs.begin();
            std::vector<Tab>::iterator itEnd = tabs.end();
            for(;it != itEnd;++it, ++i)
            {
                if((*it).m_original == m_original)
                {
                    already = i;
                    useCurrent = true;
                    break;
                }
            }
        }

        // 0  - new page
        // 1  - replace current tab
        // 2  - close all and open in a new tab
        // -1 - ignore 'result' at all
        if(already == -1
            && decoded
            && m_expected != m_original
            && !useCurrent
            && SQ_Config::instance()->readBoolEntry("tabs_ask", false))
        {
            SQ_TabOpenDialog tbo(this);
            tbo.exec();
            result = tbo.result();
        }
    }

    int curtab = SQ_GLView::window()->tabbar()->indexOf(SQ_GLView::window()->tabbar()->currentTab());
    int curindex = (already == -1) ? curtab : already;

    if(curindex != -1 && (m_expected == m_original || result == 1 || useCurrent))
    {
        if(curtab != curindex)
            tabs[curtab].removeParts();

        tabs[curindex].clearParts();
        tabs[curindex] = tmptab;
        tab = &tabs[curindex];

        SQ_GLView::window()->tabbar()->blockSignals(true);
        SQ_GLView::window()->tabbar()->setCurrentTab(SQ_GLView::window()->tabbar()->tabAt(curindex));
        SQ_GLView::window()->tabbar()->tabAt(curindex)->setText(KStringHandler::csqueeze(tab->m_original.fileName(), SQ_TAB_TEXT_LENGTH));
        SQ_GLView::window()->tabbar()->blockSignals(false);
    }
    else
    {
        if(curindex != -1)
        {
            if(result == 2)
                closeAllTabs();
            else
                tabs[curindex].removeParts();
        }

        tabs.push_back(tmptab);
        int lastId = tabs.size() - 1;
        tab = &tabs[lastId];

        SQ_GLView::window()->tabbar()->blockSignals(true);
        SQ_GLView::window()->addPage(KStringHandler::csqueeze(tab->m_original.fileName(), SQ_TAB_TEXT_LENGTH));
        SQ_GLView::window()->tabbar()->setCurrentTab(SQ_GLView::window()->tabbar()->tabAt(lastId));

        // QTabBar::show will emit selected(int),
        // we don't want it
        emit tabCountChanged();
        SQ_GLView::window()->tabbar()->blockSignals(false);
    }

    gls->setVisible(false);

    SQ_CodecSettings::applySettings(tab->lib, SQ_CodecSettings::ImageViewer);

    // determine codec
    tab->codeK = tab->lib->codec;

    // start decoding!
    i = tab->codeK->read_init(tab->File);

    // oops, error...
    if(i != SQE_OK)
    {
        decodeFailedOn0(i);
        m_expected = KURL();
        return false;
    }

    return true;
}

void SQ_GLWidget::decode()
{
    // prepare decoding...
    if(!prepare())
        return;

    KSquirrel::app()->setCaption(originalURL());

    zoomFactor = getZoom();
    matrix_pure_reset();
    matrixChanged();

#ifdef SQ_HAVE_KEXIF
    KExifData d;
    d.readFromFile(tab->m_File);
    tab->orient = d.getImageOrientation();
    tab->wm = SQ_Utils::exifGetMatrix(QString::null, tab->orient);

    tab->rotate = (tab->orient == KExifData::ROT_90_HFLIP || tab->orient == KExifData::ROT_90
                || tab->orient == KExifData::ROT_90_VFLIP || tab->orient == KExifData::ROT_270);
#else
    tab->orient = -1;
    tab->rotate = false;
#endif

    errors = 0;

/* *********************************************************** */

    int i, j, id;
    int line, res, first_id = 0;
    fmt_image *im;
    memoryPart *pt;
    bool progr;

    SQ_Config::instance()->setGroup("GL view");

    // in fullscreen mode progressive loading is disabled anyway
    if(fullscreen())
       progr = false;
    else
       progr = SQ_Config::instance()->readBoolEntry("progressiv", true);

    int allpages = SQ_Config::instance()->readNumEntry("load_pages", 0);
    int pages_num = SQ_Config::instance()->readNumEntry("load_pages_number", 1);

    if(pages_num < 1) pages_num = 1;

    SQ_GLView::window()->sbarWidget("SBFile")->setText(tab->m_original.fileName());

    tab->current = 0;

    bool notexpected = m_expected.isEmpty();
    m_expected = KURL();

    // start time counting
    QTime started;
    started.start();

    while(true)
    {
        if((allpages == 1 && tab->current) || (allpages == 2 && tab->current == pages_num))
            break;

        // absolute evil, but should do...
        if(notexpected)
            KApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput | QEventLoop::ExcludeSocketNotifiers);

        i = tab->codeK->read_next();

        // something went wrong. SQE_NOTOK is a special type of error. It means
        // that we decoded all pages.
        if(i != SQE_OK)
        {
            if(i == SQE_NOTOK || tab->current)
                break;
            else
            {
                decodeFailedOn0(i);
                return;
            }
        }

        im = tab->codeK->image(tab->current);

        Parts pp;

        // find tile size
        SQ_GLWidget::findCloserTiles(im->w, im->h, pp.tilesx, pp.tilesy);
        QPair<int, int> pair = SQ_GLWidget::calcRealDimensions(pp);
        pp.realw = pair.first;
        pp.realh = pair.second;

        // setup current Part
        pp.w = im->w;
        pp.h = im->h;

        // create textures and display lists
        if(!pp.makeParts())
        {
            if(tab->current)
                break;
            else
            {
                KMessageBox::error(this,
                            i18n("Memory allocation failed for %1 of memory")
                            .arg(KIO::convertSize(pp.realw * pp.realh * sizeof(RGBA))));

                decodeFailedOn0(SQE_R_NOMEMORY);
                return;
            }
        }

        pt = new memoryPart(pp.realw * pp.realh);
        pt->create();

        if(!pt->valid())
        {
            pp.removeParts();

            if(tab->current)
                break;
            else
            {
                KMessageBox::error(this, i18n("Memory allocation failed"));
                decodeFailedOn0(SQE_R_NOMEMORY);
                return;
            }
        }

        line = 0;

        pp.computeCoords();
        pp.buffer = pt;

        tab->finfo = tab->codeK->information();

        if(!tab->current)
        {
            tab->isflippedH = tab->isflippedV = false;
            slotZoomIfLess();
            matrixChanged();
            updateCurrentFileInfo();
        }

        matrix_move_z(SQ_FIRST_TILE_LAYER+tab->current);

        for(int pass = 0;pass < im->passes;pass++)
        {
            if(tab->codeK->read_next_pass() != SQE_OK)
                break;

            bool flip = tab->finfo.image[tab->current].needflip;
            line = 0;
            int tlsy = pp.tilesy.size();
            int tlsyval, offs = 0, O, iA;

            for(i = 0;i < tlsy;i++)
            {
                iA = flip ? (tlsy-i-1) : i;
                tlsyval = pp.tilesy[iA];

                for(j = 0;j < tlsyval;j++)
                {
                    O = flip ? (pp.realw*(im->h - offs - j-1)) : (offs + j)*pp.realw;
                    res = tab->codeK->read_scanline(pp.buffer->data() + O);
                    errors += (int)(res != SQE_OK);

                    if(++line == im->h)
                        break;
                }

                offs += tlsyval;

                // last pass
                if(pass == im->passes-1)
                {
//                    if(!tab->current)
                    {
                        bool b = showFrames(iA, &pp, progr);

                        if(!b)
                            kdWarning() << "Showframes failed for image " << tab->current << ", tiley " << i << endl;
                    }
                }

            }
        }

        id = images->insertItem(QString::fromLatin1("#%1 [%2x%3@%4]").arg(tab->current+1).arg(im->w).arg(im->h).arg(im->bpp));

        images->setItemParameter(id, tab->current);

        if(!tab->current)
        {
            SQ_GLView::window()->sbarWidget("SBDecodedI")->setPixmap(tab->lib->mime);
            old_id = first_id = id;
        }

        tab->parts.push_back(pp);

        calcFrameLabelWidth();
        frameChanged();
        tab->current++;
    }

    tab->finfo = tab->codeK->information();
    tab->codeK->read_close();
    tab->total = tab->finfo.image.size();
    tab->current = 0;
    frameChanged();

    enableSettingsButton(!tab->lib->config.isEmpty());

    decoded = true;
    reset_mode = false;
    updateGL();

    tab->quickImageInfo = tab->lib->quickinfo;
    tab->elapsed = started.elapsed();

    SQ_GLView::window()->sbarWidget("SBLoaded")->setText(
                        KGlobal::locale()->formatLong(tab->elapsed) + i18n(" ms."));

    images->setItemChecked(first_id, true);

    if(tab->finfo.animated)
        QTimer::singleShot(tab->finfo.image[tab->current].delay, this, SLOT(slotAnimateNext()));
}

/*
 *  Palette changed. Let's update tickmarks and background color.
 */
void SQ_GLWidget::paletteChange(const QPalette &oldPalette)
{
    QGLWidget::paletteChange(oldPalette);

    SQ_Config::instance()->setGroup("GL view");

    if(SQ_Config::instance()->readNumEntry("GL view background type", 1) == 0)
    {
        QColor color = colorGroup().color(QColorGroup::Base);
        qglClearColor(color);
        updateGL();
    }
}

void SQ_GLWidget::slotFirst()
{
    if(!reset_mode)
        SQ_WidgetStack::instance()->firstFile();
}

void SQ_GLWidget::slotLast()
{
    if(!reset_mode)
        SQ_WidgetStack::instance()->lastFile();
}

void SQ_GLWidget::slotNext()
{
    timer_next->stop();
    timer_prev->stop();

    timer_next->start(timer_delay_file, true);
}

void SQ_GLWidget::slotPrev()
{
    timer_prev->stop();
    timer_next->stop();

    timer_prev->start(timer_delay_file, true);
}

void SQ_GLWidget::slotZoomMenu()
{
    zoom->exec(QCursor::pos());
}

void SQ_GLWidget::slotAnimateNext()
{
    // Some time ago we started to decode new image, but it is
    // not guaranteed that animation stopped!
    //
    // We should return now to avoid segfaults...
    if(reset_mode)
        return;

//    tab->parts[tab->current].removeParts();

    tab->current++;

    if(tab->current >= (int)tab->finfo.image.size())
        tab->current = 0;

    updateCurrentFileInfo();
    updateGL();

    int delay = tab->finfo.image[tab->current].delay;

    timer_anim->start(delay, true);
}

void SQ_GLWidget::startAnimation()
{
    if(!tab->finfo.animated) return;

    timer_anim->start(tab->finfo.image[tab->current].delay, true);
}

void SQ_GLWidget::stopAnimation()
{
    timer_anim->stop();
}

void SQ_GLWidget::slotToggleAnimate()
{
    if(!tab->finfo.animated || gls->valid()) return;

    if(!timer_anim->isActive())
    {
        tab->manualBlocked = false;
        startAnimation();
    }
    else
    {
        tab->manualBlocked = true;
        stopAnimation();
    }
}

/*
 *  Next image in animated sequence. Called by user (with F3).
 */
void SQ_GLWidget::nextImage()
{
    // if the image has only one page - do nothing
    if(tab->total == 1)
        return;

    tab->current++;

    if(tab->current >= tab->total)
        tab->current = 0;

    updateGL();
    updateCurrentFileInfo();
}

/*
 *  Previous image in animated sequence. Called by user (with F2).
 */
void SQ_GLWidget::prevImage()
{
    if(tab->total == 1)
        return;

    tab->current--;

    if(tab->current < 0)
        tab->current = tab->total - 1;

    updateGL();
    updateCurrentFileInfo();
}

/*
 *  Jump to first/last image page.
 */
void SQ_GLWidget::jumpToImage(bool last)
{
    // if the image has only one page - do nothing
    if(tab->total == 1)
        return;

    tab->current = (last) ? tab->finfo.image.size() - 1 : 0;

    updateGL();
    updateCurrentFileInfo();
}

void SQ_GLWidget::initMarks()
{
    for(int i = 0;i < 4;i++)
    {
        glGenTextures(1, &mark[i]);

        glBindTexture(GL_TEXTURE_2D, mark[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, mm[i].bits());
    }
}

void SQ_GLWidget::internalZoom(const GLfloat &zF)
{
    tab->curangle = 0.0;

    oldZoom = getZoom();
    matrix_pure_reset();

    exifRotate(false);

    matrix_zoom(zF);
}

void SQ_GLWidget::updateFilter(bool nice)
{
    if(nice == linear)
        return;

    // store
    linear = nice;

    int filter = nice ? GL_LINEAR : GL_NEAREST;
    Parts *pt;

    // update all textures
    for(int i = 0;i < tab->total;i++)
    {
        pt = tab->broken ? parts_broken : &tab->parts[i];
        int toxy = pt->m_parts.size();

        for(int j = 0;j < toxy;j++)
        {
            glBindTexture(GL_TEXTURE_2D, pt->m_parts[j].tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        }
    }

    updateGL();
}

/*
 *  Cleanup method.
 */
void SQ_GLWidget::decodeFailedOn0(const int err_code)
{
    tab->codeK->read_close();
    tab->finfo.image.clear();
    tab->finfo.meta.clear();
    tab->total = 0;
    decoded = (bool)tabs.size();
    reset_mode = false;
    tab->broken = true;
    tab->lib = 0;

    useBrokenImage(err_code);
}

/*
 *  Force using broken image + update context.
 *  Show appropriate error message in statusbar.
 */
void SQ_GLWidget::useBrokenImage(const int err_index)
{
    enableSettingsButton(false);
    enableActions(false);

    // save "broken" image information in 'tab->finfo'
    tab->finfo.image.push_back(image_broken);

    // reset statusbar widgets
    SQ_GLView::window()->resetStatusBar();

    // show error message instead of file name
    SQ_GLView::window()->sbarWidget("SBFile")->setText(SQ_ErrorString::instance()->string(err_index));

    KSquirrel::app()->setCaption(QString::null);

    matrix_pure_reset();
    tab->curangle = 0;
    tab->isflippedH = tab->isflippedV = false;

    changeSlider(1.0);

    // update context and show "broken" image
    updateGL();
}

/*
 *  Are we in fullscreen state ?
 */
bool SQ_GLWidget::fullscreen() const
{
    KWin::WindowInfo wi = KWin::windowInfo(SQ_GLView::window()->winId());

    if(wi.valid())
        pAFull->setChecked((wi.state() & NET::FullScreen));

    return pAFull->isChecked();
}

/*
 *  Toggle fullscreen state.
 */
void SQ_GLWidget::toggleFullScreen()
{
    bool fs = !fullscreen();

    pAFull->setChecked(fs);
    pAToolFull->setOn(fs);

    KSquirrel::app()->slotFullScreen(fs);
}

void SQ_GLWidget::slotSetZoomPercents(int perc)
{
    if(tab->broken || tab->finfo.image.empty()) return;

    GLfloat z = (perc <= 20) ? (GLfloat)perc / 20 : ((GLfloat)perc - 20)/2 + 1.0;

    internalZoom(z);
}

void SQ_GLWidget::updateFactors()
{
    zoomfactor = SQ_Config::instance()->readNumEntry("zoom", 25);
    movefactor = SQ_Config::instance()->readNumEntry("move", 5);
    rotatefactor = SQ_Config::instance()->readNumEntry("angle", 90);
}

void SQ_GLWidget::slotSelectionRect()
{
    stopAnimation();
    tab->glselection = SQ_GLSelectionPainter::Rectangle;
    gls->end();
}

void SQ_GLWidget::slotSelectionEllipse()
{
    stopAnimation();
    tab->glselection = SQ_GLSelectionPainter::Ellipse;
    gls->end();
}

void SQ_GLWidget::slotSelectionClear()
{
    tab->glselection = -1;
    gls->end();

    pASelectionEllipse->setChecked(false);
    pASelectionRect->setChecked(false);

    if(!manualBlocked())
        startAnimation();
}

bool SQ_GLWidget::manualBlocked()
{
    // selection is also blocks animation
    return tab->manualBlocked || gls->valid();
}

void SQ_GLWidget::setDownloadPercents(int p)
{
    if(p < 0)
        percentsLabel->hide();
    else
    {
        percentsLabel->setText(i18n("Downloading...") + ' ' + KIO::convertSize(p));
        percentsLabel->adjustSize();
        percentsLabel->show();
    }
}

#include "sq_glwidget.moc"
