/***************************************************************************
                          sq_glview.h  -  description
                             -------------------
    begin                : ??? ??? 5 2004
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

#ifndef SQ_GLVIEW_H
#define SQ_GLVIEW_H

#include <qvbox.h>
#include <qmap.h>

#include <ktoolbar.h>

class SQ_GLWidget;

#ifdef SQ_SMALL

class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_Config;

#endif

class KStatusBar;

class QLabel;

class SQ_ToolBar : public KToolBar
{
    public:
        SQ_ToolBar(QWidget *parent);
        ~SQ_ToolBar();

    protected:
        void mouseReleaseEvent(QMouseEvent *);
};

/*
 *  SQ_GLView represents a widget containing SQ_GLWidget, toolbar and statusbar.
 *
 *  If SQ_SMALL is defined, it becomes the main widget (and class) in 'small' version
 *  of KSquirrel.
 *
 *
 *  + -------------------------------------------------+
 *  |  | | | | | | | | | | | | | | | | |               |  <= toolbar with actions (SQ_ToolBar)
 *  |--------------------------------------------------|
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |  <= OpenGL widget (SQ_GLWidget)
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |
 *  |                                                  |
 *  |--------------------------------------------------|  <= statusbar (KStatusBar)
 *  +--------------------------------------------------+
 *
 *
 */

class SQ_GLView : public QVBox
{
    Q_OBJECT

    public: 
        SQ_GLView(QWidget *parent = 0, const char *name = 0);
        ~SQ_GLView();

#ifndef SQ_SMALL

        /*
         *  Is this widget separate ?
         *
         *  Note: it can be separate, or built-in.
         */
        bool isSeparate() const;

        /*
         *  Make widget built-in with reparenting it.
         */
        void reparent(QWidget *parent, const QPoint &p, bool showIt = false);

        void statusBarLikeGQview(bool b);

#endif

        /*
         *  Save current position and size to config
         */
        void saveGeometry();
        void restoreGeometry();

        /*
         *  Reset all statusbar's labels to default values.
         */
        void resetStatusBar();

        /*
         *  Get a pointer to statusbar.
         */
        KStatusBar* statusbar();


        /*
         *  Get a poniter to toolbar with actions (next file, zoom, rotate...)
         */ 
        SQ_ToolBar* toolbar();

        /*
         *  Get a pointer to a widget in statusbar by name.
         */
        QLabel* sbarWidget(const QString &name) const;

        static SQ_GLView* window() { return m_instance; }

    protected:

        /*
         *  We want to eat close events. If current version is not 'small',
         *  ignore close event. If current version is 'small', save all important
         *  parameters to config file and accept close event (exit).
         */
        void closeEvent(QCloseEvent *e);

    private:

        /*
         *  Internal.
         */
        void createContent();

#ifndef SQ_SMALL

        virtual bool eventFilter(QObject *watched, QEvent *e);

#endif

    public slots:
        /*
         *  Goto fullscreen. If current version is NOT
         *  'small', fullscreen state will be managed by KSquirrel.
         */
        void slotFullScreen(bool full);

    private slots:
        /*
         *  All libraries now loaded.
         */
        void slotContinueLoading();

#ifdef SQ_SMALL

    private:
        SQ_LibraryListener *libl;
        SQ_LibraryHandler  *libh;
        SQ_Config          *kconf;

#endif

    private:

#ifndef SQ_SMALL

        bool separate;

#endif
        SQ_GLWidget             *gl;
        KStatusBar              *sbar;
        QMap<QString, QLabel* > names;
        SQ_ToolBar              *m_toolbar;

        static SQ_GLView *m_instance;
};

inline
SQ_ToolBar* SQ_GLView::toolbar()
{
    return m_toolbar;
}

inline
KStatusBar* SQ_GLView::statusbar()
{
    return sbar;
}

inline
QLabel* SQ_GLView::sbarWidget(const QString &name) const
{
    return names[name];
}

#ifndef SQ_SMALL

inline
bool SQ_GLView::isSeparate() const
{
    return separate;
}

#endif

#endif
