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
#include <qeventloop.h>
#include <qstringlist.h>
#include <qdragobject.h>
#include <qcursor.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <qsignalmapper.h>
#include <qlayout.h>
#include <qpopupmenu.h>

#include <kaction.h>
#include <kcursor.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kapplication.h>
#include <klocale.h>
#include <kstringhandler.h>
#include <kimageeffect.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kdebug.h>

#include <math.h>
#include <stdlib.h>

#undef SQ_SECTION

#ifndef SQ_SMALL // light version doesn't require all this stuff...

#define SQ_SECTION "GL view"

#include "ksquirrel.h"
#include "sq_externaltool.h"
#include "sq_widgetstack.h"
#include "sq_fileiconview.h"
#include "sq_quickbrowser.h"
#include "sq_diroperator.h"

#else

#define SQ_SECTION "ksquirrel-small"

#endif

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

#include <fileio.h>
#include <fmt_codec_base.h>
#include <error.h>

#include "file_broken.xpm"

/* ***************************************************************************************** */

SQ_GLWidget * SQ_GLWidget::m_instance = NULL;

static const int timer_delay =    20;
static const int timer_delay_file = 10;
static const int len =    5;

static const float SQ_WINDOW_BACKGROUND_POS = -1000.0f;
static const float SQ_IMAGE_CHECKER_POS =     -999.0f;
static const float SQ_FIRST_FRAME_POS =       -998.0f;
static const float SQ_MARKS_POS =             -997.0f;
static const float SQ_FIRST_TILE_LAYER =      -995.0f;

/* ***************************************************************************************** */

Parts::Parts() : tilesx(0), tilesy(0), tiles(0), tileSize(0), w(0), h(0), realw(0),
                    realh(0), m_parts(0)
{}

Part::Part() : x1(0), y1(0), x2(0), y2(0), tex(0), list(0)
{}

/* ***************************************************************************************** */

memoryPart::memoryPart(const int sz) : m_size(sz), m_data(NULL)
{}

memoryPart::~memoryPart()
{
    del();
}

void memoryPart::create()
{
    m_data = new unsigned char [m_size * m_size * sizeof(RGBA)];
}

/* ***************************************************************************************** */

SQ_GLWidget::SQ_GLWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
    kdDebug() << "+SQ_GLWidget" << endl;

    // init all variables...
    m_instance = this;
    ac = new KActionCollection(NULL, this, "GLWidget actionCollection");
    isflippedV = isflippedH = changed = blocked = blocked_force = decoded = reset_mode = false;
    movetype = -1;
    memset(matrix, 0, sizeof(matrix));
    matrix[10] = matrix[5] = matrix[0] = 1.0f;
    curangle = 0.0f;
    next = NULL;
    changed2 = true;
    current = 0;
    total = 0;
    old_id = -1;
    zoomFactor = 1.0f;
    menu = new QPopupMenu(this);

    SQ_Config::instance()->setGroup(SQ_SECTION);
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
    QPixmap pusual = QPixmap(locate("appdata", "cursors/usual.png"));
    cusual = (!pusual.isNull()) ? QCursor(pusual) : KCursor::arrowCursor();

    QPixmap pdrag = QPixmap(locate("appdata", "cursors/drag.png"));
    cdrag = (!pdrag.isNull()) ? QCursor(pdrag) : cusual;

    cZoomIn = KCursor::crossCursor();

    setCursor(cusual);

    setFocusPolicy(QWidget::WheelFocus);
    setAcceptDrops(true);

    // popup menu with image pages
    images = new KPopupMenu;
    images->setCheckable(true);

#ifndef SQ_SMALL

    // create Quick Browser
    createQuickBrowser();

#endif

    // create actions
    createActions();

    // create toolbars
    createToolbar();

    // create tickmarks
    createMarks();

    KCursor::setAutoHideCursor(this, true);
    KCursor::setHideCursorDelay(2500);

#ifndef SQ_SMALL

    timer_prev = new QTimer(this);
    timer_next = new QTimer(this);

    Q_CHECK_PTR(timer_prev);
    Q_CHECK_PTR(timer_next);

    connect(timer_prev, SIGNAL(timeout()), SQ_WidgetStack::instance(), SLOT(emitPreviousSelected()));
    connect(timer_next, SIGNAL(timeout()), SQ_WidgetStack::instance(), SLOT(emitNextSelected()));

#endif

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

    clearParts(parts_broken);

    delete parts_broken;

    removeCurrentParts();

    delete zoom;
    delete images;
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

#ifndef SQ_SMALL
    pAClose = new KAction(i18n("Close"), locate("appdata", "images/menu/close16.png"), 0, KSquirrel::app(), SLOT(slotCloseGLWidget()), ac, "SQ GL close");
#else
    pAClose = new KAction(i18n("Close"), locate("appdata", "images/menu/close16.png"), 0, SQ_GLView::window(), SLOT(close()), ac, "SQ GL close");
