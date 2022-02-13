/***************************************************************************
                          sq_glviewwidget.h  -  description
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


#ifndef SQ_GLWIDGET_H
#define SQ_GLWIDGET_H

#include <kurl.h>

#include <qgl.h>
#include <qcursor.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qrect.h>

//#define SQ_NEED_RGBA_OPERATOR
#include "defs.h"

#define	MATRIX_C1	matrix[0]
#define	MATRIX_S1	matrix[1]
#define	MATRIX_X	matrix[3]
#define	MATRIX_S2	matrix[4]
#define	MATRIX_C2	matrix[5]
#define	MATRIX_Y	matrix[7]
#define	MATRIX_Z	matrix[11]

class KAction;
class KActionCollection;
class KToggleAction;
class KPopupMenu;
class KRadioAction;
class KToolBar;

class QTimer;
class QWidgetStack;
class QToolButton;
class QPainter;

class SQ_DirOperator;
class SQ_QuickBrowser;
class SQ_LIBRARY;

struct Parts;
struct Part;
class MemoryPart256;
class MemoryPart128;
class MemoryPart64;
class MemoryPart32;

class SQ_GLWidget : public QGLWidget
{
	Q_OBJECT

	public:
		SQ_GLWidget(QWidget *parent = 0, const char *name = 0);
		~SQ_GLWidget();

		void setZoomFactor(const GLfloat &newfactor);
		void setMoveFactor(const GLfloat &newfactor);
		void setRotateFactor(const GLfloat &newfactor);

		void setClearColor();

		void matrix_move(GLfloat x, GLfloat y);
		void matrix_move_z(GLfloat z);
		bool matrix_zoom(GLfloat ratio);
		void matrix_reset();
		void matrix_pure_reset();
		void matrix_pure_reset_notflip();
		void matrix_push();
		void matrix_pop();
		void write_gl_matrix();
		void matrix_rotate(GLfloat angle);
		void flip(int);
		void flip_h();
		void flip_v();

		GLfloat get_zoom() const;
		GLfloat get_zoom_pc() const;
		GLfloat get_angle() const;

		void updateGLA() { updateGL(); }

		bool actions() const;
		bool actionsHidden() const;
		int zoomType();
		void createQuickBrowser();
		void glInitA() { QGLWidget::glInit(); }
		void startAnimation();
		void stopAnimation();
		bool manualBlocked();

	protected:
		void initializeGL();
		void paintGL();
		void resizeGL(int,int);
		void wheelEvent(QWheelEvent *);
		void keyPressEvent(QKeyEvent *);
		void paletteChange(const QPalette &oldPalette);

		void dragEnterEvent(QDragEnterEvent *);
		void dropEvent(QDropEvent *);

		void contextMenuEvent(QContextMenuEvent *);
		void mousePressEvent(QMouseEvent *);
		void mouseReleaseEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);

	private:
		void createActions();
		void createToolbar();
		void draw_background(void *bits, unsigned int *tex, int dim, GLfloat w, GLfloat h, bool &bind, bool deleteOld);
		void coordChanged();
		void matrixChanged();
		void setupBits(Parts *p, RGBA *b, int y);
		void adjustTimeFromMsecs(int &msecs, int &secs);
		void showFrames(int);
		void jumpToImage(bool);
		void nextImage();
		void prevImage();
		void showHelp();
		void toggleDrawingBackground();
		void showExternalTools();

		bool prepare();
		bool zoomRect(const QRect &r);

		void findCloserTiles(int w, int h, int &tile1, int &tile2);

	public slots:
		void slotStartDecoding(const QString &file, bool = false);
		void slotStartDecoding(const KURL &url);

	private slots:
		void slotZoomW();
		void slotZoomH();
		void slotZoomWH();
		void slotZoomPlus();
		void slotZoom100();
		void slotZoomLast();
		void slotZoomMinus();
		void slotZoomIfLess(bool);
		void slotRotateLeft();
		void slotRotateRight();
		void slotFlipV();
		void slotFlipH();
		void slotMatrixReset();
		void slotProperties();
		void slotShowToolbar();
		void slotHideToolbar();
		void slotStepToolbarMove();
		void slotShowQuick(bool);
		void slotDecode();
		void slotHideToolbars(bool);
		void slotToggleStatus(bool);
		void slotFirst();
		void slotLast();
		void slotNext();
		void slotPrev();
		void slotZoomMenu();
		void slotAnimateNext();
		void slotToggleAnimate();
		void slotSetCurrentImage(int);
		void slotShowImages();
		void slotImagedHidden();

	public:
		KAction			*pARotateLeft, *pARotateRight, *pAZoomPlus, *pAZoomMinus,
						*pAFlipV, *pAFlipH, *pAReset, *pAClose, *pAProperties, *pANext, *pAPrev, *pAHide, *pAShow,
						*pAFirst, *pALast;

		QToolButton	*pAToolClose, 	*pAToolFull, *pAToolQuick, *pAToolZoom, *pAToolImages;

		SQ_QuickBrowser	*v;
		QWidgetStack 	*s;
		KToggleAction 	*pAFull, *pAQuick, *pAIfLess, *pAStatus;
		KToggleAction	*pAZoomW, *pAZoomH, *pAZoomWH, *pAZoom100, *pAHideToolbars, *pAZoomLast;
		SQ_DirOperator 	*quick;

	private:
		GLfloat 			matrix[12], saved[12];
		GLfloat			zoomfactor, movefactor, rotatefactor;
		GLfloat			curangle;
		fmt_info			*finfo;
		int				xmoveold, ymoveold, xmove, ymove, current;
		bool			reset_mode, decoded, blocked, blocked_force, isflippedV, isflippedH;
		QString			File, m_File;
		QCursor			cusual, cdrag, cZoomIn;
		QFileInfo		fm;
		QImage 		BGpixmap, BGquads;
		KToolBar		*toolbar, *toolbar2;
		QTimer			*timer_show, *timer_hide, *timer_decode, *timer_prev, *timer_next, *timer_anim;
		SQ_LIBRARY	*lib;
		int				steps;
		bool			m_hiding;
		int				zoom_type, old_id;
		KActionCollection*ac;
		RGBA			*next;
		unsigned int		texQuads, texPixmap;
		bool			changed, inMouse, crossDrawn;
		QPainter		*pRect;
		QRect			lastRect;
		KPopupMenu 	*zoom, *images;

		Parts 			*parts;
		int 				total, errors, tileSize;
		float			zoomFactor;

		void 			*mem_parts;
		MemoryPart256	*m256;
		MemoryPart128	*m128;
		MemoryPart64	*m64;
		MemoryPart32	*m32;
};

class MemoryPart256
{
	public:
		unsigned char part [65536 * sizeof(RGBA)];
};

class MemoryPart128
{
	public:
		unsigned char part [16384 * sizeof(RGBA)];
};

class MemoryPart64
{
	public:
		unsigned char part [4096 * sizeof(RGBA)];
};

class MemoryPart32
{
	public:
		unsigned char part [1024 * sizeof(RGBA)];
};

struct Part
{
	int x1, y1, x2, y2;
	unsigned int tex;
	GLuint list;
};

struct Parts
{
	int tilesx, tilesy, tiles, tileSize;
	int w, h, rw, rh;

	Part *m_parts;

	void setRWH(const int rw2, const int rh2);
	void setWH(const int w2, const int h2);
	void setTileSize(const int t);
	bool makeParts();
	void removeParts();
	void computeCoords();
};

#endif
