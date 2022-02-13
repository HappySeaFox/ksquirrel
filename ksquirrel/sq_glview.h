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
class SQ_LibraryListener;
class SQ_LibraryHandler;
class SQ_Config;

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
        SQ_GLView(QWidget *parent = 0);
        ~SQ_GLView();

        /*
         *  Is this widget separate ?
         *
         *  Note: it can be separate, or built-in.
         */
        bool isSeparate() const;

        /*
         *  Make widget built-in with reparenting it.
         */
        void reparent(QWidget *parent, const QPoint &p = QPoint(0,0), bool showIt = false);

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

        virtual bool eventFilter(QObject *watched, QEvent *e);

    private:

        bool separate;
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

inline
bool SQ_GLView::isSeparate() const
{
    return separate;
}

#endif