#endif

    pAProperties = new KAction(i18n("Properties"), locate("appdata", "images/menu/prop16.png"), 0, this, SLOT(slotProperties()), ac, "SQ GL Prop");
    pAFull = new KToggleAction(QString::null, 0, 0, 0, ac, "SQ GL Full");

#ifndef SQ_SMALL
    pAQuick = new KToggleAction(QString::null, 0, 0, 0, ac, "quickbrowser");
#endif

    pAHideToolbars = new KToggleAction(QString::null, QPixmap(), 0, 0, 0, ac, "toggle toolbar");
    pAStatus = new KToggleAction(QString::null, QPixmap(), 0, 0, 0, ac, "toggle status");

    pAZoomW->setExclusiveGroup("squirrel_zoom_actions");
    pAZoomH->setExclusiveGroup("squirrel_zoom_actions");
    pAZoomWH->setExclusiveGroup("squirrel_zoom_actions");
    pAZoom100->setExclusiveGroup("squirrel_zoom_actions");
    pAZoomLast->setExclusiveGroup("squirrel_zoom_actions");

    connect(pAIfLess, SIGNAL(toggled(bool)), this, SLOT(slotZoomIfLess()));

#ifndef SQ_SMALL
    connect(pAFull, SIGNAL(toggled(bool)), KSquirrel::app(), SLOT(slotFullScreen(bool)));
    connect(pAQuick, SIGNAL(toggled(bool)), this, SLOT(slotShowQuick(bool)));
#else
    connect(pAFull, SIGNAL(toggled(bool)), SQ_GLView::window(), SLOT(slotFullScreen(bool)));
#endif

    connect(pAHideToolbars, SIGNAL(toggled(bool)), this, SLOT(slotHideToolbars(bool)));
    connect(pAStatus, SIGNAL(toggled(bool)), this, SLOT(slotToggleStatus(bool)));

    SQ_Config::instance()->setGroup(SQ_SECTION);

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

    switch(zoom_type)
    {
        case 0: pAZoomW->setChecked(true); break;
        case 1: pAZoomH->setChecked(true); break;
        case 3: pAZoom100->setChecked(true); break;
        case 4: pAZoomLast->setChecked(true); break;

        // "case 2" too
        default: pAZoomWH->setChecked(true);
    }

//
// We will create QToolButtons and put them in toolbar.
// Of course, we can just KAction::plug(), BUT plugged KActions
// will produce buttons, which cann't be clicked twise! I think
// plugged KActions will treat our attempt as double-click, not two single-clicks.
// On the other hand, we can click QToolButton as frequently as we want.
//
// Plugged KActions also don't know about autorepeat :(
//

#ifndef SQ_SMALL

    // Ugly hack: I don't see the reason why image flickers in
    // fullscreen state with hidden toolbar. The only way to prevent it
    // is to create very small child widget (1x1).
    hack = new QWidget(this);
    hack->setPaletteBackgroundColor(QColor(128,128,128));
    hack->setGeometry(0,0,1,1);
    hack->show();

    // create toolbuttons and connect them to appropriate slots
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_first.png")), i18n("Go to first file"), this, SLOT(slotFirst()), toolbar);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_prev.png")), i18n("Previous file"), this, SLOT(slotPrev()), toolbar);
    pATool->setAutoRepeat(true);
    pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_next.png")), i18n("Next file"), this, SLOT(slotNext()), toolbar);
    pATool->setAutoRepeat(true);
    new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_last.png")), i18n("Go to last file"), this, SLOT(slotLast()), toolbar);

#endif

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

#ifndef SQ_SMALL

    pAToolQuick = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/quick.png")), i18n("\"Quick Browser\""), pAQuick, SLOT(activate()), toolbar);
    pAToolQuick->setToggleButton(true);

#endif

    pAToolImages = new SQ_ToolButtonPopup(QPixmap(locate("appdata", "images/actions/images.png")), i18n("Select image"), toolbar);
    pAToolImages->setPopup(images);

#ifndef SQ_SMALL

    KAction *n = KSquirrel::app()->actionCollection()->action("SQ Slideshow");
    pAToolSlideShow = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/slideshow.png")), n->text(), n, SLOT(activate()), toolbar);
    pAToolSlideShow->setToggleButton(true);

#endif

    pAToolClose = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/close.png")), i18n("Close"), pAClose, SLOT(activate()), toolbar);

    anim = new SQ_ToolButtonPage(this, SLOT(slotToolButtonPageClicked()), toolbar);
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
    static bool rrr = false;
    bindMarks(changed2, rrr);
    rrr = true;

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
        SQ_Config::instance()->setGroup(SQ_SECTION);

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
        for(z = 0;z < pt->tilesy;z++)
            if(glIsList(pt->m_parts[z * pt->tilesx].list))
                glCallList(pt->m_parts[z * pt->tilesx].list);

        // draw tickmarks ("broken" image won't have tickmarks)
        if(!use_broken && SQ_Config::instance()->readBoolEntry("marks", true) && marks)
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
    coordChanged();

    if(!use_broken && total > 1)
        frameChanged();
}

