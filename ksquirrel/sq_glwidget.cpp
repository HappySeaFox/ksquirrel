/***************************************************************************
                          sq_glwidget.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by Baryshev Dmitry
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qapplication.h>
#include <qclipboard.h>
#include <qeventloop.h>
#include <qstringlist.h>
#include <qdragobject.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <qsignalmapper.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qslider.h>

#include <kaction.h>
#include <kcursor.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstringhandler.h>
#include <kimageeffect.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <math.h>
#include <stdlib.h>

#include "ksquirrel.h"
#include "sq_externaltool.h"
#include "sq_widgetstack.h"
#include "sq_fileiconview.h"
#include "sq_diroperator.h"
#include "sq_popupmenu.h"
#include "sq_libraryhandler.h"
#include "sq_config.h"
#include "sq_glwidget_helpers.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_imageproperties.h"
#include "sq_helpwidget.h"
#include "sq_glu.h"
#include "sq_errorstring.h"
#include "sq_iconloader.h"
#include "sq_codecsettingsskeleton.h"
#include "sq_filedialog.h"
#include "sq_codecsettings.h"

#include <ksquirrel-libs/fileio.h>
#include <ksquirrel-libs/fmt_codec_base.h>
#include <ksquirrel-libs/error.h>

#include "file_broken.xpm"

/* ***************************************************************************************** */

SQ_GLWidget * SQ_GLWidget::m_instance = 0;

static const int timer_delay =    20;
static const int timer_delay_file = 10;
static const int len =    5;

static const float SQ_WINDOW_BACKGROUND_POS = -1000.0f;
static const float SQ_IMAGE_CHECKER_POS =     -999.0f;
static const float SQ_FIRST_FRAME_POS =       -998.0f;
static const float SQ_MARKS_POS =             -997.0f;
static const float SQ_FIRST_TILE_LAYER =      -995.0f;
static const float SQ_ONSCREEN_LAYER =      10000.0f;

/* ***************************************************************************************** */

Parts::Parts() : w(0), h(0), realw(0), realh(0), m_parts(0), buffer(0)
{}

Part::Part() : x1(0), y1(0), x2(0), y2(0), tex(0), list(0)
{}

/* ***************************************************************************************** */

memoryPart::memoryPart(const int sz) : m_size(sz), m_data(0)
{}

memoryPart::~memoryPart()
{
    del();
}

void memoryPart::create()
{
    m_data = new RGBA [m_size];

//    if(m_data)
//        memset(m_data, 0, m_size * sizeof(RGBA));
}

/* ***************************************************************************************** */

SQ_GLWidget::SQ_GLWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
    kdDebug() << "+SQ_GLWidget" << endl;

    // init all variables...
    m_instance = this;
    ac = new KActionCollection(0, this, "GLWidget actionCollection");
    isflippedV = isflippedH = changed = blocked = blocked_force = decoded = reset_mode = false;
    movetype = -1;
    memset(matrix, 0, sizeof(matrix));
    matrix[10] = matrix[5] = matrix[0] = 1.0f;
    curangle = 0.0f;
    lib = 0;
//    next = 0;
    buffer = new RGBA [512 * 512];
    changed2 = true;
    current = 0;
    total = 0;
    old_id = -1;
    zoomFactor = 1.0f;
    menu = new QPopupMenu(this);

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

    KCursor::setAutoHideCursor(this, true);
    KCursor::setHideCursorDelay(2500);

    timer_prev = new QTimer(this);
    timer_next = new QTimer(this);

    Q_CHECK_PTR(timer_prev);
    Q_CHECK_PTR(timer_next);

    connect(timer_prev, SIGNAL(timeout()), SQ_WidgetStack::instance(), SLOT(emitPreviousSelected()));
    connect(timer_next, SIGNAL(timeout()), SQ_WidgetStack::instance(), SLOT(emitNextSelected()));

    timer_decode = new QTimer(this);
    timer_anim = new QTimer(this);

    Q_CHECK_PTR(timer_anim);
    Q_CHECK_PTR(timer_decode);

    connect(timer_anim, SIGNAL(timeout()), this, SLOT(slotAnimateNext()));
    connect(timer_decode, SIGNAL(timeout()), this, SLOT(slotDecode()));

    connect(images, SIGNAL(activated(int)), this, SLOT(slotSetCurrentImage(int)));
    connect(images, SIGNAL(aboutToHide()), this, SLOT(slotImagesHidden()));
    connect(images, SIGNAL(aboutToShow()), this, SLOT(slotImagesShown()));

    createContextMenu(menu);
}

SQ_GLWidget::~SQ_GLWidget()
{
    kdDebug() << "-SQ_GLWidget" << endl;

    parts_broken->deleteBuffer();

    delete parts_broken;

    removeCurrentParts();

    delete zoom;
    delete images;
    delete buffer;
}

/*
 *  With 'T' user can hide toolbar (with KToolBar::hide()).
 *  This method will help determine if toolbar is visible.
 */
bool SQ_GLWidget::actionsHidden() const
{
    return pAHideToolbars->isChecked();
}

// Create actions
void SQ_GLWidget::createActions()
{
    pAZoomW = new KToggleAction(i18n("Fit width"), QPixmap(locate("appdata", "images/actions/zoomW.png")), 0, this, SLOT(slotZoomW()), ac, "SQ ZoomW");
    pAZoomH = new KToggleAction(i18n("Fit height"), QPixmap(locate("appdata", "images/actions/zoomH.png")), 0, this, SLOT(slotZoomH()), ac, "SQ ZoomH");
    pAZoomWH = new KToggleAction(i18n("Fit image"), QPixmap(locate("appdata", "images/actions/zoomWH.png")), 0, this, SLOT(slotZoomWH()), ac, "SQ ZoomWH");
    pAZoom100 = new KToggleAction(i18n("Zoom 100%"), QPixmap(locate("appdata", "images/actions/zoom100.png")), 0, this, SLOT(slotZoom100()), ac, "SQ Zoom100");
    pAZoomLast = new KToggleAction(i18n("Leave previous zoom"), QPixmap(locate("appdata", "images/actions/zoomlast.png")), 0, this, SLOT(slotZoomLast()), ac, "SQ ZoomLast");
    pAIfLess = new KToggleAction(i18n("Ignore, if the image is less than window"), QPixmap(locate("appdata", "images/actions/ifless.png")), 0, 0, 0, ac, "if less");

    pAHelp = new KAction(i18n("Hotkeys"), "help", 0, this, SLOT(slotShowHelp()), ac, "SQ help");
    pAReset = new KAction(i18n("Reset"), locate("appdata", "images/menu/reset16.png"), 0, this, SLOT(slotMatrixReset()), ac, "SQ ResetGL");

    pAClose = new KAction(i18n("Close"), locate("appdata", "images/menu/close16.png"), 0, KSquirrel::app(), SLOT(slotCloseGLWidget()), ac, "SQ GL close");

    pAProperties = new KAction(i18n("Properties"), locate("appdata", "images/menu/prop16.png"), 0, this, SLOT(slotProperties()), ac, "SQ GL Prop");
    pAFull = new KToggleAction(QString::null, 0, 0, 0, ac, "SQ GL Full");
    pAHideToolbars = new KToggleAction(QString::null, QPixmap(), 0, 0, 0, ac, "toggle toolbar");
    pAStatus = new KToggleAction(QString::null, QPixmap(), 0, 0, 0, ac, "toggle status");

    pAZoomW->setExclusiveGroup("squirrel_zoom_actions");
    pAZoomH->setExclusiveGroup("squirrel_zoom_actions");
    pAZoomWH->setExclusiveGroup("squirrel_zoom_actions");
    pAZoom100->setExclusiveGroup("squirrel_zoom_actions");
    pAZoomLast->setExclusiveGroup("squirrel_zoom_actions");

    connect(pAIfLess, SIGNAL(toggled(bool)), this, SLOT(slotZoomIfLess()));

    connect(pAFull, SIGNAL(toggled(bool)), KSquirrel::app(), SLOT(slotFullScreen(bool)));

    connect(pAHideToolbars, SIGNAL(toggled(bool)), this, SLOT(slotHideToolbars(bool)));
    connect(pAStatus, SIGNAL(toggled(bool)), this, SLOT(slotToggleStatus(bool)));

    SQ_Config::instance()->setGroup("GL view");

    pAStatus->setChecked(SQ_Config::instance()->readBoolEntry("statusbar", true));
    pAIfLess->setChecked(SQ_Config::instance()->readBoolEntry("ignore", true));
    pAHideToolbars->setChecked(SQ_Config::instance()->readBoolEntry("toolbars_hidden", false));
}

