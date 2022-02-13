/***************************************************************************
                          sq_multibar.h  -  description
                             -------------------
    begin                : ??? Nov 28 2005
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

#ifndef SQ_MULTIBAR_H
#define SQ_MULTIBAR_H

#include <qhbox.h>

class KMultiTabBar;

class QWidgetStack;
class QSignalMapper;

/*
 *  Konqueror-like sidebar.
 *
 *    +----------------------------------> KMultiTabBar, only contains buttons
 *    |           +-----------------+
 *    |           |                 |
 *    |           |                 |
 *  +------------------------+      |
 *  | 1 |                    |      +----> QWidgetStack, contains all widgets
 *  |---|                    |
 *  | 2 |                    |
 *  |---|                    |
 *  | 3 |                    |
 *  |---|                    |
 *  |   |    visible page    |
 *  |   |                    |
 *  | 4 |        N4          |
 *  |   |                    |
 *  |   |                    |
 *  |---|                    |
 *  |   |                    |
 *  |   |                    |
 *  |   |                    |
 *  |   |                    |
 *  +------------------------+
 *
 */

class SQ_MultiBar : public QHBox
{
    Q_OBJECT

    public: 
        SQ_MultiBar(QWidget *parent = 0, const char *name = 0);
	 ~SQ_MultiBar();

        /*
         *  Add new widget with text label 'text' and icon 'icon'. SQ_MultiBar
         *  will use SQ_IconLoader to load given icon.
         */
        void addWidget(QWidget *new_w, const QString &text, const QString &icon);

        /*
         *  Current page index. 0 means first page.
         */
        int currentPage();

    public slots:

        /*
         *  Hide current widget and show a widget with
         *  id 'id'.
         */
        void raiseWidget(int id);

    private:
        KMultiTabBar *mt;
        QWidgetStack *stack;
        int m_id, m_selected;
        QSignalMapper *mapper;
};

inline
int SQ_MultiBar::currentPage()
{
    return m_selected < 0 ? 0 : m_selected;
}

#endif
