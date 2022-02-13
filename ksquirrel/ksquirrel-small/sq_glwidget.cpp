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

#include <kaction.h>
#include <kcursor.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kapp.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kstringhandler.h>
#include <kimageeffect.h>
#include <kmessagebox.h>

#include <math.h>
#include <stdlib.h>

#ifndef SQ_SMALL // light version doesn't require all this stuff...

#include "ksquirrel.h"
#include "sq_externaltool.h"
#include "sq_widgetstack.h"
#include "sq_fileiconview.h"
#include "sq_quickbrowser.h"
#include "sq_diroperator.h"

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

#include <csetjmp>

#include "fileio.h"
#include "fmt_codec_base.h"
#include "error.h"

SQ_GLWidget * SQ_GLWidget::sing = NULL;
int SQ_ToolButtonPopup::id = 10;

static const int timer_delay =		20;
static const int timer_delay_file = 10;
static const int len =				5;

static const float SQ_WINDOW_BACKGROUND_POS =	-1000.0f;
static const float SQ_IMAGE_CHECKER_POS = 		-999.0f;
static const float SQ_FIRST_FRAME_POS = 		-998.0f;
static const float SQ_MARKS_POS = 				-997.0f;
static const float SQ_FIRST_TILE_LAYER = 		-995.0f;

SQ_ToolButtonPopup::SQ_ToolButtonPopup(const QPixmap &pix, const QString &textLabel, QWidget *parent)
		: KToolBarButton(pix, ++SQ_ToolButtonPopup::id, parent, 0, textLabel)
{
	setFixedWidth(SQ_ToolButton::fixedWidth());
}

SQ_ToolButtonPopup::~SQ_ToolButtonPopup()
{}

SQ_ToolButton::SQ_ToolButton(const QIconSet &iconSet, const QString &textLabel,
                                                QObject *receiver, const char *slot, QToolBar *parent, const char *name)
                                                : QToolButton(iconSet, textLabel, QString::null, receiver, slot, parent, name)
{
	setFixedWidth(SQ_ToolButton::fixedWidth());
}

SQ_ToolButton::~SQ_ToolButton()
{}

int SQ_ToolButton::fixedWidth()
{
        return 28;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQ_ToolBar::SQ_ToolBar(QWidget *parent, const int members) : KToolBar(parent)
{
	setIconSize(22);
	setFixedHeight(34);
	boxLayout()->setSpacing(0);
	resize(SQ_ToolButton::fixedWidth() * members + 6, height());
}

SQ_ToolBar::~SQ_ToolBar()
{}

void SQ_ToolBar::mouseReleaseEvent(QMouseEvent *e)
{
	e->accept();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

memoryPart::memoryPart() : m_size(0), m_data(NULL)
{}

memoryPart::~memoryPart()
{
	if(m_data) delete m_data;
}

int memoryPart::size() const
{
  	return m_size;
}

void memoryPart::del()
{
	if(m_data) delete m_data;
}

bool memoryPart::valid() const
{
  	return m_data != NULL;
}

memoryPart32::memoryPart32()  : memoryPart()
{}

memoryPart32::~memoryPart32()
{}

void memoryPart32::create()
{
	m_size = 32;
	m_data = new unsigned char [32 * 32 * sizeof(RGBA)];
}

memoryPart64::memoryPart64()  : memoryPart()
{}

memoryPart64::~memoryPart64()
{}

void memoryPart64::create()
{
	m_size = 64;
	m_data = new unsigned char [64 * 64 * sizeof(RGBA)];
}

memoryPart128::memoryPart128()  : memoryPart()
{}

memoryPart128::~memoryPart128()
{}

void memoryPart128::create()
{
	m_size = 128;
	m_data = new unsigned char [128 * 128 * sizeof(RGBA)];
}

memoryPart256::memoryPart256()  : memoryPart()
{}

memoryPart256::~memoryPart256()
{}

void memoryPart256::create()
{
	m_size = 256;
	m_data = new unsigned char [256 * 256 * sizeof(RGBA)];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQ_GLWidget::SQ_GLWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
	// init all variables...
	sing = this;
	ac = new KActionCollection(this);
	isflippedV = false;
	isflippedH = false;
	memset(matrix, 0, sizeof(matrix));
	matrix[10] = matrix[5] = matrix[0] = 1.0f;
	curangle = 0.0f;
	decoded = reset_mode = false;
	next = NULL;
	changed = false;
	changed2 = true;
	inMouse = false;
	current = 0;
	total = 0;
	old_id = -1;
	blocked = false;
	blocked_force = false;
	zoomFactor = 1.0f;
	menu = new KPopupMenu(this);
	
	zoom_type = SQ_Config::instance()->readNumEntry("GL view", "zoom type", 3);
	m_hiding = SQ_Config::instance()->readBoolEntry("GL view", "actions", true);

	// load background for transparent image
	BGquads = QImage(locate("appdata", "images/checker.png"));

	if(BGquads.isNull())
	{
		BGquads = QImage(32, 32, 32).copy();
		BGquads.setAlphaBuffer(true);
		BGquads.fill(0);
	}

	zoomfactor = SQ_Config::instance()->readNumEntry("GL view", "zoom", 25);
	movefactor = SQ_Config::instance()->readNumEntry("GL view", "move", 5);
	rotatefactor = SQ_Config::instance()->readNumEntry("GL view", "angle", 90);

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
{}

/*
 *  With 'T' user can totally hide toolbars (with KToolBar::hide()).
 *  This method will help determine if toolbars are visible.
 */
bool SQ_GLWidget::actionsHidden() const
{
        return pAHideToolbars->isChecked();
}

// Create actions
void SQ_GLWidget::createActions()
{
	pARotateLeft = new KAction(QString::null, 0, this, SLOT(slotRotateLeft()), ac, "SQ Rotate picture left");
	pARotateRight = new KAction(QString::null, 0, this, SLOT(slotRotateRight()), ac, "SQ Rotate picture right");
	pAZoomPlus = new KAction(QString::null, 0, this, SLOT(slotZoomPlus()), ac, "SQ Zoom+");
	pAZoomMinus = new KAction(QString::null, 0, this, SLOT(slotZoomMinus()), ac, "SQ Zoom-");

	pAZoomW = new KToggleAction(i18n("Fit width"), QPixmap(locate("appdata", "images/actions/zoomW.png")), 0, this, SLOT(slotZoomW()), ac, "SQ ZoomW");
	pAZoomH = new KToggleAction(i18n("Fit height"), QPixmap(locate("appdata", "images/actions/zoomH.png")), 0, this, SLOT(slotZoomH()), ac, "SQ ZoomH");
	pAZoomWH = new KToggleAction(i18n("Fit image"), QPixmap(locate("appdata", "images/actions/zoomWH.png")), 0, this, SLOT(slotZoomWH()), ac, "SQ ZoomWH");
	pAZoom100 = new KToggleAction(i18n("Zoom 100%"), QPixmap(locate("appdata", "images/actions/zoom100.png")), 0, this, SLOT(slotZoom100()), ac, "SQ Zoom100");
	pAZoomLast = new KToggleAction(i18n("Leave previous zoom"), QPixmap(locate("appdata", "images/actions/zoomlast.png")), 0, this, SLOT(slotZoomLast()), ac, "SQ ZoomLast");
	pAIfLess = new KToggleAction(i18n("Ignore, if the image is less than window"), QPixmap(locate("appdata", "images/actions/ifless.png")), 0, 0, 0, ac, "SQ GL If Less");

	pAHelp = new KAction(i18n("Help"), "help", 0, this, SLOT(slotShowHelp()), ac, "SQ help");
	pAFlipV = new KAction(QString::null, 0, this, SLOT(slotFlipV()), ac, "SQ flip_v");
	pAFlipH = new KAction(QString::null, 0, this, SLOT(slotFlipH()), ac, "SQ flip_h");
	pAReset = new KAction(i18n("Reset"), locate("appdata", "images/menu/reset16.png"), 0, this, SLOT(slotMatrixReset()), ac, "SQ ResetGL");

#ifndef SQ_SMALL

	pAClose = new KAction(i18n("Close"), locate("appdata", "images/menu/close16.png"), 0, KSquirrel::app(), SLOT(slotCloseGLWidget()), ac, "SQ GL close");

#else

	pAClose = new KAction(i18n("Close"), locate("appdata", "images/menu/close16.png"), 0, SQ_GLView::window(), SLOT(close()), ac, "SQ GL close");

#endif

	pAProperties = new KAction(i18n("Properties"), locate("appdata", "images/menu/prop16.png"), 0, this, SLOT(slotProperties()), ac, "SQ GL Prop");
	pAFull = new KToggleAction(QString::null, 0, 0, 0, ac, "SQ GL Full");

#ifndef SQ_SMALL

	pAQuick = new KToggleAction(QString::null, 0, 0, 0, ac, "SQ GL Quick");

#endif

	pAHide = new KAction(QString::null, 0, this, SLOT(slotHideToolbar()), ac, "SQ GL THide");
	pAShow = new KAction(QString::null, 0, 0, this, SLOT(slotShowToolbar()), ac, "SQ GL TShow");

#ifndef SQ_SMALL

	pANext = new KAction(QString::null, 0, this, SLOT(slotNext()), ac, "SQ GL Next file");
	pAPrev = new KAction(QString::null, 0, this, SLOT(slotPrev()), ac, "SQ GL Prev file");
	pAFirst = new KAction(QString::null, 0, this, SLOT(slotFirst()), ac, "SQ GL File First");
	pALast = new KAction(QString::null, 0, this, SLOT(slotLast()), ac, "SQ GL File Last");
#endif

	pAHideToolbars = new KToggleAction(QString::null, QPixmap(), 0, 0, 0, ac, "SQ GL Hide toolbars");
	pAStatus = new KToggleAction(QString::null, QPixmap(), 0, 0, 0, ac, "SQ GL Toggle Status");

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

	pAStatus->setChecked(SQ_Config::instance()->readBoolEntry("GL view", "statusbar", true));
	pAIfLess->setChecked(SQ_Config::instance()->readBoolEntry("GL view", "ignore", false));
	pAHideToolbars->setChecked(SQ_Config::instance()->readBoolEntry("GL view", "toolbars_hidden", false));
}

void SQ_GLWidget::createToolbar()
{
	KActionSeparator *pASep = new KActionSeparator;
	zoom = new KPopupMenu;
	SQ_ToolButton	*pATool;

	QPalette pall(QColor(255,255,255), QColor(255,255,255));

	toolbar2 = new SQ_ToolBar(this, 1);
	toolbar2->move(0, 0);
	toolbar2->setPalette(pall);
	(void)new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/toolbar_show.png")), i18n("Show"), pAShow, SLOT(activate()), toolbar2);

#ifndef SQ_SMALL
	toolbar = new SQ_ToolBar(this, 19);
#else
	toolbar = new SQ_ToolBar(this, 13);
#endif

	toolbar->move(0, 0);
	toolbar->setPalette(pall);

	if(m_hiding)
		toolbar->show();
	else
		toolbar->move(-toolbar->width(), 0);

	toolbar2->setShown(!m_hiding);

	if(pAHideToolbars->isChecked())
	{
		toolbar->hide();
		toolbar2->hide();
	}

	pAZoom100->plug(zoom);
	pAZoomLast->plug(zoom);
	pASep->plug(zoom);
	pAZoomW->plug(zoom);
	pAZoomH->plug(zoom);
	pAZoomWH->plug(zoom);
	pASep->plug(zoom);
	pAIfLess->plug(zoom);

	switch(zoom_type)
	{
		case 0: pAZoomW->setChecked(true); break;
		case 1: pAZoomH->setChecked(true); break;
		case 2: pAZoomWH->setChecked(true); break;
		case 3: pAZoom100->setChecked(true); break;
		case 4: pAZoomLast->setChecked(true); break;

		default: pAZoom100->setChecked(true);
	}

#ifndef SQ_SMALL

	(void)new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_first.png")), i18n("Go to first file"), pAFirst, SLOT(activate()), toolbar);
	pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/prev.png")), i18n("Previous file"), pAPrev, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/next.png")), i18n("Next file"), pANext, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	(void)new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/file_last.png")), i18n("Go to last file"), pALast, SLOT(activate()), toolbar);

#endif

	pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/zoom+.png")), i18n("Zoom +"), pAZoomPlus, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/zoom-.png")), i18n("Zoom -"), pAZoomMinus, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pAToolZoom = new SQ_ToolButtonPopup(QPixmap(locate("appdata", "images/actions/zoom_template.png")), i18n("Zoom"), toolbar);
	pAToolZoom->setPopup(zoom);
	pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/rotateL.png")), i18n("Rotate left"), pARotateLeft, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/rotateR.png")), i18n("Rotate right"), pARotateRight, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/flipV.png")), i18n("Flip vertically"), pAFlipV, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/flipH.png")), i18n("Flip horizontally"), pAFlipH, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	(void)new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/reload.png")), i18n("Normalize"), pAReset, SLOT(activate()), toolbar);
	(void)new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/prop.png")), i18n("Image Properties"), pAProperties, SLOT(activate()), toolbar);
	pAToolFull = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/fullscreen.png")), i18n("Fullscreen"), pAFull, SLOT(activate()), toolbar);
	pAToolFull->setToggleButton(true);

