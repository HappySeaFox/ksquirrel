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
#include <qdir.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <kaction.h>
#include <kcursor.h>
#include <kwin.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kapp.h>

#include <math.h>

#include "ksquirrel.h"
#include "sq_libraryhandler.h"
#include "sq_config.h"
#include "sq_widgetstack.h"
#include "sq_glviewwidget.h"
#include "sq_glbase.h"
#include "err.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

SQ_GLViewWidget::SQ_GLViewWidget(QWidget *parent, const char *name) : QGLWidget(parent, name)
{
	isflippedV = 0;
	isflippedH = 0;
	memset(matrix, 0, sizeof(matrix));
	matrix[5] = matrix[0] = 1.0f;
	rgba = 0;
	curangle = 0.0f;
	ZoomModelArray[0] = GL_LINEAR;
	ZoomModelArray[1] = GL_NEAREST;
	ShadeModelArray[0] = GL_FLAT;
	ShadeModelArray[1] = GL_SMOOTH;

	zoomfactor = sqConfig->readNumEntry("GL view", "zoom", 25);
	movefactor = sqConfig->readNumEntry("GL view", "move", 5);

	cusual = QCursor(QPixmap::fromMimeSource(locate("appdata", "cursors/usual.png")));
	cdrag = QCursor(QPixmap::fromMimeSource(locate("appdata", "cursors/drag.png")));

	setCursor(cusual);

	int tp = sqConfig->readNumEntry("GL view", "zoom model", 1);
	ZoomModel = ZoomModelArray[tp];
	tp = sqConfig->readNumEntry("GL view", "shade model", 0);
	ShadeModel = ShadeModelArray[tp];

	setFocusPolicy(QWidget::WheelFocus);
	setAcceptDrops(sqConfig->readBoolEntry("GL view", "enable drop", true));

	pARotateLeft = new KAction("Rotate left", QIconSet(sqLoader->loadIcon("undo", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("undo", KIcon::Desktop, KIcon::SizeMedium)), 0, this, SLOT(slotRotateLeft()), sqApp->actionCollection(), "SQ Rotate picture left");
	pARotateRight = new KAction("Rotate right", QIconSet(sqLoader->loadIcon("redo", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("redo", KIcon::Desktop, KIcon::SizeMedium)), 0, this, SLOT(slotRotateRight()), sqApp->actionCollection(), "SQ Rotate picture right");
	pAZoomPlus = new KAction("Zoom +", QIconSet(sqLoader->loadIcon("viewmag+", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("viewmag+", KIcon::Desktop, KIcon::SizeMedium)), 0, this, SLOT(slotZoomPlus()), sqApp->actionCollection(), "SQ Zoom+");
	pAZoomMinus = new KAction("Zoom -", QIconSet(sqLoader->loadIcon("viewmag-", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("viewmag-", KIcon::Desktop, KIcon::SizeMedium)), 0, this, SLOT(slotZoomMinus()), sqApp->actionCollection(), "SQ Zoom-");
	pAFlipV = new KAction("Flip vertically", 0, 0, this, SLOT(slotFlipV()), sqApp->actionCollection(), "SQ flip_v");
	pAFlipH = new KAction("Flip horizontally", 0, 0, this, SLOT(slotFlipH()), sqApp->actionCollection(), "SQ flip_h");
	pAReset = new KAction("Normalize", QIconSet(sqLoader->loadIcon("reload", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("reload", KIcon::Desktop, KIcon::SizeMedium)), 0, this, SLOT(slotMatrixReset()), sqApp->actionCollection(), "SQ ResetGL");
	pAHelp = new KAction("Some help", QIconSet(sqLoader->loadIcon("help", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("help", KIcon::Desktop, KIcon::SizeMedium)), 0, this, SLOT(slotSomeHelp()), sqApp->actionCollection(), "SQ GL some help");
	pAClose = new KAction("Close", QIconSet(sqLoader->loadIcon("exit", KIcon::Desktop, KIcon::SizeSmall), sqLoader->loadIcon("exit", KIcon::Desktop, KIcon::SizeMedium)), 0, this, SLOT(slotCloseGLView()), sqApp->actionCollection(), "SQ GL close");

	KCursor::setAutoHideCursor(this, true);
	KCursor::setHideCursorDelay(3000);

	pARender = new KAction("Render into file", 0, 0, this, SLOT(slotRenderPixmapIntoFile()), sqApp->actionCollection(), "SQ render GL into file");

	connect(this, SIGNAL(matrixChanged()), SLOT(slotSetMatrixParamsString()));
	connect(this, SIGNAL(showImage(const QString&)), SLOT(slotShowImage(const QString&)));
}

SQ_GLViewWidget::~SQ_GLViewWidget()
{}

void SQ_GLViewWidget::initializeGL()
{
	glEnable(GL_TEXTURE_2D);
	setClearColor();
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
	glViewport(-width/2, -height/2, width*2, height*2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-width, width, -height, height, 0.1f, 100.0f);
	gluLookAt(0,0,1, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);
}

void SQ_GLViewWidget::paintGL()
{
	GLfloat half_w = (GLfloat)w / 2.0f, half_h = (GLfloat)h / 2.0f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-half_w, half_h);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( half_w, half_h);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( half_w, -half_h);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-half_w, -half_h);
	glEnd();

	glDisable(GL_TEXTURE_2D);
/*
	const GLfloat border = 20.0f;

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-half_w2-border, half_h2+border);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( half_w2+border, half_h2+border);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( half_w2+border, -half_h2-border);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-half_w2-border, -half_h2-border);
	glEnd();*/
}

void SQ_GLViewWidget::slotCloseGLView()
{
	sqGLView->close();
}

void SQ_GLViewWidget::slotSetMatrixParamsString()
{
	QString str;

	str.sprintf("%s%s %.1f%% (%.1f deg)", ((isflippedV)?"V":""), ((isflippedH)?"H":""), get_zoom() * 100.0f, get_angle());

	sqSBGLreport->setText(str);
}

void SQ_GLViewWidget::setZoomFactor(const GLfloat &newfactor)
{
	zoomfactor = newfactor;
}

const GLfloat SQ_GLViewWidget::getZoomFactor() const
{
	return zoomfactor;
}

void SQ_GLViewWidget::setMoveFactor(const GLfloat &newfactor)
{
	movefactor = newfactor;
}

const GLfloat SQ_GLViewWidget::getMoveFactor() const
{
	return movefactor;
}

void SQ_GLViewWidget::wheelEvent(QWheelEvent *e)
{
	if(e->delta() < 0 && e->state() == Qt::NoButton)
		sqWStack->emitNextSelected();
	else if(e->delta() > 0 && e->state() == Qt::NoButton)
		sqWStack->emitPreviousSelected();
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

void SQ_GLViewWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->state() == Qt::LeftButton || e->state() == Qt::NoButton)
	{
		setCursor(cdrag);

		xmoveold = e->x();
		ymoveold = e->y();
	}
	else e->accept();
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
		e->accept();
}

void SQ_GLViewWidget::mouseReleaseEvent(QMouseEvent *e)
{
	setCursor(cusual);
	e->accept();
}

void SQ_GLViewWidget::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Left && (e->state() == Qt::Keypad || e->state() == Qt::NoButton))
		matrix_move(movefactor, 0);
	else if(e->key() == Qt::Key_Right && (e->state() == Qt::Keypad || e->state() == Qt::NoButton))
		matrix_move(-movefactor, 0);
	else if(e->key() == Qt::Key_Up && (e->state() == Qt::Keypad || e->state() == Qt::NoButton))
		matrix_move(0, -movefactor);
	else if(e->key() == Qt::Key_Down && (e->state() == Qt::Keypad || e->state() == Qt::NoButton))
		matrix_move(0, movefactor);
	else if(e->key() == Qt::Key_Plus  && (e->state() == Qt::Keypad || e->state() == Qt::NoButton))
		pAZoomPlus->activate();
	else if(e->key() == Qt::Key_Minus  && (e->state() == Qt::Keypad || e->state() == Qt::NoButton))
		pAZoomMinus->activate();
	else if(e->key() == Qt::Key_Plus && (e->state() == Qt::ControlButton|Qt::Keypad || e->state() == Qt::ControlButton))
		matrix_zoom(2.0f);
	else if(e->key() == Qt::Key_Minus && (e->state() == Qt::ControlButton|Qt::Keypad || e->state() == Qt::ControlButton))
		matrix_zoom(0.5f);
	else if(e->key() == Qt::Key_V && e->state() == Qt::NoButton)
		pAFlipV->activate();
	else if(e->key() == Qt::Key_H && e->state() == Qt::NoButton)
		pAFlipH->activate();
	else if(e->key() == Qt::Key_Left && e->state() == Qt::ControlButton)
		pARotateLeft->activate();
	else if(e->key() == Qt::Key_Right && e->state() == Qt::ControlButton)
		pARotateRight->activate();
	else if(e->key() == Qt::Key_R && e->state() == Qt::NoButton)
		pAReset->activate();
	else if(e->key() == Qt::Key_R && e->state() == Qt::ControlButton)
		pARender->activate();
	else if(e->key() == Qt::Key_Up && e->state() == Qt::ControlButton)
		matrix_rotate(180.0f);
	else if(e->key() == Qt::Key_Down && e->state() == Qt::ControlButton)
		matrix_rotate(-180.0f);
	else if(e->key() == Qt::Key_Left && e->state() == Qt::ShiftButton)
		matrix_rotate(-1.0f);
	else if(e->key() == Qt::Key_Right && e->state() == Qt::ShiftButton)
		matrix_rotate(1.0f);
	else if(e->key() == Qt::Key_Space && e->state() == Qt::NoButton)
		sqWStack->emitNextSelected();
	else if(e->key() == Qt::Key_BackSpace && e->state() == Qt::NoButton)
		sqWStack->emitPreviousSelected();
	else
		e->accept();
}

void SQ_GLViewWidget::dropEvent(QDropEvent *e)
{
	QStringList files;
	QString m;

	if(QUriDrag::decodeLocalFiles(e, files))
	{
		QStringList::Iterator i=files.begin();

		m = *i;

		emitShowImage(m);
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
	pAHelp->plug(menu);

	if(sqGLView->isSeparate())
	{
		pASep->plug(menu);
		pAClose->plug(menu);
	}

	menu->exec(QCursor::pos());

	delete pASep;
	delete menu;
}

//SLOTS
void SQ_GLViewWidget::slotSomeHelp()
{
	static const QString help = "\
Shift+\"->\" rotate image for 1 degree right\nShift+\"<-\" rotate image for 1 degree left\n\n\
Ctrl+\"->\" rotate image for 90 degrees right\nCtrl+\"<-\" rotate image for 90 degrees left\n\
Ctrl+\"Up Arrow\" rotate image for 180 degrees right\nCtrl+\"Down Arrow\" rotate image for 180 degrees left\n\n\
\"Left Arrow\" move image for 5 pixels right\n\"Right Arrow\" move image for 5 pixels left\n\n\
\"Up Arrow\" move image for 5 pixels down\n\"Down Arrow\" move image for 5 pixels up\n\n\
\"+\" zoom in\n\"-\" zoom out\n\n\
\"Ctrl + '+'\" zoom in 2x\n\"Ctrl + '-'\" zoom out 2x\n\n\
\"V\" flip vertically\n\"H\" flip horizontally\n\n\
\"R\" reset GL view (restore zoom, position, etc.)\
";

	QWhatsThis *gl_tip = new QWhatsThis(this);
	gl_tip->display(help);
}

void SQ_GLViewWidget::slotZoomPlus()
{
	matrix_zoom(1.0f+(GLfloat)zoomfactor/100.0f);
}

void SQ_GLViewWidget::slotZoomMinus()
{
	matrix_zoom(1.0f/(1.0f+(GLfloat)zoomfactor/100.0f));
}

void SQ_GLViewWidget::slotRotateLeft()
{
	matrix_rotate(-(GLfloat)sqConfig->readNumEntry("GL view", "angle", 90));
}

void SQ_GLViewWidget::slotRotateRight()
{
	matrix_rotate((GLfloat)sqConfig->readNumEntry("GL view", "angle", 90));
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

void SQ_GLViewWidget::write_gl_matrix()
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

	emit matrixChanged();

	updateGL();
}

void SQ_GLViewWidget::matrix_move(GLfloat x, GLfloat y)
{
	MATRIX_X += x;
	MATRIX_Y += y;

	write_gl_matrix();
}

void SQ_GLViewWidget::matrix_reset()
{
	int i;

	for (i = 0; i < 8; i++)
		matrix[i] = (GLfloat) (i % 5 == 0);

	curangle = 0.0f;
	isflippedH = isflippedV = 0;

	if(finfo)
		if(finfo->needflip)
			slotFlipV();

	write_gl_matrix();
}

void SQ_GLViewWidget::matrix_zoom(GLfloat ratio)
{
	matrix_move(0, 0);

	MATRIX_C1 *= ratio;
	MATRIX_S1 *= ratio;
	MATRIX_X *= ratio;
	MATRIX_S2 *= ratio;
	MATRIX_C2 *= ratio;
	MATRIX_Y *= ratio;

	matrix_move(-0, -0);

	write_gl_matrix();
}

GLfloat SQ_GLViewWidget::get_zoom() const
{
	return hypotf(MATRIX_C1, MATRIX_S1);
}

GLfloat SQ_GLViewWidget::get_angle() const
{
	return curangle;
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

	curangle += angle;

	if(curangle == 360.0f || curangle == -360.0f)
		curangle = 0.0f;
	else if(curangle > 360.0f)
		curangle = curangle - 360.0f;
	else if(curangle < -360.0f)
		curangle = curangle + 360.0f;

	write_gl_matrix();
}

void SQ_GLViewWidget::emitShowImage(const QString &file)
{
	sqApp->raiseGLWidget();

	sqGLView->setCaption(file);

	emit showImage(file);
}

void SQ_GLViewWidget::emitShowImage(const KURL &url)
{
	sqApp->raiseGLWidget();

	sqGLView->setCaption(url.path());

	emit showImage(url.path());
}

void SQ_GLViewWidget::setClearColor()
{
	QColor color;
	const GLdouble koeff = 0.003921568;

	if(sqConfig->readBoolEntry("GL view", "system color", true))
		color = KGlobalSettings::baseColor();
	else
		color.setNamedColor(sqConfig->readEntry("GL view", "GL view background", "#cccccc"));

	glClearColor(color.red()*koeff, color.green()*koeff, color.blue()*koeff, 0.0f);
}

void SQ_GLViewWidget::setTextureParams()
{
	int tp = sqConfig->readNumEntry("GL view", "zoom model", 1);
	ZoomModel = ZoomModelArray[tp];
	tp = sqConfig->readNumEntry("GL view", "shade model", 0);
	ShadeModel = ShadeModelArray[tp];

	glEnable(GL_TEXTURE_2D);
	glShadeModel(SQ_GLViewWidget::ShadeModel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SQ_GLViewWidget::ZoomModel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SQ_GLViewWidget::ZoomModel);

	updateGL();
}

void SQ_GLViewWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
//	close();
	e->accept();
}

void SQ_GLViewWidget::slotRenderPixmapIntoFile()
{
	QString file, final;
	QImage im_rendered = grabFrameBuffer(true);
	QPixmap px_rendered;
	QTime time = QTime::currentTime(Qt::LocalTime);
	QDate date = QDate::currentDate(Qt::LocalTime);

	px_rendered.convertFromImage(im_rendered, AvoidDither);

	file.sprintf("ksquirrel GL pixmap - %d.%d.%d - %d.%d.%d.png", date.day(), date.month(), date.year(), time.hour(), time.minute(), time.second());
	final = QDir::homeDirPath() + "/" + file;

	px_rendered.save(final, "PNG");
}

unsigned long findCloserPower2(unsigned long num)
{
	unsigned long i = 1;

	do
	{
		i <<= 1;
	}while(num >= i);

	return i;
}

void SQ_GLViewWidget::slotShowImage(const QString &file)
{
	QFileInfo 					fm(file);
	const char 				*name = file.ascii();
	SQ_LIBRARY				*lib;
	QString					status;
	unsigned int				i;

/*
	// @todo maybe remove it ?
	if(!sqLibHandler->supports(fm.extension(false)))
	{
		sqSBDecoded->setText("Format \""+ fm.extension(false)+"\" not supported");
		return;
	}
*/
	sqLibHandler->setCurrentLibrary(fm.extension(false));
	lib = sqLibHandler->getCurrentLibrary();

	lib->fmt_init(&finfo, name);
	i = lib->fmt_read_info(finfo);

	real_size = finfo->w * finfo->h * 4;

	dump = QString("Directory: %1\nFile: %2\nSize: %3\nUncompressed size: %4\nCompress ratio: %5\n\n")
			.arg(fm.dirPath(true))
			.arg(fm.fileName())
			.arg(KIO::convertSize(fm.size()))
			.arg(KIO::convertSize(real_size))
			.arg((double)real_size / fm.size());

	dump = dump +  finfo->dump;

//	QMessageBox::warning(sqApp, "Decoding", dump, QMessageBox::Ok, QMessageBox::NoButton);

	if(i != SQERR_OK)
	{
		switch(i)
		{
			case SQERR_BADFILE: status = "File corrupted !\n\nTried library: " + lib->libpath + "\nHandled extensions: " + lib->sinfo; break;
			case SQERR_NOMEMORY: status = "Library couldn't alloc memory for internal usage !"; break;
			case SQERR_NOTSUPPORTED: status = "This format type is not supported."; break;

			default: status = "Library returned unknown error code.";
		}

		QMessageBox::warning(sqGLView, "Decoding", status, QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}

	realW = findCloserPower2(finfo->w);
	realH = findCloserPower2(finfo->h);

	offsetX = (realW - finfo->w) / 2;
	offsetY = (realH - finfo->h) / 2;

	pict_size = realW * realH * 4;
	rgba = (RGBA*)realloc(rgba, pict_size);
//	scan = (RGBA*)realloc(scan, realW * 4);

	if(rgba == NULL)
	{
		status.sprintf("Oops! Memory realloc failed.\n\nI tried to realloc memory for image:\n\nwidth: %ld\nheight: %ld\nbpp: %d\nTotal needed: ", realW, realH, finfo->bpp);
		status = status + KIO::convertSize(pict_size) + " of memory.";
		QMessageBox::warning(sqGLView, "Decoding", status, QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
/*
	if(scan == NULL)
	{
		status.sprintf("Oops! Memory realloc failed.\n\nI tried to realloc memory for one scanline:\n\nwidth: %ld\nTotal needed: ", realW);
		status = status + KIO::convertSize(realW * 4) + " of memory.";
		QMessageBox::warning(sqApp, "Decoding", status, QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
*/
	memset(rgba, 0, pict_size);
//	memset(scan, 0, realW*4);

	w = (GLfloat)realW;
	h = (GLfloat)realH;

	status.sprintf("%ldx%ld@%d bit", finfo->w, finfo->h, finfo->bpp);
	sqSBDecoded->setText(status);

	const int param = offsetX + offsetY*realW;

	glTexImage2D(GL_TEXTURE_2D, 0, 4, realW, realH, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	matrix_reset();

	posX = -(double)finfo->w / 2.0f;
	posY = (double)finfo->h / 2.0f - 1;
	rgba_next = rgba + param;

	int swapon = sqConfig->readNumEntry("GL view", "swap on", 2);

	for(i = 0;i < finfo->h;i++)
	{
		lib->fmt_read_scanline(finfo, rgba_next + realW*i);
		glRasterPos2d(posX, posY-i);
		glDrawPixels(realW, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgba_next + realW*i);
		if(!(i%swapon)) swapBuffers();
	}

	lib->fmt_close(finfo);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, realW, realH, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	updateGL();
	
	free(rgba);
	rgba = 0L;
}
