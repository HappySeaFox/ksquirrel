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

#ifndef SQ_GLVIEWWIDGET_H
#define SQ_GLVIEWWIDGET_H

#include <qgl.h>
#include <kaction.h>

/**
@author ckult
*/

#include "defs.h"

class SQ_Decoder;

// thanks to GLiv
static GLfloat matrix[8] =
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f
};

#define	MATRIX_C1	matrix[0]
#define	MATRIX_S1	matrix[1]
#define	MATRIX_X	matrix[3]
#define	MATRIX_S2	matrix[4]
#define	MATRIX_C2	matrix[5]
#define	MATRIX_Y	matrix[7]


class SQ_GLViewWidget : public QGLWidget
{
	Q_OBJECT

	private:
		RGBA	*rgba;

		GLuint	texture[1];
		GLfloat	w, h;
		GLfloat	zoomfactor;
		GLfloat	density;

		GLfloat fogcolor[4];

	public:
		SQ_GLViewWidget(QWidget *parent = 0, const char *name = 0);
		~SQ_GLViewWidget();

		void setZoomFactor(const GLfloat &newfactor);
		const GLfloat getZoomFactor() const;

		void setFogColor(const GLfloat,const GLfloat,const GLfloat,const GLfloat);
		const GLfloat* getFogColor() const;
		bool showIfCan(const QString &file);

		GLint 			ZoomModel, ShadeModel;
		GLint			ZoomModelArray[2], ShadeModelArray[2];
		SQ_Decoder		*decoder;

		KAction *pARotate, *pAZoomPlus, *pAZoomMinus;

	protected:
		void initializeGL();
		void	paintGL();
		void resizeGL(int,int);
		void wheelEvent(QWheelEvent *);
		void keyPressEvent(QKeyEvent *);

		void dragEnterEvent(QDragEnterEvent *);
		void dropEvent(QDropEvent *);

		void contextMenuEvent(QContextMenuEvent *);

	public slots:
		void slotRotate();
		void slotZoomPlus();
		void slotZoomMinus();
};

#define	sqGLDecoder		(SQ_GLViewWidget::decoder)
#define	sqGLRotate		(SQ_GLViewWidget::pARotate)
#define	sqGLZoomPlus	(SQ_GLViewWidget::pAZoomPlus)
#define	sqGLZoomMinus	(SQ_GLViewWidget::pAXoomMinus)

#endif