#ifndef SQ_SMALL

	pAToolQuick = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/quick.png")), i18n("\"Quick Browser\""), pAQuick, SLOT(activate()), toolbar);
	pAToolQuick->setToggleButton(true);

#endif

	pAToolImages = new SQ_ToolButtonPopup(QPixmap(locate("appdata", "images/actions/images.png")), i18n("Select image"), toolbar);
	pAToolImages->setPopup(images);

#ifndef SQ_SMALL

	KToggleAction *n = KSquirrel::app()->pASlideShow;
	pAToolSlideShow = new SQ_ToolButton(n->iconSet(KIcon::Desktop).pixmap(QIconSet::Large, QIconSet::Normal), n->text(), n, SLOT(activate()), toolbar);
	pAToolSlideShow->setToggleButton(true);

#endif

	pAToolClose = new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/close.png")), i18n("Close"), pAClose, SLOT(activate()), toolbar);
	(void)new SQ_ToolButton(QPixmap(locate("appdata", "images/actions/toolbar_hide.png")), i18n("Hide"), pAHide, SLOT(activate()), toolbar);

	timer_show = new QTimer(this);
	timer_hide = new QTimer(this);

	connect(timer_show, SIGNAL(timeout()), SLOT(slotStepToolbarMove()));
	connect(timer_hide, SIGNAL(timeout()), SLOT(slotStepToolbarMove()));
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
}

// Resize OpenGL context. Used internally by QGLWidget.
void SQ_GLWidget::resizeGL(int width, int height)
{
	// set new viewport
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-width/2, width/2, -height/2, height/2, 0.1f, 10000.0f);
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

		// set parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // and repeat param
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);      // repeat texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// assign image to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

		bind = false;
	}
	else
		glBindTexture(GL_TEXTURE_2D, *tex);

	// draw texture
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);							glVertex2f(-half_w, half_h);
		glTexCoord2f(w/(GLfloat)dim, 0.0);				glVertex2f(half_w, half_h);
		glTexCoord2f(w/(GLfloat)dim, h/(GLfloat)dim);	glVertex2f(half_w, -half_h);
		glTexCoord2f(0.0, h/(GLfloat)dim);				glVertex2f(-half_w, -half_h);
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
	if(SQ_Config::instance()->readNumEntry("GL view", "GL view background type", 0) == 2)
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
		// if the image is transparent, and we should draw background for image
		if(finfo.image[current].hasalpha && SQ_Config::instance()->readBoolEntry("GL view", "alpha_bkgr", true))
		{
			static bool t = true;
			GLfloat w = (float)finfo.image[current].w / 2.0, h = (float)finfo.image[current].h / 2.0;

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

		// draw current image
		for(z = 0;z < parts[current].tilesy;z++)
			if(glIsList(parts[current].m_parts[z * parts[current].tilesx].list))
				glCallList(parts[current].m_parts[z * parts[current].tilesx].list);

		// draw tickmarks
		if(SQ_Config::instance()->readBoolEntry("GL view", "marks", true) && marks)
		{
			GLfloat zum = get_zoom();
			GLfloat x = fabsf(parts[current].m_parts[0].x1) * zum, y = parts[current].m_parts[0].y1 * zum;
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
				      rx, ly,      -x, ly,          -x, y,       rx, y,
  				      x, ly,        lx, ly,           lx, y,       x, y,
			      	x, -y,       lx, -y,          lx, ry,      x, ry,
				     rx, -y,      -x, -y,         -x, ry,       rx, ry
			};

			for(z = 0;z < 4;z++)
			{
				glBindTexture(GL_TEXTURE_2D, mark[z]);

				glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0);			glVertex2f(coords[z][0], coords[z][1]);
					glTexCoord2f(1.0, 0.0);			glVertex2f(coords[z][2], coords[z][3]);
					glTexCoord2f(1.0, 1.0);			glVertex2f(coords[z][4], coords[z][5]);
					glTexCoord2f(0.0, 1.0);			glVertex2f(coords[z][6], coords[z][7]);
				glEnd();
			}

			matrix_pop();
			write_gl_matrix();
		}
	}

	glDisable(GL_TEXTURE_2D);

	// update statusbar info
	matrixChanged();
	coordChanged();

	if(total > 1)
		frameChanged();
}