void SQ_GLWidget::createToolbar()
{
    zoom = new KPopupMenu;
    SQ_ToolButton    *pATool;

    SQ_ToolBar *toolbar = SQ_GLView::window()->toolbar();

    if(pAHideToolbars->isChecked())
        toolbar->hide();

    pAZoom100->plug(zoom);
    pAZoomLast->plug(zoom);
    zoom->insertSeparator();
    pAZoomW->plug(zoom);
    pAZoomH->plug(zoom);
    pAZoomWH->plug(zoom);
    pAIfLess->plug(zoom);
    zoom->insertSeparator();

    slider_zoom = new QSlider(Qt::Horizontal , zoom);
    slider_zoom->setTickmarks(QSlider::Below);
    slider_zoom->setRange(10, 500);
    slider_zoom->setTickInterval(40);
    connect(slider_zoom, SIGNAL(valueChanged(int)), this, SLOT(slotSetZoomPercents(int)));
    zoom->insertItem(slider_zoom);

    switch(zoom_type)
    {
        case 0: pAZoomW->setChecked(true); break;
        case 1: pAZoomH->setChecked(true); break;
        case 3: pAZoom100->setChecked(true); break;
        case 4: pAZoomLast->setChecked(true); break;

        // "case 2" too
        default: pAZoomWH->setChecked(true);
    }

/*
 * We will create QToolButtons and put them in toolbar.
 * Of course, we can just KAction::plug(), BUT plugged KActions
 * will produce buttons, which cann't be clicked twise! I think
 * plugged KActions will treat our attempt as double-click, not two single-clicks.
 * On the other hand, we can click QToolButton as frequently as we want.
 *
 * Plugged KActions also don't know about autorepeat :(
 */
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_first.png")), i18n("Go to first file"), this, SLOT(slotFirst()), toolbar);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_prev.png")), i18n("Previous file"), this, SLOT(slotPrev()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_next.png")), i18n("Next file"), this, SLOT(slotNext()), toolbar);
    pATool->setAutoRepeat(true);
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_last.png")), i18n("Go to last file"), this, SLOT(slotLast()), toolbar);

    // some toolbuttons need autorepeat...
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/zoom+.png")), i18n("Zoom +"), this, SLOT(slotZoomPlus()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/zoom-.png")), i18n("Zoom -"), this, SLOT(slotZoomMinus()), toolbar);
    pATool->setAutoRepeat(true);
    pAToolZoom = new SQ_ToolButtonPopup(QPixmap(locate("appdata", "images/actions/zoom_template.png")), i18n("Zoom"), toolbar);
    pAToolZoom->setPopup(zoom);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/rotateL.png")), i18n("Rotate left"), this, SLOT(slotRotateLeft()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/rotateR.png")), i18n("Rotate right"), this, SLOT(slotRotateRight()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/flipV.png")), i18n("Flip vertically"), this, SLOT(slotFlipV()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/flipH.png")), i18n("Flip horizontally"), this, SLOT(slotFlipH()), toolbar);
    pATool->setAutoRepeat(true);
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/reload.png")), i18n("Normalize"), this, SLOT(slotMatrixReset()), toolbar);
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/prop.png")), i18n("Image Properties"), this, SLOT(slotProperties()), toolbar);
    pAToolFull = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/fullscreen.png")), i18n("Fullscreen"), pAFull, SLOT(activate()), toolbar);
    pAToolFull->setToggleButton(true);
    pAToolImages = new SQ_ToolButtonPopup(QPixmap(locate("appdata", "images/actions/images.png")), i18n("Select image"), toolbar);
    pAToolImages->setPopup(images);

    KAction *n = KSquirrel::app()->actionCollection()->action("SQ Slideshow");
    pAToolSlideShow = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/slideshow.png")), n->text(), n, SLOT(activate()), toolbar);
    pAToolSlideShow->setToggleButton(true);
    pAToolQuick = new SQ_ToolButton(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, 22), i18n("Codec Settings"), this, SLOT(slotShowCodecSettings()), toolbar);
    pAToolQuick->setEnabled(false);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/close.png")), i18n("Close"), pAClose, SLOT(activate()), toolbar);
    toolbar->insertWidget(1000, 0, pATool);
    toolbar->alignItemRight(1000);
}

// Initialize OpenGL context. Used internally by QGLWidget.
void SQ_GLWidget::initializeGL()
{
    // some init...
    setClearColor();
    glClearDepth(1.0f);
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
}

