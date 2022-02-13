/***************************************************************************
                          sq_slideshowwidget.h  -  description
                             -------------------
    begin                : ??? Mar 30 2007
    copyright            : (C) 2007 by Baryshev Dmitry
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

#ifndef SQ_SLIDESHOWWIDGET_H
#define SQ_SLIDESHOWWIDGET_H

#include <qwidget.h>
#include <qpixmap.h>

class QTimer;
class QHBox;
class QLabel;
class QToolButton;

/**
  *@author Baryshev Dmitry
  *
  * A widget to show slideshow. It contains toolbar
  *  with some useful actions like "pause", "close", "next image" etc.
  * It also can show onscreen message, which format is configured through
  * "Action -> Slideshow advanced".
  */

class SQ_SlideShowWidget : public QWidget
{
    Q_OBJECT

    public: 
	SQ_SlideShowWidget(QWidget *parent = 0, const char *name = 0);
	~SQ_SlideShowWidget();

        void beginSlideShow(int totl);
        void endSlideShow();
        void loadImage(const QString &path, int);

        virtual bool eventFilter(QObject * watched, QEvent *e);

        static SQ_SlideShowWidget* instance() { return m_inst; }

    protected:
        /*
         *  Draw decoded image
         */
        virtual void paintEvent(QPaintEvent *);

        /*
         *  Ignore close event and tell KSquirrel to stop slideshow
         */
        virtual void closeEvent(QCloseEvent *);

        /*
         *  Return, Escape = stop slideshow
         *  Pause, Space = pause/unpause slideshow
         *  Page Up = previous image
         *  Page Down = next image
         */
        virtual void keyPressEvent(QKeyEvent *);

        /*
         *  show toolbar with actions
         */
        virtual void mouseMoveEvent(QMouseEvent *);

    private:
        void constructMessage();

    signals:
        void stopSlideShow();
        void pause();
        void next();
        void previous();

    private slots:
        void slotHide();
        void slotShowHelp();
        void slotResetPause();

    private:
        QString path;
        int current;
        QPixmap pixmap;
        QColor bgcolor, tcolor;
        QTimer *timerHide;
        QLabel *message;
        QHBox *options;
        bool messages, mes_name, mes_size, mes_pos;
        int total;
        QToolButton *buttonPause;

        static SQ_SlideShowWidget *m_inst;
};

#endif
