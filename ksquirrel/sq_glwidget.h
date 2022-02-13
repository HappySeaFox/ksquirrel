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

#include <qgl.h>
#include <qcursor.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qrect.h>
#include <qpair.h>

#include <kurl.h>

#include <ksquirrel-libs/fmt_types.h>
#include <ksquirrel-libs/fmt_defs.h>

#include <vector>

// parameters in GL matrix
#define    MATRIX_C1    matrix[0]
#define    MATRIX_S1    matrix[1]
#define    MATRIX_X     matrix[3]
#define    MATRIX_S2    matrix[4]
#define    MATRIX_C2    matrix[5]
#define    MATRIX_Y     matrix[7]
#define    MATRIX_Z     matrix[11]

class KAction;
class KActionCollection;
class KToggleAction;
class KPopupMenu;
class KRadioAction;

class QTimer;
class QPainter;
class QPopupMenu;
class QSlider;

class SQ_ToolButtonPopup;
class SQ_ToolButton;
class SQ_ToolBar;

struct SQ_LIBRARY;
class fmt_codec_base;

struct Parts;
struct Part;
class memoryPart;

/* *************************************************************** */

/*
 *  SQ_GLWidget represents a widget, which loads, shows and manipulates
 *  an image.
 *
 *  It contains complex decoding method. Since OpenGL can show textures
 *  only with dimensions of power of 2 (32x62, 512, 256 etc.), we should divide
 * decoded image into quads and only then generate textures.
  * It means, that the image you see consists of several parts:
 *
 *   |----------------------------------
 *   |                |                |
 *   |    tex N1      |     tex N2     |
 *   |                |                |
 *   -----------------------------------    <= Entire image
 *   |                |                |
 *   |    tex N3      |     tex N4     |
 *   |                |                |
 *   -----------------------------------
 *
 *
 *  The main class member is 'parts'. It's an array of decoded images
 *  and appropriate memory buffers and textures. Here comes an explanation:
 *  [ use monotype fonts :) ]
 *
 *                                                           textures & coordinates           Part
 *     +-----------------------------------+                 #####################     ##################
 *     |                          parts[2] |                 #         #         #===> #                #
 *     | +-------------------------------------+             #         #         #     # texture id     #
 *     | |                            parts[1] |    |======> #####################     # texture coords #
 *     | | +-------------------------------------+  |        #         #         #     # ...            #
 *     | | |    m_parts                 parts[0] |  |        #         #         #     ##################
 *     | | |    #####                            |  |        #####################
 *     | | |    #   #                            |  |
 *     | | |    #   #===============================|
 *     | | |    #   #                            |
 *     | | |    #####                            |
 *     | | |                                     |              memory buffers
 *     | | |     m32                             |       ###############################
 *     | | |    #####                            |       #RGBA.#.....#.....#.....#.....#
 *     | | |    #   #                            |       #RGBA.#.....#.....#.....#.....#
 *     | | |    #   #==================================> ###############################
 *     +-| |    #   #                            |       #RGBA.#.....#.....#.....#.....#
 *       +-|    #####                            |       #RGBA.#.....#.....#.....#.....#
 *         +-------------------------------------+       ###############################
 *                       \      / 
 *                         \  /
 *                        parts
 *
 *
 ******************************************************************************
 *
 */

class SQ_GLWidget : public QGLWidget
{
    Q_OBJECT

    public:
        SQ_GLWidget(QWidget *parent = 0, const char *name = 0);
        ~SQ_GLWidget();

        /*
         *  Set zoom, move and rotate factors.
         */
        void setZoomFactor(const GLfloat &newfactor);
        void setMoveFactor(const GLfloat &newfactor);
        void setRotateFactor(const GLfloat &newfactor);

        /*
         *  Set clear color for context.
         */
        void setClearColor();

        GLfloat get_zoom() const;
        GLfloat get_zoom_pc() const;

        KActionCollection* actionCollection() const;

        /*
         *  Are we in fullscreen state ?
         */
        bool fullscreen() const;

        /*
         * Toggle fullscreen state.
         */
        void toggleFullScreen();

        /*
         *  Direct call to updateGL().
         */
        void updateGLA() { updateGL(); }

        /*
         *  With 'T' user can hide toolbar (with KToolBar::hide()).
         *  This method will help determine if toolbar is visible.
         */
        bool actionsHidden() const;

        /*
         *  Type of zoom: fit width, fit height...
         */
        int zoomType();

        /*
         *  Filter is GL_LINEAR ?
         */
        bool isnice();

        /*
         *  Direct call to glInit();
         */
        void glInitA() { QGLWidget::glInit(); }

        /*
         *  Start animation, if loaded image is animated.
         */
        void startAnimation();

        /*
         *  Stop animation, if loaded image is animated.
         */
        void stopAnimation();

        /*
         *  Is animation stopped by user ?
         */
        bool manualBlocked();

        /*
         *  Change statusbar info according with
         *  current matrix (it shows current zoom & angle values).
         */
        void matrixChanged();