// Resize OpenGL context. Used internally by QGLWidget.
void SQ_GLWidget::resizeGL(int width, int height)
{
    // set new viewport
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // O(0,0) will be in the center of the window
    glOrtho(-width/2, width/2, -height/2, height/2, 0.1f, 10000.0f);

    // camera setup
    SQ_GLU::gluLookAt(0,0,1, 0,0,0, 0,1,0);
    glMatrixMode(GL_MODELVIEW);

    if(decoded)
        slotZoomIfLess();
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
    glEnable(GL_TEXTURE_2D);

    // bind tickmarks, if needed
    static bool grrr = false;
    bindMarks(changed2, grrr);
    grrr = true;

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

        fmt_image *im = &finfo.image[current];

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

        Parts *pt = use_broken ? parts_broken : &parts[current];

        // draw current image
        int toy = pt->tilesy.size();
        int tox = pt->tilesx.size();
        for(z = 0;z < toy;z++)
            if(glIsList(pt->m_parts[z * tox].list))
                glCallList(pt->m_parts[z * tox].list);

        // draw tickmarks ("broken" image won't have tickmarks)
        if(!use_broken && marks && SQ_Config::instance()->readBoolEntry("marks", true))
        {
            GLfloat zum = get_zoom();
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
            matrix_rotate2(curangle);
            matrix_move_z(SQ_MARKS_POS);

            GLfloat coords[4][8] =
            {
                rx, ly,      -x, ly,      -x, y,       rx, y,
                x, ly,       lx, ly,      lx, y,       x, y,
                x, -y,       lx, -y,      lx, ry,      x, ry,
                rx, -y,      -x, -y,      -x, ry,      rx, ry
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

    if(!use_broken && total > 1)
        frameChanged();
}

/*
 *  Change statusbar info according with
 *  current matrix (it shows current zoom & angle values).
 */
void SQ_GLWidget::matrixChanged()
{
    QString str;

    float m = get_zoom();
    float zoom = m * 100.0f;
    float z = (m < 1.0f) ? 1.0f/m : m;

    slider_zoom->blockSignals(true);
    slider_zoom->setValue((int)zoom);
    slider_zoom->blockSignals(false);

    // construct zoom
    str = QString::fromLatin1("%1% [%2:%3]")
            .arg(zoom, 0, 'f', 1)
            .arg((m < 1.0f)?1.0f:z, 0, 'f', 1)
            .arg((m > 1.0f)?1.0f:z, 0, 'f', 1);

    SQ_GLView::window()->sbarWidget("SBGLZoom")->setText(str);

    // construct rotation angle
    str = QString::fromLatin1("%1%2 %3 deg")
            .arg((isflippedV)?"V":"")
            .arg((isflippedH)?"H":"")
            .arg(curangle, 0, 'f', 1);

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
        matrix_zoom(2.0f);
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
    if(e->button() == Qt::LeftButton && e->state() == Qt::NoButton)
    {
        setCursor(KCursor::sizeAllCursor());

        xmoveold = e->x();
        ymoveold = e->y();
        movetype = 1;
    }
    // left button + SHIFT, let's start drawing zoom frame
    else if(e->button() == Qt::LeftButton && e->state() == Qt::ShiftButton)
    {
        // stop animation!
        stopAnimation();

        // update cursor to crosshair
        setCursor(KCursor::crossCursor());

        // save rectangle
        pRect = new QPainter(this);
        pRect->setPen(QPen(white, 2));
        pRect->setRasterOp(Qt::XorROP);
        lastRect = QRect();
        xmoveold = e->x();
        ymoveold = e->y();
        movetype = 2;
        crossDrawn = false;
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
    if(movetype == -1)
        return;

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
    {
        int X, Y, Xmin, Ymin;
        xmove = e->x();
        ymove = e->y();

        X = QMAX(xmove, xmoveold);
        Y = QMAX(ymove, ymoveold);
        Xmin = QMIN(xmove, xmoveold);
        Ymin = QMIN(ymove, ymoveold);

        QRect rect(Xmin, Ymin, X-Xmin, Y-Ymin);

        QPoint lastC = lastRect.center();
        pRect->drawRect(lastRect);

        // draw crosshair in the center of rectangle
        if(crossDrawn)
        {
            pRect->drawLine(lastC.x(), lastC.y() - len, lastC.x(), lastC.y() + len);
            pRect->drawLine(lastC.x() - len, lastC.y(), lastC.x() + len, lastC.y());
        }

        crossDrawn = !(rect.width() < len*2 + 2 || rect.height() < len*2 + 2);

        QPoint Cntr = rect.center();
        pRect->drawRect(rect);

        if(crossDrawn)
        {
            pRect->drawLine(Cntr.x(), Cntr.y() - len, Cntr.x(), Cntr.y() + len);
            pRect->drawLine(Cntr.x() - len, Cntr.y(), Cntr.x() + len, Cntr.y());
        }

        lastRect = rect;
    }
}

// User released some mouse button.
void SQ_GLWidget::mouseReleaseEvent(QMouseEvent *)
{
    if(movetype == -1)
        return;

    // left button - restore cursor
    if(movetype == 1)
        setCursor(KCursor::arrowCursor());
    // left button + SHIFT - zoom to selected rectangle (if needed)
    else if(movetype == 2)
    {
        setCursor(KCursor::arrowCursor());

        QPoint lastC = lastRect.center();
        QPoint O(width() / 2, height() / 2);

        delete pRect;

        updateGL();

        if(lastRect.width() > 2 && lastRect.height() > 2)
        {
            bool lastReset = reset_mode;
            reset_mode = true;
            float X = MATRIX_X, Y = MATRIX_Y;
            matrix_move(O.x() - lastC.x(), lastC.y() - O.y());
            reset_mode = lastReset;

            // try to zoom
            bool zoomed = use_broken ? false : zoomRect(lastRect);

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

/*
 *  Keyboard events.
 */
void SQ_GLWidget::keyPressEvent(QKeyEvent *e)
{
    unsigned int value = e->key();
    const unsigned short Key_KC = Qt::ControlButton|Qt::Keypad;
    const int sz = (sizeof(unsigned int) * 4);

    value <<= sz;
    value |= e->state();

#define SQ_KEYSTATES(a,b) ((a << sz) | b)

    // determine key, flags and do what user wants
    switch(value)
    {
        case SQ_KEYSTATES(Qt::Key_Left, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Left, Qt::NoButton):        matrix_move(movefactor, 0);    break;
        case SQ_KEYSTATES(Qt::Key_Right, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Right, Qt::NoButton):       matrix_move(-movefactor, 0);   break;
        case SQ_KEYSTATES(Qt::Key_Up, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Up, Qt::NoButton):          matrix_move(0, -movefactor);   break;
        case SQ_KEYSTATES(Qt::Key_Down, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Down, Qt::NoButton):        matrix_move(0, movefactor);    break;
        case SQ_KEYSTATES(Qt::Key_Plus, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Equal, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_Plus, Qt::NoButton):        slotZoomPlus();           break;
        case SQ_KEYSTATES(Qt::Key_Minus, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Minus, Qt::NoButton):       slotZoomMinus();          break;
        case SQ_KEYSTATES(Qt::Key_Plus, Key_KC):
        case SQ_KEYSTATES(Qt::Key_Equal, Qt::ControlButton):
        case SQ_KEYSTATES(Qt::Key_Plus, Qt::ControlButton):   matrix_zoom(2.0f);        break;
        case SQ_KEYSTATES(Qt::Key_Minus, Key_KC):
        case SQ_KEYSTATES(Qt::Key_Minus, Qt::ControlButton):  matrix_zoom(0.5f);        break;
        case SQ_KEYSTATES(Qt::Key_Q, Qt::NoButton):           toClipboard();              break;
        case SQ_KEYSTATES(Qt::Key_V, Qt::NoButton):           slotFlipV();              break;
        case SQ_KEYSTATES(Qt::Key_H, Qt::NoButton):           slotFlipH();              break;
        case SQ_KEYSTATES(Qt::Key_Left, Qt::ControlButton):   slotRotateLeft();         break;
        case SQ_KEYSTATES(Qt::Key_Right, Qt::ControlButton):  slotRotateRight();        break;
        case SQ_KEYSTATES(Qt::Key_R, Qt::NoButton):           slotMatrixReset();        break;
        case SQ_KEYSTATES(Qt::Key_Up, Qt::ControlButton):     matrix_rotate(180.0f);    break;
        case SQ_KEYSTATES(Qt::Key_Down, Qt::ControlButton):   matrix_rotate(-180.0f);   break;
        case SQ_KEYSTATES(Qt::Key_Left, Qt::ShiftButton):     matrix_rotate(-1.0f);     break;
        case SQ_KEYSTATES(Qt::Key_Right, Qt::ShiftButton):    matrix_rotate(1.0f);      break;
        case SQ_KEYSTATES(Qt::Key_N, Qt::NoButton):           updateFilter(!linear);    break;
        case SQ_KEYSTATES(Qt::Key_Pause, Qt::NoButton):       KSquirrel::app()->pauseSlideShow();    break;
        case SQ_KEYSTATES(Qt::Key_PageDown, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_PageDown, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_Space, Qt::NoButton):       slotNext();               break;
        case SQ_KEYSTATES(Qt::Key_PageUp, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_PageUp, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_BackSpace, Qt::NoButton):   slotPrev();               break;
        case SQ_KEYSTATES(Qt::Key_X, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_Escape, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_Return, Qt::NoButton):     // Thanks JaguarWan for suggestion.
        case SQ_KEYSTATES(Qt::Key_Enter, Qt::Keypad):        pAClose->activate();         break;
        case SQ_KEYSTATES(Qt::Key_P, Qt::NoButton):          pAProperties->activate();    break;
        case SQ_KEYSTATES(Qt::Key_Home, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Home, Qt::NoButton):       slotFirst();                 break;
        case SQ_KEYSTATES(Qt::Key_End, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_End, Qt::NoButton):        slotLast();                  break;
        case SQ_KEYSTATES(Qt::Key_C, Qt::NoButton):          slotShowCodecSettings();        break;
        case SQ_KEYSTATES(Qt::Key_F, Qt::NoButton):          toggleFullScreen();         break;
        case SQ_KEYSTATES(Qt::Key_T, Qt::NoButton):          pAHideToolbars->activate(); break;
        case SQ_KEYSTATES(Qt::Key_Z, Qt::NoButton):          slotZoomMenu();             break;
        case SQ_KEYSTATES(Qt::Key_Y, Qt::NoButton):          pAStatus->activate();       break;
        case SQ_KEYSTATES(Qt::Key_S, Qt::NoButton):          saveAs();       break;
        case SQ_KEYSTATES(Qt::Key_A, Qt::NoButton):          slotToggleAnimate();        break;
        case SQ_KEYSTATES(Qt::Key_I, Qt::NoButton):          slotShowImages();           break;
        case SQ_KEYSTATES(Qt::Key_F1, Qt::NoButton):         jumpToImage(false);         break;
        case SQ_KEYSTATES(Qt::Key_F2, Qt::NoButton):         prevImage();                break;
        case SQ_KEYSTATES(Qt::Key_F3, Qt::NoButton):         nextImage();                break;
        case SQ_KEYSTATES(Qt::Key_F4, Qt::NoButton):         jumpToImage(true);          break;
        case SQ_KEYSTATES(Qt::Key_Slash, Qt::NoButton):      slotShowHelp();             break;
        case SQ_KEYSTATES(Qt::Key_B, Qt::NoButton):          toggleDrawingBackground();  break;
        case SQ_KEYSTATES(Qt::Key_K, Qt::NoButton):          toogleTickmarks();          break;
        case SQ_KEYSTATES(Qt::Key_E, Qt::NoButton):          showExternalTools();        break;
        case SQ_KEYSTATES(Qt::Key_Delete, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Delete, Qt::NoButton):     deleteWrapper();            break;

        case SQ_KEYSTATES(Qt::Key_Menu, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_M, Qt::NoButton):          menu->exec(QCursor::pos()); break;

        default:
        {
            // 1 ... 9, 0 should zoom image to 100% ... 900%, 1000%
            switch(e->key())
            {
                case Qt::Key_1:
                case Qt::Key_2:
                case Qt::Key_3:
                case Qt::Key_4:
                case Qt::Key_5:
                case Qt::Key_6:
                case Qt::Key_7:
                case Qt::Key_8:
                case Qt::Key_9:
                case Qt::Key_0:
                {
                    if(use_broken || finfo.image.empty()) break;

                    int val = e->key();
                    val = val - Qt::Key_1 + 1;
                    matrix_pure_reset();
                    if(finfo.image[current].needflip) flip(4, false);

                    if(val == 0) val = 10;

                    matrix_zoom(val);
                }
                break;

                case Qt::Key_Comma:     slotZoomW();      break;
                case Qt::Key_Period:    slotZoomH();      break;
                case Qt::Key_Asterisk:  slotZoomWH();     break;
                case Qt::Key_L:
                {
                    bool b = pAIfLess->isChecked();
                    pAIfLess->setChecked(!b);
                    slotZoomIfLess();
                }
                break;
            }
        }
    }
}

// Accept drop events.
void SQ_GLWidget::dropEvent(QDropEvent *e)
{
    QStringList files;

    if(QUriDrag::decodeLocalFiles(e, files))
    {
        // go through array and find first supported image format
        for(QStringList::iterator it = files.begin();it != files.end();++it)
        {
            if(SQ_LibraryHandler::instance()->supports(*it))
            {
                slotStartDecoding(*it);
                break;
            }
        }
    }
}

// Accept drag events.
void SQ_GLWidget::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(QUriDrag::canDecode(e));
}

// Fit width.
void SQ_GLWidget::slotZoomW()
{
    zoom_type = 0;
    pAZoomW->setChecked(true);

    // no image decoded
    if(use_broken || finfo.image.empty()) return;

    // calculate zoom factor
    float factor = (float)width() / (float)finfo.image[current].w;

    // "Ignore, if the image is less than window" - restore zoom factor to 1.0
    if(pAIfLess->isChecked() && (finfo.image[current].w < width() && finfo.image[current].h < height()))
        factor = 1.0f;

    // zoom...
    internalZoom(factor);
}

// Fit height.
void SQ_GLWidget::slotZoomH()
{
    zoom_type = 1;
    pAZoomH->setChecked(true);

    if(use_broken || finfo.image.empty()) return;

    float factor = (float)height() / (float)finfo.image[current].h;

    if(pAIfLess->isChecked() && (finfo.image[current].w < width() && finfo.image[current].h < height()))
        factor = 1.0f;

    internalZoom(factor);
}

// Fit image (e.g. width and height).
void SQ_GLWidget::slotZoomWH()
{
    zoom_type = 2;
    pAZoomWH->setChecked(true);

    if(use_broken || finfo.image.empty()) return;

    float factor = 1.0;
    float w = (float)width(), h = (float)height();
    float factorw = w / (float)finfo.image[current].w;
    float factorh = h / (float)finfo.image[current].h;
    float t = w / h;

    if(t > (float)finfo.image[current].w / (float)finfo.image[current].h)
        factor = factorh;
    else
        factor = factorw;

    if(pAIfLess->isChecked() && (finfo.image[current].w < width() && finfo.image[current].h < height()))
        factor = 1.0f;

    internalZoom(factor);
}

// Previous zoom.
void SQ_GLWidget::slotZoomLast()
{
    zoom_type = 4;
    pAZoomLast->setChecked(true);

    if(use_broken || finfo.image.empty()) return;

    internalZoom(zoomFactor);
}

// Zoom 100%.
void SQ_GLWidget::slotZoom100()
{
    zoom_type = 3;
    pAZoom100->setChecked(true);

    if(use_broken || finfo.image.empty()) return;

    internalZoom(1.0f);
}

// "Ignore, if the image is less than window"
void SQ_GLWidget::slotZoomIfLess()
{
    if(use_broken || finfo.image.empty()) return;

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
    matrix_zoom(1.0f+zoomfactor/100.0f);
}

// Zoom-
void SQ_GLWidget::slotZoomMinus()
{
    matrix_zoom(1.0f/(1.0f+zoomfactor/100.0f));
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

// Show image properties.
void SQ_GLWidget::slotProperties()
{
    if(use_broken || finfo.image.empty()) return;

    // Stop animation...
    stopAnimation();

    const int real_size = finfo.image[current].w * finfo.image[current].h * sizeof(RGBA);
    QString sz = KIO::convertSize(real_size);
    QStringList list;

    QValueVector<QPair<QString,QString> > meta;

    if(!finfo.meta.empty())
    {
        for(std::vector<fmt_metaentry>::iterator it = finfo.meta.begin();it != finfo.meta.end();++it)
        {
            meta.append(QPair<QString,QString>((*it).group, (*it).data));
        }
    }

    // save current image parameters and some additioanl information
    // in list
    list    << quickImageInfo
            << QString::fromLatin1("%1x%2").arg(finfo.image[current].w).arg(finfo.image[current].h)
            << QString::fromLatin1("%1").arg(finfo.image[current].bpp)
            << finfo.image[current].colorspace
            << finfo.image[current].compression
            << sz
            << QString::fromLatin1("%1").arg((double)real_size / fm.size(), 0, 'f', 2)
            << ((finfo.image[current].interlaced) ? i18n("yes") : i18n("no"))
            << QString::fromLatin1("%1").arg(errors)
            << QString::fromLatin1("%1").arg(finfo.image.size())
            << QString::fromLatin1("#%1").arg(current+1)
            << QString::fromLatin1("%1").arg(finfo.image[current].delay);

    // create dialog and setup it
    SQ_ImageProperties prop(this);
    prop.setFile(File);
    prop.setParams(list);
    prop.setMetaInfo(meta);

    // show!
    prop.exec();

    // restore animation
    if(!manualBlocked())
        startAnimation();
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

    matrix[id] *= -1.0;
    matrix[id + 1] *= -1.0;
    matrix[id + 3] *= -1.0;

    MATRIX_X = x;
    MATRIX_Y = y;

    write_gl_matrix();

    if(!reset_mode && U)
        updateGL();
}

void SQ_GLWidget::slotFlipH()
{
    if(!use_broken)
    {
        isflippedH = !isflippedH;
        flip(0);
    }
}

void SQ_GLWidget::slotFlipV()
{
    if(!use_broken)
    {
        isflippedV = !isflippedV;
        flip(4);
    }
}

void SQ_GLWidget::slotMatrixReset()
{
    if(!use_broken)
    {
        matrix_reset();
        updateGL();
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
    if(use_broken) return;

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
    memcpy(saved, matrix, sizeof(matrix));
}

void SQ_GLWidget::matrix_pop()
{
    memcpy(matrix, saved, sizeof(matrix));
}

void SQ_GLWidget::matrix_reset()
{
    int i;

    for (i = 0; i < 12; i++)
        matrix[i] = (GLfloat) (i % 5 == 0);

    curangle = 0.0f;
    isflippedH = isflippedV = false;

    if(decoded)
        if(finfo.image[current].needflip)
        {
            slotFlipV();
            return;
        }

    write_gl_matrix();
}

void SQ_GLWidget::matrix_pure_reset()
{
    int i;

    for (i = 0; i < 12; i++)
        matrix[i] = (GLfloat) (i % 5 == 0);

    write_gl_matrix();
}

bool SQ_GLWidget::matrix_zoom(GLfloat ratio)
{
    if(use_broken) return false;

    SQ_Config::instance()->setGroup("GL view");

    int zoom_lim = SQ_Config::instance()->readNumEntry("zoom limit", 1);
    float zoom_min, zoom_max, zoom_tobe;

    zoom_tobe = hypotf(MATRIX_C1 * ratio, MATRIX_S1 * ratio) * 100.0f;

    switch(zoom_lim)
    {
        case 2:
            zoom_min = (float)SQ_Config::instance()->readNumEntry("zoom_min", 1);
            zoom_max = (float)SQ_Config::instance()->readNumEntry("zoom_max", 10000);
        break;

        default: // "case 1:" too
            zoom_min = 1.0f;
            zoom_max = 10000.0f;
    }

    if(zoom_lim)
    {
        float z = get_zoom_pc();

        // zoom limit exceeded - do nothing
        if((z >= zoom_max && ratio > 1.0) || (z <= zoom_min && ratio < 1.0))
            return false;

        // if the new zoom will be greater (smaller) than maximum
        // (minimum) zoom - scale it.
        if(ratio < 1.0f && zoom_min >= zoom_tobe)
            ratio = ratio * zoom_min / zoom_tobe;
        else if(ratio > 1.0f && zoom_max <= zoom_tobe)
            ratio = ratio * zoom_max / zoom_tobe;
    }

    MATRIX_C1 *= ratio;
    MATRIX_S1 *= ratio;
    MATRIX_X  *= ratio;
    MATRIX_S2 *= ratio;
    MATRIX_C2 *= ratio;
    MATRIX_Y  *= ratio;

    write_gl_matrix();

    if(!reset_mode)
        updateGL();

    return true;
}

GLfloat SQ_GLWidget::get_zoom() const
{
    return hypotf(MATRIX_C1, MATRIX_S1);
}

GLfloat SQ_GLWidget::get_zoom_pc() const
{
    return get_zoom() * 100.0f;
}

void SQ_GLWidget::matrix_rotate(GLfloat angle)
{
    if(use_broken) return;

    double cosine, sine, rad;
    const double rad_const = 0.017453;
    GLfloat c1 = MATRIX_C1, c2 = MATRIX_C2, s1 = MATRIX_S1, s2 = MATRIX_S2;

    rad = (double)angle * rad_const;
    cosine = cos(rad);
    sine = sin(rad);

    MATRIX_C1 = c1  * cosine + s2 * sine;
    MATRIX_S1 = s1  * cosine + c2 * sine;
    MATRIX_S2 = -c1 * sine   + s2 * cosine;
    MATRIX_C2 = -s1 * sine   + c2 * cosine;

    curangle += angle;

    if(curangle == 360.0f || curangle == -360.0f)
        curangle = 0.0f;
    else if(curangle > 360.0f)
        curangle -= 360.0f;
    else if(curangle < -360.0f)
        curangle += 360.0f;

    write_gl_matrix();
    updateGL();
}

void SQ_GLWidget::matrix_rotate2(GLfloat angle)
{
    double cosine, sine, rad;
    const double rad_const = 0.017453;
    GLfloat c1 = MATRIX_C1, c2 = MATRIX_C2, s1 = MATRIX_S1, s2 = MATRIX_S2;

    rad = angle * rad_const;
    cosine = cos(rad);
    sine = sin(rad);

    MATRIX_C1 = c1 * cosine + s2 * sine;
    MATRIX_S1 = s1 * cosine + c2 * sine;
    MATRIX_S2 = -c1 * sine + s2 * cosine;
    MATRIX_C2 = -s1 * sine + c2 * cosine;

    write_gl_matrix();
}

/*
 *  Set clear color for context.
 */
void SQ_GLWidget::setClearColor()
{
    QColor color;
    QString path;

    SQ_Config::instance()->setGroup("GL view");

    switch(SQ_Config::instance()->readNumEntry("GL view background type", 1))
    {
        // system color
        case 0:
            color = colorGroup().color(QColorGroup::Base);
        break;

        // custom color
        case 1:
            color.setNamedColor(SQ_Config::instance()->readEntry("GL view background", "#4e4e4e"));
        break;

        // repeated texture
        case 2:
            path = SQ_Config::instance()->readEntry("GL view custom texture", "");
            BGpixmap.load(path);

            if(BGpixmap.isNull())
            {
                kdWarning() << "Texture is corrupted! Falling back to 'System color'" << endl;
                SQ_Config::instance()->writeEntry("GL view background type", 0);
                setClearColor();
                return;
            }

            BGpixmap.convertDepth(32);
            BGpixmap = BGpixmap.swapRGB();

            changed = true;
        break;

        default: ;
    }

    changed2 = true;

    // update clear color
    qglClearColor(color);

    if(decoded)
        updateGL();
}

void SQ_GLWidget::findCloserTiles(int w, int h, std::vector<int> &x, std::vector<int> &y)
{
    static int s = 9;
    static int dims[10] = { 2, 4, 8, 16, 32, 64, 128, 256, 512 };

    int *dd[2] = { &w, &h };
    std::vector<int> *dv[2] = { &x, &y };
    int del;

    for(int ff = 0;ff < 2;ff++)
    {
        if(*dd[ff] == 1) *dd[ff]=2;
        else if((*dd[ff])%2) (*dd[ff])++;

        while((*dd[ff]) >= 512)
        {
            (*dv[ff]).push_back(512);
            (*dd[ff]) -= 512;
        }

        for(int i = 0;i < s-1;i++)
            if((*dd[ff]) >= dims[i] && (*dd[ff]) < dims[i+1])
            {
                del = dims[i] + (dims[i]>>1);

                if((*dd[ff]) <= del)
                {
                    (*dv[ff]).push_back(dims[i]);
                    (*dd[ff]) -= dims[i];
                }
                else
                {
                    (*dv[ff]).push_back(dims[i+1]);
                    (*dd[ff]) -= dims[i+1];
                }

                i = -1;
            }
    }
/*
    printf("X: ");
    for(uint i = 0;i < x.size();i++)
        printf("%d ", x[i]);
    printf("\n");

    printf("Y: ");
    for(uint i = 0;i < y.size();i++)
        printf("%d ", y[i]);
    printf("\n");
*/
}

QPair<int, int> SQ_GLWidget::calcRealDimensions(Parts &p, int y, int x)
{
    int rw = 0, rh = 0;
    int toy = y == -1 ? p.tilesy.size() : y;
    int tox = x == -1 ? p.tilesx.size() : x;

//    printf("tox: %d, toy: %d\nsizes: %u, %u\n", tox, toy, p.tilesx.size(), p.tilesy.size());

    std::vector<int>::iterator itEnd = p.tilesx.end();

    for(std::vector<int>::iterator it = p.tilesx.begin();it != itEnd && tox--;++it)
    {
//        printf("VAL %d\n", (*it));
        rw += (*it);
    }

//    printf("rw: %d\n", rw);

    itEnd = p.tilesy.end();

    for(std::vector<int>::iterator it = p.tilesy.begin();it != itEnd && toy--;++it)
    {
        rh += (*it);
    }

//    printf("rh: %d\n", rh);

    return QPair<int, int>(rw, rh);
}

void SQ_GLWidget::slotShowCodecSettings()
{
    if(!lib || lib->config.isEmpty()) // gg ?
    {
        enableSettingsButton(false);
        return;
    }

    SQ_CodecSettingsSkeleton skel(this);

    connect(&skel, SIGNAL(apply()), this, SLOT(slotApplyCodecSettings()));

    skel.addSettingsWidget(lib->config);
    skel.setCodecInfo(lib->mime, lib->quickinfo);
    skel.adjustSize();

    if(skel.exec(lib->settings) == QDialog::Accepted)
        lib->codec->set_settings(lib->settings);
}

void SQ_GLWidget::slotApplyCodecSettings()
{
    if(lib)
    {
        // new settings are already set by SQ_CodecSettingsSkeleton
        lib->codec->set_settings(lib->settings);
        slotStartDecoding(File, true);
    }
}

void SQ_GLWidget::removeCurrentParts()
{
    // if use_broken, 'parts' has no members
    if(decoded && !use_broken)
    {
        for(std::vector<Parts>::iterator it = parts.begin();it != parts.end();++it)
        {
            // delete textures and memory buffers
            (*it).removeParts();
            (*it).deleteBuffer();
        }

        parts.clear();
    }
}

/*
 *  Prepare decoding. It will find proper library for decoding,
 *  clear old memory buffers, etc.
 */
bool SQ_GLWidget::prepare()
{
    QString status;
    int     i;

    fm.setFile(File);

    // hope somebody already called SQ_LibraryHandler::instance()->supports() :-)
    lib = SQ_LibraryHandler::instance()->latestLibrary();

    if(!lib)
    {
        KMessageBox::error(KSquirrel::app(), i18n("Library for %1 format not found").arg(fm.extension(false)));
        reset_mode = false;
        return false;
    }

    // determine codec
    codeK = lib->codec;

    // clear any previous info
    finfo.image.clear();
    finfo.meta.clear();

    removeCurrentParts();

    // start decoding!
    i = codeK->read_init(File);

    // oops, error...
    if(i != SQE_OK)
    {
        codeK->read_close();
        useBrokenImage(i);
        return false;
    }

    return true;
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

    for(z = a;z < b;z++)
    {
        glBindTexture(GL_TEXTURE_2D, p->m_parts[z].tex);

        // setup texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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

/*
 *  Divide currently decoded image into
 *  several parts and store them in MemoryPart::m_data.
 */
void SQ_GLWidget::setupBits(Parts *p, RGBA *_buffer, int y, int x)
{
//    printf("XY: %d %d\n", x, y);
    QPair<int, int> pair = calcRealDimensions(*p, y, x);
//    printf("OFFSET %d, %d\n", pair.first, pair.second);

    int offs = p->realw * pair.second + pair.first;
    RGBA *orig = p->buffer->data() + offs;

    int toy = p->tilesy[y];
    int tox = p->tilesx[x];

    for(int j = 0;j < toy;j++)
    {
//        printf("COPY TO %d FROM %d\n", tox*j, offs + p->realw*j);
        memcpy(_buffer + tox*j, orig + p->realw*j, tox*sizeof(RGBA));
    }
}

// Delete textures and display lists.
void Parts::removeParts()
{
    if(!m_parts.empty())
    {
        int toy = tilesy.size();
        int toxy = tilesx.size() * toy;

        for(int z = 0;z < toxy;z++)
            glDeleteTextures(1, &m_parts[z].tex);

        glDeleteLists(m_parts[0].list, toy);

        m_parts.clear();
    }
}

// Create parts: generate textures and display lists.
bool Parts::makeParts()
{
    int z;
    int toy = tilesy.size();

    GLuint base = glGenLists(toy);

    if(!base)
        return false;

    Part pt;
    int tox = tilesx.size();
    int toxy = tox * toy;

    for(z = 0;z < toxy;z++)
    {
        glGenTextures(1, &pt.tex);
        m_parts.push_back(pt);
    }

    // calculate display list's id
    for(z = 0;z < toy;z++)
        m_parts[z * tox].list = base + z;

    return true;
}

// Calculate coordinates for textures
void Parts::computeCoords()
{
    Part *p;
    int index = 0;
    float X, Y;

    Y = (float)h / 2.0;

    int tlsy = tilesy.size();
    int tlsx = tilesx.size();

    for(int y = 0;y < tlsy;y++)
    {
        X = -(float)w / 2.0;

        for(int x = 0;x < tlsx;x++)
        {
            p = &m_parts[index];

            p->x1 = X;
            p->y1 = Y;
            p->x2 = X + tilesx[x];
            p->y2 = Y - tilesy[y];

            p->tx1 = 0.0;
            p->tx2 = 1.0;
            p->ty1 = 0.0;
            p->ty2 = 1.0;

            index++;
            X += tilesx[x];
        }

        Y -= tilesy[y];
    }
}

void SQ_GLWidget::slotStartDecoding(const KURL &url)
{
    slotStartDecoding(url.path());
}

/*
 *  Start decoding given image. We can call it from anywhere.
 */
void SQ_GLWidget::slotStartDecoding(const QString &file, bool isMouseEvent)
{
    if(reset_mode)
        return;

    reset_mode = true;
    current = 0;

    timer_anim->stop();
    images->clear();

    m_File = file;
    File = QFile::encodeName(m_File);

    // prepare decoding...
    if(!prepare())
        return;

    use_broken = false;

    bool slideShowRunning = KSquirrel::app()->slideShowRunning();

    SQ_Config::instance()->setGroup("Slideshow");

    // goto fullscreen, if needed
    if(slideShowRunning && !fullscreen() && SQ_Config::instance()->readBoolEntry("fullscreen", true))
        toggleFullScreen();

    // show window with image
    if(!slideShowRunning || (slideShowRunning && SQ_Config::instance()->readBoolEntry("force", true)))
        KSquirrel::app()->raiseGLWidget();

    KSquirrel::app()->setCaption(file);

    if(!pAHideToolbars->isChecked())
        SQ_GLView::window()->toolbar()->show();

    if(!decoded || isMouseEvent)
        qApp->processEvents();

    zoomFactor = get_zoom();
    matrix_pure_reset();
    matrixChanged();

    SQ_CodecSettings::applySettings(lib, SQ_CodecSettings::ImageViewer);

    errors = 0;
    timer_decode->start(timer_delay, true);
}

void SQ_GLWidget::slotDecode()
{
    int i, j, id;
    int line, res, first_id = 0;
    fmt_image *im;
    memoryPart *pt;
    SQ_Config::instance()->setGroup("GL view");
    bool progr;

    // in fullscreen mode progressive loading is disabled anyway
    if(fullscreen() || KSquirrel::app()->slideShowRunning())
       progr = false;
    else
       progr = SQ_Config::instance()->readBoolEntry("progressiv", true);

    int allpages = KSquirrel::app()->slideShowRunning() ? 1 : SQ_Config::instance()->readNumEntry("load_pages", 0);
    int pages_num = KSquirrel::app()->slideShowRunning() ? 1 : SQ_Config::instance()->readNumEntry("load_pages_number", 1);

    if(pages_num < 1) pages_num = 1;

    QFileInfo ff(m_File);

    SQ_GLView::window()->sbarWidget("SBFile")->setText(ff.fileName());

    current = 0;

    // start time counting
    QTime started;
    started.start();

    while(true)
    {
        if((allpages == 1 && current) || (allpages == 2 && current == pages_num))
            break;

        i = codeK->read_next();

        // something went wrong. SQE_NOTOK is a special type of error. It means
        // that we decoded all pages.
        if(i != SQE_OK)
        {
            if(i == SQE_NOTOK || current)
                break;
            else
            {
                decodeFailedOn0();
                useBrokenImage(i);
                return;
            }
        }

        im = codeK->image(current);

        qApp->processEvents();

        Parts pp;

        // find tile size
        findCloserTiles(im->w, im->h, pp.tilesx, pp.tilesy);
        QPair<int, int> pair = calcRealDimensions(pp);
        pp.realw = pair.first;
        pp.realh = pair.second;

        // setup current Part
        pp.w = im->w;
        pp.h = im->h;

        // create textures and display lists
        if(!pp.makeParts())
        {
            KMessageBox::error(KSquirrel::app(),
                        i18n("Memory allocation failed for %1 of memory.")
                        .arg(KIO::convertSize(pp.realw * pp.realh * sizeof(RGBA))));

            decodeFailedOn0();
            return;
        }

#define SQ_FAIL_RET \
                if(current)    \
                    break;     \
                else            \
                {                 \
                    KMessageBox::error(KSquirrel::app(), i18n("Decoding failed")); \
                    decodeFailedOn0(); \
                    return;                    \
                }

        pt = new memoryPart(pp.realw * pp.realh);
        pt->create();

        if(!pt->valid())
        {
            KMessageBox::error(KSquirrel::app(), i18n("Memory allocation failed"));

            SQ_FAIL_RET
        }

#undef SQ_FAIL_RET

        line = 0;

        pp.computeCoords();
        pp.buffer = pt;

        if(!current)
        {
            finfo = codeK->information();
            isflippedH = isflippedV = false;
            slotZoomIfLess();
            matrixChanged();
        }

        matrix_move_z(SQ_FIRST_TILE_LAYER+current);

        for(int pass = 0;pass < im->passes;pass++)
        {
            if(codeK->read_next_pass() != SQE_OK)
                break;

            line = 0;
            int tlsy = pp.tilesy.size();
            int tlsyval, offs = 0;

            for(i = 0;i < tlsy;i++)
            {
                tlsyval = pp.tilesy[i];

                for(j = 0;j < tlsyval;j++)
                {
                    res = codeK->read_scanline(pp.buffer->data() + (offs + j)*pp.realw);
                    errors += (int)(res != SQE_OK);

                    if(++line == im->h)
                        break;
                }

                offs += tlsyval;

                // last pass
                if(pass == im->passes-1)
                {
//                    setupBits(&pp, next, (canbemulti ? i : 0));
//                    setupBits(&pp, next, i);

//                    if(!current)
                    {
                        bool b = showFrames(i, &pp, progr);

                        if(!b)
                            kdWarning() << "Showframes failed for image " << current << ", tiley " << i << endl;
                    }
                }

            }
        }

        id = images->insertItem(QString::fromLatin1("#%1 [%2x%3@%4]").arg(current+1).arg(im->w).arg(im->h).arg(im->bpp));

        images->setItemParameter(id, current);

        if(!current)
        {
            SQ_GLView::window()->sbarWidget("SBDecodedI")->clear();
            SQ_GLView::window()->sbarWidget("SBDecodedI")->setPixmap(lib->mime);

            old_id = first_id = id;
        }

        parts.push_back(pp);

        frameChanged();
        current++;
    }

    finfo = codeK->information();
    codeK->read_close();
    total = finfo.image.size();
    current = 0;
    updateCurrentFileInfo();

    menuFile->setItemEnabled(id_saveas, true);
    enableSettingsButton(!lib->config.isEmpty());

    decoded = true;
    reset_mode = false;
    frameChanged();
    updateGL();

    quickImageInfo = lib->quickinfo;

    SQ_GLView::window()->sbarWidget("SBLoaded")->setText(
                        KGlobal::locale()->formatLong(started.elapsed()) + i18n(" ms."));

    images->setItemChecked(first_id, true);

    if(finfo.animated)
        QTimer::singleShot(finfo.image[current].delay, this, SLOT(slotAnimateNext()));
}

void SQ_GLWidget::slotHideToolbars(bool hidden)
{
    SQ_GLView::window()->toolbar()->setHidden(hidden);
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
        changed2 = true;
        updateGL();
    }
}

void SQ_GLWidget::slotToggleStatus(bool s)
{
    SQ_GLView::window()->statusbar()->setShown(s);
}

void SQ_GLWidget::slotFirst()
{
    KSquirrel::app()->stopSlideShow();

    if(!reset_mode)
        SQ_WidgetStack::instance()->slotFirstFile();
}

void SQ_GLWidget::slotLast()
{
    KSquirrel::app()->stopSlideShow();

    if(!reset_mode)
        SQ_WidgetStack::instance()->slotLastFile();
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

//    parts[current].removeParts();

    current++;

    if(current >= (int)finfo.image.size())
        current = 0;

    updateCurrentFileInfo();
    updateGL();

    int delay = finfo.image[current].delay;

    timer_anim->start(delay, true);
}

void SQ_GLWidget::startAnimation()
{
    if(!finfo.animated) return;

    timer_anim->start(finfo.image[current].delay, true);
}

void SQ_GLWidget::stopAnimation()
{
    timer_anim->stop();
}

void SQ_GLWidget::slotToggleAnimate()
{
    if(!finfo.animated) return;

    if(!timer_anim->isActive())
    {
        blocked_force = false;
        startAnimation();
    }
    else
    {
        blocked_force = true;
        stopAnimation();
    }
}

void SQ_GLWidget::slotSetCurrentImage(int id)
{
    if(total == 1)
        return;

    images->setItemChecked(old_id, false);
    current = images->itemParameter(id);

    images->setItemChecked(id, true);
    old_id = id;

    updateCurrentFileInfo();
    updateGL();
}

void SQ_GLWidget::slotImagesShown()
{
    if(finfo.animated)
    {
        if(!timer_anim->isActive())
            blocked = false;
        else
        {
            stopAnimation();
            blocked = true;
        }
    }

    images->setItemChecked(old_id, false);
    int id = images->idAt(current);
    images->setItemChecked(id, true);
    old_id = id;
}

void SQ_GLWidget::slotShowImages()
{
    images->exec(QCursor::pos());
}

void SQ_GLWidget::slotImagesHidden()
{
    if(blocked && finfo.animated)
        startAnimation();
}

/*
 *  Next image in animated sequence. Called by user (with F3).
 */
void SQ_GLWidget::nextImage()
{
    // if the image has only one page - do nothing
    if(total == 1)
        return;

    current++;

    if(current >= total)
        current = 0;

    updateGL();
    updateCurrentFileInfo();
}

/*
 *  Previous image in animated sequence. Called by user (with F2).
 */
void SQ_GLWidget::prevImage()
{
    if(total == 1)
        return;

    current--;

    if(current < 0)
        current = total - 1;

    updateGL();
    updateCurrentFileInfo();
}

/*
 *  Jump to first/last image page.
 */
void SQ_GLWidget::jumpToImage(bool last)
{
    // if the image has only one page - do nothing
    if(total == 1)
        return;

    current = (last) ? finfo.image.size() - 1 : 0;

    updateGL();
    updateCurrentFileInfo();
}

void SQ_GLWidget::slotShowHelp()
{
    SQ_HelpWidget help_w(this);

    help_w.exec();
}

// Show/hide background for transparent image.
void SQ_GLWidget::toggleDrawingBackground()
{
    SQ_Config::instance()->setGroup("GL view");

    bool b = SQ_Config::instance()->readBoolEntry("alpha_bkgr", true);

    b = !b;

    SQ_Config::instance()->writeEntry("alpha_bkgr", b);

    updateGL();
}

void SQ_GLWidget::showExternalTools()
{
    SQ_ExternalTool::instance()->constPopupMenu()->exec(QCursor::pos());
}

void SQ_GLWidget::bindMarks(bool &first, bool deleteOld)
{
    if(!marks || !first)
        return;

    first = false;

    for(int i = 0;i < 4;i++)
    {
        if(deleteOld)
            glDeleteTextures(1, &mark[i]);

        glGenTextures(1, &mark[i]);

        glBindTexture(GL_TEXTURE_2D, mark[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, mm[i].bits());
    }
}

void SQ_GLWidget::createMarks()
{
    mm[0] = QImage(locate("appdata", "images/marks/mark_1.png"));
    mm[1] = QImage(locate("appdata", "images/marks/mark_2.png"));
    mm[2] = QImage(locate("appdata", "images/marks/mark_3.png"));
    mm[3] = QImage(locate("appdata", "images/marks/mark_4.png"));

    marks = (mm[0].isNull() || mm[1].isNull() || mm[2].isNull() || mm[3].isNull()) ? false : true;

    if(!marks)
        return;

    for(int i = 0;i < 4;i++)
    {
        mm[i] = mm[i].convertDepth(32);
        mm[i].setAlphaBuffer(true);
    }
}

// Delete current image (user pressed 'Delete' key).
void SQ_GLWidget::deleteWrapper()
{

    if(m_File.isEmpty())
        return;

    KURL url = m_File;
    KFileItemList list;
    list.append(new KFileItem(url, QString::null, KFileItem::Unknown));

    SQ_WidgetStack::instance()->diroperator()->del(list, this);
}

/*
 *  Show current image's width, height and bpp in statusbar.
 */
void SQ_GLWidget::updateCurrentFileInfo()
{
    QString status = QString::fromLatin1("%1x%2@%3")
    .arg(finfo.image[current].w)
    .arg(finfo.image[current].h)
    .arg(finfo.image[current].bpp);

    SQ_GLView::window()->sbarWidget("SBDecoded")->setText(status);
}

// Show/hide tickmarks around the image.
void SQ_GLWidget::toogleTickmarks()
{
    SQ_Config::instance()->setGroup("GL view");

    bool b = SQ_Config::instance()->readBoolEntry("marks", true);

    b = !b;

    SQ_Config::instance()->writeEntry("marks", b);

    updateGL();
}

/*
 *  Show current page number in multipaged images.
 * 
 *  For example: "3/11" means that current page is the third in current image,
 *  which has 11 pages.
 */
void SQ_GLWidget::frameChanged()
{
    QString s = QString::fromLatin1("%1/%2").arg(current+1).arg(total);

    SQ_GLView::window()->sbarWidget("SBFrame")->setFixedWidth(
            SQ_GLView::window()->sbarWidget("SBFrame")->fontMetrics()
            .boundingRect(QString::fromLatin1("0%1/0%2").arg(total).arg(total)).width());

    SQ_GLView::window()->sbarWidget("SBFrame")->setText(s);
}

void SQ_GLWidget::internalZoom(const GLfloat &zF, bool U)
{
    curangle = 0.0f;
    matrix_pure_reset();

    if(finfo.image[current].needflip)
        flip(4, U);

    matrix_zoom(zF);
}

// Create context menu with actions.
void SQ_GLWidget::createContextMenu(QPopupMenu *m)
{
    QPopupMenu *menuRotate = new QPopupMenu(m);
    QPopupMenu *menuZoom = new QPopupMenu(m);
    QPopupMenu *menuMove = new QPopupMenu(m);
    QPopupMenu *menuWindow = new QPopupMenu(m);
    menuImage = new QPopupMenu(m);

    connect(m, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));

    menuFile = new QPopupMenu(m);
    m->insertItem(SQ_IconLoader::instance()->loadIcon("icons", KIcon::Desktop, KIcon::SizeSmall), i18n("File"), menuFile);
    connect(menuFile, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));

    m->insertItem(SQ_IconLoader::instance()->loadIcon("view_orientation", KIcon::Desktop, KIcon::SizeSmall), i18n("Rotate"), menuRotate);
    m->insertItem(SQ_IconLoader::instance()->loadIcon("viewmag", KIcon::Desktop, KIcon::SizeSmall), i18n("Zoom"), menuZoom);
    m->insertItem(i18n("Move"), menuMove);
    m->insertItem(i18n("Window"), menuWindow);
    m->insertItem(i18n("Image"), menuImage);

    connect(menuRotate, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));
    connect(menuZoom, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));
    connect(menuMove, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));
    connect(menuWindow, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));
    connect(menuImage, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));

    id_saveas = menuFile->insertItem(SQ_IconLoader::instance()->loadIcon("filesaveas", KIcon::Desktop, KIcon::SizeSmall), i18n("Save As..."), (Qt::NoButton << 16 ) | Qt::Key_S);
    menuFile->setItemEnabled(id_saveas, false);
    menuFile->insertSeparator();
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/next16.png")), i18n("Next") + "\tPageDown", (Qt::NoButton << 16 ) | Qt::Key_PageDown);
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/prev16.png")), i18n("Previous") + "\tPageUp", (Qt::NoButton << 16 ) | Qt::Key_PageUp);
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/first16.png")), i18n("First") + "\tHome", (Qt::NoButton << 16 ) | Qt::Key_Home);
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/last16.png")), i18n("Last") + "\tEnd", (Qt::NoButton << 16 ) | Qt::Key_End);
    menuFile->insertSeparator();
    menuFile->insertItem(i18n("Delete"), (Qt::NoButton << 16 ) | Qt::Key_Delete);

    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/rotateLeft16.png")), i18n("Rotate left") + "\tCtrl+Left", (Qt::ControlButton << 16 ) | Qt::Key_Left);
    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/rotateRight16.png")), i18n("Rotate right") + "\tCtrl+Right", (Qt::ControlButton << 16 ) | Qt::Key_Right);
    menuRotate->insertSeparator();
    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/18016.png")), i18n("Rotate 180'") + "\tCtrl+Up", (Qt::ControlButton << 16 ) | Qt::Key_Up);
    menuRotate->insertSeparator();
    menuRotate->insertItem(i18n("Rotate 1' left") + "\tShift+Left", (Qt::ShiftButton << 16 ) | Qt::Key_Left);
    menuRotate->insertItem(i18n("Rotate 1' right") + "\tShift+Right", (Qt::ShiftButton << 16 ) | Qt::Key_Right);

    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom+16.png")), i18n("Zoom +") + "\t+", (Qt::NoButton << 16 ) | Qt::Key_Plus);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom-16.png")), i18n("Zoom -") + "\t-", (Qt::NoButton << 16 ) | Qt::Key_Minus);
    menuZoom->insertItem(i18n("Zoom 2x") + "\tCtrl++", (Qt::ControlButton << 16 ) | Qt::Key_Plus);
    menuZoom->insertItem(i18n("Zoom 1/2x") + "\tCtrl+-", (Qt::ControlButton << 16 ) | Qt::Key_Minus);
    menuZoom->insertSeparator();
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom10016.png")), QString::fromLatin1("100%") + "\t1", (Qt::NoButton << 16 ) | Qt::Key_1);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom20016.png")), QString::fromLatin1("200%") + "\t2", (Qt::NoButton << 16 ) | Qt::Key_2);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom30016.png")), QString::fromLatin1("300%") + "\t3", (Qt::NoButton << 16 ) | Qt::Key_3);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom50016.png")), QString::fromLatin1("500%") + "\t5", (Qt::NoButton << 16 ) | Qt::Key_5);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom70016.png")), QString::fromLatin1("700%") + "\t7", (Qt::NoButton << 16 ) | Qt::Key_7);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom90016.png")), QString::fromLatin1("900%") + "\t9", (Qt::NoButton << 16 ) | Qt::Key_9);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom100016.png")), QString::fromLatin1("1000%") + "\t0", (Qt::NoButton << 16 ) | Qt::Key_0);

    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveLeft16.png")), i18n("Move left") + "\tRight", (Qt::NoButton << 16 ) | Qt::Key_Right);
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveRight16.png")), i18n("Move right") + "\tLeft", (Qt::NoButton << 16 ) | Qt::Key_Left);
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveUp16.png")), i18n("Move up") + "\tDown", (Qt::NoButton << 16 ) | Qt::Key_Down);
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveDown16.png")), i18n("Move down") + "\tUp", (Qt::NoButton << 16 ) | Qt::Key_Up);

    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/animate16.png")), i18n("Start/stop animation") + "\tA", (Qt::NoButton << 16 ) | Qt::Key_A);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/background16.png")), i18n("Hide/show background") + "\tB", (Qt::NoButton << 16 ) | Qt::Key_B);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/tickmarks16.png")), i18n("Hide/show tickmarks") + "\tK", (Qt::NoButton << 16 ) | Qt::Key_K);
    menuImage->insertSeparator();
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/flipV16.png")), i18n("Flip vertically") + "\tV", (Qt::NoButton << 16 ) | Qt::Key_V);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/flipH16.png")), i18n("Flip horizontally") + "\tH", (Qt::NoButton << 16 ) | Qt::Key_H);
    menuImage->insertSeparator();
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page116.png")), i18n("First page") + "\tF1", (Qt::NoButton << 16 ) | Qt::Key_F1);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page216.png")), i18n("Previous page") + "\tF2", (Qt::NoButton << 16 ) | Qt::Key_F2);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page316.png")), i18n("Next page") + "\tF3", (Qt::NoButton << 16 ) | Qt::Key_F3);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page416.png")), i18n("Last page") + "\tF4", (Qt::NoButton << 16 ) | Qt::Key_F4);
    menuImage->insertSeparator();
    menuImage->insertItem(i18n("To clipboard") + "\tQ", (Qt::NoButton << 16 ) | Qt::Key_Q);
    menuImage->insertSeparator();
    id_settings = menuImage->insertItem(SQ_IconLoader::instance()->loadIcon("configure", KIcon::Desktop, KIcon::SizeSmall), i18n("Codec Settings") + "\tC", (Qt::NoButton << 16 ) | Qt::Key_C);
    menuImage->setItemEnabled(id_settings, false);

    menuWindow->insertItem(QPixmap(locate("appdata", "images/menu/fullscreen16.png")), i18n("Fullscreen") + "\tF", (Qt::NoButton << 16 ) | Qt::Key_F);
    menuWindow->insertItem(QPixmap(locate("appdata", "images/menu/toolbar16.png")), i18n("Hide/show toolbar") + "\tT", (Qt::NoButton << 16 ) | Qt::Key_T);
    menuWindow->insertItem(QPixmap(locate("appdata", "images/menu/statusbar16.png")), i18n("Hide/show statusbar") + "\tY", (Qt::NoButton << 16 ) | Qt::Key_Y);
    menuWindow->insertSeparator();
    menuWindow->insertItem(i18n("Close") + "\tX", (Qt::NoButton << 16 ) | Qt::Key_X);

    m->insertSeparator();
    pAReset->plug(m);
    pAProperties->plug(m);
    m->insertSeparator();
    pAHelp->plug(m);
    m->insertSeparator();
    pAClose->plug(m);
}

