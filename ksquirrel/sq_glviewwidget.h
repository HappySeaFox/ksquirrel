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

		int xmoveold, ymoveold, xmove, ymove;

	public:
		SQ_GLViewWidget(QWidget *parent = 0, const char *name = 0);
		~SQ_GLViewWidget();

		void setZoomFactor(const GLfloat &newfactor);
		const GLfloat getZoomFactor() const;

		void setFogColor(const GLfloat,const GLfloat,const GLfloat,const GLfloat);
		const GLfloat* getFogColor() const;
		bool showIfCan(const QString &file);

		GLint 			ZoomModel, ShadeModel;
		SQ_Decoder		*decoder;

		KAction *pARotate, *pAZoomPlus, *pAZoomMinus;

	protected:
		void initializeGL();
		void	paintGL();
		void resizeGL(int,int);
		void wheelEvent(QWheelEvent *);
		void keyPressEvent(QKeyEvent *);

		void mousePressEvent(QMouseEvent *);
		void mouseReleaseEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);

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
