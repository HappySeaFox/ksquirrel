/***************************************************************************
                          sq_quickbrowser.h  -  description
                             -------------------
    begin                : ??? ??? 4 2004
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

#ifndef SQ_QUICKBROWSER_H
#define SQ_QUICKBROWSER_H

#include <qvbox.h>
#include <qstatusbar.h>
#include <qsizegrip.h>

#include <ktoolbar.h>

class KFileItem;

class SQ_DirOperatorBase;
class SQ_FileIconView;

/*
 *  SQ_QuickBrowser is a simpe filemanager with toolbar and statusbar.
 *
 *  It used by SQ_GLWidget.
 */

/*
 *  Toolbar for SQ_QuickBrowser.
 */
class SQ_Header : public KToolBar
{
    public:
        SQ_Header(QWidget *parent = 0);
        ~SQ_Header();

    protected:
        void mousePressEvent(QMouseEvent *e);
        void mouseMoveEvent(QMouseEvent *e);
        void mouseReleaseEvent(QMouseEvent *e);

    private:
        int oldX, oldY, oldParentX, oldParentY;
        bool inMouse;
        QWidget *p;
};

/*
 *  SizeGrip for SQ_QuickBrowser's statusbar.
 *
 *  QSizeGrip itself resizes toplevel widget,
 *  but we want to resize our SQ_QuickBrowser (which
 *  is not toplevel)
 */
class SQ_SizeGrip : public QSizeGrip
{
    public:
        SQ_SizeGrip(QWidget *top, QWidget *parent = 0, const char *name = 0);
        ~SQ_SizeGrip();

    protected:
        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseMoveEvent(QMouseEvent *e);

    private:
        QWidget *p;
        QPoint mother;
};

/*
 *  Statusbar for SQ_QuickBrowser.
 */ 
class SQ_QuickStatus : public QStatusBar
{
    public:
        SQ_QuickStatus(QWidget *parent = 0, const char *name = 0);
        ~SQ_QuickStatus();

    protected:
        virtual void mousePressEvent(QMouseEvent *e);
};

/*
 *  SQ_QuickBrowser itself :)
 */
class SQ_QuickBrowser : public QVBox
{
    Q_OBJECT

    public: 
        SQ_QuickBrowser(QWidget *parent = 0, const char *name = 0);
        ~SQ_QuickBrowser();

        SQ_DirOperatorBase *quick;

        void updateCurrentFileItem();

        static SQ_QuickBrowser* window() { return m_instance; }
        static SQ_DirOperatorBase *quickOperator() { return op; }

    public slots:

        /*
         *  Close(hide) SQ_QuickBrowser.
         */
        void slotClose();

    protected:
        /*
         *  Reimplement closeEvent() and showEvent()
         */
        void closeEvent(QCloseEvent *);
        void showEvent(QShowEvent *);

    private:
        SQ_FileIconView *iv;

        static SQ_QuickBrowser *m_instance;
        static SQ_DirOperatorBase *op;
};

#endif
