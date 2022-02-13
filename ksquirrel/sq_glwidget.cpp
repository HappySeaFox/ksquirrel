/***************************************************************************
                          sq_glviewwidget.cpp  -  description
                             -------------------
    begin                : Mon Mar 15 2004
    copyright            : (C) 2004 by ckult
    email                : squirrel-sf@yandex.ru
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
#include <qmessagebox.h>
#include <qstringlist.h>
#include <qdragobject.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qserversocket.h>
#include <qsocket.h>

#include <kaction.h>
#include <kcursor.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kapp.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include <math.h>
#include <stdlib.h>

#include "ksquirrel.h"
#include "sq_libraryhandler.h"
#include "sq_decodingthread.h"
#include "sq_config.h"
#include "sq_widgetstack.h"
#include "sq_quickbrowser.h"
#include "sq_diroperator.h"
#include "sq_glwidget.h"
#include "sq_glview.h"
#include "sq_imageproperties.h"
#include "sq_server.h"
#include "sq_glu.h"
#include "err.h"

#ifndef SQ_SECTION_NAME
#define SQ_SECTION_NAME ("squirrel image viewer file browser")
#endif

#define	glColor3fA(r,g,b) 		glColor3f(r*0.003921568, g*0.003921568, b*0.003921568)
#define	glClearColorA(r,g,b)	glClearColor(r*0.003921568, g*0.003921568, b*0.003921568, 0.0f)

SQ_GLWidget::SQ_GLWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
	isflippedV = 0;
	isflippedH = 0;
	memset(matrix, 0, sizeof(matrix));
	matrix[10] = matrix[5] = matrix[0] = 1.0f;
	rgba = 0L;
	finfo = 0L;
	curangle = 0.0f;
	quick = 0L;
	decoded = reset_mode = false;

	zoom_type = sqConfig->readNumEntry("GL view", "zoom type", 3);
	m_hiding = sqConfig->readBoolEntry("GL view", "actions", true);

	BGquads = QImage(locate("appdata", "images/checker.png"));

	ZoomModel = (sqConfig->readBoolEntry("GL view", "zoom pretty", true))?GL_LINEAR:GL_NEAREST;

	zoomfactor = sqConfig->readNumEntry("GL view", "zoom", 25);
	movefactor = sqConfig->readNumEntry("GL view", "move", 5);
	rotatefactor = sqConfig->readNumEntry("GL view", "angle", 90);

	cusual = QCursor(QPixmap::fromMimeSource(locate("appdata", "cursors/usual.png")));
	cdrag = QCursor(QPixmap::fromMimeSource(locate("appdata", "cursors/drag.png")));

	setCursor(cusual);

	setFocusPolicy(QWidget::WheelFocus);
	setAcceptDrops(true);

	createQuickBrowser();
	createActions();
	createToolbar();

	KCursor::setAutoHideCursor(this, true);
	KCursor::setHideCursorDelay(3000);

	pARender = new KAction(i18n("Render into file"), 0, 0, this, SLOT(slotRenderPixmapIntoFile()), sqApp->actionCollection(), "SQ render GL into file");

	server = new SQ_Server(0);

	connect(this, SIGNAL(matrixChanged()), SLOT(slotSetMatrixParamsString()));

	createMenu();
}

SQ_GLWidget::~SQ_GLWidget()
{
	delete thread;
}

void SQ_GLWidget::createDecodingThread()
{
	thread = new SQ_DecodingThread(this);
}

void	SQ_GLWidget::createActions()
{
	pARotateLeft = new KAction(i18n("Rotate left"), QPixmap::fromMimeSource(locate("appdata", "images/actions/rotateL.png")), 0, this, SLOT(slotRotateLeft()), sqApp->actionCollection(), "SQ Rotate picture left");
	pARotateRight = new KAction(i18n("Rotate right"), QPixmap::fromMimeSource(locate("appdata", "images/actions/rotateR.png")), 0, this, SLOT(slotRotateRight()), sqApp->actionCollection(), "SQ Rotate picture right");
	pAZoomPlus = new KAction(i18n("Zoom +"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoom+.png")), 0, this, SLOT(slotZoomPlus()), sqApp->actionCollection(), "SQ Zoom+");
	pAZoomMinus = new KAction(i18n("Zoom -"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoom-.png")), 0, this, SLOT(slotZoomMinus()), sqApp->actionCollection(), "SQ Zoom-");
	pAZoomW = new KToggleAction(i18n("Fit width"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoomW.png")), 0, this, SLOT(slotZoomW()), sqApp->actionCollection(), "SQ ZoomW");
	pAZoomH = new KToggleAction(i18n("Fit height"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoomH.png")), 0, this, SLOT(slotZoomH()), sqApp->actionCollection(), "SQ ZoomH");
	pAZoomWH = new KToggleAction(i18n("Fit image"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoomWH.png")), 0, this, SLOT(slotZoomWH()), sqApp->actionCollection(), "SQ ZoomWH");
	pAZoom100 = new KToggleAction(i18n("Zoom 100%"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoom100.png")), 0, this, SLOT(slotZoom100()), sqApp->actionCollection(), "SQ Zoom100");
	pAFlipV = new KAction(i18n("Flip vertically"), QPixmap::fromMimeSource(locate("appdata", "images/actions/flipV.png")), 0, this, SLOT(slotFlipV()), sqApp->actionCollection(), "SQ flip_v");
	pAFlipH = new KAction(i18n("Flip horizontally"), QPixmap::fromMimeSource(locate("appdata", "images/actions/flipH.png")), 0, this, SLOT(slotFlipH()), sqApp->actionCollection(), "SQ flip_h");
	pAReset = new KAction(i18n("Normalize"), QPixmap::fromMimeSource(locate("appdata", "images/actions/reload.png")), 0, this, SLOT(slotMatrixReset()), sqApp->actionCollection(), "SQ ResetGL");
	pAClose = new KAction(i18n("Close"), QPixmap::fromMimeSource(locate("appdata", "images/actions/close.png")), 0, sqApp, SLOT(slotCloseGLWidget()), sqApp->actionCollection(), "SQ GL close");
	pAProperties = new KAction(i18n("Image Properties"), QPixmap::fromMimeSource(locate("appdata", "images/actions/prop.png")), 0, this, SLOT(slotProperties()), sqApp->actionCollection(), "SQ GL Prop");
	pAFull = new KToggleAction(i18n("Fullscreen"), QPixmap::fromMimeSource(locate("appdata", "images/actions/fullscreen.png")), 0, 0, 0, sqApp->actionCollection(), "SQ GL Full");
	pAQuick = new KToggleAction(i18n("\"Quick Browser\""), QPixmap::fromMimeSource(locate("appdata", "images/actions/quick.png")), 0, 0, 0, sqApp->actionCollection(), "SQ GL Quick");
	pAIfLess = new KToggleAction(i18n("Ignore if little image"), QPixmap::fromMimeSource(locate("appdata", "images/actions/ifless.png")), 0, 0, 0, sqApp->actionCollection(), "SQ GL If Less");
	pANext = new KAction(i18n("Next file"), QPixmap::fromMimeSource(locate("appdata", "images/actions/next.png")), 0, sqWStack, SLOT(emitNextSelected()), sqApp->actionCollection(), "SQ GL Next file");
	pAPrev = new KAction(i18n("Previous file"), QPixmap::fromMimeSource(locate("appdata", "images/actions/prev.png")), 0, sqWStack, SLOT(emitPreviousSelected()), sqApp->actionCollection(), "SQ GL Prev file");
	pAHide = new KAction(i18n("Hide"), QPixmap::fromMimeSource(locate("appdata", "images/actions/toolbar_hide.png")), 0, this, SLOT(slotHideToolbar()), sqApp->actionCollection(), "SQ GL THide");
	pAShow = new KAction(i18n("Show"), QPixmap::fromMimeSource(locate("appdata", "images/actions/toolbar_show.png")), 0, this, SLOT(slotShowToolbar()), sqApp->actionCollection(), "SQ GL TShow");
	pAFirst = new KAction(i18n("Go to first file"), QPixmap::fromMimeSource(locate("appdata", "images/actions/file_first.png")), 0, sqWStack, SLOT(slotFirstFile()), sqApp->actionCollection(), "SQ GL File First");
	pALast = new KAction(i18n("Go to last file"), QPixmap::fromMimeSource(locate("appdata", "images/actions/file_last.png")), 0, sqWStack, SLOT(slotLastFile()), sqApp->actionCollection(), "SQ GL File Last");

	pAZoomW->setExclusiveGroup("squirrel_zoom_actions");
	pAZoomH->setExclusiveGroup("squirrel_zoom_actions");
	pAZoomWH->setExclusiveGroup("squirrel_zoom_actions");
	pAZoom100->setExclusiveGroup("squirrel_zoom_actions");

	connect(pAFull, SIGNAL(toggled(bool)), sqApp, SLOT(slotFullScreen(bool)));
	connect(pAIfLess, SIGNAL(toggled(bool)), SLOT(slotZoomIfLess(bool)));
	connect(pAQuick, SIGNAL(toggled(bool)), v, SLOT(setShown(bool)));

	pAIfLess->setChecked(sqConfig->readBoolEntry("GL view", "ignore", false));
}

void SQ_GLWidget::createToolbar()
{
	toolbar2 = new KToolBar(this);
	toolbar2->setGeometry(0, 0, 35, 30);
	toolbar2->setIconSize(22);
	toolbar2->setPalette(QPalette(QColor(255,255,255), QColor(255,255,255)));
	pAShow->plug(toolbar2);
 
	toolbar = new KToolBar(this);
	toolbar->setGeometry(0, 0, 510, 30);
	toolbar->setIconSize(22);
	toolbar->setPalette(QPalette(QColor(255,255,255), QColor(255,255,255)));

	if(m_hiding)
		toolbar->show();
	else
		toolbar->move(-toolbar->width(), 0);

	toolbar2->setShown(!m_hiding);

	KActionMenu *zoom = new KActionMenu(i18n("Zoom"), QPixmap::fromMimeSource(locate("appdata", "images/actions/zoom_template.png")));
	zoom->setDelayed(false);

	KActionSeparator *pASep = new KActionSeparator;

	zoom->insert(pAZoom100);
	zoom->insert(pASep);
	zoom->insert(pAZoomW);
	zoom->insert(pAZoomH);
	zoom->insert(pAZoomWH);
	zoom->insert(pASep);
	zoom->insert(pAIfLess);

	switch(zoom_type)
	{
		case 0: pAZoomW->setChecked(true); break;
		case 1: pAZoomH->setChecked(true); break;
		case 2: pAZoomWH->setChecked(true); break;
		case 3: pAZoom100->setChecked(true); break;

		default: pAZoom100->setChecked(true);
	}

	pAFirst->plug(toolbar);
	pAPrev->plug(toolbar);
	pANext->plug(toolbar);
	pALast->plug(toolbar);
	pAZoomPlus->plug(toolbar);
	pAZoomMinus->plug(toolbar);
	zoom->plug(toolbar);
	pARotateLeft->plug(toolbar);
	pARotateRight->plug(toolbar);
	pAFlipV->plug(toolbar);
	pAFlipH->plug(toolbar);
	pAReset->plug(toolbar);
	pAProperties->plug(toolbar);
	pAFull->plug(toolbar);
	pAQuick->plug(toolbar);
	pAClose->plug(toolbar);
	pASep->plug(toolbar);
	pAHide->plug(toolbar);

	timer_show = new QTimer(this);
	timer_hide = new QTimer(this);

	connect(timer_show, SIGNAL(timeout()), SLOT(slotStepToolbarMove()));
	connect(timer_hide, SIGNAL(timeout()), SLOT(slotStepToolbarMove()));
}

void SQ_GLWidget::initializeGL()
{
	glEnable(GL_TEXTURE_2D);
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SQ_GLWidget::ZoomModel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SQ_GLWidget::ZoomModel);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SQ_GLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-width/2, width/2, -height/2, height/2, 0.1f, 100.0f);
	SQ_GLU::gluLookAt(0,0,1, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);

	thread->moveProgressBar();
}

void SQ_GLWidget::draw_background(void *bits, int dim, GLfloat w, GLfloat h)
{
	static unsigned int tex_id = 0;
	float half_w, half_h;

	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

	half_w = w / 2.0;
	half_h = h / 2.0;

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);						glVertex2f(-half_w, half_h);
		glTexCoord2f(w/(GLfloat)dim, 0.0);			glVertex2f(half_w, half_h);
		glTexCoord2f(w/(GLfloat)dim, h/(GLfloat)dim);	glVertex2f(half_w, -half_h);
		glTexCoord2f(0.0, h/(GLfloat)dim);			glVertex2f(-half_w, -half_h);
	glEnd();
}

void SQ_GLWidget::paintGL()
{
	GLfloat half_w, half_h;
	GLfloat half_w2, half_h2;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	if(sqConfig->readNumEntry("GL view", "GL view background type", 0) == 2)
	{
		matrix_push();
		matrix_pure_reset();
		matrix_move_z(-10.0f);
		draw_background(BGpixmap.bits(), BGpixmap.width(), width(), height());
		matrix_pop();
		write_gl_matrix();
	}

//	printf("decoded: %d\n", decoded);
//	if(!decoded) return;

	half_w = (GLfloat)realW / 2.0f, half_h = (GLfloat)realH / 2.0f;
	half_w2 = (GLfloat)finfo->w / 2.0f, half_h2 = (GLfloat)finfo->h / 2.0f;

	if(!reset_mode && decoded)
	{
		if(finfo->hasalpha)
		{
//			matrix_push();
//			matrix_pure_reset_noxy();
			matrix_move_z(-8.0f);
			draw_background(BGquads.bits(), 32, (float)finfo->w, (float)finfo->h);
//			matrix_pop();
//			write_gl_matrix();
		}
	}

	if(!reset_mode)
	{
		matrix_move_z(-7.0f);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-half_w, half_h);
			glTexCoord2f(1.0f, 0.0f); glVertex2f( half_w, half_h);
			glTexCoord2f(1.0f, 1.0f); glVertex2f( half_w, -half_h);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-half_w, -half_h);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}

void SQ_GLWidget::slotSetMatrixParamsString()
{
	QString str;

	str.sprintf("%s%s %.1f%% (%.1f deg)", ((isflippedV)?"V":""), ((isflippedH)?"H":""), get_zoom() * 100.0f, get_angle());

	sqSBGLreport->setText(str);
}

void SQ_GLWidget::setZoomFactor(const GLfloat &newfactor)
{
	zoomfactor = newfactor;
}

const GLfloat SQ_GLWidget::getZoomFactor() const
{
	return zoomfactor;
}

void SQ_GLWidget::setMoveFactor(const GLfloat &newfactor)
{
	movefactor = newfactor;
}

const GLfloat SQ_GLWidget::getMoveFactor() const
{
	return movefactor;
}

void SQ_GLWidget::setRotateFactor(const GLfloat &newfactor)
{
	rotatefactor = newfactor;
}

const GLfloat SQ_GLWidget::getRotateFactor() const
{
	return rotatefactor;
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
	else if(e->delta() > 0 && e->state() == Qt::ControlButton)
		matrix_zoom(2.0f);
	else if(e->delta() < 0 && e->state() == Qt::ControlButton)
		matrix_zoom(0.5f);
	else if(e->delta() > 0 && e->state() == Qt::ShiftButton)
		slotZoomPlus();
	else if(e->delta() < 0 && e->state() == Qt::ShiftButton)
		slotZoomMinus();
	else
		e->accept();
}

void SQ_GLWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->state() == Qt::LeftButton || e->state() == Qt::NoButton)
	{
		setCursor(cdrag);

		xmoveold = e->x();
		ymoveold = e->y();
	}
	else e->accept();
}

void SQ_GLWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(e->state() == Qt::LeftButton)
	{
		xmove = e->x();
		ymove = e->y();

		matrix_move(xmove-xmoveold, -ymove+ymoveold);

		xmoveold = e->x();
		ymoveold = e->y();
	}
	else
		e->accept();
}

void SQ_GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
	setCursor(cusual);
	e->accept();
}

void SQ_GLWidget::keyPressEvent(QKeyEvent *e)
{
	unsigned long value = e->key();
	const unsigned short Key_KC = Qt::ControlButton|Qt::Keypad;

	value <<= 16;
	value |= e->state();

#define SQ_KEYSTATES(a,b) ((a << 16) | b)

	switch(value)
	{
		case SQ_KEYSTATES(Qt::Key_Left, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Left, Qt::NoButton):matrix_move(movefactor, 0);break;
		case SQ_KEYSTATES(Qt::Key_Right, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Right, Qt::NoButton):matrix_move(-movefactor, 0);break;
		case SQ_KEYSTATES(Qt::Key_Up, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Up, Qt::NoButton):matrix_move(0, -movefactor);break;
		case SQ_KEYSTATES(Qt::Key_Down, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Down, Qt::NoButton):matrix_move(0, movefactor);break;
		case SQ_KEYSTATES(Qt::Key_Plus, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Plus, Qt::NoButton):pAZoomPlus->activate();break;
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::Keypad):
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::NoButton):pAZoomMinus->activate();break;
		case SQ_KEYSTATES(Qt::Key_Plus, Key_KC):
		case SQ_KEYSTATES(Qt::Key_Plus, Qt::ControlButton):matrix_zoom(2.0f);break;
		case SQ_KEYSTATES(Qt::Key_Minus, Key_KC):
		case SQ_KEYSTATES(Qt::Key_Minus, Qt::ControlButton):matrix_zoom(0.5f);break;
		case SQ_KEYSTATES(Qt::Key_V, Qt::NoButton):pAFlipV->activate();break;
		case SQ_KEYSTATES(Qt::Key_H, Qt::NoButton):pAFlipH->activate();break;
		case SQ_KEYSTATES(Qt::Key_Left, Qt::ControlButton):pARotateLeft->activate();break;
		case SQ_KEYSTATES(Qt::Key_Right, Qt::ControlButton):pARotateRight->activate();break;
		case SQ_KEYSTATES(Qt::Key_R, Qt::NoButton):pAReset->activate();break;
		case SQ_KEYSTATES(Qt::Key_R, Qt::ControlButton):pARender->activate();break;
		case SQ_KEYSTATES(Qt::Key_Up, Qt::ControlButton):matrix_rotate(180.0f);break;
		case SQ_KEYSTATES(Qt::Key_Down, Qt::ControlButton):matrix_rotate(-180.0f);break;
		case SQ_KEYSTATES(Qt::Key_Left, Qt::ShiftButton):matrix_rotate(-1.0f);break;
		case SQ_KEYSTATES(Qt::Key_Right, Qt::ShiftButton):matrix_rotate(1.0f);break;
		case SQ_KEYSTATES(Qt::Key_Space, Qt::NoButton):sqWStack->emitNextSelected();;break;
		case SQ_KEYSTATES(Qt::Key_BackSpace, Qt::NoButton):sqWStack->emitPreviousSelected();break;
		case SQ_KEYSTATES(Qt::Key_X, Qt::NoButton):
		case SQ_KEYSTATES(Qt::Key_Escape, Qt::NoButton):pAClose->activate();break;
		case SQ_KEYSTATES(Qt::Key_P, Qt::NoButton):pAProperties->activate();break;
		case SQ_KEYSTATES(Qt::Key_Home, Qt::NoButton):pAFirst->activate();break;
		case SQ_KEYSTATES(Qt::Key_End, Qt::NoButton):pALast->activate();break;
		case SQ_KEYSTATES(Qt::Key_Q, Qt::NoButton):pAQuick->activate();break;
		case SQ_KEYSTATES(Qt::Key_F, Qt::NoButton):pAFull->activate();break;

		default:
			e->accept();
	}
}

void SQ_GLWidget::dropEvent(QDropEvent *e)
{
	QStringList files;

	if(QUriDrag::decodeLocalFiles(e, files))
	{
		QStringList::Iterator i=files.begin();

		emitShowImage(*i);
	}
}

void SQ_GLWidget::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept(QUriDrag::canDecode(e));
}

void SQ_GLWidget::contextMenuEvent(QContextMenuEvent*)
{
	menu->exec(QCursor::pos());
}

void SQ_GLWidget::slotZoomW()
{
	zoom_type = 0;
	pAZoomW->setChecked(true);
	float factor = (float)width() / (float)finfo->w;

	if(pAIfLess->isChecked() && (finfo->w < (unsigned long)width() && finfo->h < (unsigned long)height()))
		factor = 1.0f;

	matrix_reset();
	matrix_zoom(factor);
}

void SQ_GLWidget::slotZoomH()
{
	zoom_type = 1;
	pAZoomH->setChecked(true);
	float factor = (float)height() / (float)finfo->h;

	if(pAIfLess->isChecked() && (finfo->w < (unsigned long)width() && finfo->h < (unsigned long)height()))
		factor = 1.0f;

	matrix_reset();
	matrix_zoom(factor);
}

void SQ_GLWidget::slotZoomWH()
{
	zoom_type = 2;
	pAZoomWH->setChecked(true);

	float factor = 1.0;
	float w = (float)width(), h = (float)height();
	float factorw = w / (float)finfo->w;
	float factorh = h / (float)finfo->h;
	float t = w / h;

	if(t > (float)finfo->w / (float)finfo->h)
		factor = factorh;
	else
		factor = factorw;

	if(pAIfLess->isChecked() && (finfo->w < (unsigned long)width() && finfo->h < (unsigned long)height()))
		factor = 1.0f;

	matrix_reset();
	matrix_zoom(factor);
}

void SQ_GLWidget::slotZoom100()
{
	zoom_type = 3;
	pAZoom100->setChecked(true);

	matrix_reset();
	matrix_zoom(1.0);
}

void SQ_GLWidget::slotZoomIfLess(bool)
{
	switch(zoom_type)
	{
		case 0: pAZoomW->activate(); break;
		case 1: pAZoomH->activate(); break;
		case 2: pAZoomWH->activate(); break;
	}

	if(!reset_mode)
		updateGL();
}

void SQ_GLWidget::slotShowToolbar()
{
	m_hiding = false;
	steps = 30;

	toolbar2->hide();

	timer_show->start(5);
}

void SQ_GLWidget::slotHideToolbar()
{
	m_hiding = true;
	steps = 30;

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
		if(m_hiding)
		{
			timer_hide->stop();
			toolbar2->show();
		}
		else
			timer_show->stop();
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

	const unsigned long real_size = (finfo->w * finfo->h) << 2;
	QStringList list = QStringList::split("\n", finfo->dump);

	list << QString("%1").arg(KIO::convertSize(real_size)) << QString("%1").arg((double)real_size / fm.size());
	SQ_ImageProperties *prop = new SQ_ImageProperties(this);
	prop->show();
	prop->setFile(File);
	prop->setParams(list);
}

/*
	 operations with matrices are taken from GLiv =)

	thanks to Guillaume Chazarian.
*/
void SQ_GLWidget::flip(int id)
{
	GLfloat	x = 0.0, y = 0.0;
	int	center = sqConfig->readNumEntry("GL view", "manipulation center", 0);

	if(!center)
	{
		x = MATRIX_X, y = MATRIX_Y;
		MATRIX_X = MATRIX_Y = 0;
		write_gl_matrix();
	}

	matrix[id] *= -1.0;
	matrix[id + 1] *= -1.0;
	matrix[id + 3] *= -1.0;

	if(!center)
	{
		MATRIX_X = x;
		MATRIX_Y = y;
	}

	write_gl_matrix();

	if(!reset_mode)
		updateGL();
}

