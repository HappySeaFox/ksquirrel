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
#include <kconfig.h>
#include <kio/global.h>

#include <math.h>

#include "err.h"

SQ_GLViewWidget::SQ_GLViewWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
	isflippedV = 0;
	isflippedH = 0;

	memset(matrix, 0, sizeof(matrix));
	matrix[5] = matrix[0] = 1.0f;

	rgba = 0;
	setZoomFactor(0.10f);

	ZoomModelArray[0] = GL_LINEAR;
	ZoomModelArray[1] = GL_NEAREST;
	ShadeModelArray[0] = GL_FLAT;
	ShadeModelArray[1] = GL_SMOOTH;

	sqConfig->setGroup("GL view");
	int tp = sqConfig->readNumEntry("zoom model", 1);
	ZoomModel = ZoomModelArray[tp];
	tp = sqConfig->readNumEntry("shade model", 0);
       ShadeModel = ShadeModelArray[tp];

	setFocusPolicy(QWidget::WheelFocus);
	sqConfig->setGroup("GL view");
	setAcceptDrops(sqConfig->readBoolEntry("enable drop", true));

	pARotateLeft = new KAction("Rotate <=", 0, 0, this, SLOT(slotRotateLeft()), sqApp->actionCollection(), "SQ Rotate picture left");
	pARotateRight = new KAction("Rotate =>", 0, 0, this, SLOT(slotRotateRight()), sqApp->actionCollection(), "SQ Rotate picture right");
	pAZoomPlus = new KAction("Zoom+", 0, 0, this, SLOT(slotZoomPlus()), sqApp->actionCollection(), "SQ Zoom+");
	pAZoomMinus = new KAction("Zoom-", 0, 0, this, SLOT(slotZoomMinus()), sqApp->actionCollection(), "SQ Zoom-");
	pAFlipV = new KAction("Flip vertically", 0, 0, this, SLOT(slotFlipV()), sqApp->actionCollection(), "SQ flip_v");
	pAFlipH = new KAction("Flip horizontally", 0, 0, this, SLOT(slotFlipH()), sqApp->actionCollection(), "SQ flip_h");
	pAReset = new KAction("Normal view", 0, 0, this, SLOT(slotMatrixReset()), sqApp->actionCollection(), "SQ ResetGL");
}

SQ_GLViewWidget::~SQ_GLViewWidget()
{}

void SQ_GLViewWidget::initializeGL()
{
	const GLdouble koeff = 0.003921568;

	glEnable(GL_TEXTURE_2D);
	glClearColor(sqGLViewBGColor.red()*koeff, sqGLViewBGColor.green()*koeff, sqGLViewBGColor.blue()*koeff, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(SQ_GLViewWidget::ShadeModel);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SQ_GLViewWidget::ZoomModel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SQ_GLViewWidget::ZoomModel);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SQ_GLViewWidget::resizeGL(int width, int height)
{
	GLfloat w2 = (width+1) / 2.0f, h2 = (height+1) / 2.0f;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w2, w2, -h2, h2, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void SQ_GLViewWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	GLfloat half_w = (GLfloat)w / 2.0f, half_h = (GLfloat)h / 2.0f;

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-half_w, half_h);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( half_w, half_h);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( half_w, -half_h);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-half_w, -half_h);
	glEnd();
}

void SQ_GLViewWidget::setZoomFactor(const GLfloat &newfactor)
{
	zoomfactor = newfactor;
}

const GLfloat SQ_GLViewWidget::getZoomFactor() const
{
	return zoomfactor;
}

void SQ_GLViewWidget::wheelEvent(QWheelEvent *e)
{
	if(e->delta() > 0 && e->state() == Qt::ControlButton)
		matrix_zoom(2.0f, width()/2, height()/2);
	else if(e->delta() < 0 && e->state() == Qt::ControlButton)
		matrix_zoom(0.5f, width()/2, height()/2);
	else if(e->delta() > 0 && e->state() == Qt::ShiftButton)
		matrix_zoom(0.1f, width()/2, height()/2);
	else if(e->delta() < 0 && e->state() == Qt::ShiftButton)
		matrix_zoom(0.90f, width()/2, height()/2);
	else { e->ignore(); return; }

	updateGL();
}

void SQ_GLViewWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->state() == Qt::LeftButton || e->state() == Qt::NoButton)
	{
		xmoveold = e->x();
		ymoveold = e->y();
	}
	else e->ignore();
}