void SQ_GLWidget::slotContextMenuItem(int id)
{
    int key = id & 0xFFFF;
    int state = id >> 16;

    QKeyEvent e(QEvent::KeyPress, key, key, state);

    keyPressEvent(&e);
}

void SQ_GLWidget::updateSlideShowButton(bool toggled)
{
    pAToolSlideShow->setOn(toggled);
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
    for(int i = 0;i < total;i++)
    {
        pt = use_broken ? parts_broken : &parts[i];
        int toxy = pt->tilesx.size() * pt->tilesy.size();

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
void SQ_GLWidget::decodeFailedOn0()
{
    codeK->read_close();
    finfo.image.clear();
    finfo.meta.clear();
    reset_mode = false;
    total = 0;
    lib = 0;
    menuFile->setItemEnabled(id_saveas, false);
    enableSettingsButton(false);
    decoded = false;
    SQ_GLView::window()->resetStatusBar();
}

/*
 *  Are we in fullscreen state ?
 */
bool SQ_GLWidget::fullscreen() const
{
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

void SQ_GLWidget::initBrokenImage()
{
    memoryPart *pt;
    QImage broken = QPixmap(file_broken_xpm).convertToImage().swapRGB();
    broken.setAlphaBuffer(true);

    parts_broken = new Parts;

    // setup parts_broken. It will have only one 64x64 tile
    parts_broken->tilesx.push_back(broken.width());
    parts_broken->tilesy.push_back(broken.height());
    parts_broken->realw = broken.width();
    parts_broken->realh = broken.height();
    parts_broken->w = broken.width();
    parts_broken->h = broken.height();
    parts_broken->makeParts();
    parts_broken->computeCoords();

    pt = new memoryPart(broken.width() * broken.width());
    pt->create();

    memcpy(pt->data(), broken.bits(), broken.numBytes());

    parts_broken->buffer = pt;

    showFrames(0, parts_broken, false);

    image_broken.w = parts_broken->w;
    image_broken.h = parts_broken->h;
    image_broken.bpp = broken.depth();
    image_broken.compression = "-";
    image_broken.colorspace = "RGBA";
    image_broken.hasalpha = false;
}

/*
 *  Force using broken image + update context.
 *  Show appropriate error message in statusbar.
 */
void SQ_GLWidget::useBrokenImage(const int err_index)
{
    // some init
    reset_mode = false;
    use_broken = true;
    decoded = true;
    lib = 0;
    menuImage->setItemEnabled(id_saveas, false);
    enableSettingsButton(false);

    // save "broken" image information in 'finfo'
    finfo.image.push_back(image_broken);

    // reset statusbar widgets
    SQ_GLView::window()->resetStatusBar();

    // show error message instead of file name
    SQ_GLView::window()->sbarWidget("SBFile")->setText(SQ_ErrorString::instance()->string(err_index));

    // update context and show "broken" image
    updateGL();
}

void  SQ_GLWidget::enableSettingsButton(bool enab)
{
    pAToolQuick->setEnabled(enab);
    menuImage->setItemEnabled(id_settings, enab);
}

void  SQ_GLWidget::saveAs()
{
    if(!lib) // nothing to save
        return;

    SQ_FileDialog d(QString::null, this);

    // set filter: writable codecs without *.*
    d.setFilter(SQ_LibraryHandler::instance()->allFiltersFileDialogString(false, false));
    d.setOperationMode(KFileDialog::Saving);
    d.updateCombo(false);

    int result = d.exec();

    if(result == QDialog::Rejected || d.selectedURL().isEmpty())
        return;

    QString path = d.selectedURL().path();

    SQ_LIBRARY *wlib = SQ_LibraryHandler::instance()->libraryByName(d.nameFilter());

    if(!wlib || !wlib->writestatic)
    {
        KMessageBox::error(this, i18n("Sorry, could not perfom write operation\nfor library \"%1\"").arg(d.nameFilter()));
        return;
    }

    RGBA *buf = parts[current].buffer->data();

    fmt_image *im = &finfo.image[current];

    fmt_writeoptions opt;
    opt.interlaced = false;
    opt.alpha = im->hasalpha;
    opt.bitdepth = im->bpp;
    opt.compression_scheme = (wlib->opt.compression_scheme & CompressionNo) ? CompressionNo : CompressionInternal;
    opt.compression_level = wlib->opt.compression_def;

    int err = wlib->codec->write_init(path, *im, opt);

    if(err != SQE_OK)
    {
        KMessageBox::error(this, i18n("Error writing image"));
        return;
    }

    err = wlib->codec->write_next();

    if(err != SQE_OK)
    {
        KMessageBox::error(this, i18n("Error writing image"));
        return;
    }

    wlib->codec->write_next_pass();

    int H = parts[current].h, W = parts[current].realw;
    int Y0 = (im->needflip ? (wlib->opt.needflip ? 0:(-H+1)) : (wlib->opt.needflip ? (-H+1):0));
    int Y = (im->needflip ? (wlib->opt.needflip ? H:1) : (wlib->opt.needflip ? 1:H));
    int f;

//    printf("WRITE %s %d %d\n", path.ascii(), Y0, Y);

    for(int j = Y0;j < Y;j++)
    {
        f = (j < 0) ? -j : j;

        err = wlib->codec->write_scanline(buf + W*f);

        if(err != SQE_OK)
        {
            wlib->codec->write_close();
            KMessageBox::error(this, i18n("Error writing image"));
            return;
        }
    }

    wlib->codec->write_close();
}

void SQ_GLWidget::slotSetZoomPercents(int perc)
{
    if(use_broken || finfo.image.empty()) return;

    internalZoom((GLfloat)perc / 100);
}

void SQ_GLWidget::toClipboard()
{
    if(!decoded || use_broken)
        return;

    QImage im((uchar *)parts[current].buffer->data(), parts[current].realw, parts[current].realh, 32, 0, 0, QImage::LittleEndian);

    // image doesn't have extra regions
    if(parts[current].realw == parts[current].w && parts[current].realh == parts[current].h)
        QApplication::clipboard()->setImage(im, QClipboard::Clipboard);
    else
        QApplication::clipboard()->setImage(im.copy(0, 0, parts[current].w, parts[current].h), QClipboard::Clipboard);
}

#include "sq_glwidget.moc"