void SQ_GLWidget::slotFlipH()
{
	isflippedH = ~isflippedH;
	flip(0);
}

void SQ_GLWidget::slotFlipV()
{
	isflippedV = ~isflippedV;
	flip(4);
}

void SQ_GLWidget::slotMatrixReset()
{
	matrix_reset();
	updateGL();
}

void SQ_GLWidget::write_gl_matrix()
{
	static GLfloat transposed[16] =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0};

	transposed[0] = MATRIX_C1;
	transposed[5] = MATRIX_C2;
	transposed[4] = MATRIX_S1;
	transposed[1] = MATRIX_S2;
	transposed[12] = MATRIX_X;
	transposed[13] = MATRIX_Y;
	transposed[14] = MATRIX_Z;

	glLoadMatrixf(transposed);

	emit matrixChanged();
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
	isflippedH = isflippedV = 0;

	if(finfo)
		if(finfo->needflip)
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

	curangle = 0.0f;
	isflippedH = isflippedV = 0;

	write_gl_matrix();
}

void SQ_GLWidget::matrix_pure_reset_noxy()
{
	int i;

	GLfloat x = MATRIX_X, y = MATRIX_Y;

	for (i = 0; i < 12; i++)
		matrix[i] = (GLfloat) (i % 5 == 0);

	curangle = 0.0f;
	isflippedH = isflippedV = 0;

	MATRIX_X = x;
	MATRIX_Y = y;

	write_gl_matrix();
}