        /*
         *  Check or uncheck 'Slideshow' button in toolbar.
         */
        void updateSlideShowButton(bool toggled);

        static SQ_GLWidget* window() { return m_instance; }

    protected:
        /*
         *  Next three methods should be reimplemented in
         *  every QGLWidget's subclass.
         */
        void initializeGL();
        void paintGL();
        void resizeGL(int,int);

        /*
         *  Mouse wheel event. Let's load next/previous image, or
         *  zoom in/zoom out (depends on settings).
         */
        void wheelEvent(QWheelEvent *);

        /*
         *  Keyboard events.
         */
        void keyPressEvent(QKeyEvent *);

        /*
         *  Palette changed. Let's update tickmarks and background color.
         */
        void paletteChange(const QPalette &oldPalette);

        /*
         *  Accept drag-and-drop events. We can drop some images
         *  on this widget, SQ_GLWidget will try to decode first file.
         *
         *  TODO: find first supported image and decode it ?
         */
        void dragEnterEvent(QDragEnterEvent *);
        void dropEvent(QDropEvent *);

        /*
         *  Mouse events.
         */
        void mousePressEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);

    private:
        void toClipboard();
        void saveAs();
        void  enableSettingsButton(bool enab);

        /*
         *  Remove currently loaded textures and memory buffers.
         */
        void removeCurrentParts();

        /*
         *  Since 0.6.0-final KSquirrel doesn't show error messages,
         *  if the image is broken or not supported. It uses "broken" image
         *  instead. This method does all needed init.
         */
        void initBrokenImage();

        /*
         *  Force using broken image + update context.
         *  Show appropriate error message in statusbar.
         */
        void useBrokenImage(const int err_index);

        /*
         *  Update filter. If 'nice' is true, use GL_LINEAR
         *  and GL_NEAREST otherwise.
         */
        void updateFilter(bool nice);

        /*
         *  Cleanup method.
         */
        void decodeFailedOn0();

        /*
         *  Create KActions.
         */
        void createActions();

        /*
         *  Create toolbars.
         */
        void createToolbar();

        /*
         *  Fill a w x h region with texture. Generate texture if needed.
         */
        void draw_background(void *bits, unsigned int *tex, int dim, GLfloat w, GLfloat h, bool &bind, bool deleteOld);

        /*
         *  Divide currently decoded image into
         *  several parts and store them in MemoryPart::m_data.
         */
        void setupBits(Parts *p, RGBA *buffer, int y, int x);

        /*
         *  Jump to first or last image in animated sequence.
         */
        void jumpToImage(bool);

        /*
         *  Next image in animated sequence. Called by user (with F3).
         */
        void nextImage();

        /*
         *  Previous image in animated sequence. Called by user (with F2).
         */
        void prevImage();

        /*
         *  Draw (or not) image's background.
         */
        void toggleDrawingBackground();

        /*
         *  Show popup menu with external tools.
         */
        void showExternalTools();

        /*
         *  Generate textures for tickmarks and bind them.
         */
        void bindMarks(bool &first, bool deleteOld);

        /*
         *  Load and check tickmarks from disk.
         */
        void createMarks();

        /*
         *  Wrapper for 'delete' key. Called from keyPressEvent().
         */
        void deleteWrapper();

        /*
         *  Show current image's width, height and bpp in statusbar.
         */
        void updateCurrentFileInfo();

        /*
         *  Show/hide tickmarks.
         */
        void toogleTickmarks();

        /*
         *  Show current page number in multipaged images.
         * 
         *  For example: "3/11" means that current page is the third in current image,
         *  which has 11 pages.
         */
        void frameChanged();

        /*
         *  Set current zoom to 'z'.
         */
        void internalZoom(const GLfloat &z, bool U = false);

        /*
         *  Create context menu :-)
         */
        void createContextMenu(QPopupMenu *m);

        /*
         *  Find best tile's width and height for given width and height.
         */
        void findCloserTiles(int w, int h, std::vector<int> &x, std::vector<int> &y);
        QPair<int, int> calcRealDimensions(Parts &, int y = -1, int x = -1);

        /*
         *  Prepare decoding. It will find proper library for decoding,
         *  clear old memory buffers, etc.
         */
        bool prepare();

        /*
         *  Zoom to 'r'. Will be called after somebody used mouse button
         *  to select zoom region. Return true, if zoomed.
         */
        bool zoomRect(const QRect &r);

        /*
         *  Bind textures, draw them and create GL lists.
         *  If 'swap' it true, swap buffers.
         */
        bool showFrames(int y, Parts *, bool swap);

        /*
         *  OpenGL-related methods, not interesting :-)
         *
         *  Move, zoom, reset, flip and rotate current matrix.
         */
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

    public slots:

        /*
         *  Start decoding given image. We can call it from anywhere.
         */
        void slotStartDecoding(const QString &file, bool = false);
        void slotStartDecoding(const KURL &url);

    private slots:

        /*
         *  Slots for toolbar's actions:
         *  fit width, fit height, zoom+, zoom-, rotate, flip,
         *  first file, last file, reset...
         */
        void slotSetZoomPercents(int);
        void slotZoomW();
        void slotZoomH();
        void slotZoomWH();
        void slotZoomPlus();
        void slotZoom100();
        void slotZoomLast();
        void slotZoomMinus();
        void slotZoomIfLess();
        void slotRotateLeft();
        void slotRotateRight();
        void slotFlipV();
        void slotFlipH();
        void slotMatrixReset();
        void slotProperties(); // show image properties
        void slotDecode();
        void slotHideToolbars(bool); // show/hide toolbars (with 'T')
        void slotToggleStatus(bool);
        void slotFirst();
        void slotLast();
        void slotNext();
        void slotPrev();
        void slotZoomMenu();
        void slotAnimateNext();
        void slotToggleAnimate(); // start/stop animation
        void slotSetCurrentImage(int);
        void slotShowImages();
        void slotImagesHidden();
        void slotImagesShown();
        void slotContextMenuItem(int);
        void slotShowHelp();
        void slotShowCodecSettings();
        void slotApplyCodecSettings();

    private:
        KAction         *pAReset, *pAClose, *pAProperties, *pAHelp;
        KToggleAction   *pAFull, *pAIfLess, *pAStatus, *pAZoomW,
                        *pAZoomH, *pAZoomWH, *pAZoom100,
                        *pAZoomLast, *pAHideToolbars;

        SQ_ToolButton   *pAToolSlideShow, *pAToolQuick, *pAToolFull;
        SQ_ToolButtonPopup    *pAToolZoom,  *pAToolImages;

        KActionCollection     *ac;
        QPopupMenu            *menu, *menuFile, *menuImage;
        int                                 id_settings, id_saveas;

        // popup menu with zoom types (fit width, fit height, zoom 100%...)
        KPopupMenu            *zoom,

                              // popup menu with image pages
                              *images;

        QString               File, m_File;
        QFileInfo             fm;
        QImage                BGpixmap, BGquads;

        QTimer                *timer_prev, *timer_next;
        QTimer                *timer_decode, *timer_anim;

        QString               quickImageInfo;
        QPainter              *pRect;
        QRect                 lastRect;
        QImage                mm[4];

        SQ_LIBRARY            *lib;
        fmt_codec_base        *codeK;