void SQ_GLViewWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(e->state() == Qt::LeftButton)
	{
		xmove = e->x();
		ymove = e->y();

		matrix_move(xmove-xmoveold, -ymove+ymoveold);

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
		matrix_move(-1, 0);
	else if(e->key() == Qt::Key_Right && e->state() == Qt::NoButton)
		matrix_move(1, 0);
	else if(e->key() == Qt::Key_Up && e->state() == Qt::NoButton)
		matrix_move(0, -1);
	else if(e->key() == Qt::Key_Down && e->state() == Qt::NoButton)
		matrix_move(0, 1);
	else if(e->key() == Qt::Key_Plus  && (e->state() == Qt::Keypad || e->state() == Qt::NoButton))
		slotZoomPlus();
	else if(e->key() == Qt::Key_Minus  && (e->state() == Qt::Keypad || e->state() == Qt::NoButton))
		slotZoomMinus();
	else if(e->key() == Qt::Key_Plus && (e->state() == Qt::ControlButton|Qt::Keypad || e->state() == Qt::ControlButton))
		matrix_zoom(2.0f, width()/2, height()/2);
	else if(e->key() == Qt::Key_Minus && (e->state() == Qt::ControlButton|Qt::Keypad || e->state() == Qt::ControlButton))
		matrix_zoom(0.5f, width()/2, height()/2);
	else if(e->key() == Qt::Key_V && e->state() == Qt::NoButton)
		slotFlipV();
	else if(e->key() == Qt::Key_H && e->state() == Qt::NoButton)
		slotFlipH();
	else if(e->key() == Qt::Key_Left && e->state() == Qt::ControlButton)
		slotRotateLeft();
	else if(e->key() == Qt::Key_Right && e->state() == Qt::ControlButton)
		slotRotateRight();
	else if(e->key() == Qt::Key_R && e->state() == Qt::NoButton)
		slotMatrixReset();
	else if(e->key() == Qt::Key_Up && e->state() == Qt::ControlButton)
		matrix_rotate(180.0f);
	else if(e->key() == Qt::Key_Down && e->state() == Qt::ControlButton)
		matrix_rotate(-180.0f);
	else if(e->key() == Qt::Key_Left && e->state() == Qt::ShiftButton)
		matrix_rotate(-1.0f);
	else if(e->key() == Qt::Key_Right && e->state() == Qt::ShiftButton)
		matrix_rotate(1.0f);
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

	menu->exec(QCursor::pos());
}

//SLOTS
void SQ_GLViewWidget::slotZoomPlus()
{
	matrix_zoom(1.10f, width()/2, height()/2);
}

void SQ_GLViewWidget::slotZoomMinus()
{
	matrix_zoom(0.90f, width()/2, height()/2);
}

void SQ_GLViewWidget::slotRotateLeft()
{
	sqConfig->setGroup("GL view");
	matrix_rotate(-(GLfloat)sqConfig->readNumEntry("angle", 90));
}

void SQ_GLViewWidget::slotRotateRight()
{
	sqConfig->setGroup("GL view");
	matrix_rotate((GLfloat)sqConfig->readNumEntry("angle", 90));
}

/*
	 operations with matrices are taken from GLiv =)

	thanks to Guillaume Chazarian.
*/
void SQ_GLViewWidget::flip(int id)
{
	matrix[id] *= -1.0;
	matrix[id + 1] *= -1.0;
	matrix[id + 3] *= -1.0;

	write_gl_matrix();
}

void SQ_GLViewWidget::slotFlipH()
{
	isflippedH = ~isflippedH;
	flip(0);
}

void SQ_GLViewWidget::slotFlipV()
{
	isflippedV = ~isflippedV;
	flip(4);
}

void SQ_GLViewWidget::slotMatrixReset()
{
	matrix_reset();
}

void SQ_GLViewWidget::write_gl_matrix(void)
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

	glLoadMatrixf(transposed);
	gluLookAt(0,0,5, 0,0,0, 0,1,0);
}

void SQ_GLViewWidget::matrix_move(GLfloat x, GLfloat y)
{
	MATRIX_X += x;
	MATRIX_Y += y;

	write_gl_matrix();
}

void SQ_GLViewWidget::matrix_reset(void)
{
	int i;

	for (i = 0; i < 8; i++)
		matrix[i] = (GLfloat) (i % 5 == 0);

	write_gl_matrix();
}