void SQ_GLWidget::matrix_zoom(GLfloat ratio)
{
	MATRIX_C1 *= ratio;
	MATRIX_S1 *= ratio;
	MATRIX_X *= ratio;
	MATRIX_S2 *= ratio;
	MATRIX_C2 *= ratio;
	MATRIX_Y *= ratio;

	write_gl_matrix();

	if(!reset_mode)
		updateGL();
}

GLfloat SQ_GLWidget::get_zoom() const
{
	return hypotf(MATRIX_C1, MATRIX_S1);
}

GLfloat SQ_GLWidget::get_angle() const
{
	return curangle;
}

void SQ_GLWidget::matrix_rotate(GLfloat angle)
{
	double cosine, sine, rad;
	const double rad_const = 0.017453;
	GLfloat c1 = MATRIX_C1, c2 = MATRIX_C2, s1 = MATRIX_S1, s2 = MATRIX_S2, x = MATRIX_X, y = MATRIX_Y;

	rad = angle * rad_const;
	cosine = cos(rad);
	sine = sin(rad);

	MATRIX_C1 = c1 * cosine + s2 * sine;
	MATRIX_S1 = s1 * cosine + c2 * sine;
	MATRIX_S2 = -c1 * sine + s2 * cosine;
	MATRIX_C2 = -s1 * sine + c2 * cosine;

	if(sqConfig->readNumEntry("GL view", "manipulation center", 0) == 1)
	{
		MATRIX_X = x * cosine + y * sine;
		MATRIX_Y = -x * sine + y * cosine;
	}

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

void SQ_GLWidget::emitShowImage(const KURL &url)
{
	emitShowImage(url.path());
}

void SQ_GLWidget::setClearColor()
{
	QColor color;
	QString path;

	switch(sqConfig->readNumEntry("GL view", "GL view background type", 0))
	{
		case 0:
			color = KGlobalSettings::baseColor();
		break;

		case 1:
			color.setNamedColor(sqConfig->readEntry("GL view", "GL view background", "#cccccc"));
		break;

		case 2:
			path = sqConfig->readEntry("GL view", "GL view custom texture", "");
			BGpixmap = QPixmap(path).convertToImage();
			BGpixmap.convertDepth(32);
			BGpixmap = QGLWidget::convertToGLFormat(BGpixmap);
		break;

		default: ;
	}

	glClearColorA(color.red(), color.green(), color.blue());

	if(finfo)
		updateGL();
}

void SQ_GLWidget::setTextureParams()
{
	ZoomModel = (sqConfig->readBoolEntry("GL view", "zoom pretty", true))?GL_LINEAR:GL_NEAREST;

	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SQ_GLWidget::ZoomModel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SQ_GLWidget::ZoomModel);

	updateGL();
}

