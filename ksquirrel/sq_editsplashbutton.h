/***************************************************************************
                          sq_editsplashbutton.h  -  description
                             -------------------
    begin                : ??? May 5 2005
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

#ifndef SQ_EDITSPLASHBUTTON_H
#define SQ_EDITSPLASHBUTTON_H

#include <qwidget.h>
#include <qpixmap.h>

/*
 *  With pressing CTRL+E you can invoke dialog, which can
 *  call different edit tools. SQ_EditSplashButton represents
 *  a single button in this dialog.
 *
 *  It has big pixmap (about 96x96) and some text at the bottom.
 *  It aslo handles mouse press events.
 */

class SQ_EditSplashButton : public QWidget
{
    Q_OBJECT

    public: 
        SQ_EditSplashButton(QWidget *parent=0, const char *name=0);
        ~SQ_EditSplashButton();

        /*
         *  Set text and pixmap.
         */
        void setText(const QString &_text);
        void setPixmap(const QPixmap &_pix);

    protected:
        /*
         *  Reimplement mouse events to handle clicks.
         */
        virtual void mouseReleaseEvent(QMouseEvent *e);
        virtual void mousePressEvent(QMouseEvent *e);

        /*
         *  Reimplement enterEvent() and leaveEvent()
         *  to make hightlighting possible
         */ 
        virtual void enterEvent(QEvent *);
        virtual void leaveEvent(QEvent *);

        /*
         *  Draw button
         */
        virtual void paintEvent(QPaintEvent *);

    signals:
        /*
         *  Emitted, when user clicked on button.
         *  Hope, somebody will catch this signal ... :)
         */
        void clicked();

    private:
        bool on, pressed;
        QPixmap pix;
        QString text;
};

inline
void SQ_EditSplashButton::setText(const QString &_text)
{
    text = _text;
}

#endif