/*
 *  Change statusbar info according with
 *  current matrix (it shows current coordinates).
 */
void SQ_GLWidget::coordChanged()
{
    QString str;

    // construct string
    str = QString::fromLatin1("%1,%2")
    .arg(MATRIX_X, 0, 'f', 0)
    .arg(MATRIX_Y, 0, 'f', 0);

    // update QLabel
    SQ_GLView::window()->sbarWidget("SBGLCoord")->setText(str);
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

#ifndef SQ_SMALL

        SQ_Config::instance()->setGroup("GL view");

        // load next file in current directory
        if(SQ_Config::instance()->readNumEntry("scroll", 0))
            slotNext();

        else

#endif

        // zoom image otherwise
        slotZoomPlus();
    }
    else if(e->delta() > 0 && e->state() == Qt::NoButton)
    {

#ifndef SQ_SMALL

        SQ_Config::instance()->setGroup("GL view");

        if(SQ_Config::instance()->readNumEntry("scroll", 0))
            slotPrev();
        else

#endif

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
    // left button, update cursor
    if(e->button() == Qt::LeftButton && e->state() == Qt::NoButton)
    {
        setCursor(cdrag);

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
        setCursor(cZoomIn);

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
        setCursor(cusual);
    // left button + SHIFT - zoom to selected rectangle (if needed)
    else if(movetype == 2)
    {
        setCursor(cusual);

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

#ifndef SQ_SMALL

        case SQ_KEYSTATES(Qt::Key_Pause, Qt::NoButton):       KSquirrel::app()->pauseSlideShow();    break;
        case SQ_KEYSTATES(Qt::Key_PageDown, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_PageDown, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_Space, Qt::NoButton):       slotNext();               break;
        case SQ_KEYSTATES(Qt::Key_PageUp, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_PageUp, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_BackSpace, Qt::NoButton):   slotPrev();               break;

#endif

        case SQ_KEYSTATES(Qt::Key_X, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_Escape, Qt::NoButton):
        case SQ_KEYSTATES(Qt::Key_Return, Qt::NoButton):     // Thanks JaguarWan for suggestion.
        case SQ_KEYSTATES(Qt::Key_Enter, Qt::Keypad):        pAClose->activate();         break;
        case SQ_KEYSTATES(Qt::Key_P, Qt::NoButton):          pAProperties->activate();    break;

#ifndef SQ_SMALL

        case SQ_KEYSTATES(Qt::Key_Home, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_Home, Qt::NoButton):       slotFirst();                 break;
        case SQ_KEYSTATES(Qt::Key_End, Qt::Keypad):
        case SQ_KEYSTATES(Qt::Key_End, Qt::NoButton):        slotLast();                  break;
        case SQ_KEYSTATES(Qt::Key_Q, Qt::NoButton):          toggleQuickBrowser();        break;

#endif

        case SQ_KEYSTATES(Qt::Key_F, Qt::NoButton):          toggleFullScreen();         break;
        case SQ_KEYSTATES(Qt::Key_T, Qt::NoButton):          pAHideToolbars->activate(); break;
        case SQ_KEYSTATES(Qt::Key_Z, Qt::NoButton):          slotZoomMenu();             break;
        case SQ_KEYSTATES(Qt::Key_S, Qt::NoButton):          pAStatus->activate();       break;
        case SQ_KEYSTATES(Qt::Key_A, Qt::NoButton):          slotToggleAnimate();        break;
        case SQ_KEYSTATES(Qt::Key_I, Qt::NoButton):          slotShowImages();           break;
        case SQ_KEYSTATES(Qt::Key_F1, Qt::NoButton):         jumpToImage(false);         break;
        case SQ_KEYSTATES(Qt::Key_F2, Qt::NoButton):         prevImage();                break;
        case SQ_KEYSTATES(Qt::Key_F3, Qt::NoButton):         nextImage();                break;
        case SQ_KEYSTATES(Qt::Key_F4, Qt::NoButton):         jumpToImage(true);          break;
        case SQ_KEYSTATES(Qt::Key_Slash, Qt::NoButton):      slotShowHelp();             break;
        case SQ_KEYSTATES(Qt::Key_B, Qt::NoButton):          toggleDrawingBackground();  break;
        case SQ_KEYSTATES(Qt::Key_K, Qt::NoButton):          toogleTickmarks();          break;

#ifndef SQ_SMALL

        case SQ_KEYSTATES(Qt::Key_E, Qt::NoButton):          showExternalTools();        break;

#endif

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
#ifndef QT_NO_STL
            meta.append(QPair<QString,QString>((*it).group, (*it).data));
#else
            meta.append(QPair<QString,QString>((*it).group.c_str(), (*it).data.c_str()));
#endif
        }
    }

    // save current image parameters and some additioanl information
    // in list
    list    << quickImageInfo
            << QString::fromLatin1("%1x%2").arg(finfo.image[current].w).arg(finfo.image[current].h)
            << QString::fromLatin1("%1").arg(finfo.image[current].bpp)
#ifndef QT_NO_STL
            << finfo.image[current].colorspace
            << finfo.image[current].compression
#else
            << finfo.image[current].colorspace.c_str()
            << finfo.image[current].compression.c_str()
#endif
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

int SQ_GLWidget::findCloserTiles(int w, int h, int &tile1, int &tile2)
{
    int i = 0;
    int max = QMAX(w, h);
    float fw = (float)w;
    float fh = (float)h;
    bool tile_factor = false;
    int tileSize = 64;

    if(w && h)
        if(fw/fh > 2.0f || fh/fw > 2.0f)
            tile_factor = true;

    static int S[] = { 32, 64, 128, 256 };

    if(max >= 256)
        tileSize = 256;
    else
        for(int q = 0;q < 4;q++)
            if(max <= S[q])
            {
                tileSize = S[q];
                break;
            }

    if(tileSize != 32 && tile_factor)
        tileSize /= 2;

    tile1 = 0;

    do
    {
        i += tileSize;
        tile1++;
    }while(w >  i);

    tile2 = 0;
    i = 0;

    do
    {
        i += tileSize;
        tile2++;
    }while(h >  i);

    return tileSize;
}

#ifndef SQ_SMALL

void SQ_GLWidget::createQuickBrowser()
{
    v = new SQ_QuickBrowser(this);
}

/*
 * Toggle quickbrowser visible.
 */
void SQ_GLWidget::toggleQuickBrowser()
{
    bool q = pAQuick->isChecked();
    q = !q;

    pAQuick->setChecked(q);
    pAToolQuick->setOn(q);

    slotShowQuick(q);
}

#endif

void SQ_GLWidget::removeCurrentParts()
{
    // if use_broken, 'parts' has no members
    if(decoded && !use_broken)
    {
        parts[current].removeParts();

        // remove previous parts
        for(std::vector<Parts>::iterator it = parts.begin();it != parts.end();++it)
        {
            // delete textures and memory buffers
            clearParts(&(*it));

            (*it).m32.clear();

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
#ifdef SQ_SMALL
        KMessageBox::error(this, i18n("Library for %1 format not found").arg(fm.extension(false)));
#else
        KMessageBox::error(KSquirrel::app(), i18n("Library for %1 format not found").arg(fm.extension(false)));
#endif
        reset_mode = false;

        return false;
    }

    // determine codec
    codeK = lib->codec;

    // clear any previous info
    finfo.image.clear();
    finfo.meta.clear();

    removeCurrentParts();

#ifndef QT_NO_STL
    // start decoding!
    i = codeK->fmt_read_init(File);
#else
    i = codeK->fmt_read_init(File.ascii());
#endif

    // oops, error...
    if(i != SQE_OK)
    {
        codeK->fmt_read_close();

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
    int z;
    const int a = p->tilesx * i, b = p->tilesx * (i+1);
    int filter = linear ? GL_LINEAR : GL_NEAREST;

    // for safety...
    makeCurrent();

    glEnable(GL_TEXTURE_2D);

    for(z = a;z < b;z++)
    {
        glBindTexture(GL_TEXTURE_2D, p->m_parts[z].tex);

//    ??
//    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ARB, SQ_DIV, SQ_DIV, 0, GL_RGBA, GL_UNSIGNED_BYTE, mem_parts[z].part);

        // setup texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p->tileSize, p->tileSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, ((p->m32)[z])->data());
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
void SQ_GLWidget::setupBits(Parts *p, RGBA *b, int y)
{
    int index = y * p->tilesx;
    int f = p->tileSize * sizeof(RGBA);
    unsigned char *vv = 0;

    for(int x = 0;x < p->tilesx;x++)
    {
        vv = ((p->m32)[index])->data();

        for(int j = 0,k = 0;j < p->tileSize;j++,k++)
            memcpy(vv + k*f, b + x*p->tileSize + k*p->realw, f);

        index++;
    }
}

// Delete textures and display lists.
void Parts::removeParts()
{
    if(!m_parts.empty())
    {
        for(int z = 0;z < tiles;z++)
            glDeleteTextures(1, &m_parts[z].tex);

        glDeleteLists(m_parts[0].list, tilesy);

        m_parts.clear();
    }
}

// Create parts: generate textures and display lists.
bool Parts::makeParts()
{
    int z;

    GLuint base = glGenLists(tilesy);

    if(!base)
        return false;

    Part pt;

    for(z = 0;z < tiles;z++)
    {
        glGenTextures(1, &pt.tex);
        m_parts.push_back(pt);
    }

    // calculate display list's id
    for(z = 0;z < tilesy;z++)
        m_parts[z * tilesx].list = base + z;

    return true;
}

// Calculate coordinates for textures
void Parts::computeCoords()
{
    Part *p;
    int index = 0;
    float X, Y;

    Y = (float)h / 2.0;

    for(int y = 0;y < tilesy;y++)
    {
        X = -(float)w / 2.0;

        for(int x = 0;x < tilesx;x++)
        {
            p = &m_parts[index];

            p->x1 = X;
            p->y1 = Y;
            p->x2 = X + tileSize;
            p->y2 = Y - tileSize;

            p->tx1 = 0.0;
            p->tx2 = 1.0;
            p->ty1 = 0.0;
            p->ty2 = 1.0;

            index++;
            X += tileSize;
        }

        Y -= tileSize;
    }
}

#ifndef SQ_SMALL

// Show/hide Quick Browser.
void SQ_GLWidget::slotShowQuick(bool b)
{
    v->setShown(b);

    if(b)
        SQ_QuickBrowser::window()->updateCurrentFileItem();
}

#else

void SQ_GLWidget::slotShowQuick(bool)
{}

#endif

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

#ifndef SQ_SMALL

    bool slideShowRunning = KSquirrel::app()->slideShowRunning();

    SQ_Config::instance()->setGroup("Slideshow");

    // goto fullscreen, if needed
    if(slideShowRunning && !fullscreen() && SQ_Config::instance()->readBoolEntry("fullscreen", true))
        toggleFullScreen();

    // show window with image
    if(!slideShowRunning || (slideShowRunning && SQ_Config::instance()->readBoolEntry("force", true)))
        KSquirrel::app()->raiseGLWidget();

    KSquirrel::app()->setCaption(file);

#else

    SQ_GLView::window()->setCaption(file);

#endif

    if(!pAHideToolbars->isChecked())
        SQ_GLView::window()->toolbar()->show();

    if(!decoded || isMouseEvent)
        qApp->processEvents();

    zoomFactor = get_zoom();
    matrix_pure_reset();
    matrixChanged();

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
    bool progr = SQ_Config::instance()->readBoolEntry("progressiv", true);

#ifdef SQ_SMALL

    int allpages = SQ_Config::instance()->readNumEntry("load_pages", 0);
    int pages_num = SQ_Config::instance()->readNumEntry("load_pages_number", 1);

#else

    int allpages = KSquirrel::app()->slideShowRunning() ? 1 : SQ_Config::instance()->readNumEntry("load_pages", 0);
    int pages_num = KSquirrel::app()->slideShowRunning() ? 1 : SQ_Config::instance()->readNumEntry("load_pages_number", 1);

#endif

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

        i = codeK->fmt_read_next();

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

        anim->advanceValue();
        qApp->processEvents();

        Parts pp;

        // find tile size
        int tileSize = findCloserTiles(im->w, im->h, pp.tilesx, pp.tilesy);

        // setup current Part
        pp.tiles = pp.tilesx * pp.tilesy;
        pp.realw = pp.tilesx * tileSize;
        pp.realh = pp.tilesy * tileSize;
        pp.w = im->w;
        pp.h = im->h;
        pp.tileSize = tileSize;

        // call makeParts() only for the first image page
        // and check result
        if(!current && !pp.makeParts())
        {
            QString status = i18n("Memory realloc failed.\n\nI tried to realloc memory for image:\n\nwidth: %1\nheight: %2\nbpp: %3\nTotal needed: %4 of memory.")
                    .arg(im->w)
                    .arg(im->h)
                    .arg(im->bpp)
                    .arg(KIO::convertSize(pp.realw * pp.realh * sizeof(RGBA)));

#ifdef SQ_SMALL
            KMessageBox::error(this, status);
#else
            KMessageBox::error(KSquirrel::app(), status);
#endif

            decodeFailedOn0();
            return;
        }

#define SQ_FAIL_RET \
                if(current)    \
                    break;     \
                else            \
                {                 \
                    decodeFailedOn0(); \
                    return;                    \
                }

        for(int s = 0;s < pp.tiles;s++)
        {
            pt = new memoryPart(tileSize);

            pt->create();

            // memory allocation failed! We needn't previously malloc'ed
            // data, so let's delete it and run away from here as fast as we can :-)
            if(!pt->valid())
            {
                // delete memory buffers [0...s]
                clearParts(&pp, s);

                SQ_FAIL_RET
            }

            pp.m32.push_back(pt);
        }

        next = (RGBA *)realloc(next, pp.realw * tileSize * sizeof(RGBA));

        if(!next)
        {

#ifdef SQ_SMALL
            KMessageBox::error(this, i18n("Memory allocation failed"));
#else
            KMessageBox::error(KSquirrel::app(), i18n("Memory allocation failed"));
#endif

            // delete memory buffers
            for(int k = 0;k < pp.tiles;k++)
                pp.m32[k]->del();

            pp.m32.clear();

            SQ_FAIL_RET
        }

#undef SQ_FAIL_RET

        line = 0;

        if(!current)
        {
            pp.computeCoords();
            finfo = codeK->information();
            slotZoomIfLess();
            curangle = 0.0f;
            isflippedH = isflippedV = false;
            matrixChanged();
        }

        matrix_move_z(SQ_FIRST_TILE_LAYER+current);

        for(int pass = 0;pass < im->passes;pass++)
        {
            if(codeK->fmt_read_next_pass() != SQE_OK)
                break;

            line = 0;

            for(i = 0;i < pp.tilesy;i++)
            {
                memset(next, 0, pp.realw * tileSize * sizeof(RGBA));

                for(j = 0;j < tileSize;j++)
                {
                    res = codeK->fmt_read_scanline(next + pp.realw*j);
                    errors += (int)(res != SQE_OK);

                    if(++line == im->h)
                        break;
                }

                if(pass == im->passes-1)
                {
                    setupBits(&pp, next, i);

                    if(!current)
                    {
                        bool b = showFrames(i, &pp, progr);

                        if(!b)
                            printf("Showframes failed for image %d, tiley %d\n", current, i);
                    }
                }
            }
        }

        id = images->insertItem(QString::fromLatin1("#%1 [%2x%3@%4]").arg(current+1).arg(im->w).arg(im->h).arg(im->bpp));

        images->setItemParameter(id, current);

        if(!current)
        {
            SQ_GLView::window()->sbarWidget("SBDecodedI")->clear();
            SQ_GLView::window()->sbarWidget("SBDecodedI")->setPixmap(QPixmap(lib->mime));

            old_id = first_id = id;
        }

        parts.push_back(pp);

        current++;
    }

    anim->flush();

    if(next)
    {
        free(next);
        next = NULL;
    }

    finfo = codeK->information();

    codeK->fmt_read_close();
    total = finfo.image.size();

    current = 0;
    updateCurrentFileInfo();

    decoded = true;
    reset_mode = false;
    frameChanged();
    updateGL();

    quickImageInfo = lib->quickinfo;

    int secs;
    int msecs = started.elapsed();
    adjustTimeFromMsecs(secs, msecs);
    QString r = (secs) ? i18n("Loaded in %1'%2 ms.").arg(secs).arg(msecs) : i18n("Loaded in %1 ms.").arg(msecs);
    SQ_GLView::window()->sbarWidget("SBLoaded")->setText(r);

    images->setItemChecked(first_id, true);

    if(finfo.animated)
        QTimer::singleShot(finfo.image[current].delay, this, SLOT(slotAnimateNext()));
}

void SQ_GLWidget::adjustTimeFromMsecs(int &secs, int &msecs)
{
    secs = 0;

    if(msecs >= 1000)
    {
        secs = msecs / 1000;
        msecs -= secs*1000;
    }
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

#ifndef SQ_SMALL

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

#else

void SQ_GLWidget::slotFirst(){}
void SQ_GLWidget::slotLast(){}
void SQ_GLWidget::slotNext(){}
void SQ_GLWidget::slotPrev(){}

#endif

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

    parts[current].removeParts();

    current++;

    if(current >= (int)finfo.image.size())
        current = 0;

    reassignParts();
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

    parts[current].removeParts();

    images->setItemChecked(old_id, false);
    current = images->itemParameter(id);

    reassignParts();

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

    parts[current].removeParts();

    current++;

    if(current >= total)
        current = 0;

    reassignParts();
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

    parts[current].removeParts();

    current--;

    if(current < 0)
        current = total - 1;

    reassignParts();
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

    parts[current].removeParts();

    current = (last) ? finfo.image.size() - 1 : 0;

    reassignParts();
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
    SQ_Config::instance()->setGroup(SQ_SECTION);

    bool b = SQ_Config::instance()->readBoolEntry("alpha_bkgr", true);

    b = !b;

    SQ_Config::instance()->writeEntry("alpha_bkgr", b);

    updateGL();
}

#ifndef SQ_SMALL

void SQ_GLWidget::showExternalTools()
{
    SQ_ExternalTool::instance()->constPopupMenu()->exec(QCursor::pos());
}

#endif

void SQ_GLWidget::bindMarks(bool &first, bool deleteOld)
{
    if(!marks || !first)
        return;

    first = false;

    int param[4];
    unsigned char p[3];
    glGetIntegerv(GL_COLOR_CLEAR_VALUE, param);

    p[0] = param[0] >> 23;
    p[1] = param[1] >> 23;
    p[2] = param[2] >> 23;

    SQ_Config::instance()->setGroup("GL view");

    QColor adj = calculateAdjustedColor(BGpixmap, QColor(p[0],p[1],p[2]),
                    (SQ_Config::instance()->readNumEntry("GL view background type", 1) != 2));

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

        mm[i] = KImageEffect::fade(mm[i], 0.9, adj);

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

QColor SQ_GLWidget::calculateAdjustedColor(const QImage &im, const QColor &rgb, bool color)
{
    int r, g, b;

    if(color)
    {
        r = (rgb.red() < 128)   ? 255 : 0;
        g = (rgb.green() < 128) ? 255 : 0;
        b = (rgb.blue() < 128)  ? 255 : 0;
    }
    else
    {
        int kR = 0, kG = 0, kB = 0;
        int F = im.height() * im.width();
        int Z = F * 4;

        unsigned char *p = im.bits();

        for(int i = 0;i < Z;i += 4)
        {
            kR += *(p+i);
            kG += *(p+i+1);
            kB += *(p+i+2);
        }

        kR /= F;
        kG /= F;
        kB /= F;

//    printf("Med. color: %d,%d,%d\n", R, G, B);

        r = (kR < 128) ? 255 : 0;
        g = (kG < 128) ? 255 : 0;
        b = (kB < 128) ? 255 : 0;
    }

    return QColor(b, g, r);
}

// Delete current image (user pressed 'Delete' key).
void SQ_GLWidget::deleteWrapper()
{

#ifndef SQ_SMALL

    if(m_File.isEmpty())
        return;

    KURL url = m_File;
    KFileItemList list;
    list.append(new KFileItem(url, QString::null, KFileItem::Unknown));

    SQ_WidgetStack::instance()->diroperator()->del(list, this);

#endif

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
    SQ_Config::instance()->setGroup(SQ_SECTION);

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

void SQ_GLWidget::internalZoom(const GLfloat &zF)
{
    matrix_pure_reset();

    if(finfo.image[current].needflip)
        flip(4);

    matrix_zoom(zF);
}

// Create context menu with actions.
void SQ_GLWidget::createContextMenu(QPopupMenu *m)
{
    QPopupMenu *menuRotate = new QPopupMenu(m);
    QPopupMenu *menuZoom = new QPopupMenu(m);
    QPopupMenu *menuMove = new QPopupMenu(m);
    QPopupMenu *menuWindow = new QPopupMenu(m);
    QPopupMenu *menuImage = new QPopupMenu(m);

    connect(m, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));

#ifndef SQ_SMALL
    QPopupMenu *menuFile = new QPopupMenu(m);
    m->insertItem(SQ_IconLoader::instance()->loadIcon("fileopen", KIcon::Desktop, KIcon::SizeSmall), i18n("File"), menuFile);
    connect(menuFile, SIGNAL(activated(int)), this, SLOT(slotContextMenuItem(int)));
#endif

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

#ifndef SQ_SMALL

    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/next16.png")), i18n("Next"), (Qt::NoButton << 16 ) | Qt::Key_PageDown);
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/prev16.png")), i18n("Previous"), (Qt::NoButton << 16 ) | Qt::Key_PageUp);
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/first16.png")), i18n("First"), (Qt::NoButton << 16 ) | Qt::Key_Home);
    menuFile->insertItem(QPixmap(locate("appdata", "images/menu/last16.png")), i18n("Last"), (Qt::NoButton << 16 ) | Qt::Key_End);
    menuFile->insertSeparator();
    menuFile->insertItem(SQ_IconLoader::instance()->loadIcon("edittrash", KIcon::Desktop, KIcon::SizeSmall), i18n("Delete"), (Qt::NoButton << 16 ) | Qt::Key_Delete);

#endif

    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/rotateLeft16.png")), i18n("Rotate left"), (Qt::ControlButton << 16 ) | Qt::Key_Left);
    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/rotateRight16.png")), i18n("Rotate right"), (Qt::ControlButton << 16 ) | Qt::Key_Right);
    menuRotate->insertSeparator();
    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/18016.png")), i18n("Rotate 180'"), (Qt::ControlButton << 16 ) | Qt::Key_Up);
    menuRotate->insertSeparator();
    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/116.png")), i18n("Rotate 1' left"), (Qt::ShiftButton << 16 ) | Qt::Key_Left);
    menuRotate->insertItem(QPixmap(locate("appdata", "images/menu/116.png")), i18n("Rotate 1' right"), (Qt::ShiftButton << 16 ) | Qt::Key_Right);

    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom+16.png")), i18n("Zoom +"), (Qt::NoButton << 16 ) | Qt::Key_Plus);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom-16.png")), i18n("Zoom -"), (Qt::NoButton << 16 ) | Qt::Key_Minus);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom216.png")), i18n("Zoom 2x"), (Qt::ControlButton << 16 ) | Qt::Key_Plus);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom0.516.png")), i18n("Zoom 1/2x"), (Qt::ControlButton << 16 ) | Qt::Key_Minus);
    menuZoom->insertSeparator();
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom10016.png")), QString::fromLatin1("100%"), (Qt::NoButton << 16 ) | Qt::Key_1);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom20016.png")), QString::fromLatin1("200%"), (Qt::NoButton << 16 ) | Qt::Key_2);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom30016.png")), QString::fromLatin1("300%"), (Qt::NoButton << 16 ) | Qt::Key_3);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom50016.png")), QString::fromLatin1("500%"), (Qt::NoButton << 16 ) | Qt::Key_5);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom70016.png")), QString::fromLatin1("700%"), (Qt::NoButton << 16 ) | Qt::Key_7);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom90016.png")), QString::fromLatin1("900%"), (Qt::NoButton << 16 ) | Qt::Key_9);
    menuZoom->insertItem(QPixmap(locate("appdata", "images/menu/zoom100016.png")), QString::fromLatin1("1000%"), (Qt::NoButton << 16 ) | Qt::Key_0);

    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveLeft16.png")), i18n("Move left"), (Qt::NoButton << 16 ) | Qt::Key_Right);
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveRight16.png")), i18n("Move right"), (Qt::NoButton << 16 ) | Qt::Key_Left);
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveUp16.png")), i18n("Move up"), (Qt::NoButton << 16 ) | Qt::Key_Down);
    menuMove->insertItem(QPixmap(locate("appdata", "images/menu/moveDown16.png")), i18n("Move down"), (Qt::NoButton << 16 ) | Qt::Key_Up);

    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/animate16.png")), i18n("Start/stop animation"), (Qt::NoButton << 16 ) | Qt::Key_A);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/background16.png")), i18n("Hide/show background"), (Qt::NoButton << 16 ) | Qt::Key_B);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/tickmarks16.png")), i18n("Hide/show tickmarks"), (Qt::NoButton << 16 ) | Qt::Key_K);
    menuImage->insertSeparator();
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/flipV16.png")), i18n("Flip vertically"), (Qt::NoButton << 16 ) | Qt::Key_V);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/flipH16.png")), i18n("Flip horizontally"), (Qt::NoButton << 16 ) | Qt::Key_H);
    menuImage->insertSeparator();
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page116.png")), i18n("First page"), (Qt::NoButton << 16 ) | Qt::Key_F1);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page216.png")), i18n("Previous page"), (Qt::NoButton << 16 ) | Qt::Key_F2);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page316.png")), i18n("Next page"), (Qt::NoButton << 16 ) | Qt::Key_F3);
    menuImage->insertItem(QPixmap(locate("appdata", "images/menu/page416.png")), i18n("Last page"), (Qt::NoButton << 16 ) | Qt::Key_F4);

    menuWindow->insertItem(QPixmap(locate("appdata", "images/menu/fullscreen16.png")), i18n("Fullscreen"), (Qt::NoButton << 16 ) | Qt::Key_F);
