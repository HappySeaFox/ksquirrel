/***************************************************************************
                          sq_multibar.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qwidgetstack.h>
#include <qsignalmapper.h>
#include <qsplitter.h>

#include <kmultitabbar.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "ksquirrel.h"
#include "sq_previewwidget.h"
#include "sq_multibar.h"
#include "sq_config.h"

SQ_MultiBar * SQ_MultiBar::m_inst = 0;

SQ_MultiBar::SQ_MultiBar(QWidget *parent, const char *name) : QHBox(parent, name)
{
    m_inst = this;
    m_id = 0;
    m_selected = -1;

    SQ_Config::instance()->setGroup("Interface");
    m_width = SQ_Config::instance()->readNumEntry("splitter", 220);

    mapper = new QSignalMapper(this);

    connect(mapper, SIGNAL(mapped(int)), this, SLOT(raiseWidget(int)));

    mt = new KMultiTabBar(KMultiTabBar::Vertical, this);

    // setup multibar: style = VSNET, show text labels on the left side
    mt->setStyle(KMultiTabBar::VSNET);
    mt->setPosition(KMultiTabBar::Left);
    mt->showActiveTabTexts(true);

    setSpacing(0);

    QSplitter   *ts = new QSplitter(Qt::Vertical, this);
    ts->setOpaqueResize(false);

    // QWigdetStack will contain all widgets
    stack = new QWidgetStack(ts);

    new SQ_PreviewWidget(ts);

    QValueList<int> sz;
    sz.append(5500);
    sz.append(4500);
    ts->setSizes(sz);
}

SQ_MultiBar::~SQ_MultiBar()
{}

void SQ_MultiBar::addWidget(QWidget *new_w, const QString &text, const QString &icon)
{
    // add widget to stack
    stack->addWidget(new_w, m_id);

    // add button
    mt->appendTab(KGlobal::iconLoader()->loadIcon(icon, KIcon::Desktop, 22), m_id, text);

    // since we cann't determine which tab was clicked,
    // we should use QSignalMapper to determine it.
    mapper->setMapping(mt->tab(m_id), m_id);

    connect(mt->tab(m_id), SIGNAL(clicked()), mapper, SLOT(map()));

    m_id++;
}

void SQ_MultiBar::raiseWidget(int id)
{
    if(m_selected != -1)
        mt->setTab(m_selected, false);

    if(mt->isTabRaised(id))
    {
        if(m_selected != -1)
            m_width = stack->width();

        m_selected = id;

        setMinimumSize(QSize(0, 0));
        setMaximumSize(QSize(10000, 10000));
        stack->raiseWidget(id);
        stack->resize(m_width, stack->height());
        stack->show();

        SQ_PreviewWidget::instance()->ignore(false);
        SQ_PreviewWidget::instance()->loadPending();
    }
    else
    {
        SQ_PreviewWidget::instance()->ignore(true);

        KSquirrel::app()->saveLayout();

        m_selected = -1;
        m_width = stack->width();
        stack->hide();
        setFixedWidth(mt->width());
    }
}

void SQ_MultiBar::updateLayout()
{
    setFixedWidth(mt->sizeHint().width());
    stack->hide();
}

#include "sq_multibar.moc"