//        RGBA                  *next;
        fmt_info              finfo;
        fmt_image             image_broken;

        std::vector<Parts>    parts;
        Parts                 *parts_broken;

        GLfloat               matrix[12], saved[12], zoomfactor, movefactor, rotatefactor, curangle;

        unsigned int          texQuads, texPixmap, mark[4];
        int                   xmoveold, ymoveold, xmove, ymove, current,
                              zoom_type, old_id, total, errors, movetype;
        bool                  reset_mode, decoded, blocked, blocked_force, isflippedV, isflippedH,
                              changed, crossDrawn, changed2, marks, linear, use_broken;
        float                 zoomFactor;
        QWidget               *hack;
        RGBA                    *buffer;
        QSlider *slider_zoom;
        bool messages;

        static SQ_GLWidget    *m_instance;
};

inline
bool SQ_GLWidget::manualBlocked()
{
    return blocked_force;
}

inline
int SQ_GLWidget::zoomType()
{
    return zoom_type;
}

inline
bool SQ_GLWidget::isnice()
{
    return linear;
}

// Set zoom factor.
inline
void SQ_GLWidget::setZoomFactor(const GLfloat &newfactor)
{
    zoomfactor = newfactor;
}

// Set move factor.
inline
void SQ_GLWidget::setMoveFactor(const GLfloat &newfactor)
{
    movefactor = newfactor;
}

// Set rotate factor.
inline
void SQ_GLWidget::setRotateFactor(const GLfloat &newfactor)
{
    rotatefactor = newfactor;
}

inline
KActionCollection* SQ_GLWidget::actionCollection() const
{
    return ac;
}

/* *************************************************************** */

class memoryPart
{
    public:
        memoryPart(const int sz);
        ~memoryPart();

        // create/delete memory buffer
        void create();
        void del();

        bool valid() const;
        RGBA *data();

    private:
        int m_size;
        RGBA *m_data;
};

inline
RGBA* memoryPart::data()
{
    return m_data;
}

inline
void memoryPart::del()
{
    delete [] m_data;
    m_data = 0;
}

inline
bool memoryPart::valid() const
{
    return m_data != 0;
}

/* *************************************************************** */

struct Part
{
    Part();

    float           x1, y1, x2, y2, tx1, tx2, ty1, ty2;
    unsigned int    tex;
    GLuint          list;
};

/* *************************************************************** */

// one image page. All pages are stored in SQ_GLWidget::parts
struct Parts
{
    Parts();

    int w, h, realw, realh;

    std::vector<Part> m_parts;
    std::vector<int> tilesx, tilesy;
    memoryPart *buffer;

    bool makeParts();
    void removeParts();
    void computeCoords();
    void deleteBuffer();
};

inline
void Parts::deleteBuffer()
{
    delete buffer;
    buffer = 0;
}

#endif