#ifndef SQ_SMALL
    menuWindow->insertItem(QPixmap(locate("appdata", "images/menu/quick16.png")), i18n("Quick Browser"), (Qt::NoButton << 16 ) | Qt::Key_Q);
#endif
    menuWindow->insertItem(QPixmap(locate("appdata", "images/menu/toolbar16.png")), i18n("Hide/show toolbar"), (Qt::NoButton << 16 ) | Qt::Key_T);
    menuWindow->insertItem(QPixmap(locate("appdata", "images/menu/statusbar16.png")), i18n("Hide/show statusbar"), (Qt::NoButton << 16 ) | Qt::Key_S);

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

#ifndef SQ_SMALL

void SQ_GLWidget::updateSlideShowButton(bool toggled)
{
    pAToolSlideShow->setOn(toggled);
}

#endif

void SQ_GLWidget::reassignParts()
{
    Parts *p = &parts[current];

    // generate textures and display list's id
    p->makeParts();
    p->computeCoords();

    for(int j = 0;j < p->tilesy;j++)
        showFrames(j, p, false);
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

        for(int j = 0;j < pt->tiles;j++)
        {
            glBindTexture(GL_TEXTURE_2D, pt->m_parts[j].tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        }
    }

    updateGL();
}

void SQ_GLWidget::slotToolButtonPageClicked()
{
}

