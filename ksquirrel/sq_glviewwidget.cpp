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

#include "sq_glviewwidget.h"

#include <qmessagebox.h>
#include <qstringlist.h>
#include <qdragobject.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qfileinfo.h>

#include <kaction.h>

#include "ksquirrel.h"
#include "sq_libraryhandler.h"
#include "sq_decoder.h"

SQ_GLViewWidget::SQ_GLViewWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
	decoder = new SQ_Decoder;

	setZoomFactor(0.15f);

	sqConfig->setGroup("GL view");

	int tp = sqConfig->readNumEntry("zoom model", 0);
       if(tp) ZoomModel = GL_NEAREST;
       else ZoomModel = GL_LINEAR;

	tp = sqConfig->readNumEntry("shade model", 0);
       if(tp) ShadeModel = GL_SMOOTH;
       else ShadeModel = GL_FLAT;
	
	density = 0.0f;

	setFogColor(0.0, 0.9, 0.0, 1);

	setFocusPolicy(QWidget::ClickFocus);
	setAcceptDrops(true);
}

SQ_GLViewWidget::~SQ_GLViewWidget()
{}

void SQ_GLViewWidget::setZoomFactor(const GLfloat &newfactor)
{
	zoomfactor = newfactor;
}

const GLfloat SQ_GLViewWidget::getZoomFactor() const
{
	return zoomfactor;
}

void SQ_GLViewWidget::setFogColor(const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat w)
{
	fogcolor[0] = r;
	fogcolor[1] = g;
	fogcolor[2] = b;
	fogcolor[3] = w;
}

const GLfloat* SQ_GLViewWidget::getFogColor() const
{
	return fogcolor;
}

void SQ_GLViewWidget::initializeGL()
{
	const GLdouble koeff = 0.003921567;

	glEnable(GL_TEXTURE_2D);
	glClearColor(sqGLViewBGColor.red()*koeff, sqGLViewBGColor.green()*koeff, sqGLViewBGColor.blue()*koeff, 0.0f);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FOG);
	glFogfv(GL_FOG_COLOR, fogcolor);
	glFogf(GL_FOG_DENSITY, density);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(SQ_GLViewWidget::ShadeModel);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glClear(GL_COLOR_BUFFER_BIT);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SQ_GLViewWidget::ZoomModel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SQ_GLViewWidget::ZoomModel);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, sqGLDecoder->width(), sqGLDecoder->height(), GL_RGBA, GL_UNSIGNED_BYTE, rgba);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.5f);
}

void SQ_GLViewWidget::resizeGL(int width, int height)
{
//	GLfloat w2 = (width+1) / 2.0f, h2 = (height+1) / 2.0f;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	glOrtho(-w2, w2, -h2, h2, -1.0f, 1.0f);
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void SQ_GLViewWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-w/h, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( w/h, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( w/h,  -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-w/h,  -1.0f);
	glEnd();
}

void SQ_GLViewWidget::wheelEvent(QWheelEvent *e)
{
	if(e->delta() > 0 && e->state() == Qt::ControlButton)
		glScalef(2.0f, 2.0f, 1.0f);
	else if(e->delta() < 0 && e->state() == Qt::ControlButton)
		glScalef(0.50f, 0.50f, 1.0f);
	else if(e->delta() > 0 && e->state() == Qt::ShiftButton)
		glScalef(1.0f+zoomfactor, 1.0f+zoomfactor, 1.0f);
	else if(e->delta() < 0 && e->state() == Qt::ShiftButton)
		glScalef(1.0f-zoomfactor, 1.0f-zoomfactor, 1.0f);
	else { e->ignore(); return; }

	updateGL();
}

void SQ_GLViewWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->state() == Qt::LeftButton)
	{
		xmoveold = e->x();
		ymoveold = e->y();
	}
	else e->ignore();
}

void SQ_GLViewWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if(e->state() == Qt::LeftButton)
	{
		xmoveold = 0;
		ymoveold = 0;
	}
	else e->ignore();
}

void SQ_GLViewWidget::mouseMoveEvent(QMouseEvent *e)
{
	return;

	if(e->state() == Qt::LeftButton)
	{
		QString cap;

		xmove = e->x();
		ymove = e->y();

		if(xmove != xmoveold)
			glTranslatef(((xmove < xmoveold)?(-0.01f):(0.01f)), 0.0f, 0.0f);
		if(ymove != ymoveold)
			glTranslatef(0.0f, ((ymove < ymoveold)?(0.01f):(-0.01f)), 0.0f);

		cap.sprintf("%d,%d", xmoveold-xmove, ymoveold-ymove);
		setCaption(cap);

		xmoveold = e->x();
		ymoveold = e->y();

		updateGL();
	}
	else
		e->ignore();
}