void SQ_GLWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
//	close();
	e->accept();
}

void SQ_GLWidget::slotRenderPixmapIntoFile()
{
	QString file, final;
	QImage im_rendered = grabFrameBuffer(true);
	QPixmap px_rendered;
	QTime time = QTime::currentTime(Qt::LocalTime);
	QDate date = QDate::currentDate(Qt::LocalTime);
	QString ext;

	if(!sqConfig->readNumEntry("GL view", "render file type", 1))
	{
		file = KFileDialog::getSaveFileName("/", "*", this, i18n("Save file as ..."));

		if(file.isEmpty())
			return;

		final = file;
	}
	else
	{
		QString dy, dmn, dw, dd, dh, dm, ds, dms;

		dy = QString("%1").arg(date.year());
		dmn = QString("%1").arg(date.month());
		dw = QString("%1").arg(date.longDayName(date.dayOfWeek()));
		dd = QString("%1").arg(date.day());
		dh = QString("%1").arg(time.hour());
		dm = QString("%1").arg(time.minute());
		ds = QString("%1").arg(time.second());
		dms = QString("%1").arg(time.msec());
		ext = sqConfig->readEntry("GL view", "render ext", "PNG");
		file = sqConfig->readEntry("GL view", "render file format", "ksquirrel %dd.%dmn.%dy, %dh.%dm.%ds");

		file.replace("%dy", dy);
		file.replace("%dmn", dmn);
		file.replace("%dw", dw);
		file.replace("%dd", dd);
		file.replace("%dh", dh);
		file.replace("%dm", dm);
		file.replace("%ds", ds);
		file.replace("%dms", dms);
		final = sqConfig->readEntry("GL view", "render path", "/") + "/" + file + "." + ext;
	}

	px_rendered.convertFromImage(im_rendered, AvoidDither);

	px_rendered.save(final, ext);
}

