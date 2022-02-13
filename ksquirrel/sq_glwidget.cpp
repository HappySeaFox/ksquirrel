/***************************************************************************
                          sq_glviewwidget.cpp  -  description
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

#include <qapplication.h>
#include <qeventloop.h>
#include <qmessagebox.h>
#include <qstringlist.h>
#include <qdragobject.h>
#include <qcursor.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qpainter.h>
#include <qdatetime.h>

#include <kaction.h>
#include <kcursor.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kapp.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kstringhandler.h>
#include <kdebug.h>

#include <math.h>
#include <stdlib.h>

#include "ksquirrel.h"
#include "sq_externaltool.h"
#include "sq_libraryhandler.h"
#include "sq_config.h"
#include "sq_widgetstack.h"
#include "sq_quickbrowser.h"
#include "sq_diroperator.h"
#include "sq_fileiconview.h"
#include "sq_glview.h"
#include "sq_glwidget.h"
#include "sq_imageproperties.h"
#include "sq_helpwidget.h"
#include "sq_glu.h"
#include "err.h"

static const int timer_delay = 20;
static const int timer_delay_file = 10;
static const int len = 5;
static const float SQ_WINDOW_BACKGROUND_POS = -1000.0f;
static const float SQ_IMAGE_CHECKER_POS = -999.0f;
static const float SQ_FIRST_FRAME_POS = -998.0f;

SQ_GLWidget::SQ_GLWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
	ac = new KActionCollection(this);
	isflippedV = false;
	isflippedH = false;
	memset(matrix, 0, sizeof(matrix));
	matrix[10] = matrix[5] = matrix[0] = 1.0f;
	finfo = 0L;
	curangle = 0.0f;
	quick = 0L;
	decoded = reset_mode = false;
	next = 0L;
	changed = false;
	inMouse = false;
	current = 0;
	mem_parts = 0;
	parts = 0;
	total = 0;
	old_id = -1;
	blocked = false;
	blocked_force = false;
	zoomFactor = 1.0f;

	zoom_type = sqConfig->readNumEntry("GL view", "zoom type", 3);
	m_hiding = sqConfig->readBoolEntry("GL view", "actions", true);

	BGquads = QImage(locate("appdata", "images/checker.png"));

	if(BGquads.isNull())
	{
		BGquads = QImage(32, 32, 32);
		BGquads.setAlphaBuffer(true);
		BGquads.fill(0);
	}

	zoomfactor = sqConfig->readNumEntry("GL view", "zoom", 25);
	movefactor = sqConfig->readNumEntry("GL view", "move", 5);
	rotatefactor = sqConfig->readNumEntry("GL view", "angle", 90);

	QPixmap pusual = QPixmap::fromMimeSource(locate("appdata", "cursors/usual.png"));
	cusual = (!pusual.isNull()) ? QCursor(pusual) : KCursor::arrowCursor();

	QPixmap pdrag = QPixmap::fromMimeSource(locate("appdata", "cursors/drag.png"));
	cdrag = (!pdrag.isNull()) ? QCursor(pdrag) : cusual;

	cZoomIn = KCursor::crossCursor();

	setCursor(cusual);

	setFocusPolicy(QWidget::WheelFocus);
	setAcceptDrops(true);

	createQuickBrowser();
	createActions();
	createToolbar();

	KCursor::setAutoHideCursor(this, true);
	KCursor::setHideCursorDelay(2500);

	timer_prev = new QTimer(this);
	timer_next = new QTimer(this);
	timer_decode = new QTimer(this);
	timer_anim = new QTimer(this);

	Q_CHECK_PTR(timer_prev);
	Q_CHECK_PTR(timer_next);
	Q_CHECK_PTR(timer_decode);
	Q_CHECK_PTR(timer_anim);

	connect(timer_prev, SIGNAL(timeout()), sqWStack, SLOT(emitPreviousSelected()));
	connect(timer_next, SIGNAL(timeout()), sqWStack, SLOT(emitNextSelected()));
	connect(timer_decode, SIGNAL(timeout()), this, SLOT(slotDecode()));
	connect(timer_anim, SIGNAL(timeout()), this, SLOT(slotAnimateNext()));

	matrixChanged();

	images = new KPopupMenu(this);
	images->setCheckable(true);

	connect(images, SIGNAL(activated(int)), this, SLOT(slotSetCurrentImage(int)));
	connect(images, SIGNAL(aboutToHide()), this, SLOT(slotImagedHidden()));
}

SQ_GLWidget::~SQ_GLWidget()
{}

void SQ_GLWidget::createActions()
{
	pARotateLeft = new KAction(QString::null, 0, this, SLOT(slotRotateLeft()), ac, "SQ Rotate picture left");
	pARotateRight = new KAction(QString::null, 0, this, SLOT(slotRotateRight()), ac, "SQ Rotate picture right");
	pAZoomPlus = new KAction(QString::null, 0, this, SLOT(slotZoomPlus()), ac, "SQ Zoom+");
	pAZoomMinus = new KAction(QString::null, 0, this, SLOT(slotZoomMinus()), ac, "SQ Zoom-");

	pAZoomW = new KToggleAction(i18n("Fit width"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoomW.png")), 0, this, SLOT(slotZoomW()), ac, "SQ ZoomW");
	pAZoomH = new KToggleAction(i18n("Fit height"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoomH.png")), 0, this, SLOT(slotZoomH()), ac, "SQ ZoomH");
	pAZoomWH = new KToggleAction(i18n("Fit image"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoomWH.png")), 0, this, SLOT(slotZoomWH()), ac, "SQ ZoomWH");
	pAZoom100 = new KToggleAction(i18n("Zoom 100%"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoom100.png")), 0, this, SLOT(slotZoom100()), ac, "SQ Zoom100");
	pAZoomLast = new KToggleAction(i18n("Leave previous zoom"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoomlast.png")), 0, this, SLOT(slotZoomLast()), ac, "SQ ZoomLast");
	pAIfLess = new KToggleAction(i18n("Ignore, if the image is less than window"), QPixmap::fromMimeSource(locate("appdata", "images/actions/ifless.png")), 0, 0, 0, ac, "SQ GL If Less");

	pAFlipV = new KAction(QString::null, 0, this, SLOT(slotFlipV()), ac, "SQ flip_v");
	pAFlipH = new KAction(QString::null, 0, this, SLOT(slotFlipH()), ac, "SQ flip_h");
	pAReset = new KAction(QString::null, 0, this, SLOT(slotMatrixReset()), ac, "SQ ResetGL");
	pAClose = new KAction(QString::null, 0, sqApp, SLOT(slotCloseGLWidget()), ac, "SQ GL close");
	pAProperties = new KAction(QString::null, 0, this, SLOT(slotProperties()), ac, "SQ GL Prop");
	pAFull = new KToggleAction(QString::null, 0, 0, 0, ac, "SQ GL Full");
	pAQuick = new KToggleAction(QString::null, 0, 0, 0, ac, "SQ GL Quick");
	pANext = new KAction(QString::null, 0, this, SLOT(slotNext()), ac, "SQ GL Next file");
	pAPrev = new KAction(QString::null, 0, this, SLOT(slotPrev()), ac, "SQ GL Prev file");
	pAHide = new KAction(QString::null, 0, this, SLOT(slotHideToolbar()), ac, "SQ GL THide");
	pAShow = new KAction(i18n("Show"), QPixmap::fromMimeSource(locate("appdata", "images/actions/toolbar_show.png")), 0, this, SLOT(slotShowToolbar()), ac, "SQ GL TShow");
	pAFirst = new KAction(QString::null, 0, this, SLOT(slotFirst()), ac, "SQ GL File First");
	pALast = new KAction(QString::null, 0, this, SLOT(slotLast()), ac, "SQ GL File Last");
	pAHideToolbars = new KToggleAction(QString::null, QPixmap(), 0, 0, 0, ac, "SQ GL Quick");
	pAStatus = new KToggleAction(QString::null, QPixmap(), 0, 0, 0, ac, "SQ GL Toggle Status");

	pAZoomW->setExclusiveGroup("squirrel_zoom_actions");
	pAZoomH->setExclusiveGroup("squirrel_zoom_actions");
	pAZoomWH->setExclusiveGroup("squirrel_zoom_actions");
	pAZoom100->setExclusiveGroup("squirrel_zoom_actions");
	pAZoomLast->setExclusiveGroup("squirrel_zoom_actions");

	connect(pAFull, SIGNAL(toggled(bool)), sqApp, SLOT(slotFullScreen(bool)));
	connect(pAIfLess, SIGNAL(toggled(bool)), this, SLOT(slotZoomIfLess(bool)));
	connect(pAQuick, SIGNAL(toggled(bool)), this, SLOT(slotShowQuick(bool)));
	connect(pAHideToolbars, SIGNAL(toggled(bool)), this, SLOT(slotHideToolbars(bool)));
	connect(pAStatus, SIGNAL(toggled(bool)), this, SLOT(slotToggleStatus(bool)));

	pAStatus->setChecked(sqConfig->readBoolEntry("GL view", "statusbar", true));
	pAIfLess->setChecked(sqConfig->readBoolEntry("GL view", "ignore", false));
	pAHideToolbars->setChecked(sqConfig->readBoolEntry("GL view", "toolbars_hidden", false));
}

void SQ_GLWidget::createToolbar()
{
	KActionSeparator *pASep = new KActionSeparator;
	zoom = new KPopupMenu;
	QToolButton	*pATool;

	QPalette pall(QColor(255,255,255), QColor(255,255,255));

	toolbar2 = new KToolBar(this);
	toolbar2->setGeometry(0, 0, 35, 34);
	toolbar2->setIconSize(22);
	toolbar2->setPalette(pall);
	pAShow->plug(toolbar2);
 
	toolbar = new KToolBar(this);
	toolbar->setGeometry(0, 0, 525, 34);
	toolbar->setIconSize(22);
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
	pASep->plug(zoom);
	pAZoomW->plug(zoom);
	pAZoomH->plug(zoom);
	pAZoomWH->plug(zoom);
	pAZoomLast->plug(zoom);
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

	(void)new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/file_first.png")), i18n("Go to first file"), QString::null, pAFirst, SLOT(activate()), toolbar);
	pATool = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/prev.png")), i18n("Previous file"), QString::null, pAPrev, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/next.png")), i18n("Next file"), QString::null, pANext, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	(void)new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/file_last.png")), i18n("Go to last file"), QString::null, pALast, SLOT(activate()), toolbar);
	pATool = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/zoom+.png")), i18n("Zoom +"), QString::null, pAZoomPlus, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/zoom-.png")), i18n("Zoom -"), QString::null, pAZoomMinus, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pAToolZoom = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/zoom_template.png")), i18n("Zoom"), QString::null, this, SLOT(slotZoomMenu()), toolbar);
	pATool = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/rotateL.png")), i18n("Rotate left"), QString::null, pARotateLeft, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/rotateR.png")), i18n("Rotate right"), QString::null, pARotateRight, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/flipV.png")), i18n("Flip vertically"), QString::null, pAFlipV, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	pATool = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/flipH.png")), i18n("Flip horizontally"), QString::null, pAFlipH, SLOT(activate()), toolbar);
	pATool->setAutoRepeat(true);
	(void)new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/reload.png")), i18n("Normalize"), QString::null, pAReset, SLOT(activate()), toolbar);
	(void)new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/prop.png")), i18n("Image Properties"), QString::null, pAProperties, SLOT(activate()), toolbar);
	pAToolFull = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/fullscreen.png")), i18n("Fullscreen"), QString::null, pAFull, SLOT(activate()), toolbar);
	pAToolFull->setToggleButton(true);
	pAToolQuick = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/quick.png")), i18n("\"Quick Browser\""), QString::null, pAQuick, SLOT(activate()), toolbar);
	pAToolQuick->setToggleButton(true);
	pAToolImages = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/images.png")), i18n("Select image"), QString::null, this, SLOT(slotShowImages()), toolbar);
	pAToolClose = new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/close.png")), i18n("Close"), QString::null, pAClose, SLOT(activate()), toolbar);
	(void)new QToolButton(QPixmap::fromMimeSource(locate("appdata", "images/actions/toolbar_hide.png")), i18n("Hide"), QString::null, pAHide, SLOT(activate()), toolbar);

	timer_show = new QTimer(this);
	timer_hide = new QTimer(this);

	connect(timer_show, SIGNAL(timeout()), SLOT(slotStepToolbarMove()));
	connect(timer_hide, SIGNAL(timeout()), SLOT(slotStepToolbarMove()));
}

void SQ_GLWidget::initializeGL()
{
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SQ_GLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-width/2, width/2, -height/2, height/2, 0.1f, 10000.0f);
	SQ_GLU::gluLookAt(0,0,1, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);
}

void SQ_GLWidget::draw_background(void *bits, unsigned int *tex, int dim, GLfloat w, GLfloat h, bool &bind, bool deleteOld)
{
	float half_w, half_h;

	half_w = w / 2.0;
	half_h = h / 2.0;

	if(bind)
	{
		kdDebug() << "Background changed. Binding texture ..." << endl;

		if(deleteOld)
			glDeleteTextures(1, tex);

		glGenTextures(1, tex);
		glBindTexture(GL_TEXTURE_2D, *tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

		bind = false;
	}
	else
		glBindTexture(GL_TEXTURE_2D, *tex);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);							glVertex2f(-half_w, half_h);
		glTexCoord2f(w/(GLfloat)dim, 0.0);				glVertex2f(half_w, half_h);
		glTexCoord2f(w/(GLfloat)dim, h/(GLfloat)dim);	glVertex2f(half_w, -half_h);
		glTexCoord2f(0.0, h/(GLfloat)dim);				glVertex2f(-half_w, -half_h);
	glEnd();
}

void SQ_GLWidget::paintGL()
{
	int z;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	if(sqConfig->readNumEntry("GL view", "GL view background type", 0) == 2)
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

	if(!reset_mode && decoded)
	{
		if(finfo->image[current].hasalpha && sqConfig->readBoolEntry("GL view", "alpha_bkgr", true))
		{
			static bool t = true;

			static GLdouble eq[4][4] =
			{
				{0.0, 1.0, 0.0, 0.0},
				{1.0, 0.0, 0.0, 0.0},
				{0.0, -1.0, 0.0, 0.0},
				{-1.0, 0.0, 0.0, 0.0}
			};

			glPushMatrix();
			glTranslatef(-finfo->image[current].w/2, -finfo->image[current].h/2, 0.0);

			glClipPlane(GL_CLIP_PLANE0, eq[0]);
			glClipPlane(GL_CLIP_PLANE1, eq[1]);
			glEnable(GL_CLIP_PLANE0);
			glEnable(GL_CLIP_PLANE1);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(finfo->image[current].w/2, finfo->image[current].h/2, 0.0);
			glClipPlane(GL_CLIP_PLANE2, eq[2]);
			glClipPlane(GL_CLIP_PLANE3, eq[3]);
			glEnable(GL_CLIP_PLANE2);
			glEnable(GL_CLIP_PLANE3);
			glPopMatrix();

			matrix_push();
			matrix_pure_reset();
			matrix_move_z(SQ_IMAGE_CHECKER_POS);
			draw_background(BGquads.bits(), &texQuads, 32, width(), height(), t, !t);
			matrix_pop();
			write_gl_matrix();

			glDisable(GL_CLIP_PLANE3);
			glDisable(GL_CLIP_PLANE2);
			glDisable(GL_CLIP_PLANE1);
			glDisable(GL_CLIP_PLANE0);
		}

		matrix_move_z(SQ_FIRST_FRAME_POS);

		for(z = 0;z < parts[current].tilesy;z++)
			glCallList(parts[current].m_parts[z * parts[current].tilesx].list);
	}

	glDisable(GL_TEXTURE_2D);
}

void SQ_GLWidget::coordChanged()
{
	QString str;

	str = QString::fromLatin1("%1,%2")
			.arg(MATRIX_X, 0, 'f', 0)
			.arg(MATRIX_Y, 0, 'f', 0);
	sqSBGLCoord->setText(str);
}

void SQ_GLWidget::matrixChanged()
{
	QString str;

	float m = get_zoom();
	float zoom = m * 100.0f;
	float z = (m < 1.0f) ? 1.0f/m : m;

	str = QString::fromLatin1("%1% [%2:%3]")
			.arg(zoom, 0, 'f', 1)
			.arg((m < 1.0f)?1.0f:z, 0, 'f', 1)
			.arg((m > 1.0f)?1.0f:z, 0, 'f', 1);
	sqSBGLZoom->setText(str);

	str = QString::fromLatin1("%1%2 %3 deg")
			.arg((isflippedV)?"V":"")
			.arg((isflippedH)?"H":"")
			.arg(get_angle(), 0, 'f', 1);
	sqSBGLAngle->setText(str);
}

void SQ_GLWidget::setZoomFactor(const GLfloat &newfactor)
{
	zoomfactor = newfactor;
}

void SQ_GLWidget::setMoveFactor(const GLfloat &newfactor)
{
	movefactor = newfactor;
}

void SQ_GLWidget::setRotateFactor(const GLfloat &newfactor)
{
	rotatefactor = newfactor;
}

void SQ_GLWidget::wheelEvent(QWheelEvent *e)
{
	if(e->delta() < 0 && e->state() == Qt::NoButton)
	{
		if(!sqConfig->readNumEntry("GL view", "scroll", 1))
			pAZoomPlus->activate();
		else
			sqWStack->emitNextSelected();
	}
	else if(e->delta() > 0 && e->state() == Qt::NoButton)
	{
		if(!sqConfig->readNumEntry("GL view", "scroll", 1))
			pAZoomMinus->activate();
		else
			sqWStack->emitPreviousSelected();
	}
	else if(e->delta() < 0 && e->state() == Qt::ControlButton)
		matrix_zoom(2.0f);
	else if(e->delta() > 0 && e->state() == Qt::ControlButton)
		matrix_zoom(0.5f);
	else if(e->delta() < 0 && e->state() == Qt::ShiftButton)
		slotZoomPlus();
	else if(e->delta() > 0 && e->state() == Qt::ShiftButton)
		slotZoomMinus();
	else
		e->accept();
}

void SQ_GLWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == Qt::LeftButton)
	{
		setCursor(cdrag);

		xmoveold = e->x();
		ymoveold = e->y();
		inMouse = true;
	}
	else if(e->button() == Qt::RightButton)
	{
		stopAnimation();
		setCursor(cZoomIn);
		pRect = new QPainter(this);
		pRect->setPen(QPen(white, 2));
		pRect->setRasterOp(Qt::XorROP);
		lastRect = QRect();
		xmoveold = e->x();
		ymoveold = e->y();
		inMouse = true;
		crossDrawn = false;
	}
	else
	    e->accept();
}

void SQ_GLWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(!inMouse)
	{
		e->accept();
		return;
	}

	if(e->state() == Qt::LeftButton)
	{
		xmove = e->x();
		ymove = e->y();

		matrix_move(xmove-xmoveold, -ymove+ymoveold);

		xmoveold = e->x();
		ymoveold = e->y();
	}
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

void SQ_GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if(e->state() == Qt::LeftButton)
		setCursor(cusual);
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
			bool zoomed = zoomRect(lastRect);

			if(!zoomed)
			{
				MATRIX_X = X;
				MATRIX_Y = Y;
				write_gl_matrix();
			}
		}

		updateGL();

		if(!manualBlocked())
			startAnimation();
	}

	inMouse = false;
	e->accept();
}

bool SQ_GLWidget::zoomRect(const QRect &r)
{
	float factor = 1.0;
	float w = (float)width(), h = (float)height();
	float factorw = w / (float)r.width();
	float factorh = h / (float)r.height();
	float t = w / h;

	if(t > (float)r.width() / (float)r.height())
		factor = factorh;
	else
		factor = factorw;

	return matrix_zoom(factor);
}

void SQ_GLWidget::keyPressEvent(QKeyEvent *e)
{
	unsigned long value = e->key();
	kdDebug() << "keyPressEvent, key = " << value << endl;
	const unsigned short Key_KC = Qt::ControlButton|Qt::Keypad;

	value <<= 16;
	value |= e->state();

#define SQ_KEYSTATES(a,b) ((a << 16) | b)

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
		case SQ_KEYSTATES(Qt::Key_Plus, Qt::NoButton):			pAZoomPlus->activate();			break;
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::NoButton):		pAZoomMinus->activate();		break;
		case SQ_KEYSTATES(Qt::Key_Plus, Key_KC):
		case SQ_KEYSTATES(Qt::Key_Plus, Qt::ControlButton):		matrix_zoom(2.0f);				break;
		case SQ_KEYSTATES(Qt::Key_Minus, Key_KC):
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::ControlButton):	matrix_zoom(0.5f);				break;
		case SQ_KEYSTATES(Qt::Key_V, Qt::NoButton):				pAFlipV->activate();								break;
		case SQ_KEYSTATES(Qt::Key_H, Qt::NoButton):			pAFlipH->activate();								break;
		case SQ_KEYSTATES(Qt::Key_Left, Qt::ControlButton):		pARotateLeft->activate();							break;
		case SQ_KEYSTATES(Qt::Key_Right, Qt::ControlButton):	pARotateRight->activate();						break;
		case SQ_KEYSTATES(Qt::Key_R, Qt::NoButton):				pAReset->activate();								break;
		case SQ_KEYSTATES(Qt::Key_Up, Qt::ControlButton):		matrix_rotate(180.0f);								break;
		case SQ_KEYSTATES(Qt::Key_Down, Qt::ControlButton):	matrix_rotate(-180.0f);							break;
		case SQ_KEYSTATES(Qt::Key_Left, Qt::ShiftButton):		matrix_rotate(-1.0f);								break;
		case SQ_KEYSTATES(Qt::Key_Right, Qt::ShiftButton):		matrix_rotate(1.0f);								break;

		case SQ_KEYSTATES(Qt::Key_PageDown, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Space, Qt::NoButton):		pANext->activate();								break;
		case SQ_KEYSTATES(Qt::Key_PageUp, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_BackSpace, Qt::NoButton):	pAPrev->activate();								break;
		case SQ_KEYSTATES(Qt::Key_X, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Escape, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Return, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Enter, Qt::Keypad):		pAToolClose->animateClick();							break;
		case SQ_KEYSTATES(Qt::Key_P, Qt::NoButton):			pAProperties->activate();								break;
		case SQ_KEYSTATES(Qt::Key_Home, Qt::NoButton):	pAFirst->activate();									break;
		case SQ_KEYSTATES(Qt::Key_End, Qt::NoButton):		pALast->activate();									break;
		case SQ_KEYSTATES(Qt::Key_Q, Qt::NoButton):		pAToolQuick->animateClick();						break;
		case SQ_KEYSTATES(Qt::Key_F, Qt::NoButton):			pAToolFull->animateClick();							break;
		case SQ_KEYSTATES(Qt::Key_T, Qt::NoButton):			pAHideToolbars->activate();							break;
		case SQ_KEYSTATES(Qt::Key_BracketLeft, Qt::NoButton):			pAHideToolbars->setChecked(false);pAHide->activate();	break;
		case SQ_KEYSTATES(Qt::Key_BracketRight, Qt::NoButton):			pAHideToolbars->setChecked(false);pAShow->activate();break;
		case SQ_KEYSTATES(Qt::Key_Z, Qt::NoButton):			slotZoomMenu();											break;
		case SQ_KEYSTATES(Qt::Key_S, Qt::NoButton):			pAStatus->activate();										break;
		case SQ_KEYSTATES(Qt::Key_A, Qt::NoButton):			slotToggleAnimate();										break;
		case SQ_KEYSTATES(Qt::Key_I, Qt::NoButton):			slotShowImages();										break;
		case SQ_KEYSTATES(Qt::Key_F1, Qt::NoButton):		jumpToImage(false);										break;
		case SQ_KEYSTATES(Qt::Key_F2, Qt::NoButton):		prevImage();												break;
		case SQ_KEYSTATES(Qt::Key_F3, Qt::NoButton):		nextImage();												break;
		case SQ_KEYSTATES(Qt::Key_F4, Qt::NoButton):		jumpToImage(true);										break;
		case SQ_KEYSTATES(Qt::Key_Slash, Qt::NoButton):	showHelp();												break;
		case SQ_KEYSTATES(Qt::Key_B, Qt::NoButton):			toggleDrawingBackground();								break;
		case SQ_KEYSTATES(Qt::Key_E, Qt::NoButton):			showExternalTools();								break;

		default:
		{
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
				{
					int val = e->key();
					val = val - Qt::Key_1 + 1;
					matrix_pure_reset_notflip();
					matrix_zoom((float)val);
				}
				break;

				case Qt::Key_0:
				{
					matrix_pure_reset_notflip();
					matrix_zoom(10.0f);
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

void SQ_GLWidget::dropEvent(QDropEvent *e)
{
	QStringList files;

	if(QUriDrag::decodeLocalFiles(e, files))
	{
		QStringList::Iterator i = files.begin();

		slotStartDecoding(*i);
	}
}

void SQ_GLWidget::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept(QUriDrag::canDecode(e));
}

void SQ_GLWidget::contextMenuEvent(QContextMenuEvent*)
{
//	menu->exec(QCursor::pos());
}

void SQ_GLWidget::slotZoomW()
{
	zoom_type = 0;
	pAZoomW->setChecked(true);

	if(!finfo)
		return;
	
	float factor = (float)width() / (float)finfo->image[current].w;

	if(pAIfLess->isChecked() && (finfo->image[current].w < width() && finfo->image[current].h < height()))
		factor = 1.0f;

	matrix_pure_reset();
	matrix_zoom(factor);
}

void SQ_GLWidget::slotZoomH()
{
	zoom_type = 1;
	pAZoomH->setChecked(true);

	if(!finfo)
		return;

	float factor = (float)height() / (float)finfo->image[current].h;

	if(pAIfLess->isChecked() && (finfo->image[current].w < width() && finfo->image[current].h < height()))
		factor = 1.0f;

	matrix_pure_reset();
	matrix_zoom(factor);
}

void SQ_GLWidget::slotZoomWH()
{
	zoom_type = 2;
	pAZoomWH->setChecked(true);

	if(!finfo)
		return;

	float factor = 1.0;
	float w = (float)width(), h = (float)height();
	float factorw = w / (float)finfo->image[current].w;
	float factorh = h / (float)finfo->image[current].h;
	float t = w / h;

	if(t > (float)finfo->image[current].w / (float)finfo->image[current].h)
		factor = factorh;
	else
		factor = factorw;

	if(pAIfLess->isChecked() && (finfo->image[current].w < width() && finfo->image[current].h < height()))
		factor = 1.0f;

	matrix_pure_reset();
	matrix_zoom(factor);
}

void SQ_GLWidget::slotZoomLast()
{
	zoom_type = 4;
	pAZoomLast->setChecked(true);

	if(!finfo)
		return;

	matrix_pure_reset();
	matrix_zoom(zoomFactor);

	if(!reset_mode)
		updateGL();
}

void SQ_GLWidget::slotZoom100()
{
	zoom_type = 3;
	pAZoom100->setChecked(true);

	matrix_pure_reset();
	matrix_zoom(1.0);
}

void SQ_GLWidget::slotZoomIfLess(bool)
{
	if(!finfo)
		return;

	switch(zoom_type)
	{
		case 0: pAZoomW->activate(); break;
		case 1: pAZoomH->activate(); break;
		case 2: pAZoomWH->activate(); break;
		case 3: break;

		default:
			matrix_pure_reset();
			matrix_zoom(zoomFactor);
	}

	if(!reset_mode)
		updateGL();
}

void SQ_GLWidget::slotZoomPlus()
{
	matrix_zoom(1.0f+zoomfactor/100.0f);
}

void SQ_GLWidget::slotZoomMinus()
{
	matrix_zoom(1.0f/(1.0f+zoomfactor/100.0f));
}

void SQ_GLWidget::slotRotateLeft()
{
	matrix_rotate(-rotatefactor);
}

void SQ_GLWidget::slotRotateRight()
{
	matrix_rotate(rotatefactor);
}

void SQ_GLWidget::slotProperties()
{
	if(!finfo) return;

	stopAnimation();

	QStringList list = QStringList::split('\n', finfo->image[current].dump);
	QString s = list.last();
	list.pop_back();

	const unsigned long real_size = s.toInt();

	s = KIO::convertSize(real_size);

	QValueVector<QPair<QString,QString> > meta;
	fmt_meta_entry *entry;

	if(finfo->image[current].meta)
	{
		for(int i = 0;i < finfo->image[current].meta->entries;i++)
		{
			entry = &(finfo->image[current].meta->m[i]);

			if(entry)
			{
				meta.append(QPair<QString,QString>(QString::fromLatin1(entry->group), QString::fromLatin1(entry->data, entry->datalen)));
			}
		}
	}

	list << s << QString::fromLatin1("%1").arg((double)real_size / fm.size())
	     << ((finfo->interlaced) ? i18n("yes") : i18n("no"))
	     << QString::fromLatin1("%1").arg(errors)
	     << QString::fromLatin1("%1").arg(finfo->images) << QString::fromLatin1("#%1").arg(current);

	SQ_ImageProperties *prop = new SQ_ImageProperties(this);
	prop->setFile(File);
	prop->setParams(list);
	prop->setMetaInfo(meta);
	prop->exec();

	if(!manualBlocked())
		startAnimation();
}

/*
	 operations with matrices are taken from GLiv =)

	thanks to Guillaume Chazarian.
*/
void SQ_GLWidget::flip(int id)
{
	GLfloat	x = 0.0, y = 0.0;

	x = MATRIX_X, y = MATRIX_Y;
	MATRIX_X = MATRIX_Y = 0;
	write_gl_matrix();

	matrix[id] *= -1.0;
	matrix[id + 1] *= -1.0;
	matrix[id + 3] *= -1.0;

	MATRIX_X = x;
	MATRIX_Y = y;

	write_gl_matrix();

	if(!reset_mode)
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

	matrixChanged();
	coordChanged();
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

	if(finfo)
		if(finfo->image[current].needflip)
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
		if(finfo->image[current].needflip)
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

void SQ_GLWidget::matrix_pure_reset_notflip()
{
	int i;
	int id;

	if(isflippedH)
		id = 0;
	else if(isflippedV)
		id = 4;
	else
		id = -1;

	for (i = 0; i < 12; i++)
		matrix[i] = (GLfloat) (i % 5 == 0);

	bool old = reset_mode;
	reset_mode = true;
	if(id != -1)
		flip(id);
	else
		write_gl_matrix();
	reset_mode = old;
}

bool SQ_GLWidget::matrix_zoom(GLfloat ratio)
{
	int zoom_lim = sqConfig->readNumEntry("GL view", "zoom limit", 0);
	float zoom_min, zoom_max, zoom_tobe;

	zoom_tobe = hypotf(MATRIX_C1 * ratio, MATRIX_S1 * ratio) * 100.0f;

	switch(zoom_lim)
	{
		case 0:
		break;

		case 2:
			zoom_min = (float)sqConfig->readNumEntry("GL view", "zoom_min", 1);
			zoom_max = (float)sqConfig->readNumEntry("GL view", "zoom_max", 10000);
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

	rad = angle * rad_const;
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

void SQ_GLWidget::setClearColor()
{
	QColor color;
	QString path;

	switch(sqConfig->readNumEntry("GL view", "GL view background type", 0))
	{
		case 0:
			color = colorGroup().color(QColorGroup::Base);
		break;

		case 1:
			color.setNamedColor(sqConfig->readEntry("GL view", "GL view background", "#cccccc"));
		break;

		case 2:
			path = sqConfig->readEntry("GL view", "GL view custom texture", "");
			BGpixmap.load(path);

			if(BGpixmap.isNull())
			{
				qWarning("Texture is corrupted! Falling back to 'System color'\n");
				sqConfig->setGroup("GL view");
				sqConfig->writeEntry("GL view background type", 0);
				setClearColor();
				return;
			}

			BGpixmap.convertDepth(32);
			BGpixmap = QGLWidget::convertToGLFormat(BGpixmap);

			changed = true;
		break;

		default: ;
	}

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

	if(fw/fh > 2.0f || fh/fw > 2.0f)
		tile_factor = true;

	static int S[] = { 32, 64, 128, 256 };

//	printf("Testing num = %d\n", num);

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

//	printf("tileSize = %d\n", tileSize);

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

void SQ_GLWidget::createQuickBrowser()
{
	v = new SQ_QuickBrowser(this);
}

bool SQ_GLWidget::prepare()
{
	const char 		*name = File.ascii();
	QString		status;
	unsigned int	i;

	fm.setFile(File);

	lib = sqLibHandler->libraryForFile(m_File);

	if(!lib)
	{
		QMessageBox::warning(sqApp, i18n("Decoding"), i18n(QString("Library for %1 format not found")).arg(fm.extension(false)), QMessageBox::Ok, QMessageBox::NoButton);
		return false;
	}

	if(finfo && finfo->image)
	{
		for(int f = 0;f < finfo->images;f++)
		{
			if(finfo->image[f].dump)
				free(finfo->image[f].dump);

			if(finfo->image[f].meta)
			{
				if(finfo->image[f].meta->m)
					for(int h = 0;h < finfo->image[f].meta->entries;h++)
					{
						if(finfo->image[f].meta->m[h].data)
							free(finfo->image[f].meta->m[h].data);
					}
				free(finfo->image[f].meta->m);
				free(finfo->image[f].meta);
			}
		}

		free(finfo->image);
		memset(finfo, 0, sizeof(fmt_info));
		finfo->image = (fmt_image *)calloc(1, sizeof(fmt_image));
	}
	else
	{
		finfo = (fmt_info *)calloc(1, sizeof(fmt_info));
		finfo->image = (fmt_image *)calloc(1, sizeof(fmt_image));
	}

	i = lib->fmt_init(finfo, name);

	if(i != SQERR_OK)
	{
		status = QString::fromLatin1("fmt_read_info: ");

		switch(i)
		{
			case SQERR_BADFILE: status = status + i18n("File corrupted !\n\nLibrary: ") + lib->libpath + i18n("\nHandled extensions: ") + lib->filter; break;
			case SQERR_NOMEMORY: status = status + i18n("Library couldn't alloc memory for internal usage !"); break;
			case SQERR_NOTSUPPORTED: status = status + i18n("This format type is not supported."); break;

			default: status = status + i18n("Library returned unknown error code.");
		}

		QMessageBox::warning(sqApp, i18n("Decoding"), status, QMessageBox::Ok, QMessageBox::NoButton);
		return false;
	}

	if(parts)
	{
		for(int z = 0;z < total;z++)
			parts[z].removeParts();

		free(parts);
	}

	if(mem_parts)
		free(mem_parts);

	current = 0;
	parts = 0;
	mem_parts = 0;

/*
	status.sprintf("%dx%d@%d", finfo->image[current].w, finfo->image[current].h, finfo->image[current].bpp);
	sqSBDecoded->setText(status);
	sqSBDecodedI->clear();
	sqSBDecodedI->setPixmap(QPixmap(lib->mime));
*/
	return true;
}

void SQ_GLWidget::showFrames(int i)
{
	int z;
	const int a = parts[current].tilesx * i, b = parts[current].tilesx * (i+1);

	makeCurrent();

	glEnable(GL_TEXTURE_2D);

	for(z = a;z < b;z++)
	{
		glBindTexture(GL_TEXTURE_2D, parts[current].m_parts[z].tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// ??
//		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ARB, SQ_DIV, SQ_DIV, 0, GL_RGBA, GL_UNSIGNED_BYTE, mem_parts[z].part);

		switch(tileSize)
		{
			case 32:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tileSize, tileSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, m32[z].part);
			break;
			case 64:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tileSize, tileSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, m64[z].part);
			break;
			case 128:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tileSize, tileSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, m128[z].part);
			break;
			case 256:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tileSize, tileSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, m256[z].part);
			break;
		}
	}

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
	swapBuffers();
}

bool SQ_GLWidget::actions() const
{
	return m_hiding;
}

int SQ_GLWidget::zoomType()
{
	return zoom_type;
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

void SQ_GLWidget::setupBits(Parts *p, RGBA *b, int y)
{
	int index = y * p->tilesx;
	int rw2 = p->rw;
	int f = tileSize * sizeof(RGBA);
	unsigned char *vv;
//	const int S = tileSize * tileSize * sizeof(RGBA);

	for(int x = 0;x < p->tilesx;x++)
	{
		switch(tileSize)
		{
			case 32:
				vv = m32[index].part;
			break;
			case 64:
				vv = m64[index].part;
			break;
			case 128:
				vv = m128[index].part;
			break;
			case 256:
				vv = m256[index].part;
			break;
		}

//		memset(/*mem_parts[index].part*/vv, 0, S);

		for(int j = 0,k = 0;j < tileSize;j++,k++)
			memcpy(/*mem_parts[index].part*/vv + k*f, b + x*tileSize + k*rw2, f);

		index++;
	}
}

void Parts::removeParts()
{
	if(m_parts)
	{
		for(int z = 0;z < tiles;z++)
			glDeleteTextures(1, &m_parts[z].tex);

		glDeleteLists(m_parts[0].list, tilesy);

		free(m_parts);
	}
}

bool Parts::makeParts()
{
     int z;

	tiles = tilesx * tilesy;

	m_parts = /*new Part[(const int)tiles];*/(Part *)calloc(tiles, sizeof(Part));

	if(!m_parts)
		return false;

	for(z = 0;z < tiles;z++)
		glGenTextures(1, &m_parts[z].tex);

	GLuint base = glGenLists(tilesy);

	if(!base)
	{
		for(z = 0;z < tiles;z++)
			glDeleteTextures(1, &m_parts[z].tex);

		return false;
	}

	for(z = 0;z < tilesy;z++)
		m_parts[z * tilesx].list = base + z;

	return true;
}

void Parts::setTileSize(const int t)
{
	tileSize = t;
}

void Parts::computeCoords()
{
	int index = 0;
	int X, Y;

	Y = h / 2;

	for(int y = 0;y < tilesy;y++)
	{
	    X = -w / 2;

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

void SQ_GLWidget::slotShowQuick(bool b)
{
	v->setShown(b);

	if(b)
	{
		KFileItem *f = sqQuickOperator->iv->currentFileItem();

		if(f)
			sqQuickOperator->setCurrentItem(f);
	}
}

void SQ_GLWidget::slotStartDecoding(const KURL &url)
{
	slotStartDecoding(url.path());
}

void SQ_GLWidget::slotStartDecoding(const QString &file, bool isMouseEvent)
{
	if(reset_mode)
		return;

	reset_mode = true;

	m_File = file;
	File = QFile::encodeName(m_File);

	if(!prepare())
	{
		reset_mode = false;
		return;
	}

	sqApp->raiseGLWidget();
	sqApp->setCaption(file);

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
	Parts *t;

	QFileInfo ff(m_File);

	timer_anim->stop();
	images->clear();
//	images->insertTitle(KStringHandler::rsqueeze(ff.fileName(), 20));

	QTime started;
	started.start();

	current = 0;

	while(true)
	{
		finfo->image = (fmt_image *)realloc(finfo->image, sizeof(fmt_image) * (finfo->images+1));
		memset(&finfo->image[current], 0, sizeof(fmt_image));

		if(lib->fmt_next(finfo) != SQERR_OK)
		{
//			QMessageBox::warning(sqApp, i18n("Decoding"), i18n("fmt_next() failed"), QMessageBox::Ok, QMessageBox::NoButton);
			break;
		}

		t = (Parts *)realloc(parts, sizeof(Parts) * (current+1));

		if(!t)
		{
			current = 0;
			total = finfo->images - 1;
//			lib->fmt_close();
			QMessageBox::warning(this, i18n("Decoding"), i18n("Memory allocation failed"), QMessageBox::Ok, QMessageBox::NoButton);
//			goto closs;
		}

		parts = t;

		memset(&parts[current], 0, sizeof(Parts));

		findCloserTiles(finfo->image[current].w, finfo->image[current].h, parts[current].tilesx, parts[current].tilesy);
		realW = parts[current].tilesx * tileSize;
		realH = parts[current].tilesy * tileSize;

		printf("tilesx: %d, tilesy: %d\n", parts[current].tilesx, parts[current].tilesy);

		parts[current].setRWH(realW, realH);
		parts[current].setWH(finfo->image[current].w, finfo->image[current].h);
		parts[current].setTileSize(tileSize);

		if(!parts[current].makeParts())
		{
			QString status = i18n("Oops! Memory realloc failed.\n\nI tried to realloc memory for image:\n\nwidth: %1\nheight: %2\nbpp: %3\nTotal needed: ")
					.arg(finfo->image[current].w)
					.arg(finfo->image[current].h)
					.arg(finfo->image[current].bpp)
					+ KIO::convertSize(realW * realH * sizeof(RGBA)) + i18n(" of memory.");

			QMessageBox::warning(sqApp, i18n("Decoding"), status, QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}

		if(mem_parts)
			free(mem_parts);

		switch(tileSize)
		{
			case 32:
				m32 = (MemoryPart32 *)calloc(parts[current].tiles, sizeof(MemoryPart32));
				mem_parts = m32;
			break;
			case 64:
				m64 = (MemoryPart64 *)calloc(parts[current].tiles, sizeof(MemoryPart64));
				mem_parts = m64;
			break;
			case 128:
				m128 = (MemoryPart128 *)calloc(parts[current].tiles, sizeof(MemoryPart128));
				mem_parts = m128;
			break;
			case 256:
				m256 = (MemoryPart256 *)calloc(parts[current].tiles, sizeof(MemoryPart256));
				mem_parts = m256;
			break;
		}

		if(!mem_parts)
			return;

		parts[current].computeCoords();

		next = (RGBA*)realloc(next, realW * tileSize * sizeof(RGBA));

		if(!next)
		{
			QMessageBox::warning(sqApp, i18n("Decoding"), i18n("Memory allocation failed"), QMessageBox::Ok, QMessageBox::NoButton);

			if(finfo->images == 1)
				break;
			else
			{
				finfo->images--;
				current = 0;
				return;
			}
		}

		line = 0;

		if(!current)
		{
			slotZoomIfLess(true);

			curangle = 0.0f;
			isflippedH = isflippedV = false;
			matrixChanged();

			if(finfo->image[current].needflip)
				slotFlipV();
		}

		for(int pass = 0;pass < finfo->passes;pass++)
		{
			if(lib->fmt_next_pass(finfo) != SQERR_OK)
			{
//				qWarning("%s", i18n("fmt_next_pass() failed").ascii());
				break;
			}

//			printf("__Doing pass %d\n", pass);

			line = 0;

//			printf("finfo->image[current].h: %d\n", finfo->image[current].h);

			for(i = 0;i < parts[current].tilesy;i++)
			{
				memset(next, 0, realW * tileSize * sizeof(RGBA));

				for(j = 0;j < tileSize;j++)
				{
					if(line++ >= finfo->image[current].h)
						break;

//					printf("Got line %d\n", line-1);
					res = lib->fmt_read_scanline(finfo, next + realW*j);
					errors += (int)(res != SQERR_OK);
				}

				if(pass == finfo->passes-1)
				{
					setupBits(&parts[current], next, i);
					showFrames(i);
				}
			}
		}

		id = images->insertItem(QString::fromLatin1("#%1 [%2x%3@%4]")
									.arg(current)
									.arg(finfo->image[current].w)
									.arg(finfo->image[current].h)
									.arg(finfo->image[current].bpp));

		images->setItemParameter(id, current);

//		fprintf(stderr, "id: %d, current: %d\n", id, current);

		if(!current)
		{
			QString status;
			status.sprintf("%dx%d@%d", finfo->image[current].w, finfo->image[current].h, finfo->image[current].bpp);
			sqSBDecoded->setText(status);
			sqSBDecodedI->clear();
			sqSBDecodedI->setPixmap(QPixmap(lib->mime));

			first_id = id;
		}
//		printf("Done for current = #%d\n", current);
		current++;
	} // while(true)

	lib->fmt_close();

//	QMessageBox::warning(sqApp, i18n("Decoding"), i18n("Cycle done"), QMessageBox::Ok, QMessageBox::NoButton);
	current = 0;
	total = finfo->images;

	decoded = true;
	reset_mode = false;
	updateGL();

//	printf("Decoded. total = %d\n", total);

	if(finfo->animated)
		QTimer::singleShot(0, this, SLOT(slotAnimateNext()));
	else
		images->setItemChecked(first_id, true);

	int secs;
	int msecs = started.elapsed();
	adjustTimeFromMsecs(secs, msecs);
	QString r = (secs) ? i18n("Loaded in %1'%2 msec.").arg(secs).arg(msecs) : i18n("Loaded in %1 msec.").arg(msecs);
	sqSBLoaded->setText(r);

	printf("tileSize: %d\n", tileSize);
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

bool SQ_GLWidget::actionsHidden() const
{
	return pAHideToolbars->isChecked();
}

void SQ_GLWidget::paletteChange(const QPalette &oldPalette)
{
	QGLWidget::paletteChange(oldPalette);

	if(sqConfig->readNumEntry("GL view", "GL view background type", 0) == 0)
	{
		QColor color = colorGroup().color(QColorGroup::Base);
		qglClearColor(color);
		updateGL();
	}
}

void SQ_GLWidget::slotToggleStatus(bool s)
{
	sqGLView->statusbar()->setShown(s);
}

void SQ_GLWidget::slotFirst()
{
	if(!reset_mode)
		sqWStack->slotFirstFile();
}

void SQ_GLWidget::slotLast()
{
	if(!reset_mode)
		sqWStack->slotLastFile();
}

void SQ_GLWidget::slotNext()
{
	timer_next->stop();

	if(!reset_mode)
		timer_next->start(timer_delay_file, true);
}

void SQ_GLWidget::slotPrev()
{
	timer_prev->stop();

	if(!reset_mode)
		timer_prev->start(timer_delay_file, true);
}

void SQ_GLWidget::slotZoomMenu()
{
	zoom->exec(QCursor::pos());
}

void SQ_GLWidget::slotAnimateNext()
{
	updateGL();

	int delay = finfo->image[current].delay;

	current++;

	if(current >= finfo->images)
		current = 0;

	timer_anim->start(delay, true);
}

void SQ_GLWidget::startAnimation()
{
	if(!finfo) return;
	if(!finfo->animated) return;

	timer_anim->start(finfo->image[current].delay, true);
}

void SQ_GLWidget::stopAnimation()
{
	timer_anim->stop();
}

void SQ_GLWidget::slotToggleAnimate()
{
	if(!finfo) return;
	if(!finfo->animated) return;

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

	updateGL();
}

void SQ_GLWidget::slotShowImages()
{
	if(!finfo)
		return;

	if(finfo->animated)
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

	images->exec(QCursor::pos());
}

void SQ_GLWidget::slotImagedHidden()
{
	if(blocked && finfo->animated)
		startAnimation();
}

void SQ_GLWidget::nextImage()
{
	if(!finfo) return;

	current++;

	if(current >= finfo->images)
		current = 0;

	updateGL();
}

void SQ_GLWidget::prevImage()
{
	if(!finfo) return;

	current--;

	if(current < 0)
		current = finfo->images - 1;

	updateGL();
}

void SQ_GLWidget::jumpToImage(bool last)
{
	if(!finfo) return;

	current = (last) ? finfo->images - 1 : 0;

	updateGL();
}

inline
bool SQ_GLWidget::manualBlocked()
{
	return blocked_force;
}

void SQ_GLWidget::showHelp()
{
	SQ_HelpWidget *help_w = new SQ_HelpWidget(this);

	help_w->exec();
}

void SQ_GLWidget::toggleDrawingBackground()
{
	bool b = sqConfig->readBoolEntry("GL view", "alpha_bkgr", true);

	b = !b;

	sqConfig->writeEntry("alpha_bkgr", b);

	updateGL();
}

void SQ_GLWidget::showExternalTools()
{
	sqExternalTool->getConstPopupMenu()->exec(QCursor::pos());
}