void SQ_GLViewWidget::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Left && e->state() == Qt::NoButton)
		glTranslatef(0.05f, 0.0f, 0.0f);
	else if(e->key() == Qt::Key_Right && e->state() == Qt::NoButton)
		glTranslatef(-0.05f, 0.0f, 0.0f);
	else if(e->key() == Qt::Key_Up && e->state() == Qt::NoButton)
		glTranslatef(0.0f, -0.05f, 0.0f);
	else if(e->key() == Qt::Key_Down && e->state() == Qt::NoButton)
		glTranslatef(0.0f, 0.05f, 0.0f);
	else if(e->key() == Qt::Key_Plus  && ((e->state()&Qt::ControlButton) == 0))
		glScalef(1.0f+zoomfactor, 1.0f+zoomfactor, 1.0f);
	else if(e->key() == Qt::Key_Minus  && ((e->state()&Qt::ControlButton) == 0))
		glScalef(1.0f-zoomfactor, 1.0f-zoomfactor, 1.0f);
	else if(e->key() == Qt::Key_Plus && ((e->state()&Qt::ControlButton) == Qt::ControlButton))
		glScalef(2.0f, 2.0f, 1.0f);
	else if(e->key() == Qt::Key_Minus  && ((e->state()&Qt::ControlButton) == Qt::ControlButton))
		glScalef(0.50f, 0.50f, 1.0f);
	else if(e->key() == Qt::Key_Left && e->state() == Qt::ControlButton)
		{ glTranslatef(0.0f, 0.0f, 0.0f); glRotatef(90.0f, 0.0f, 0.0f, 1.0f); }
	else if(e->key() == Qt::Key_Right && e->state() == Qt::ControlButton)
		{ glTranslatef(0.0f, 0.0f, 0.0f); glRotatef(-90.0f, 0.0f, 0.0f, 1.0f); }
	else if(e->key() == Qt::Key_Up && e->state() == Qt::ControlButton)
		{ glTranslatef(0.0f, 0.0f, 0.0f); glRotatef(180.0f, 0.0f, 0.0f, 1.0f); }
	else if(e->key() == Qt::Key_Down && e->state() == Qt::ControlButton)
		{ glTranslatef(0.0f, 0.0f, 0.0f); glRotatef(-180.0f, 0.0f, 0.0f, 1.0f); }
	else if(e->key() == Qt::Key_Left && e->state() == Qt::ShiftButton)
		{ glTranslatef(0.0f, 0.0f, 0.0f); glRotatef(1.0f, 0.0f, 0.0f, 1.0f); }
	else if(e->key() == Qt::Key_Right && e->state() == Qt::ShiftButton)
		{ glTranslatef(0.0f, 0.0f, 0.0f); glRotatef(-1.0f, 0.0f, 0.0f, 1.0f); }
	else if(e->key() == Qt::Key_F2 && e->state() == Qt::NoButton)
		{ if(density<1.5f) density += (GLfloat)0.01f; glFogf(GL_FOG_DENSITY, density); }
	else if(e->key() == Qt::Key_F1 && e->state() == Qt::NoButton)
		{ if(density>0) density -= (GLfloat)0.01f; glFogf(GL_FOG_DENSITY, density); }
	else { e->ignore(); return; }

	updateGL();
}

void SQ_GLViewWidget::dropEvent(QDropEvent *e)
{
	QStringList files;
	QString m = "";

	if(QUriDrag::decodeLocalFiles(e, files))
	{
		QStringList::Iterator i=files.begin();
		m = m + *i;

		showIfCan(m);
	}
}

void SQ_GLViewWidget::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept(QUriDrag::canDecode(e));
}

void SQ_GLViewWidget::contextMenuEvent(QContextMenuEvent*)
{
	QPopupMenu *menu = new QPopupMenu(this);

	KActionSeparator *pASep = new KActionSeparator;

	pARotate = new KAction("Rotate", 0, 0, this, SLOT(slotRotate()), sqApp->actionCollection(), "Rotate picture");
	pAZoomPlus = new KAction("Zoom+", 0, 0, this, SLOT(slotZoomPlus()), sqApp->actionCollection(), "Zoom+");
	pAZoomMinus = new KAction("Zoom-", 0, 0, this, SLOT(slotZoomMinus()), sqApp->actionCollection(), "Zoom-");

	pARotate->plug(menu);
	pASep->plug(menu);
	pAZoomPlus->plug(menu);
	pAZoomMinus->plug(menu);

	menu->exec(QCursor::pos());
}

//SLOTS
void SQ_GLViewWidget::slotRotate()
{
	glTranslatef(0.0f, 0.0f, 0.0f); glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	updateGL();
}

void SQ_GLViewWidget::slotZoomPlus()
{
	glScalef(1.0f+zoomfactor, 1.0f+zoomfactor, 1.0f);
	updateGL();
}

void SQ_GLViewWidget::slotZoomMinus()
{
	glScalef(1.0f-zoomfactor, 1.0f-zoomfactor, 1.0f);
	updateGL();
}

bool SQ_GLViewWidget::showIfCan(const QString &file)
{
	QFileInfo fm(file);

	if(!sqLibHandler->supports(fm.extension(false).upper()))
	{
		sqSBDecoded->setText("Format \""+ fm.extension(false)+"\" not supported");
		return false;
	}

	rgba = sqGLDecoder->decode(file);
	w = (GLfloat)sqGLDecoder->width();
	h = (GLfloat)sqGLDecoder->height();

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, (int)w, (int)h, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.5f);

	updateGL();

	QString status;
	status.sprintf("%s (%dx%d@%d bit)", fm.fileName().ascii(), (int)w, (int)h, sqGLDecoder->bpp());
	sqSBDecoded->setText(status);

	return true;
}
