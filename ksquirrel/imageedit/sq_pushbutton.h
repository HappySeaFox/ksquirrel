/***************************************************************************
                          sq_pushbutton.h  -  description
                             -------------------
    begin                : Sun Sep 25 2005
    copyright            : (C) 2005 by Baryshev Dmitry
    email                : ksquirrel.iv@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SQ_PUSHBUTTON_H
#define SQ_PUSHBUTTON_H

#include <qpushbutton.h>

/*
 *  SQ_PushButton is a simple subclass of QPushButton.
 */

class SQ_PushButton : public QPushButton
{
    public: 
        SQ_PushButton(QWidget *parent, const char *name = 0);
        SQ_PushButton(const QString &text, QWidget *parent, const char *name = 0);
        ~SQ_PushButton();

        void setPopup(QPopupMenu *popup);

    private:
        void init();
};

#endif
