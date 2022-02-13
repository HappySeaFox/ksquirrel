/***************************************************************************
                          sq_glwidget.h  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qgl.h>
#include <qcursor.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qrect.h>

#include <kurl.h>

//#define SQ_NEED_RGBA_OPERATOR
#include "fmt_types.h"
#include "fmt_defs.h"

#include <vector>

using namespace std;

#define	MATRIX_C1	matrix[0]
#define	MATRIX_S1	matrix[1]
#define	MATRIX_X		matrix[3]
#define	MATRIX_S2	matrix[4]
#define	MATRIX_C2	matrix[5]
#define	MATRIX_Y		matrix[7]
#define	MATRIX_Z		matrix[11]

class KAction;
class KActionCollection;
class KToggleAction;
class KPopupMenu;
class KRadioAction;

class QTimer;
class QWidgetStack;
class QPainter;

class SQ_DirOperator;
class SQ_QuickBrowser;
class SQ_ToolButtonPopup;
class SQ_ToolButton;
class SQ_ToolBar;

struct SQ_LIBRARY;
class fmt_codec_base;

struct Parts;
struct Part;
struct MemoryPart256;
struct MemoryPart128;
struct MemoryPart64;
struct MemoryPart32;

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
		void matrix_push();
		void matrix_pop();
		void write_gl_matrix();
		void matrix_rotate(GLfloat angle);
		void matrix_rotate2(GLfloat angle);
		void flip(int, bool = true);
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
		void matrixChanged();

		static SQ_GLWidget* window();

	protected:
		void initializeGL();
		void paintGL();
		void resizeGL(int,int);
		void wheelEvent(QWheelEvent *);
		void keyPressEvent(QKeyEvent *);
		void paletteChange(const QPalette &oldPalette);

		void dragEnterEvent(QDragEnterEvent *);
		void dropEvent(QDropEvent *);

		void mousePressEvent(QMouseEvent *);
		void mouseReleaseEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);

	private:
		void createActions();
		void createToolbar();
		void draw_background(void *bits, unsigned int *tex, int dim, GLfloat w, GLfloat h, bool &bind, bool deleteOld);
		void coordChanged();
		void setupBits(Parts *p, RGBA *b, int y);
		void adjustTimeFromMsecs(int &msecs, int &secs);
		void showFrames(int);
		void jumpToImage(bool);
		void nextImage();
		void prevImage();
		void toggleDrawingBackground();
		void showExternalTools();
		void bindMarks(bool &first, bool deleteOld);
		void createMarks();
		QColor calculateAdjustedColor(QImage im, QColor rgb, bool color);
		void deleteWrapper();
		void updateCurrentFileInfo();
		void toogleTickmarks();
		void frameChanged();
		void internalZoom(const GLfloat &z);
		void createContextMenu(KPopupMenu *m);
		void prioritizeTextures(const int &cur, bool newbind = false);

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
		void slotImagedShown();
		void signalMapped(int);
		void slotShowHelp();

	public:
		KAction					*pARotateLeft, *pARotateRight, *pAZoomPlus, *pAZoomMinus,
								*pAFlipV, *pAFlipH, *pAReset, *pAClose, *pAProperties, *pANext,
								*pAPrev, *pAHide, *pAShow, *pAFirst, *pALast, *pAHelp;
		KToggleAction 			*pAFull, *pAQuick, *pAIfLess, *pAStatus;
		KToggleAction			*pAZoomW, *pAZoomH, *pAZoomWH, *pAZoom100, *pAHideToolbars, *pAZoomLast;

		QWidgetStack 			*s;

		SQ_ToolButton			*pAToolClose, 	*pAToolFull, *pAToolQuick;
		SQ_ToolButtonPopup		*pAToolZoom, *pAToolImages;
		SQ_QuickBrowser		*v;
		SQ_DirOperator 			*quick;

	private:
		KActionCollection	*ac;
		KPopupMenu		*menu;
		KPopupMenu 		*zoom, *images;

		QString				File, m_File;
		QCursor				cusual, cdrag, cZoomIn;
		QFileInfo			fm;
		QImage 			BGpixmap, BGquads;
		QTimer				*timer_show, *timer_hide, *timer_decode, *timer_prev, *timer_next, *timer_anim;
		QString				quickImageInfo;
		QPainter			*pRect;
		QRect				lastRect;
		QImage 			mm[4];

		SQ_ToolBar			*toolbar, *toolbar2;
		SQ_LIBRARY		*lib;
		fmt_codec_base		*codeK;
		RGBA				*next;
		fmt_info				finfo;

		std::vector<Parts>				parts;
		std::vector<MemoryPart256>	m256;
		std::vector<MemoryPart128>	m128;
		std::vector<MemoryPart64>		m64;
		std::vector<MemoryPart32>		m32;

		GLfloat 				matrix[12], saved[12];
		GLfloat				zoomfactor, movefactor, rotatefactor;
		GLfloat				curangle;

		unsigned int			texQuads, texPixmap;
		unsigned int			mark[4];
		int					xmoveold, ymoveold, xmove, ymove, current;
		int					steps;
		int					zoom_type, old_id;
		int 					total, errors, tileSize;
		bool				reset_mode, decoded, blocked, blocked_force, isflippedV, isflippedH;
		bool				m_hiding;
		bool				changed, inMouse, crossDrawn, changed2;
		bool				marks;
		float				zoomFactor;

		static SQ_GLWidget	*sing;
};

inline
bool SQ_GLWidget::manualBlocked()
{
	return blocked_force;
}

struct MemoryPart256
{
	MemoryPart256() {}
	unsigned char part [65536 * sizeof(RGBA)];
};

struct MemoryPart128
{
	MemoryPart128() {}
	unsigned char part [16384 * sizeof(RGBA)];
};

struct MemoryPart64
{
	MemoryPart64() {}
	unsigned char part [4096 * sizeof(RGBA)];
};

struct MemoryPart32
{
	MemoryPart32() {}
	unsigned char part [1024 * sizeof(RGBA)];
};

struct Part
{
	Part() : x1(0), y1(0), x2(0), y2(0), tex(0), list(0), priority(0.0f)
	{}

	float 			x1, y1, x2, y2;
	unsigned int	tex;
	GLuint 		list;
	GLclampf		priority;
};

struct Parts
{
	Parts() : tilesx(0), tilesy(0), tiles(0), tileSize(0), w(0), h(0), rw(0), rh(0), m_parts(0)
	{}

	int tilesx, tilesy, tiles, tileSize;
	int w, h, rw, rh;

	std::vector<Part> m_parts;

	void setRWH(const int rw2, const int rh2);
	void setWH(const int w2, const int h2);
	void setTileSize(const int t);
	bool makeParts();
	void removeParts();
	void computeCoords();
};

#endif