/*
 *  Change statusbar info according with
 *  current matrix (it shows current coordnates).
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
			.arg(get_angle(), 0, 'f', 1);

	SQ_GLView::window()->sbarWidget("SBGLAngle")->setText(str);
}

// Set zoom factor.
void SQ_GLWidget::setZoomFactor(const GLfloat &newfactor)
{
	zoomfactor = newfactor;
}

// Set move factor.
void SQ_GLWidget::setMoveFactor(const GLfloat &newfactor)
{
	movefactor = newfactor;
}

// Set rotate factor.
void SQ_GLWidget::setRotateFactor(const GLfloat &newfactor)
{
	rotatefactor = newfactor;
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

		// load next file in current directory
		if(SQ_Config::instance()->readNumEntry("GL view", "scroll", 1))
			pANext->activate();

		else

#endif

			// zoom image otherwise
			pAZoomPlus->activate();
	}
	else if(e->delta() > 0 && e->state() == Qt::NoButton)
	{

#ifndef SQ_SMALL

		if(SQ_Config::instance()->readNumEntry("GL view", "scroll", 1))
			pAPrev->activate();
		else

#endif

			pAZoomMinus->activate();

	}
	// some special bindings:
	// if CTRL key is pressed, zoom 2x or 0.5x
	else if(e->delta() < 0 && e->state() == Qt::ControlButton)
		matrix_zoom(2.0f);
	else if(e->delta() > 0 && e->state() == Qt::ControlButton)
		matrix_zoom(0.5f);

	// TODO: remove ?
	else if(e->delta() < 0 && e->state() == Qt::ShiftButton)
		slotZoomPlus();
	else if(e->delta() > 0 && e->state() == Qt::ShiftButton)
		slotZoomMinus();
	else
		e->accept();
}

// User pressed mouse button down.
void SQ_GLWidget::mousePressEvent(QMouseEvent *e)
{
	// left button, update cursor
	if(e->button() == Qt::LeftButton)
	{
		setCursor(cdrag);

		xmoveold = e->x();
		ymoveold = e->y();
		inMouse = true;
	}
	// right button, let's start drawing zoom frame
	else if(e->button() == Qt::RightButton)
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
		inMouse = true;
		crossDrawn = false;
	}
	// middle button - show context menu
	else if(e->button() == Qt::MidButton)
		menu->exec(QCursor::pos());
	else
	    e->accept();
}

// User moved mouse.
void SQ_GLWidget::mouseMoveEvent(QMouseEvent *e)
{
	// user din't press any mouse button before ?
	if(!inMouse)
	{
		e->accept();
		return;
	}

	// left mouse button, lets move image (or matrix :-) )
	if(e->state() == Qt::LeftButton)
	{
		xmove = e->x();
		ymove = e->y();

		matrix_move(xmove-xmoveold, -ymove+ymoveold);

		xmoveold = e->x();
		ymoveold = e->y();
	}
	// right button
	else if(e->state() == Qt::RightButton)
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

		QPoint C = rect.center();
		pRect->drawRect(rect);

		if(crossDrawn)
		{
			pRect->drawLine(C.x(), C.y() - len, C.x(), C.y() + len);
			pRect->drawLine(C.x() - len, C.y(), C.x() + len, C.y());
		}

		lastRect = rect;
	}
	else
		e->accept();
}

// User released some mouse button.
void SQ_GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
	// left button - restore cursor
	if(e->state() == Qt::LeftButton)
		setCursor(cusual);
	// right button - zoom to selected rectangle (if needed)
	else if(e->state() == Qt::RightButton)
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
			bool zoomed = zoomRect(lastRect);

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
	else
		e->accept();

	inMouse = false;
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
	unsigned long value = e->key();
	const unsigned short Key_KC = Qt::ControlButton|Qt::Keypad;

	// TODO: x86-64 ???
	value <<= 16;
	value |= e->state();

#define SQ_KEYSTATES(a,b) ((a << 16) | b)

	// determine key, flags and do what user wants
	switch(value)
	{
		case SQ_KEYSTATES(Qt::Key_Left, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Left, Qt::NoButton):			matrix_move(movefactor, 0);		break;
		case SQ_KEYSTATES(Qt::Key_Right, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Right, Qt::NoButton):			matrix_move(-movefactor, 0);	break;
		case SQ_KEYSTATES(Qt::Key_Up, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Up, Qt::NoButton):			matrix_move(0, -movefactor);	break;
		case SQ_KEYSTATES(Qt::Key_Down, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Down, Qt::NoButton):		matrix_move(0, movefactor);		break;
		case SQ_KEYSTATES(Qt::Key_Plus, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Equal, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Plus, Qt::NoButton):			pAZoomPlus->activate();			break;
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::NoButton):		pAZoomMinus->activate();		break;
		case SQ_KEYSTATES(Qt::Key_Plus, Key_KC):
		case SQ_KEYSTATES(Qt::Key_Equal, Qt::ControlButton):
		case SQ_KEYSTATES(Qt::Key_Plus, Qt::ControlButton):		matrix_zoom(2.0f);				break;
		case SQ_KEYSTATES(Qt::Key_Minus, Key_KC):
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::ControlButton):	matrix_zoom(0.5f);				break;
		case SQ_KEYSTATES(Qt::Key_V, Qt::NoButton):				pAFlipV->activate();				break;
		case SQ_KEYSTATES(Qt::Key_H, Qt::NoButton):			pAFlipH->activate();				break;
		case SQ_KEYSTATES(Qt::Key_Left, Qt::ControlButton):		pARotateLeft->activate();			break;
		case SQ_KEYSTATES(Qt::Key_Right, Qt::ControlButton):	pARotateRight->activate();		break;
		case SQ_KEYSTATES(Qt::Key_R, Qt::NoButton):				pAReset->activate();				break;
		case SQ_KEYSTATES(Qt::Key_Up, Qt::ControlButton):		matrix_rotate(180.0f);				break;
		case SQ_KEYSTATES(Qt::Key_Down, Qt::ControlButton):	matrix_rotate(-180.0f);			break;
		case SQ_KEYSTATES(Qt::Key_Left, Qt::ShiftButton):		matrix_rotate(-1.0f);				break;
		case SQ_KEYSTATES(Qt::Key_Right, Qt::ShiftButton):		matrix_rotate(1.0f);				break;

#ifndef SQ_SMALL

		case SQ_KEYSTATES(Qt::Key_PageDown, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_PageDown, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Space, Qt::NoButton):		pANext->activate();				break;
		case SQ_KEYSTATES(Qt::Key_PageUp, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_PageUp, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_BackSpace, Qt::NoButton):	pAPrev->activate();				break;

#endif

		case SQ_KEYSTATES(Qt::Key_X, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Escape, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Return, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Enter, Qt::Keypad):			pAToolClose->animateClick();		break;
		case SQ_KEYSTATES(Qt::Key_P, Qt::NoButton):			pAProperties->activate();				break;

#ifndef SQ_SMALL
		
		case SQ_KEYSTATES(Qt::Key_Home, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Home, Qt::NoButton):		pAFirst->activate();				break;
		case SQ_KEYSTATES(Qt::Key_End, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_End, Qt::NoButton):			pALast->activate();				break;
		case SQ_KEYSTATES(Qt::Key_Q, Qt::NoButton):			pAToolQuick->animateClick();	break;

#endif

		case SQ_KEYSTATES(Qt::Key_F, Qt::NoButton):			pAToolFull->animateClick();			break;
		case SQ_KEYSTATES(Qt::Key_T, Qt::NoButton):			pAHideToolbars->activate();			break;
		case SQ_KEYSTATES(Qt::Key_BracketLeft, Qt::NoButton):	pAHideToolbars->setChecked(false);pAHide->activate();	break;
		case SQ_KEYSTATES(Qt::Key_BracketRight, Qt::NoButton):pAHideToolbars->setChecked(false);pAShow->activate();break;
		case SQ_KEYSTATES(Qt::Key_Z, Qt::NoButton):			slotZoomMenu();					break;
		case SQ_KEYSTATES(Qt::Key_S, Qt::NoButton):			pAStatus->activate();				break;
		case SQ_KEYSTATES(Qt::Key_A, Qt::NoButton):			slotToggleAnimate();				break;
		case SQ_KEYSTATES(Qt::Key_I, Qt::NoButton):			slotShowImages();				break;
		case SQ_KEYSTATES(Qt::Key_F1, Qt::NoButton):			jumpToImage(false);			break;
		case SQ_KEYSTATES(Qt::Key_F2, Qt::NoButton):			prevImage();					break;
		case SQ_KEYSTATES(Qt::Key_F3, Qt::NoButton):			nextImage();					break;
		case SQ_KEYSTATES(Qt::Key_F4, Qt::NoButton):			jumpToImage(true);			break;
		case SQ_KEYSTATES(Qt::Key_Slash, Qt::NoButton):		slotShowHelp();				break;
		case SQ_KEYSTATES(Qt::Key_B, Qt::NoButton):			toggleDrawingBackground();		break;
		case SQ_KEYSTATES(Qt::Key_K, Qt::NoButton):			toogleTickmarks();					break;

#ifndef SQ_SMALL

		case SQ_KEYSTATES(Qt::Key_E, Qt::NoButton):			showExternalTools();				break;

#endif

		case SQ_KEYSTATES(Qt::Key_Delete, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Delete, Qt::NoButton):		deleteWrapper();				break;
		case SQ_KEYSTATES(Qt::Key_M, Qt::NoButton):			menu->exec(QCursor::pos());break;

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
					if(finfo.image.empty()) break;

					int val = e->key();
					val = val - Qt::Key_1 + 1;
					matrix_pure_reset();
					if(finfo.image[current].needflip) flip(4, false);

					if(val == 0)
						val = 10;

					matrix_zoom(val);
				}
				break;

				case Qt::Key_Comma:	pAZoomW->activate();		break;
				case Qt::Key_Period:	pAZoomH->activate();		break;
				case Qt::Key_Asterisk:	pAZoomWH->activate();		break;
				case Qt::Key_L:			pAIfLess->activate();		break;

				default:
					e->accept();
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
		QStringList::Iterator i = files.begin();

		// try to load first file
		if(SQ_LibraryHandler::instance()->supports(*i))
			slotStartDecoding(*i);
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
	if(finfo.image.empty()) return;

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

	if(finfo.image.empty()) return;

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

	if(finfo.image.empty()) return;

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

	if(finfo.image.empty()) return;

	internalZoom(zoomFactor);
}

// Zoom 100%.
void SQ_GLWidget::slotZoom100()
{
	zoom_type = 3;
	pAZoom100->setChecked(true);

	if(finfo.image.empty()) return;

	internalZoom(1.0f);
}

// "Ignore, if the image is less than window"
void SQ_GLWidget::slotZoomIfLess()
{
	if(finfo.image.empty()) return;

	switch(zoom_type)
	{
		case 0: pAZoomW->activate(); break;
		case 1: pAZoomH->activate(); break;
		case 2: pAZoomWH->activate(); break;
		case 3: break;

		default:
			pAZoomLast->activate();
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
	if(finfo.image.empty()) return;

	// Stop animation...
	stopAnimation();

	const int real_size = finfo.image[current].w * finfo.image[current].h * sizeof(RGBA);
	QString sz = KIO::convertSize(real_size);
	QStringList list;

	QValueVector<QPair<QString,QString> > meta;

	if(!finfo.meta.empty())
	{
		std::vector<fmt_metaentry>::iterator BEGIN = finfo.meta.begin();
		std::vector<fmt_metaentry>::iterator END = finfo.meta.end();

		for(std::vector<fmt_metaentry>::iterator it = BEGIN;it != END;++it)
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
	list  << quickImageInfo
		<< QString::fromLatin1("%1x%2").arg(finfo.image[current].w).arg(finfo.image[current].h)
		<< QString::fromLatin1("%1").arg(finfo.image[current].bpp)
#ifndef QT_NO_STL
		<< finfo.image[current].colorspace
		<< finfo.image[current].compression
#else
		<< finfo.image[current].colorspace.c_str()
		<< finfo.image[current].compression.c_str()
#endif
		<<	sz
		<< QString::fromLatin1("%1").arg((double)real_size / fm.size(), 0, 'f', 2)
		<< ((finfo.image[current].interlaced) ? i18n("yes") : i18n("no"))
		<< QString::fromLatin1("%1").arg(errors)
		<< QString::fromLatin1("%1").arg(finfo.image.size())
		<< QString::fromLatin1("#%1").arg(current+1)
		<< QString::fromLatin1("%1").arg(finfo.image[current].delay);

	// create dialog and setup it
	SQ_ImageProperties *prop = new SQ_ImageProperties(this);
	prop->setFile(File);
	prop->setParams(list);
	prop->setMetaInfo(meta);

	// show!
	prop->exec();

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
//	write_gl_matrix();

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
	isflippedH = !isflippedH;
	flip(0);
}

void SQ_GLWidget::slotFlipV()
{
	isflippedV = !isflippedV;
	flip(4);
}

void SQ_GLWidget::slotMatrixReset()
{
	matrix_reset();
	updateGL();
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
/*
	if(finfo)
		if(finfo.image[current].needflip)
		{
			bool old_reset = reset_mode;
			reset_mode = true;
			slotFlipV();
			reset_mode = old_reset;
			return;
		}
*/
	write_gl_matrix();
}

