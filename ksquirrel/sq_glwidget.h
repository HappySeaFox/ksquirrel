/***************************************************************************
                          sq_glviewwidget.h  -  description
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

/**
	@author ckult
*/

#ifndef SQ_GLWIDGET_H
#define SQ_GLWIDGET_H

#include <kurl.h>

#include <qgl.h>
#include <qcursor.h>
#include <qfileinfo.h>
#include <qimage.h>

#include "defs.h"

#define	MATRIX_C1	matrix[0]
#define	MATRIX_S1	matrix[1]
#define	MATRIX_X	matrix[3]
#define	MATRIX_S2	matrix[4]
#define	MATRIX_C2	matrix[5]
#define	MATRIX_Y	matrix[7]
#define	MATRIX_Z	matrix[11]

class KAction;
class KToggleAction;
class KRadioAction;
class KToolBar;

class QTimer;
class QWidgetStack;
class QPopupMenu;

class SQ_DirOperator;
class SQ_QuickBrowser;
class SQ_DecodingThread;
class SQ_LIBRARY;
class SQ_Server;

class SQ_GLWidget : public QGLWidget
{
	Q_OBJECT

	public:
		SQ_GLWidget(QWidget *parent = 0, const char *name = 0);
		~SQ_GLWidget();

		void setZoomFactor(const GLfloat &newfactor);
		const GLfloat getZoomFactor() const;

		void setMoveFactor(const GLfloat &newfactor);
		const GLfloat getMoveFactor() const;

		void setRotateFactor(const GLfloat &newfactor);
		const GLfloat getRotateFactor() const;

		void emitShowImage(const QString &file);
		void emitShowImage(const KURL &url);

		void setClearColor();
		void setTextureParams();

		void matrix_move(GLfloat x, GLfloat y);
		void matrix_move_z(GLfloat z);
		void matrix_zoom(GLfloat ratio);
		void matrix_reset(bool update = true);
		void matrix_pure_reset();
		void matrix_push();
		void matrix_pop();
		void write_gl_matrix(bool update = true);
		void matrix_rotate(GLfloat angle);
		void flip(int);
		void flip_h();
		void flip_v();

		GLfloat get_zoom() const;
		GLfloat get_angle() const;

		void update() { updateGL(); }
		void draw_background(void *bits, int dim, GLfloat w, GLfloat h);

		bool actions();
		int zoomType();
		void createDecodingThread();
		void createQuickBrowser();
		void glInit() { QGLWidget::glInit(); }

	protected:
		void initializeGL();
		void paintGL();
		void resizeGL(int,int);
		void wheelEvent(QWheelEvent *);
		void keyPressEvent(QKeyEvent *);

		void dragEnterEvent(QDragEnterEvent *);
		void dropEvent(QDropEvent *);

		void contextMenuEvent(QContextMenuEvent *);
		void mousePressEvent(QMouseEvent *);
		void mouseReleaseEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);
		void mouseDoubleClickEvent(QMouseEvent *);

	private:
		void createActions();
		void createToolbar();
		void createMenu();
		bool prepare();
		unsigned long findCloserPower2(unsigned long num);

	signals:
		void matrixChanged();

	public slots:
		void slotZoomW();
		void slotZoomH();
		void slotZoomWH();
		void slotZoomPlus();
		void slotZoom100();
		void slotZoomMinus();
		void slotZoomIfLess(bool);
		void slotRotateLeft();
		void slotRotateRight();
		void slotFlipV();
		void slotFlipH();
		void slotMatrixReset();
		void slotSetMatrixParamsString();
		void slotShowImage();
		void slotRenderPixmapIntoFile();
		void slotProperties();
		void slotShowToolbar();
		void slotHideToolbar();
		void slotStepToolbarMove();

	public:
		KAction			*pARotateLeft, *pARotateRight, *pAZoomPlus, *pAZoomMinus,
						*pAFlipV, *pAFlipH, *pAReset, *pARender, *pAClose, *pAProperties, *pANext, *pAPrev, *pAHide, *pAShow,
						*pAFirst, *pALast;
		SQ_QuickBrowser	*v;
		QWidgetStack 	*s;
		KToggleAction 	*pAFull, *pAQuick, *pAIfLess;
		KToggleAction	*pAZoomW, *pAZoomH, *pAZoomWH, *pAZoom100;
		SQ_DirOperator 	*quick;
		bool 			dec;
		SQ_Server		*server;

	private:
		unsigned long		pict_size, realW, realH;
		GLfloat 			matrix[12], saved[12];
		GLuint			texture[1];
		GLfloat			zoomfactor, movefactor, rotatefactor;
		GLfloat			curangle;
		GLint 			ZoomModel;
		RGBA			*rgba, *rgba_next;
		fmt_info			*finfo;
		int				xmoveold, ymoveold, xmove, ymove;
		unsigned			isflippedV, isflippedH;
		bool				reset_mode, decoded;
		QString			File;
		QCursor			cusual, cdrag;
		QFileInfo			fm;
		QImage 			BGpixmap, BGquads;
		unsigned char		BGborder[16];
		KToolBar			*toolbar, *toolbar2;
		QTimer			*timer_show, *timer_hide;
		SQ_DecodingThread	*thread;
		SQ_LIBRARY		*lib;
		int				steps;
		bool				m_hiding;
		int				zoom_type;
		QPopupMenu 		*menu;
};

#endif