/*
 *  Cleanup method.
 */
void SQ_GLWidget::decodeFailedOn0()
{
    codeK->fmt_read_close();
    finfo.image.clear();
    finfo.meta.clear();
    reset_mode = false;
    total = 0;
    decoded = false;
    anim->flush();
    SQ_GLView::window()->resetStatusBar();
}

void SQ_GLWidget::clearParts(Parts *p, const int s)
{
    int sz = (s == -1) ? p->m32.size() : s;

    for(int i = 0;i < sz;i++)
        delete p->m32[i];
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

#ifndef SQ_SMALL
    KSquirrel::app()->slotFullScreen(fs);
#else
    SQ_GLView::window()->slotFullScreen(fs);
#endif
}

void SQ_GLWidget::initBrokenImage()
{
    memoryPart *pt;
    QImage broken = QPixmap(file_broken_xpm).convertToImage().swapRGB();
    broken.setAlphaBuffer(true);

    parts_broken = new Parts;

    // setup parts_broken. It will have only one 64x64 tile
    parts_broken->tiles = parts_broken->tilesx = parts_broken->tilesy = 1;
    parts_broken->realw = broken.width();
    parts_broken->realh = broken.height();
    parts_broken->w = broken.width();
    parts_broken->h = broken.height();
    parts_broken->tileSize = broken.width();
    parts_broken->makeParts();
    parts_broken->computeCoords();

    pt = new memoryPart(broken.width());
    pt->create();

    memcpy(pt->data(), broken.bits(), broken.numBytes());

    parts_broken->m32.push_back(pt);

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

    // save "broken" image information in 'finfo'
    finfo.image.push_back(image_broken);

    // reset statusbar widgets
    SQ_GLView::window()->resetStatusBar();

    // show error message instead of file name
    SQ_GLView::window()->sbarWidget("SBFile")->setText(SQ_ErrorString::instance()->string(err_index));

    // update context and show "broken" image
    updateGL();
}
