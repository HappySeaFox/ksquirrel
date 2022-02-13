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

class QWMatrix;

class KToolBar;

struct RGBA;

class SQ_ToolButtonPopup : public KToolBarButton
{
    public:
        SQ_ToolButtonPopup(const QPixmap &pix, const QString &textLabel, KToolBar *parent);
        ~SQ_ToolButtonPopup();
};

class SQ_ToolButton : public QToolButton
{
    public:
        SQ_ToolButton(const QIconSet &iconSet, const QString &textLabel, QObject *receiver,
                        const char *slot, KToolBar *parent, const char *name = 0);
        ~SQ_ToolButton();

        static int fixedWidth();
};

inline
int SQ_ToolButton::fixedWidth()
{
    return 26;
}

namespace SQ_GLHelpers
{
    //                                                                               0, 1, 2
    typedef void (* scanLineGetter)(RGBA *data, RGBA *, int rw, int w, int h, int y, int flip);

    void scanLine0(RGBA *, RGBA *, int, int, int, int, int);
    void scanLine90(RGBA *, RGBA *, int, int, int, int, int);
    void scanLine180(RGBA *, RGBA *, int, int, int, int, int);
    void scanLine270(RGBA *, RGBA *, int, int, int, int, int);

    int roundAngle(int ang);

    void subRotation(QWMatrix &wm, int curangle, int orient);

    /*
     * normalize selection rectangle
     * sx,sy are X and Y coordinates
     * sw x sh is a selection geometry
     */
    bool normalizeSelection(int &sx, int &sy, int &sw, int &sh, int w, int h, const QWMatrix&, int curangle, int orient);
}

#endif