unsigned long SQ_GLWidget::findCloserPower2(unsigned long num)
{
	unsigned long i = 1;

	do
	{
		i <<= 1;
	}while(num >  i);

	return i;
}

void	SQ_GLWidget::createQuickBrowser()
{
	v = new SQ_QuickBrowser(this);
}

bool	SQ_GLWidget::prepare()
{
	const char 		*name = File.ascii();
	QString			status;
	unsigned int		i;

	fm.setFile(File);

	lib = sqLibHandler->setCurrentLibrary(fm.extension(false));

	if(!lib)
	{
		QMessageBox::warning(sqGLView, i18n("Decoding"), i18n(QString("Library for %1 format not found")).arg(fm.extension(false)), QMessageBox::Ok, QMessageBox::NoButton);
		return false;
	}

	free(finfo);

	lib->fmt_init(&finfo, name);
	i = lib->fmt_read_info(finfo);

	if(i != SQERR_OK)
	{
		switch(i)
		{
			case SQERR_BADFILE: status = i18n("File corrupted !\n\nLibrary: ") + lib->libpath + i18n("\nHandled extensions: ") + lib->sinfo; break;
			case SQERR_NOMEMORY: status = i18n("Library couldn't alloc memory for internal usage !"); break;
			case SQERR_NOTSUPPORTED: status = i18n("This format type is not supported."); break;

			default: status = i18n("Library returned unknown error code.");
		}

		QMessageBox::warning(sqGLView, i18n("Decoding"), status, QMessageBox::Ok, QMessageBox::NoButton);
		return false;
	}

//	realW = realH = 2;
//	reset_mode = true;
//	glTexImage2D(GL_TEXTURE_2D, 0, 4, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, point);
//	matrix_reset(false);

	realW = findCloserPower2(finfo->w);
	realH = findCloserPower2(finfo->h);

	pict_size = realW * realH * 4;
	rgba = (RGBA*)realloc(rgba, pict_size);

	if(rgba == NULL)
	{
		status.sprintf(i18n("Oops! Memory realloc failed.\n\nI tried to realloc memory for image:\n\nwidth: %ld\nheight: %ld\nbpp: %d\nTotal needed: "), realW, realH, finfo->bpp);
		status = status + KIO::convertSize(pict_size) + i18n(" of memory.");
		QMessageBox::warning(sqGLView,i18n( "Decoding"), status, QMessageBox::Ok, QMessageBox::NoButton);
		return false;
	}

	memset(rgba, 0, pict_size);

	status.sprintf("%ldx%ld@%d bit", finfo->w, finfo->h, finfo->bpp);
	sqSBDecoded->setText(status);

	rgba_next = rgba + ((realW - finfo->w) / 2) + ((realH - finfo->h) / 2)*realW;

	thread->setParams(lib, finfo, realW, rgba_next);

	return true;
}

