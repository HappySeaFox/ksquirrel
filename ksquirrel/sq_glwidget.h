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

#include <qimage.h>
#include <qrect.h>
#include <qpair.h>
#include <qdatetime.h>

#include <kurl.h>

#include <vector>

#include "sq_glparts.h"

class SQ_GLSelectionPainter;
struct SQ_ImageFilterOptions;
struct SQ_ImageBCGOptions;
struct RGBA;

// parameters in GL matrix
#define    MATRIX_C1    tab->matrix[0]
#define    MATRIX_S1    tab->matrix[1]
#define    MATRIX_X     tab->matrix[3]
#define    MATRIX_S2    tab->matrix[4]
#define    MATRIX_C2    tab->matrix[5]
#define    MATRIX_Y     tab->matrix[7]
#define    MATRIX_Z     tab->matrix[11]

class KAction;
class KActionCollection;
class KToggleAction;
class KPopupMenu;
class KRadioAction;
class KTempFile;

namespace KIO { class Job; }

class QTimer;
class QPopupMenu;
class QSlider;

class SQ_ToolButtonPopup;
class SQ_ToolButton;
class SQ_ToolBar;

/* *************************************************************** */

/*
 *  SQ_GLWidget represents a widget, which loads, shows and manipulates
 *  an image.
 *
 *  It contains complex decoding method. Since OpenGL can show textures
 *  only with dimensions of power of 2 (32x62, 512, 256 etc.), we should divide
 *  decoded image into quads and only then generate textures.
 *  It means, that the image you see consists of several parts:
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

        void removeNonCurrentTabs(int);

        void setExpectedURL(const KURL &u);

        QString originalURL() const;

        /*
         *  Start decoding given image. We can call it from anywhere.
         */
        void startDecoding(const QString &file);
        void startDecoding(const KURL &url);

        /*
         *  Set zoom, move and rotate factors from config.
         */
        void updateFactors();

        /*
         *  Set clear color for context.
         */
        void setClearColor();

        void setOriginalURL(const KURL &);

        /*
         *  Get zoom value, e.g. 1.5, 2.2 ...
         */
        GLfloat getZoom() const;

        /*
         *  Get zoom value in percents, e.g. 150, 220 ...
         */
        GLfloat getZoomPercents() const;

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
        QImage generatePreview();

        void changeSlider(GLfloat z = -1.0);

        void hackMatrix();

        void enableActions(bool U);

        void initAccelsAndMenu();

        void crop();
        void bcg();
        void filter();
        void editUpdate();

        /*
         *  Save current image page to clipboard
         */
        void toClipboard();

        /*
         *  Save current image page to file
         */
        void saveAs();

        void enableSettingsButton(bool enab);

        /*
         *  Remove currently loaded textures and memory buffers.
         */
        void removeCurrentParts();
        void removeCurrentTabs();

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
        void decodeFailedOn0(const int err_code);

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
        void initMarks();

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
        void calcFrameLabelWidth();

        /*
         *  Set current zoom to 'z'.
         */
        void internalZoom(const GLfloat &z);

        /*
         *  Find best tile's width and height for given width and height.
         */
        static void findCloserTiles(int w, int h, std::vector<int> &x, std::vector<int> &y);
        static QPair<int, int> calcRealDimensions(Parts &, int y = -1, int x = -1);

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
        void matrix_reset(bool = true);
        void matrix_pure_reset();
        void matrix_push();
        void matrix_pop();
        void write_gl_matrix();
        void matrix_rotate(GLfloat angle, bool = true);
        void matrix_rotate2(GLfloat angle);
        void flip(int, bool = true);
        void flip_h();
        void flip_v();
        void exifRotate(bool);

    signals:
        void tabCountChanged();

    private slots:
        void decode();

        void slotAccelActivated();

        void slotChangeTab(int);
        void slotCloseRequest(int);

        void slotCopyJobResult(KIO::Job *job);

        /*
         *  Slots for toolbar's actions:
         *  fit width, fit height, zoom+, zoom-, rotate, flip,
         *  first file, last file, reset...
         */
        void slotShowNav();
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
        void slotShowHelp();
        void slotShowCodecSettings();
        void slotApplyCodecSettings();

        void slotSelectionEllipse();
        void slotSelectionRect();
        void slotSelectionClear();

        void slotBCG(SQ_ImageBCGOptions *);
        void slotFilter(SQ_ImageFilterOptions *fltopt);

        void slotCopyResult(KIO::Job *);

    private:
        KAction               *pASelectionClear;
        KToggleAction         *pAFull, *pAIfLess, *pAZoomW,
                              *pAZoomH, *pAZoomWH, *pAZoom100,
                              *pAZoomLast,
                              *pASelectionEllipse, *pASelectionRect;

        SQ_ToolButton         *pAToolQuick, *pAToolFull, *pAToolProp;
        SQ_ToolButtonPopup    *pAToolZoom,  *pAToolImages;

        KActionCollection     *ac, *acMain;
        QPopupMenu            *menu, *menuFile, *menuImage;
        int                             id_saveas, id_settings,
                                        id_f5, id_f6, id_f7, id_f8, id_del,
                                        id_prop;

        // popup menu with zoom types (fit width, fit height, zoom 100%...)
        KPopupMenu            *zoom, *selectionMenu,

                                  // popup menu with image pages
                                  *images;

        QImage                 BGpixmap, BGquads;

        QTimer                 *timer_prev, *timer_next;
        QTimer                 *timer_anim;

        QImage                 mm[4];

        fmt_image              image_broken;
        SQ_GLSelectionPainter  *gls;

        Parts                  *parts_broken;

        GLfloat                saved[12], zoomfactor, movefactor, rotatefactor;

        unsigned int           texQuads, texPixmap, mark[4];
        int                    xmoveold, ymoveold, xmove, ymove,
                               zoom_type, old_id, total, errors, movetype;
        bool                   reset_mode, decoded, blocked, 
                               changed, marks, linear;
        float                  zoomFactor, oldZoom;
        RGBA                   *buffer;
        QSlider                *slider_zoom;

        KTempFile              *tmp;
        KURL                   lastCopy, m_expected, m_original;
        QTime                  clickTime;

        std::vector<Tab>        tabs;
        Tab                     *tab, *tabold;
        Tab                     tmptab, taborig;
        bool                    hackResizeGL;

        static SQ_GLWidget     *m_instance;
};

inline
bool SQ_GLWidget::manualBlocked()
{
    return tab->manualBlocked;
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

inline
KActionCollection* SQ_GLWidget::actionCollection() const
{
    return ac;
}

inline
void SQ_GLWidget::setOriginalURL(const KURL &u)
{
    m_original = u;
}

inline
QString SQ_GLWidget::originalURL() const
{
    return tab->m_original.isLocalFile() ? tab->m_original.path() : tab->m_original.prettyURL();
}

inline
void SQ_GLWidget::setExpectedURL(const KURL &u)
{
    m_expected = u;
}

#endif
