/*
 *  Copyright (C) 2006 Baryshev Dmitry, KSquirrel project
 *
 *  Originally based on browser_mnu.h by (C) Matthias Elter
 *  from kde-3.2.3
 */

/*****************************************************************

Copyright (c) 2001 Matthias Elter <elter@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef __browser_mnu_h__
#define __browser_mnu_h__

#include <qmap.h>
#include <qptrlist.h>
#include <kpanelmenu.h>
#include <kdirwatch.h>

class SQ_CategoryBrowserMenu : public KPanelMenu
{
    Q_OBJECT

    public:
        SQ_CategoryBrowserMenu(QString path, QWidget *parent = 0, const char *name = 0, int startid = 0);
        ~SQ_CategoryBrowserMenu();

        void append(const QPixmap &pixmap, const QString &title, const QString &filename, bool mimecheck);
        void append(const QPixmap &pixmap, const QString &title, SQ_CategoryBrowserMenu *subMenu);

    public slots:
        void initialize();

    protected slots:
        void slotExec(int id);
        void slotAddToCategory();
        void slotMimeCheck();
        void slotClearIfNeeded(const QString&);
        void slotClear();

    protected:
        void mousePressEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void dropEvent(QDropEvent *ev);
        void dragEnterEvent(QDragEnterEvent *ev);
        void initIconMap();

        QPoint             _lastpress;
        QMap<int, QString> _filemap;
        QMap<int, bool>    _mimemap;
        QTimer            *_mimecheckTimer;
        KDirWatch          _dirWatch;
        QPtrList<SQ_CategoryBrowserMenu> _subMenus;

        int                _startid;
        bool               _showhidden;
        int                _maxentries;
        bool               _dirty;

        static QMap<QString, QPixmap> *_icons;
};

#endif