void SQ_GLWidget::emitShowImage(const QString &file)
{
	reset_mode = true;

	File = file;
	File = File.local8Bit();

	sqApp->raiseGLWidget();
	sqGLView->setCaption(file);

	if(thread->running())
	{
		thread->stop();
		thread->wait();
	}

	if(!prepare())
		return;

	thread->start();
}

void SQ_GLWidget::slotShowImage()
{
	matrix_pure_reset();
	decoded = true;

	slotZoomIfLess(true);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, realW, realH, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

	reset_mode = false;

	if(finfo->needflip)
		slotFlipV();
	else
		updateGL();

	emit matrixChanged();
}

bool SQ_GLWidget::actions()
{
	return toolbar2->isHidden();
}

int SQ_GLWidget::zoomType()
{
	return zoom_type;
}

void SQ_GLWidget::createMenu()
{
	menu = new QPopupMenu(this);

	KActionSeparator *pASep = new KActionSeparator;

	pARender->plug(menu);
	pASep->plug(menu);
	pAReset->plug(menu);
	pASep->plug(menu);
	pARotateLeft->plug(menu);
	pARotateRight->plug(menu);
	pASep->plug(menu);
	pAFlipV->plug(menu);
	pAFlipH->plug(menu);
	pASep->plug(menu);
	pAZoomPlus->plug(menu);
	pAZoomMinus->plug(menu);
	pASep->plug(menu);
	pAProperties->plug(menu);
	pASep->plug(menu);
	pAFull->plug(menu);
	pAClose->plug(menu);
}
