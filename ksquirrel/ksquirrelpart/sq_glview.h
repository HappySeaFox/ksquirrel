/***************************************************************************
                          sq_glview.h  -  description
                             -------------------
    begin                : Thu Nov 29 2007
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

#ifndef SQ_GLVIEW_H
#define SQ_GLVIEW_H

#include <qobject.h>
#include <qmap.h>
#include <qstring.h>

class SQ_TextSetter : public QObject
{
    Q_OBJECT

    public:
        SQ_TextSetter(QObject *parent = 0);
        ~SQ_TextSetter();

        void setText(const QString &);

        QString text() const;

    signals:
        void changed();

    private:
        QString dest;
};

inline
QString SQ_TextSetter::text() const
{
    return dest;
}

/***********************************************/

class SQ_GLView : public QObject
{
    Q_OBJECT

    public:
        SQ_GLView();
        ~SQ_GLView();

        SQ_TextSetter* sbarWidget(const QString &);

        void resetStatusBar();

        static SQ_GLView* window() { return m_inst; }

    signals:
        void message(const QString &);

    private slots:
        void slotChanged();

    private:
        static SQ_GLView *m_inst;

        typedef QMap<QString, SQ_TextSetter*> SQ_Setters;

        SQ_Setters map;
        SQ_TextSetter *tmp;
};

#endif