void SQ_GLViewWidget::matrix_zoom(GLfloat ratio, GLfloat x, GLfloat y)
{
	GLfloat offset_x, offset_y;

	offset_x = width() / 2.0 - x;
	offset_y = height() / 2.0 - y;

	matrix_move(offset_x, offset_y);

	MATRIX_C1 *= ratio;
	MATRIX_S1 *= ratio;
	MATRIX_X *= ratio;
	MATRIX_S2 *= ratio;
	MATRIX_C2 *= ratio;
	MATRIX_Y *= ratio;

	matrix_move(-offset_x, -offset_y);

	write_gl_matrix();
}

/*
 * Rotation:         Product:
 * cos   sin  0  0 | c1*cos+s2*sin   s1*cos+c2*sin   0  x*cos+y*sin
 * -sin  cos  0  0 | -c1*sin+s2*cos  -s1*sin+c2*cos  0  -x*sin+y*cos
 * 0     0    1  0 | 0               0               1  0
 * 0     0    0  1 | 0               0               0  1
 */
void SQ_GLViewWidget::matrix_rotate(GLfloat angle)
{
	double cosine, sine, rad;
	GLfloat c1, s1, c2, s2, x, y;
	const double rad_const = 0.017453;

	rad = angle * rad_const;
	cosine = cos(rad);
	sine = sin(rad);

	c1 = MATRIX_C1;
	c2 = MATRIX_C2;
	s1 = MATRIX_S1;
	s2 = MATRIX_S2;
	x = MATRIX_X;
	y = MATRIX_Y;

	MATRIX_C1 = c1 * cosine + s2 * sine;
	MATRIX_S1 = s1 * cosine + c2 * sine;
	MATRIX_S2 = -c1 * sine + s2 * cosine;
	MATRIX_C2 = -s1 * sine + c2 * cosine;
	MATRIX_X = x * cosine + y * sine;
	MATRIX_Y = -x * sine + y * cosine;

	write_gl_matrix();
}

bool SQ_GLViewWidget::showIfCan(const QString &file)
{
	QFileInfo fm(file);
	const char *name = file.ascii();
	static SQ_LIBRARY *lib;
	static QString status;
	unsigned int i;

	if(!sqLibHandler->supports(fm.extension(false).upper()))
	{
		sqSBDecoded->setText("Format \""+ fm.extension(false)+"\" not supported");
		return false;
	}

	sqLibHandler->setCurrentLibrary(fm.extension(false));
	lib = sqLibHandler->getCurrentLibrary();

	lib->fmt_init(&finfo, name);
	i = lib->fmt_read_info(finfo);

	if(i != SQERR_OK)
	{
		switch(i)
		{
			case SQERR_BADFILE: status = "      File corrupted !      "; break;
			case SQERR_NOMEMORY: status = "Library couldn't alloc memory for internal usage !"; break;

			default: status = "Library returned unknown error code.";
		}

		QMessageBox::warning(sqApp, "Decoding", status, QMessageBox::Ok, QMessageBox::NoButton);
		return false;
	}
	
	rgba = (RGBA*)realloc(rgba, finfo->w * finfo->h * sizeof(RGBA));

	if(rgba == NULL)
	{
		status.sprintf("Oops! Memory realloc failed.\n\nI tried to realloc memory for image:\n\nwidth: %ld\nheight: %ld\nbpp: %d\nTotal needed: ", finfo->w, finfo->h, finfo->bpp);
		status = status + KIO::convertSize(finfo->w * finfo->h * sizeof(RGBA)) + " of memory.";
		QMessageBox::warning(sqApp, "Decoding", status, QMessageBox::Ok, QMessageBox::NoButton);
		return false;
	}
	
	memset(rgba, 255, finfo->w * finfo->h * sizeof(RGBA));

	w = (GLfloat)finfo->w;
	h = (GLfloat)finfo->h;

	status.sprintf("%s (%ldx%ld@%d bit)", fm.fileName().ascii(), finfo->w, finfo->h, finfo->bpp);
	sqSBDecoded->setText(status);

	// step-by-step decoding & displaying is <NI>, so we can only decode a file like ver.0.1.3
	for(i = 0;i < finfo->h;i++)
		lib->fmt_read_scanline(finfo, rgba + i*finfo->w);

	lib->fmt_close(finfo);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	matrix_reset();

	if(finfo->needflip) slotFlipV();

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, finfo->w, finfo->h, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	updateGL();

	return true;
}
