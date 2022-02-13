/***************************************************************************
                          sq_glwidget_helpers.h  -  description
                             -------------------
    begin                : May 31 2005
    copyright            : (C) 2005 by Baryshev Dmitry
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

#ifndef SQ_GLWIDGET_HELPERS
#define SQ_GLWIDGET_HELPERS

#include <ktoolbarbutton.h>

class QToolBar;

class SQ_ToolButtonPopup : public KToolBarButton
{
    public:
        SQ_ToolButtonPopup(const QPixmap &pix, const QString &textLabel, QWidget *parent);
        ~SQ_ToolButtonPopup();

        static int id;
};

class SQ_ToolButton : public QToolButton
{
    public:
        SQ_ToolButton(const QIconSet &iconSet, const QString &textLabel, QObject *receiver,
                        const char *slot, QToolBar *parent, const char *name = 0);
        ~SQ_ToolButton();

        static int fixedWidth();
};

class SQ_ToolButtonPage : public SQ_ToolButton
{
    public:
        SQ_ToolButtonPage(QObject *receiver, const char *slot, QToolBar *parent, const char *name = 0);
        ~SQ_ToolButtonPage();

        void advanceValue();
        void flush();

        static int fixedWidth();

    private:
        QPixmap generatePixmap();

    private:
        int val;
};

#endif