bool SQ_GLWidget::matrix_zoom(GLfloat ratio)
{
	int zoom_lim = SQ_Config::instance()->readNumEntry("GL view", "zoom limit", 0);
	float zoom_min, zoom_max, zoom_tobe;

	zoom_tobe = hypotf(MATRIX_C1 * ratio, MATRIX_S1 * ratio) * 100.0f;

	switch(zoom_lim)
	{
		case 0:
		break;

		case 2:
			zoom_min = (float)SQ_Config::instance()->readNumEntry("GL view", "zoom_min", 1);
			zoom_max = (float)SQ_Config::instance()->readNumEntry("GL view", "zoom_max", 10000);
		break;

		default:
			zoom_min = 1.0f;
			zoom_max = 10000.0f;
	}

	if(zoom_lim != 0)
		if((ratio < 1.0f && zoom_min >= zoom_tobe) || (ratio > 1.0f && zoom_max <= zoom_tobe))
			return false;

	MATRIX_C1 *= ratio;
	MATRIX_S1 *= ratio;
	MATRIX_X *= ratio;
	MATRIX_S2 *= ratio;
	MATRIX_C2 *= ratio;
	MATRIX_Y *= ratio;

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

GLfloat SQ_GLWidget::get_angle() const
{
	return curangle;
}

void SQ_GLWidget::matrix_rotate(GLfloat angle)
{
	double cosine, sine, rad;
	const double rad_const = 0.017453;
	GLfloat c1 = MATRIX_C1, c2 = MATRIX_C2, s1 = MATRIX_S1, s2 = MATRIX_S2;

	rad = (double)angle * rad_const;
	cosine = cos(rad);
	sine = sin(rad);

	MATRIX_C1 = c1 * cosine + s2 * sine;
	MATRIX_S1 = s1 * cosine + c2 * sine;
	MATRIX_S2 = -c1 * sine + s2 * cosine;
	MATRIX_C2 = -s1 * sine + c2 * cosine;

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

	switch(SQ_Config::instance()->readNumEntry("GL view", "GL view background type", 0))
	{
		// system color
		case 0:
			color = colorGroup().color(QColorGroup::Base);
		break;

		// custom color
		case 1:
			color.setNamedColor(SQ_Config::instance()->readEntry("GL view", "GL view background", "#cccccc"));
		break;

		// repeated texture
		case 2:
			path = SQ_Config::instance()->readEntry("GL view", "GL view custom texture", "");
			BGpixmap.load(path);

			if(BGpixmap.isNull())
			{
				qWarning("Texture is corrupted! Falling back to 'System color'\n");
				SQ_Config::instance()->setGroup("GL view");
				SQ_Config::instance()->writeEntry("GL view background type", 0);
				setClearColor();
				return;
			}

			BGpixmap.convertDepth(32);
			BGpixmap = QGLWidget::convertToGLFormat(BGpixmap);

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

void SQ_GLWidget::findCloserTiles(int w, int h, int &tile1, int &tile2)
{
	int i = 0;
	int max = QMAX(w, h);
	float fw = (float)w;
	float fh = (float)h;
	bool tile_factor = false;

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
}

#ifndef SQ_SMALL

void SQ_GLWidget::createQuickBrowser()
{
	v = new SQ_QuickBrowser(this);
}

#endif

/*
 *  Prepare decoding. It will find proper library for decoding,
 *  clear old memory buffers, etc.
 */
bool SQ_GLWidget::prepare()
{
//	const char		*name = File.ascii();
	QString			status;
	unsigned int		i;

	fm.setFile(File);

	// hope somebody already called SQ_LibraryHandler::instance()->supports() :-)
	lib = SQ_LibraryHandler::instance()->latestLibrary();

	if(!lib)
	{

#ifdef SQ_SMALL

		KMessageBox::error(SQ_GLView::window(), i18n(QString("Library for %1 format not found")).arg(fm.extension(false)));

#else

		KMessageBox::error(KSquirrel::app(), i18n(QString("Library for %1 format not found")).arg(fm.extension(false)));


#endif

		return false;
	}

	// determine codec
	codeK = lib->codec;

	// clear any previous info
	finfo.image.clear();
	finfo.meta.clear();

#ifndef QT_NO_STL

	// start decoding!
	i = codeK->fmt_read_init(File);

#else

	i = codeK->fmt_read_init(File.ascii());

#endif

	// oops, error...
	if(i != SQE_OK)
	{

#ifdef SQ_SMALL

		KMessageBox::error(SQ_GLView::window(), i18n("fmt_read_init: Library returned error:") + SQ_ErrorString::instance()->string(i));

#else

		KMessageBox::error(KSquirrel::app(), i18n("fmt_read_init: Library returned error:") + SQ_ErrorString::instance()->string(i));

#endif

		return false;
	}

	// remove previuos parts
	if(!parts.empty())
	{
		for(int z = 0;z < total;z++)
			parts[z].removeParts();

		parts.clear();
	}

	// delete textures
	for(unsigned int s = 0;s < m32.size();s++)
		(m32[s])->del();

	m32.clear();

	current = 0;

	return true;
}

/*
 *  Bind textures, draw them and create GL lists.
 */
bool SQ_GLWidget::showFrames(int i)
{
	int z;
	const int a = parts[current].tilesx * i, b = parts[current].tilesx * (i+1);
	GLboolean resident = GL_TRUE, ret;
	int layers = b;

	// for safety...
	makeCurrent();

	glEnable(GL_TEXTURE_2D);

	for(z = a;z < b;z++)
	{
		glBindTexture(GL_TEXTURE_2D, parts[current].m_parts[z].tex);

		// ??
//		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ARB, SQ_DIV, SQ_DIV, 0, GL_RGBA, GL_UNSIGNED_BYTE, mem_parts[z].part);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tileSize, tileSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, (m32[z])->m_data);
	}

	// create new display list
	glNewList(parts[current].m_parts[a].list, GL_COMPILE_AND_EXECUTE);

	for(z = a;z < b;z++)
	{
		glBindTexture(GL_TEXTURE_2D, parts[current].m_parts[z].tex);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(parts[current].m_parts[z].x1, parts[current].m_parts[z].y1);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(parts[current].m_parts[z].x2, parts[current].m_parts[z].y1);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(parts[current].m_parts[z].x2, parts[current].m_parts[z].y2);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(parts[current].m_parts[z].x1, parts[current].m_parts[z].y2);
		glEnd();
	}

	glEndList();

	glDisable(GL_TEXTURE_2D);

	// show buffer...
	swapBuffers();

	// TODO: check!
	for(z = a;z < b;z++)
	{
		ret = glAreTexturesResident(1, &parts[current].m_parts[z].tex, &resident);

		if(ret == GL_FALSE)
			break;
	}

	return (resident == GL_TRUE);
}

/*
 *  Divide currently decoded image into
 *  several parts and store them in MemoryPart::m_data.
 */
void SQ_GLWidget::setupBits(Parts *p, RGBA *b, int y)
{
	int index = y * p->tilesx;
	int rw2 = p->rw;
	int f = tileSize * sizeof(RGBA);
	unsigned char *vv = 0;

	for(int x = 0;x < p->tilesx;x++)
	{
		vv = (m32[index])->m_data;

		for(int j = 0,k = 0;j < tileSize;j++,k++)
			memcpy(vv + k*f, b + x*tileSize + k*rw2, f);

		index++;
	}
}

void Parts::setRWH(const int rw2, const int rh2)
{
	rw = rw2;
	rh = rh2;
}

void Parts::setWH(const int w2, const int h2)
{
	w = w2;
	h = h2;
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

// Create parts.
bool Parts::makeParts()
{
     int z;

	tiles = tilesx * tilesy;

	for(int s = 0;s < tiles;s++)
		m_parts.push_back(Part());

	for(z = 0;z < tiles;z++)
		glGenTextures(1, &m_parts[z].tex);

	GLuint base = glGenLists(tilesy);

	if(!base)
	{
		for(z = 0;z < tiles;z++)
			glDeleteTextures(1, &m_parts[z].tex);

		return false;
	}

	// calculate display list's id
	for(z = 0;z < tilesy;z++)
		m_parts[z * tilesx].list = base + z;

	return true;
}

void Parts::setTileSize(const int t)
{
	tileSize = t;
}

// Calculate coordinates for textures
void Parts::computeCoords()
{
	int index = 0;
	float X, Y;

	Y = (float)h / 2.0;

	for(int y = 0;y < tilesy;y++)
	{
	    X = -(float)w / 2.0;

	    for(int x = 0;x < tilesx;x++)
	    {
			m_parts[index].x1 = X;
			m_parts[index].y1 = Y;
			m_parts[index].x2 = m_parts[index].x1 + tileSize;
			m_parts[index].y2 = m_parts[index].y1 - tileSize;

			index++;
			X += tileSize;
	    }

	    Y -= tileSize;
	} // for(int y = 0;y < tilesy;y++)
}

#ifndef SQ_SMALL

// Show/hide Quick Browser.
void SQ_GLWidget::slotShowQuick(bool b)
{
	v->setShown(b);

	if(b)
	{
		KFileItem *f = SQ_QuickBrowser::quickOperator()->iv->currentFileItem();

		if(f)
			SQ_QuickBrowser::quickOperator()->setCurrentItem(f);
	}
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

	m_File = file;
	File = QFile::encodeName(m_File);

	// prepare decoding...
	if(!prepare())
	{
		reset_mode = false;
		return;
	}

#ifndef SQ_SMALL

	bool slideShowRunning = KSquirrel::app()->slideShowRunning();

	// goto fullscreen, if needed
	if(slideShowRunning && !pAFull->isChecked() && SQ_Config::instance()->readBoolEntry("Slideshow", "fullscreen", true))
		pAToolFull->animateClick();

	// show window with image
	if(!slideShowRunning || (slideShowRunning && SQ_Config::instance()->readBoolEntry("Slideshow", "force", true)))
		KSquirrel::app()->raiseGLWidget();

	KSquirrel::app()->setCaption(file);

#else

	SQ_GLView::window()->setCaption(file);

#endif

	if(!pAHideToolbars->isChecked())
		if(m_hiding)
			toolbar->show();
		else
			toolbar2->show();

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
	int i, j, id, realW, realH;
	int line, res, first_id = 0;

	QFileInfo ff(m_File);

	timer_anim->stop();
	images->clear();
//	images->insertTitle(KStringHandler::rsqueeze(ff.fileName(), 20));
	SQ_GLView::window()->sbarWidget("SBFile")->setText(KStringHandler::rsqueeze(ff.fileName(), 25));

	QTime started;
	started.start();

	current = 0;

        fmt_image *im;

	while(true)
	{
//		finfo.image = (fmt_image *)realloc(finfo.image, sizeof(fmt_image) * (finfo.images+1));
	//	memset(&finfo.image[current], 0, sizeof(fmt_image));

		i = codeK->fmt_read_next();

		if(i != SQE_OK)
		{
			if(i == SQE_NOTOK)
				break;
			else if(current)
				break;
			else
			{
				codeK->fmt_read_close();
				finfo.image.clear();
				finfo.meta.clear();
				reset_mode = false;
				total = 0;
				decoded = false;
				return;
			}
		}

                im = codeK->image(current);

                parts.push_back(Parts());
		
		findCloserTiles(im->w, im->h, parts[current].tilesx, parts[current].tilesy);
		realW = parts[current].tilesx * tileSize;
		realH = parts[current].tilesy * tileSize;

//		printf("tilesx: %d, tilesy: %d\n", parts[current].tilesx, parts[current].tilesy);

		parts[current].setRWH(realW, realH);
		parts[current].setWH(im->w, im->h);
		parts[current].setTileSize(tileSize);

		if(!parts[current].makeParts())
		{
			QString status = i18n("Memory realloc failed.\n\nI tried to realloc memory for image:\n\nwidth: %1\nheight: %2\nbpp: %3\nTotal needed: ")
					.arg(im->w)
					.arg(im->h)
					.arg(im->bpp)
					+ KIO::convertSize(realW * realH * sizeof(RGBA)) + i18n(" of memory.");

#ifdef SQ_SMALL

			KMessageBox::error(SQ_GLView::window(), status);

#else

			KMessageBox::error(KSquirrel::app(), status);

#endif

			return;
		}

		int s;

		for(unsigned int d = 0;d < m32.size();d++)
			(m32[d])->del();

		m32.clear();

		memoryPart *pt;

		for(s = 0;s < parts[current].tiles;s++)
		{
			switch(tileSize)
			{
				case 32:	pt = new memoryPart32;	break;
				case 64:	pt = new memoryPart64;	break;
				case 128:	pt = new memoryPart128;	break;
				case 256:	pt = new memoryPart256;	break;
			}

			pt->create();

			m32.push_back(pt);
		}

		parts[current].computeCoords();

		next = (RGBA *)realloc(next, realW * tileSize * sizeof(RGBA));

		if(!next)
		{

#ifdef SQ_SMALL

			KMessageBox::error(SQ_GLView::window(), i18n("Memory allocation failed"));

#else

			KMessageBox::error(KSquirrel::app(), i18n("Memory allocation failed"));

#endif

			if(finfo.image.size() == 1)
				break;
			else
			{
				current = 0;
				return;
			}
		}

		line = 0;

		if(!current)
		{
			finfo = codeK->information();

			slotZoomIfLess();

			curangle = 0.0f;
			isflippedH = isflippedV = false;
			matrixChanged();

//			if(im->needflip)
//				flip(4, false);
		}

		matrix_move_z(SQ_FIRST_TILE_LAYER+current);

		for(int pass = 0;pass < im->passes;pass++)
		{
			if(codeK->fmt_read_next_pass() != SQE_OK)
			{
//				qWarning("%s", i18n("fmt_next_pass() failed").ascii());
				break;
			}

//			printf("__Doing pass %d\n", pass);

			line = 0;

//			printf("im->h: %d\n", im->h);

			for(i = 0;i < parts[current].tilesy;i++)
			{
				memset(next, 0, realW * tileSize * sizeof(RGBA));

				for(j = 0;j < tileSize;j++)
				{
					if(line++ >= im->h)
						break;

//					printf("Got line %d\n", line-1);
					res = codeK->fmt_read_scanline(next + realW*j);
					errors += (int)(res != SQE_OK);
				}

				if(pass == im->passes-1)
				{
					setupBits(&parts[current], next, i);
					bool b = showFrames(i);

					if(!b)
						printf("Showframes failed for image %d, tiley %d\n", current, i);
				}
			}
		}

		id = images->insertItem(QString::fromLatin1("#%1 [%2x%3@%4]")
									.arg(current+1)
									.arg(im->w)
									.arg(im->h)
									.arg(im->bpp));

		images->setItemParameter(id, current);

//		fprintf(stderr, "id: %d, current: %d\n", id, current);

		if(!current)
		{
//			QString status;
//			status.sprintf("%dx%d@%d", im->w, im->h, im->bpp);
//			sqSBDecoded->setText(status);

			SQ_GLView::window()->sbarWidget("SBDecodedI")->clear();
			SQ_GLView::window()->sbarWidget("SBDecodedI")->setPixmap(QPixmap(lib->mime));

			first_id = id;
		}
//		printf("Done for current = #%d\n", current);
		current++;
	} // while(true)

	finfo = codeK->information();

	codeK->fmt_read_close();

//	QMessageBox::warning(KSquirrel::app(), i18n("Decoding"), i18n("Cycle done"), QMessageBox::Ok, QMessageBox::NoButton);
	current = 0;
	total = finfo.image.size();
	updateCurrentFileInfo();

	decoded = true;
	reset_mode = false;
	frameChanged();
	updateGL();

//	printf("Decoded. total = %d\n", total);
	quickImageInfo = lib->quickinfo;

	int secs;
	int msecs = started.elapsed();
	adjustTimeFromMsecs(secs, msecs);
	QString r = (secs) ? i18n("Loaded in %1'%2 msec.").arg(secs).arg(msecs) : i18n("Loaded in %1 msec.").arg(msecs);
	SQ_GLView::window()->sbarWidget("SBLoaded")->setText(r);

	if(finfo.animated)
		QTimer::singleShot(1, this, SLOT(slotAnimateNext()));
	else
		images->setItemChecked(first_id, true);

//	printf("tileSize: %d\n", tileSize);
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

void SQ_GLWidget::slotShowToolbar()
{
	m_hiding = false;
	steps = 30;

	toolbar2->hide();
	toolbar->show();

	timer_show->stop();
	timer_hide->stop();

	timer_show->start(5);
}

void SQ_GLWidget::slotHideToolbar()
{
	m_hiding = true;
	steps = 30;

	timer_show->stop();
	timer_hide->stop();

	timer_hide->start(5);
}

void SQ_GLWidget::slotStepToolbarMove()
{
	static int wt = toolbar->width();

	if(m_hiding)
		toolbar->move(-wt/steps, 0);
	else
		toolbar->move(-wt + wt/steps, 0);

	if(!--steps)
	{
		if(m_hiding)
		{
			timer_hide->stop();
			toolbar2->show();
		}
		else
		{
			timer_show->stop();
			toolbar->show();
		}

		m_hiding = !m_hiding;
	}
}

void SQ_GLWidget::slotHideToolbars(bool hidden)
{
	if(!hidden)
	{
		if(m_hiding)
			toolbar->show();
		else
			toolbar2->show();
	}
	else
	{
		toolbar->hide();
		toolbar2->hide();
	}
}

/*
 *  Palette changed. Let's update tickmarks and background color.
 */
void SQ_GLWidget::paletteChange(const QPalette &oldPalette)
{
	QGLWidget::paletteChange(oldPalette);

	if(SQ_Config::instance()->readNumEntry("GL view", "GL view background type", 0) == 0)
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

//	if(!reset_mode && SQ_WidgetStack::instance()->moveTo(SQ_WidgetStack::Next) == SQ_WidgetStack::moveSuccess)
	    timer_next->start(timer_delay_file, true);
}

void SQ_GLWidget::slotPrev()
{
	timer_prev->stop();
	timer_next->stop();

//	if(!reset_mode && SQ_WidgetStack::instance()->moveTo(SQ_WidgetStack::Previous) == SQ_WidgetStack::moveSuccess)
	    timer_prev->start(timer_delay_file, true);
}

#else

void SQ_GLWidget::slotFirst()
{}

void SQ_GLWidget::slotLast()
{}

void SQ_GLWidget::slotNext()
{}

void SQ_GLWidget::slotPrev()
{}

#endif

void SQ_GLWidget::slotZoomMenu()
{
	zoom->exec(QCursor::pos());
}

void SQ_GLWidget::slotAnimateNext()
{
	updateGL();

	int delay = finfo.image[current].delay;

	current++;

	if(current >= (int)finfo.image.size())
		current = 0;

	updateCurrentFileInfo();

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
//	id++;
	images->setItemChecked(old_id, false);
	current = images->itemParameter(id);

//	fprintf(stderr, "clicked id: %d, current: %d\n", id, current);

	images->setItemChecked(id, true);
	old_id = id;

	updateCurrentFileInfo();

	updateGL();
}

void SQ_GLWidget::slotImagesShown()
{
	if(finfo.animated)
		if(!timer_anim->isActive())
			blocked = false;
		else
		{
			stopAnimation();
			blocked = true;
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

void SQ_GLWidget::nextImage()
{
	current++;

	if(current >= (int)finfo.image.size())
		current = 0;

	updateGL();

	updateCurrentFileInfo();
}

void SQ_GLWidget::prevImage()
{
	current--;

	if(current < 0)
		current = finfo.image.size() - 1;

	updateGL();

	updateCurrentFileInfo();
}

void SQ_GLWidget::jumpToImage(bool last)
{
	current = (last) ? finfo.image.size() - 1 : 0;

	updateGL();

	updateCurrentFileInfo();
}

void SQ_GLWidget::slotShowHelp()
{
	SQ_HelpWidget *help_w = new SQ_HelpWidget(this);/*, 0, false, Qt::WStyle_StaysOnTop 
								    | Qt::WStyle_Customize 
								    | Qt::WStyle_NoBorder);*/

	help_w->exec();
}

// Show/hide background for transparent image.
void SQ_GLWidget::toggleDrawingBackground()
{
	bool b = SQ_Config::instance()->readBoolEntry("GL view", "alpha_bkgr", true);

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
	if(!marks)
		return;

	if(!first)
		return;

	first = false;

	int param[4];
	unsigned char p[3];
	glGetIntegerv(GL_COLOR_CLEAR_VALUE, param);

	p[0] = param[0] >> 23;
	p[1] = param[1] >> 23;
	p[2] = param[2] >> 23;

	QColor adj = calculateAdjustedColor(BGpixmap, QColor(p[0],p[1],p[2]),
										(SQ_Config::instance()->readNumEntry("GL view", "GL view background type", 0) != 2));

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
		r = (rgb.red() < 128) ? 255 : 0;
		g = (rgb.green() < 128) ? 255 : 0;
		b = (rgb.blue() < 128) ? 255 : 0;
	}
	else
	{
		int R = 0, G = 0, B = 0;
		int F = im.height() * im.width();
		int Z = F * 4;

		unsigned char *p = im.bits();

		for(int i = 0;i < Z;i += 4)
		{
			R += *(p+i);
			G += *(p+i+1);
			B += *(p+i+2);
		}

		R /= F;
		G /= F;
		B /= F;

//		printf("Med. color: %d,%d,%d\n", R, G, B);

		r = (R < 128) ? 255 : 0;
		g = (G < 128) ? 255 : 0;
		b = (B < 128) ? 255 : 0;
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

	SQ_WidgetStack::instance()->visibleWidget()->del(list, this);

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
	bool b = SQ_Config::instance()->readBoolEntry("GL view", "marks", true);

	b = !b;

	SQ_Config::instance()->writeEntry("marks", b);

	updateGL();
}

/*
 *  Show current page number in multipaged imaged.
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

// interla class
struct KeyE
{
	KeyE()
	{}

	KeyE(QString s1, QString i, const char *s2, int s3, KActionMenu *s4, bool s5)
		: name(s1), icon(i), n(s2), flags(s3), sub(s4), sep(s5)
	{}

	QString name;
	QString icon;
	const char *n;
	int flags;
	KActionMenu *sub;
	bool sep;
};

// Create context menu with actions.
void SQ_GLWidget::createContextMenu(KPopupMenu *m)
{

#ifndef SQ_SMALL

	KActionMenu *menuFile = new KActionMenu(i18n("File"), "fileopen");

#endif

	KActionMenu *menuRotate = new KActionMenu(i18n("Rotate"), "view_orientation");
	KActionMenu *menuZoom = new KActionMenu(i18n("Zoom"), "viewmag");//, locate("appdata", "images/menu/zoom16.png"));
	KActionMenu *menuMove = new KActionMenu(i18n("Move"));
	KActionMenu *menuWindow = new KActionMenu(i18n("Window"));//, locate("appdata", "images/menu/window16.png"));
	KActionMenu *menuImage = new KActionMenu(i18n("Image"));
//	KActionMenu *menuEdit = new KActionMenu(i18n("Edit image"), "edit");

	KActionSeparator *S = new KActionSeparator;
	KAction *abstract;
	int flag;

	QValueVector<KeyE> all;

#ifndef SQ_SMALL

	all.append(KeyE(i18n("Next"), locate("appdata", "images/menu/next16.png"), "SQ Menu Next", (Qt::NoButton << 16 ) | Qt::Key_PageDown, menuFile, false));
	all.append(KeyE(i18n("Previous"), locate("appdata", "images/menu/prev16.png"), "SQ Menu Prev", (Qt::NoButton << 16 ) | Qt::Key_PageUp, menuFile, false));
	all.append(KeyE(i18n("First"), locate("appdata", "images/menu/first16.png"), "SQ Menu First", (Qt::NoButton << 16 ) | Qt::Key_Home, menuFile, false));
	all.append(KeyE(i18n("Last"), locate("appdata", "images/menu/last16.png"), "SQ Menu Last", (Qt::NoButton << 16 ) | Qt::Key_End, menuFile, true));
	all.append(KeyE(i18n("Delete"), "edittrash", "SQ Menu Delete", (Qt::NoButton << 16 ) | Qt::Key_Delete, menuFile, false));

#endif

	all.append(KeyE(i18n("Rotate left"), locate("appdata", "images/menu/rotateLeft16.png"), "SQ Menu RotL", (Qt::ControlButton << 16 ) | Qt::Key_Left, menuRotate, false));
	all.append(KeyE(i18n("Rotate right"), locate("appdata", "images/menu/rotateRight16.png"), "SQ Menu RotR", (Qt::ControlButton << 16 ) | Qt::Key_Right, menuRotate, true));
	all.append(KeyE(i18n("Rotate 180'"), locate("appdata", "images/menu/18016.png"), "SQ Menu RotL180", (Qt::ControlButton << 16 ) | Qt::Key_Up, menuRotate, true));
	all.append(KeyE(i18n("Rotate 1' left"), locate("appdata", "images/menu/116.png"), "SQ Menu RotL1", (Qt::ShiftButton << 16 ) | Qt::Key_Left, menuRotate, false));
	all.append(KeyE(i18n("Rotate 1' right"), locate("appdata", "images/menu/116.png"), "SQ Menu RotR1", (Qt::ShiftButton << 16 ) | Qt::Key_Right, menuRotate, false));

	all.append(KeyE(i18n("Zoom +"), locate("appdata", "images/menu/zoom+16.png"), "SQ Menu Zoom+", (Qt::NoButton << 16 ) | Qt::Key_Plus, menuZoom, false));
	all.append(KeyE(i18n("Zoom -"), locate("appdata", "images/menu/zoom-16.png"), "SQ Menu Zoom-", (Qt::NoButton << 16 ) | Qt::Key_Minus, menuZoom, false));
	all.append(KeyE(i18n("Zoom 2x"), locate("appdata", "images/menu/zoom216.png"), "SQ Menu Zoom2x", (Qt::ControlButton << 16 ) | Qt::Key_Plus, menuZoom, false));
	all.append(KeyE(i18n("Zoom 1/2x"), locate("appdata", "images/menu/zoom0.516.png"), "SQ Menu Zoom0.5x", (Qt::ControlButton << 16 ) | Qt::Key_Minus, menuZoom, true));
	all.append(KeyE(QString::fromLatin1("100%"), locate("appdata", "images/menu/zoom10016.png"), "SQ Menu Zoom1x", (Qt::NoButton << 16 ) | Qt::Key_1, menuZoom, false));
	all.append(KeyE(QString::fromLatin1("200%"), locate("appdata", "images/menu/zoom20016.png"), "SQ Menu Zoom2x", (Qt::NoButton << 16 ) | Qt::Key_2, menuZoom, false));
	all.append(KeyE(QString::fromLatin1("300%"), locate("appdata", "images/menu/zoom30016.png"), "SQ Menu Zoom3x", (Qt::NoButton << 16 ) | Qt::Key_3, menuZoom, false));
	all.append(KeyE(QString::fromLatin1("500%"), locate("appdata", "images/menu/zoom50016.png"), "SQ Menu Zoom5x", (Qt::NoButton << 16 ) | Qt::Key_5, menuZoom, false));
	all.append(KeyE(QString::fromLatin1("700%"), locate("appdata", "images/menu/zoom70016.png"), "SQ Menu Zoom7x", (Qt::NoButton << 16 ) | Qt::Key_7, menuZoom, false));
	all.append(KeyE(QString::fromLatin1("900%"), locate("appdata", "images/menu/zoom90016.png"), "SQ Menu Zoom9x", (Qt::NoButton << 16 ) | Qt::Key_9, menuZoom, false));
	all.append(KeyE(QString::fromLatin1("1000%"), locate("appdata", "images/menu/zoom100016.png"), "SQ Menu Zoom10x", (Qt::NoButton << 16 ) | Qt::Key_0, menuZoom, false));

	all.append(KeyE(i18n("Move left"), locate("appdata", "images/menu/moveLeft16.png"), "SQ Menu Move1", (Qt::NoButton << 16 ) | Qt::Key_Right, menuMove, false));
	all.append(KeyE(i18n("Move right"), locate("appdata", "images/menu/moveRight16.png"), "SQ Menu Move2", (Qt::NoButton << 16 ) | Qt::Key_Left, menuMove, false));
	all.append(KeyE(i18n("Move up"), locate("appdata", "images/menu/moveUp16.png"), "SQ Menu Move3", (Qt::NoButton << 16 ) | Qt::Key_Down, menuMove, false));
	all.append(KeyE(i18n("Move down"), locate("appdata", "images/menu/moveDown16.png"), "SQ Menu Move4", (Qt::NoButton << 16 ) | Qt::Key_Up, menuMove, false));

	all.append(KeyE(i18n("Start/stop animation"), locate("appdata", "images/menu/animate16.png"), "SQ Menu Image1", (Qt::NoButton << 16 ) | Qt::Key_A, menuImage, false));
	all.append(KeyE(i18n("Hide/show background"), locate("appdata", "images/menu/background16.png"), "SQ Menu Image2", (Qt::NoButton << 16 ) | Qt::Key_B, menuImage, false));
	all.append(KeyE(i18n("Hide/show tickmarks"), locate("appdata", "images/menu/tickmarks16.png"), "SQ Menu Image3", (Qt::NoButton << 16 ) | Qt::Key_K, menuImage, true));
	all.append(KeyE(i18n("Flip vertically"), locate("appdata", "images/menu/flipV16.png"), "SQ Menu Image4", (Qt::NoButton << 16 ) | Qt::Key_V, menuImage, false));
	all.append(KeyE(i18n("Flip horizontally"), locate("appdata", "images/menu/flipH16.png"), "SQ Menu Image5", (Qt::NoButton << 16 ) | Qt::Key_H, menuImage, true));
	all.append(KeyE(i18n("First page"), locate("appdata", "images/menu/page116.png"), "SQ Menu Image6", (Qt::NoButton << 16 ) | Qt::Key_F1, menuImage, false));
	all.append(KeyE(i18n("Previous page"), locate("appdata", "images/menu/page216.png"), "SQ Menu Image7", (Qt::NoButton << 16 ) | Qt::Key_F2, menuImage, false));
	all.append(KeyE(i18n("Next page"), locate("appdata", "images/menu/page316.png"), "SQ Menu Image8", (Qt::NoButton << 16 ) | Qt::Key_F3, menuImage, false));
	all.append(KeyE(i18n("Last page"), locate("appdata", "images/menu/page416.png"), "SQ Menu Image9", (Qt::NoButton << 16 ) | Qt::Key_F4, menuImage, false));

	all.append(KeyE(i18n("Fullscreen"), locate("appdata", "images/menu/fullscreen16.png"), "SQ Menu Window1", (Qt::NoButton << 16 ) | Qt::Key_F, menuWindow, false));

#ifndef SQ_SMALL

	all.append(KeyE(i18n("Quick Browser"), locate("appdata", "images/menu/quick16.png"), "SQ Menu Window2", (Qt::NoButton << 16 ) | Qt::Key_Q, menuWindow, false));

#endif

	all.append(KeyE(i18n("Hide/show toolbar"), locate("appdata", "images/menu/toolbar16.png"), "SQ Menu Window3", (Qt::NoButton << 16 ) | Qt::Key_T, menuWindow, false));
	all.append(KeyE(i18n("Hide/show statusbar"), locate("appdata", "images/menu/statusbar16.png"), "SQ Menu Window4", (Qt::NoButton << 16 ) | Qt::Key_S, menuWindow, false));
/*
	menuEdit->insert(KSquirrel::app()->pAImageConvert);
	menuEdit->insert(KSquirrel::app()->pAImageResize);
	menuEdit->insert(KSquirrel::app()->pAImageRotate);
	menuEdit->insert(KSquirrel::app()->pAImageBCG);
	menuEdit->insert(KSquirrel::app()->pAImageFilter);
	menuEdit->insert(7KSquirrel::app()->pAPrintImages);
*/
	KActionCollection *coll = new KActionCollection(this);

	QSignalMapper *mapper = new QSignalMapper(this);

	connect(mapper, SIGNAL(mapped(int)), this, SLOT(signalMapped(int)));

	QValueVector<KeyE>::iterator BEGIN = all.begin();
	QValueVector<KeyE>::iterator    END = all.end();

	for(QValueVector<KeyE>::iterator it = BEGIN;it != END;++it)
	{
		abstract = new KAction((*it).name, (*it).icon, 0, 0, 0, coll, (*it).n);
		connect(abstract, SIGNAL(activated()), mapper, SLOT(map()));
		flag = (*it).flags;
		mapper->setMapping(abstract, flag);
		((*it).sub)->insert(abstract);

		if((*it).sep)
			((*it).sub)->insert(S);
	}

#ifndef SQ_SMALL

	menuFile->plug(m);

#endif

	menuRotate->plug(m);
	menuZoom->plug(m);
	menuMove->plug(m);
	menuWindow->plug(m);
	menuImage->plug(m);
//	menuEdit->plug(m);
	S->plug(m);
	pAReset->plug(m);
	pAProperties->plug(m);
	S->plug(m);
	pAHelp->plug(m);
	S->plug(m);
	pAClose->plug(m);
}

void SQ_GLWidget::signalMapped(int id)
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

SQ_GLWidget* SQ_GLWidget::window()
{
	return sing;
}
